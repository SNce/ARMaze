#include"matrix.h"

void mult(float lhs[3],double trans_mat[3][4],float rhs[4]) 
{
	float sum;
	for(int i = 0; i < 3;i++)
	{
		sum = 0;
		for(int j = 0; j < 4;j++)
		{
			sum = sum + (trans_mat[i][j] * rhs[j]);
		}
		lhs[i] = sum;
	}
}