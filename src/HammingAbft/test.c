#include<stdio.h>
#include<mpi.h>
#include<string.h>
#include<math.h>
#include<stdlib.h>
#include"../Common/func.h"
int main(int argc, char** argv)
{
	MPI_Init(&argc, &argv);
	int a = 4;
	a = hmc_matrix_col(a);
	printf("%d\n", a);
	MPI_Finalize();
	return 0;
}
