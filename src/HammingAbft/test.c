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
	init_matrix(vector, 8, 1, 1);
	vector[4] = 3; 
	vector[5] = 3; 
	vector[6] = 3;
	vector[7] = 13;
	
	vector[0] = 2;
	vector[1] = 2;
	int cor_success = 1;
	cor_success = hmc_err_cor_matrix_int_parityck(vector, 8, 1);
	if (id == 0)
	{
		for (int i = 0; i < 7; i++)
		{
			printf("%d ", vector[i]);
		}
		printf("\n");
	}
	printf("cor_sucess is %d\n", cor_success);
	
	MPI_Finalize();
	return 0;
}
