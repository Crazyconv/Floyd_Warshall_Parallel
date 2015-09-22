#include "MatUtil.h"

void GenMatrix(int *mat, const size_t N)
{
	int i;
	for(i = 0; i < N*N; i ++)
		mat[i] = rand()%32 - 1;

}

bool CmpArray(const int *l, const int *r, const size_t eleNum)
{
	int i;
	for(i = 0; i < eleNum; i ++)
		if(l[i] != r[i])
		{
			printf("ERROR: l[%d] = %d, r[%d] = %d\n", i, l[i], i, r[i]);
			return false;
		}
	return true;
}


/*
	Sequential (Single Thread) APSP on CPU.
*/
void ST_APSP(int *mat, const size_t N)
{
	int k, i, j;
	for(k = 0; k < N; k ++)
		for(i = 0; i < N; i ++)
			for(j = 0; j < N; j ++)
			{
				int i0 = i*N + j;
				int i1 = i*N + k;
				int i2 = k*N + j;
				if(mat[i1] != -1 && mat[i2] != -1)
                     { 
			          int sum =  (mat[i1] + mat[i2]);
                          if (mat[i0] == -1 || sum < mat[i0])
 					     mat[i0] = sum;
				}
			}
}


