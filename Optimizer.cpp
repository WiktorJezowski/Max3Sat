#include "Optimizer.h"

#include <cfloat>
#include <iostream>
#include <windows.h>

using namespace std;

CIndividual::CIndividual(CProblem* pcProblem) {
	pc_problem = pcProblem;
	i_length = pc_problem->iGetClauseNumber();
	pb_genotype = new bool[i_length];
	i_clause_matched = -1;
}

CIndividual::CIndividual(const CIndividual& pcOther) {
	pc_problem = pcOther.pc_problem;
	i_length = pcOther.i_length;
	pb_genotype = new bool[i_length];
	vCopyGenotype(pcOther);
}

void CIndividual::vCopyGenotype(const CIndividual& pcOther) {
	i_clause_matched = pcOther.i_clause_matched;
	for (int i = 0; i < i_length; i++) {
		pb_genotype[i] = pcOther.pb_genotype[i];
	}
}

CIndividual::~CIndividual() {
	if (pb_genotype != NULL) delete[] pb_genotype;
}

void CIndividual::vRandomize() {
	for (int i = 0; i < i_length; i++) {
		pb_genotype[i] = rand() % 2;
	}

	vRefreshFitness();
}

int CIndividual::iFitness() { return i_clause_matched; }

void CIndividual::vRefreshFitness() {
	i_clause_matched = pc_problem->iCheck(pb_genotype);
}

void CIndividual::vCrossover(CIndividual* pcChild2, CIndividual* pcParent1, CIndividual* pcParent2) {
	if ((float)rand() / (float)RAND_MAX < dCrossoverPopability) {
		for (int i = 0; i < i_length; i++) {
			if (rand() % 2) {
				pb_genotype[i] = pcParent1->pb_genotype[i];
				pcChild2->pb_genotype[i] = pcParent2->pb_genotype[i];
			}
			else {
				pb_genotype[i] = pcParent2->pb_genotype[i];
				pcChild2->pb_genotype[i] = pcParent1->pb_genotype[i];
			}
		}
	}
	else {
		vCopyGenotype(*pcParent1);
		pcChild2->vCopyGenotype(*pcParent2);
	}
}

void CIndividual::vMutation() {
	if ((float)rand() / (float)RAND_MAX < dMutationPropability) {
		for (int i = 0; i < i_length; i++) {
			if ((float)rand() / (float)RAND_MAX < dMutationOvGenePropability) {
				int iOld = iFitness();
				pb_genotype[i] = !pb_genotype[i];
				int iNew = pc_problem->iCheck(pb_genotype);
				if (iOld > iNew) {
					pb_genotype[i] = !pb_genotype[i];
				}
				else {
					i_clause_matched = iNew;
				}
			}
		}
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////


CPopulation::CPopulation(CProblem* pcProblem) {
	pc_problem = pcProblem;
	pc_best_found = NULL;
	i_population_size = iDefaultPopulationSize;

	i_genes_number = pc_problem->iGetNumberOvVariables();
}

CPopulation::~CPopulation() {
	vExterminatePopulation();
}

void CPopulation::vInitialize() {
	for (int i = 0; i < i_population_size; i++) {
		v_population.push_back(new CIndividual(pc_problem));
		v_population[i]->vRandomize();
	}

	vFindBest();
}

int CPopulation::iFindParentIndex() {
	int iBestFoundIndex = rand() % i_population_size;

	for (int i = 1; i < iNumberOvPossibleParents; i++) {
		int iPossibleParentIndex = rand() % i_population_size;

		if (v_population.at(iPossibleParentIndex)->iFitness() > v_population.at(iBestFoundIndex)->iFitness()) {
			iBestFoundIndex = iPossibleParentIndex;
		}
	}

	return iBestFoundIndex;
}

void CPopulation::vExterminatePopulation() {
	for (int i = 0; i < i_population_size; i++) {
		if (v_population.at(i) != NULL) delete v_population.at(i);
	}
}

void CPopulation::vFindBest() {
	pc_best_found = v_population.at(0);

	for (int i = 1; i < i_population_size; i++) {
		if (pc_best_found->iFitness() < v_population.at(i)->iFitness()) {
			pc_best_found = v_population.at(i);
		}
	}
}

void CPopulation::vRunIteration() {
	vector<CIndividual*> v_new_population;
	while (v_new_population.size() < i_population_size) {
		CIndividual* c_child1 = new CIndividual(pc_problem);
		CIndividual* c_child2 = new CIndividual(pc_problem);

		c_child1->vCrossover(c_child2, v_population.at(iFindParentIndex()), v_population.at(iFindParentIndex()));

		c_child1->vMutation();
		c_child2->vMutation();

		c_child1->vRefreshFitness();
		c_child2->vRefreshFitness();

		v_new_population.push_back(c_child1);
		v_new_population.push_back(c_child2);
	}

	//vRepair();

	vExterminatePopulation();
	v_population = v_new_population;
	i_population_size = v_population.size();
	vFindBest();
	//cout << pc_best_found->iFitness() << "\n";
}



void CIndividual::vGeneRepair(int iGeneRepaired) {
	if (pc_problem->bAnalizeThisGene(pb_genotype, iGeneRepaired)) {
		pb_genotype[iGeneRepaired] = !pb_genotype[iGeneRepaired];
	}
}

void CPopulation::vRepair() {
	for (int i = 0; i < iNumberOvGenesRapaired; i++) {
		int iIndividualRepairedIndex = rand() % i_population_size;
		int iGeneRepairedIndex = rand() % i_genes_number;
		v_population.at(iIndividualRepairedIndex)->vGeneRepair(iGeneRepairedIndex);
	}
}

//////////////////////////////////////////////////////////////////////

COptimizer::COptimizer(CProblem* pcProblem) {
	pc_problem = pcProblem;
	pc_best_found = NULL;
	pc_populations = new CPopulation*[iNumberOvPopulations];
	i_turns_after_migration = 0;
}

COptimizer::~COptimizer() {
	for (int i = 0; i < iNumberOvPopulations; i++) {
		delete pc_populations[i];
	}
}

void COptimizer::vInitialize() {
	for (int i = 0; i < iNumberOvPopulations; i++) {
		pc_populations[i] = new CPopulation(pc_problem);
		pc_populations[i]->vInitialize();
	}
	vFindBest();
}

void COptimizer::vRunIteration() {
	for (int i = 0; i < iNumberOvPopulations; i++) {
		pc_populations[i]->vRunIteration();
	}

	if (i_turns_after_migration++ == iTurnsToMigration) {
		for (int i = 0; i < iNumberOvPopulations; i++) {
			CIndividual* pc_best = pc_populations[i]->pcGetBestFound();
			pc_populations[i]->vRemoveThis(pc_best);
			pc_populations[rand() % iNumberOvPopulations]->vAddThis(pc_best);
		}
		i_turns_after_migration = 0;
	}
	
	vFindBest();
	/////////////////////dorobiæ migracje
}

void COptimizer::vFindBest() {
	pc_best_found = pc_populations[0]->pcGetBestFound();
	for (int i = 1; i < iNumberOvPopulations; i++) {
		if (pc_best_found->iFitness() > pc_populations[i]->pcGetBestFound()->iFitness()) {
			pc_best_found = pc_populations[i]->pcGetBestFound();
		}
	}
}

void CPopulation::vRemoveThis(CIndividual* pcIndividual) {
	for (int i = 0; i < i_population_size; i++) {
		if (v_population.at(i) == pcIndividual) {
			v_population.at(i) = v_population.back();
			v_population.pop_back();
			i_population_size--;
			return;
		}
	}
}
void CPopulation::vAddThis(CIndividual* pcIndividual) {
	v_population.push_back(pcIndividual);
	i_population_size++;
}

int* CIndividual::piGetResultAsTable() {
	int* pi_genotype = new int[i_length];

	for (int i = 0; i < i_length; i++) {
		if (pb_genotype[i]) {
			pi_genotype[i] = 1;
		}
		else {
			pi_genotype[i] = 0;
		}
	}

	return pi_genotype;
}
