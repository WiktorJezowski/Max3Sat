#pragma once

#include "Problem.h"

#include <random>
#include <vector>

#define dCrossoverPopability 0.5
#define dMutationOvGenePropability 0.2
#define dMutationPropability 0.3
#define iDefaultPopulationSize 30
#define iNumberOvPossibleParents 4

#define iNumberOvGenesRapaired 20

#define iNumberOvPopulations 5

#define iTurnsToMigration 15

using namespace std;

class  CIndividual;

class CPopulation {
public:
	CPopulation(CProblem* pcProblem);
	~CPopulation();

	void vInitialize();
	void vRunIteration();

	int iFindParentIndex();
	void vExterminatePopulation();
	void vFindBest();

	void vRepair();

	void vRemoveThis(CIndividual* pcIndividual);
	void vAddThis(CIndividual* pcIndividual);

	CIndividual* pcGetBestFound() { return pc_best_found; }

private:

	CProblem* pc_problem;
	//mt19937 c_rand_engine;
	CIndividual* pc_best_found;
	int i_population_size;
	int i_genes_number;
	vector<CIndividual*>  v_population;
};

class COptimizer
{
public:
	COptimizer(CProblem* pcProblem);
	~COptimizer();

	void vInitialize();
	void vRunIteration();

	//int iFindParentIndex();
	//void vExterminatePopulation();
	void vFindBest();

	//void vRepair();

	CIndividual* pcGetBestFound() { return pc_best_found; }

private:

	CProblem* pc_problem;
	//mt19937 c_rand_engine;
	CIndividual* pc_best_found;
	CPopulation** pc_populations;
	int i_turns_after_migration;
};


class  CIndividual
{
	bool* pb_genotype;
	int i_length;
	int i_clause_matched;
	CProblem* pc_problem;

public:
	CIndividual(CProblem* cProblem);
	CIndividual(const CIndividual& pcOther);
	~CIndividual();

	void vRandomize();
	void vCrossover(CIndividual* pcChild2, CIndividual* pcParent1, CIndividual* pcParent2);
	void vMutation();
	void vRefreshFitness();
	int iFitness();
	void vCopyGenotype(const CIndividual& pcOther);
	void vGeneRepair(int iGeneRepaired);
	int* piGetResultAsTable();
};
