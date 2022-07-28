#pragma once

#include <string>
#include <vector>
#include <random>

#define iClauseSize 3

using namespace std;


class CProblem
{
public:
	CProblem();

	virtual ~CProblem();

	bool  bLoad(string sSourcePath);

	bool bCountFilesClause(string sSourcePath);

	bool b_dealloc_table_2_dim_int(int*** pi_table, int i_size_x, int i_size_y);
	bool b_dealloc_table_2_dim_bool(bool*** pb_table, int i_size_x, int i_size_y);

	int iGetNumberOvVariables();

	int iGetClauseNumber();

	int iCheck(bool* pbGenotype);

	bool bAnalizeThisGene(bool* pbGenotype, int iGeneAnalizedIndex);

private:
	int iNumberOvVariables;
	int** piNumber;
	bool** pbSign;
	int iClauseNumber;
};
