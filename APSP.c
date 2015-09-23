#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>

#include "MatUtil.h"

int min(int a, int b){
	return (a > b)? b:a;
}

int update(int a, int b1, int b2){
	if(a != -1 && b1 != -1 && b2 != -1)
		return min(a, b1+b2);
	else if(b1 != -1 && b2 != -1)
		return b1 + b2;
	else
		return a;
}

void printmat(int *mat, int N){
	int i, j;
	for(i = 0; i < N; i++){
		for(j = 0; j < N; j++){
			printf("%d ", *(mat+i*N+j));
		}
		printf("\n");
	}
}

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		printf("Usage: test {N}\n");
		exit(-1);
	}

	size_t N = atoi(argv[1]);
	// matrix related variables
	int *mat, *ref, *result, *part, *k_row;
	int rows, k, root;
	int i, j, vij, vik, vkj;
	int npes, rank;
	struct timeval tv1, tv2;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &npes);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if(rank == 0){
		// generate matrix and compute sequential
		mat = (int*)malloc(sizeof(int)*N*N);
		ref = (int*)malloc(sizeof(int)*N*N);
		result = (int*)malloc(sizeof(int)*N*N);
		GenMatrix(mat, N);
		memcpy(ref, mat, sizeof(int)*N*N);
		gettimeofday(&tv1, NULL);
		ST_APSP(ref, N);
		gettimeofday(&tv2, NULL);
		printf("Sequential: %ld usecs\n",(tv2.tv_sec-tv1.tv_sec)*1000000+tv2.tv_usec-tv1.tv_usec);
		gettimeofday(&tv1, NULL);
	}

	// scatter the matrix
	rows = N/npes;
	part = (int*)malloc(sizeof(int)*N*rows);
	MPI_Scatter(mat, N*rows, MPI_INT, part, N*rows, MPI_INT, 0, MPI_COMM_WORLD);

	// parallel computing
	k_row = (int*)malloc(sizeof(int)*N);
	for(k = 0; k < N; k++){
		root = k/rows;
		if(rank == root){
			for(i = 0; i < N; i ++){
				*(k_row+i) = *(part + N*(k-rows*root) + i);
			}
		}
		MPI_Bcast(k_row, N, MPI_INT, root, MPI_COMM_WORLD);
		for(i = 0; i < rows; i++){
			for(j = 0; j < N; j++){
				vij = *(part + N*i + j);
				vik = *(part + N*i + k);
				vkj = *(k_row + j);
				if(vik != -1 && vkj != -1){
					if(vij == -1 || vij > vik+vkj)
					*(part + N*i + j) = vik + vkj;	
				}
			}
		}
	}
	
	// gather the matrix
	MPI_Gather(part, N*rows, MPI_INT, result, N*rows, MPI_INT, 0, MPI_COMM_WORLD);

	//compare your result with reference result
	if(rank == 0){
		gettimeofday(&tv2, NULL);
		printf("Parallel: %ld usecs\n",(tv2.tv_sec-tv1.tv_sec)*1000000+tv2.tv_usec-tv1.tv_usec);
//		printmat(mat, N);
//		printmat(ref, N);
//		printmat(result, N);
		if(CmpArray(result, ref, N*N))
			printf("Your result is correct.\n");
		else
			printf("Your result is wrong.\n");
	}

	// free memory
	if(rank == 0){
		free(mat);
		free(ref);
		free(result);
	}
	free(part);
	free(k_row);
	MPI_Finalize();
}
