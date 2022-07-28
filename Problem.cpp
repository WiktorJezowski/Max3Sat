#include "Problem.h"


#include <algorithm>
#include <iostream>
#include <random>
#include <stdexcept>
#include <fstream>

CProblem::CProblem() {
	piNumber = NULL;
	pbSign = NULL;
	iClauseNumber = 0;
	iNumberOvVariables = 0;
}

CProblem::~CProblem()
{
	b_dealloc_table_2_dim_int(&piNumber, iClauseNumber, iClauseSize);
	b_dealloc_table_2_dim_bool(&pbSign, iClauseNumber, iClauseSize);
}

bool CProblem::b_dealloc_table_2_dim_int(int*** pi_table, int i_size_x, int i_size_y) {
	if (i_size_x <= 0 || i_size_y <= 0) {
		return false;
	}
	for (int i = 0; i < i_size_x; i++) {
		delete[](*pi_table)[i];
	}
	delete[](*pi_table);
	return true;
}

bool CProblem::b_dealloc_table_2_dim_bool(bool*** pb_table, int i_size_x, int i_size_y) {
	if (i_size_x <= 0 || i_size_y <= 0) {
		return false;
	}
	for (int i = 0; i < i_size_x; i++) {
		delete[](*pb_table)[i];
	}
	delete[](*pb_table);
	return true;
}

bool CProblem::bCountFilesClause(string sSourcePath) {
	ifstream file;
	iClauseNumber = 0;
	string sLine;

	try {
		file.open(sSourcePath + ".txt");

		if (file.is_open()) {
			while (getline(file, sLine)) {
				iClauseNumber++;
			}

			file.close();
		}
	}
	catch (const ifstream::failure& e) {
		return false;
	}
}

bool CProblem::bLoad(string sSourcePath) {
	if (!bCountFilesClause(sSourcePath)) return false();

	ifstream file;
	iNumberOvVariables = 0;
	pbSign = new bool* [iClauseNumber];
	piNumber = new int* [iClauseNumber];
	string sWord;

	try {
		file.open(sSourcePath + ".txt");

		if (file.is_open()) {
			for (int i = 0; i < iClauseNumber; i++) {
				file >> sWord;

				pbSign[i] = new bool[3];
				piNumber[i] = new int[3];

				for (int j = 0; j < iClauseSize; j++) {
					file >> sWord;

					if (sWord.at(0) == '-') {
						pbSign[i][j] = false;
						piNumber[i][j] = -stoi(sWord);
					}
					else {
						pbSign[i][j] = true;
						piNumber[i][j] = stoi(sWord);
					}

					if (piNumber[i][j] > iNumberOvVariables) iNumberOvVariables = piNumber[i][j];
				}
				file >> sWord;
			}
			file.close();
			iNumberOvVariables++;

			/*for (int i = 0; i < iClauseNumber; i++) {
				for (int j = 0; j < iClauseSize; j++) {
					if (!pbSign[i][j]) cout << "-";
					cout << piNumber[i][j] << " ";
				}
				cout << endl;
			}*/
			return true;
		}
		return false;
	}
	catch (const ifstream::failure& e)
	{
		return false;
	}
}

int CProblem::iGetNumberOvVariables() { return iNumberOvVariables; }

int CProblem::iGetClauseNumber() { return iClauseNumber; }

int CProblem::iCheck(bool* pbGenotype) {
	int iClauseMatched = 0;
	for (int i = 0; i < iClauseNumber; i++) {
		bool bMatched = false;
		for (int j = 0; j < iClauseSize && !bMatched; j++) {
			bMatched = (pbGenotype[piNumber[i][j]] == pbSign[i][j]);
		}
		iClauseMatched += bMatched;
	}
	return iClauseMatched;
}


bool CProblem::bAnalizeThisGene(bool* pbGenotype, int iGeneAnalizedIndex) {
	int iFulfilledCounter = 0;
	int iUnfulfilledCounter = 0;
	int iFulfilledGenesInFulfilled = 0;
	int iFulfilledGenesInUnfulfilled = 0;

	for (int i = 0; i < iClauseNumber; i++) {
		bool bContain = false;
		//bool bFulfilled = false;
		for (int j = 0; j < iClauseSize && !bContain; j++) {
			if (piNumber[i][j] == iGeneAnalizedIndex) {
				bContain = true;
				if (pbSign[i][j] == pbGenotype[iGeneAnalizedIndex]) {
					for (int k = 0; k < iClauseSize; k++) {
						if (pbSign[i][j] == pbGenotype[piNumber[i][j]]) {
							iFulfilledGenesInFulfilled++;
						}
					}
					iFulfilledGenesInFulfilled--;
					iFulfilledCounter++;
					//cout << "a";
				}
				else {
					for (int k = 0; k < iClauseSize; k++) {
						if (pbSign[i][j] == pbGenotype[piNumber[i][j]]) {
							iFulfilledGenesInUnfulfilled++;
						}
					}
					iUnfulfilledCounter++;
					//cout << "b";
				}
			}
		}
	}

	return ((iFulfilledGenesInFulfilled * iUnfulfilledCounter) > (iFulfilledGenesInUnfulfilled * iFulfilledCounter));
}
