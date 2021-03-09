#include<stdio.h>
#include<mpi.h>
#include<string.h>
#include<math.h>
#include<stdlib.h>
#include"../Common/func.h"
int main(int argc, char** argv)
{
	MPI_Init(&argc, &argv);
	int id = 0; 
	int loc = 0;
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	int* vector = (int*)malloc(sizeof(int) * 7);	
	int root = 0;
	init_matrix(vector, 7, 1, 1);
	vector[4] = 3; 
	vector[5] = 3; 
	vector[6] = 3; 
	
	vector[0] = 2;
	vector[1] = 2;
	if (id == 0)
	{
		for (int i = 0; i < 7; i++)
		{
			printf("%d ", vector[i]);
		}
		printf("\n");
	}
	loc = hmc_err_loc_int(vector);
	hmc_err_cor_vector_int(vector, loc);

	if (id == 0)
	{
		printf("loc %d\n", loc);
		for (int i = 0; i < 7; i++)
		{
			printf("%d ", vector[i]);
		}
		printf("\n");
	}

	loc = hmc_err_loc_int(vector);
	hmc_err_cor_vector_int(vector, loc);

	if (id == 0)
	{
		printf("loc %d\n", loc);
		for (int i = 0; i < 7; i++)
		{
			printf("%d ", vector[i]);
		}
		printf("\n");
	}

	MPI_Finalize();
	return 0;
}
