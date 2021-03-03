#include<stdio.h>
#include<mpi.h>
#include<string.h>
#include<math.h>
#include<stdlib.h>
#include"../Common/func.h"
#define BER 1e-12
#define N 64 // N is matrix size


int main(int argc, char** argv)
{
	
	int id_proc;
	int num_proc;
	int root = 0;
	int block_size = 0;
	int block_size_row = 0;
	int block_size_col = 0;
	int num_row, num_col;
	int p_i, p_j; //process index
	long long matrix_size = N * N;

	int *a, *b, *c;
	int *sub_a, *sub_b, *sub_c;
	int *sub_a_hc, *sub_b_hc;//hamming checksum sub matrix

	struct timeval current_time;

	double time, computation_time = 0, communication_time = 0;
	double start_time, end_time, start_intern_time, end_intern_time;
	double err_time_start, err_time_end, err_time;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id_proc);
	MPI_Comm_size(MPI_COMM_WORLD, &num_proc);
	MPI_Status status;	

	//check matrix size, # of cols should be mod by 4
	if (N % 4 != 0)
	{
		if (id_proc == root)
		{
			printf("matrix size should mod by 4\n");
		}
		return 0;
		MPI_Finalize();
	}

	//hmc_col = (col + 3) / 4 * 7;
	//hmc_row = row;

	gettimeofday(&current_time, NULL);
	srand((current_time.tv_sec * 1000) + (current_time.tv_usec / 1000));
	int send_recv_tag = 1;
	MPI_Comm row_comm;
	MPI_Comm col_comm;

	//SUMMA
	int row_color;
	int col_color;
	num_row = num_col = (int)sqrt((double)num_proc);
	block_size_row = N / num_row;
	block_size_col = N / num_col;
	block_size = block_size_row * block_size_col;
	
	if (block_size_row % 4 != 0 || block_size_col % 4 != 0)
	{
		if (id_proc == root)
		{
			printf("matrix size should mod by 4\n");
		}
		return 0;
		MPI_Finalize();
	}

	if (num_row * num_col != num_proc)
	{
		return 0;
	}

	p_i = id_proc / num_col;
	p_j = id_proc - p_i * num_col;

	MPI_Barrier(MPI_COMM_WORLD);

	//Initiliazton time
	start_time = MPI_Wtime();

	//create matrix
	a = (int*)malloc(sizeof(int) * N * N);
	b = (int*)malloc(sizeof(int) * N * N);
	//matrix initialization
	init_matrix(a, N, N, 1);
	init_matrix(b, N, N, 1);

	if (id_proc == root)
	{
	//	print_matrix(b, N, N);
	}

	//create sub matrix for processor
	sub_a = (int*)malloc(sizeof(int) * block_size_row * N);
	sub_b = (int*)malloc(sizeof(int) * N * block_size_col);
	sub_c = (int*)malloc(sizeof(int) * block_size_row * block_size_col);
	//sub matrix initialzation
	init_matrix(sub_a, block_size_row, N, 0);
	init_matrix(sub_b, N, block_size_col, 0);
	init_matrix(sub_c, block_size_row, block_size_col, 0);

	if (id_proc == root)
	{
	//	print_matrix(sub_a, block_size_row, N);
	}

	if (p_j == 0)
	{
		memcpy(sub_a, a + p_i * block_size_row * N, block_size_row * N * sizeof(int));
	}
	
	if (p_i == 0)
	{
		memcpy(sub_b, b + p_j * block_size_col * N, block_size_col * N * sizeof(int));
	}
	end_time = MPI_Wtime();
	//Initilization time
	time = end_time - start_time;
	MPI_Barrier(MPI_COMM_WORLD);
	if (id_proc == root)
	{
		printf("processor %d ,init use time %f\n", id_proc, time);
	}

//	Broadcast operation 
	row_color = id_proc / num_row;
	col_color = id_proc % num_col;
	MPI_Comm_split(MPI_COMM_WORLD, row_color, id_proc, &row_comm);
	MPI_Comm_split(MPI_COMM_WORLD, col_color, id_proc, &col_comm);

	MPI_Barrier(MPI_COMM_WORLD);
	//Communication time	
	start_time = MPI_Wtime();
	start_intern_time = MPI_Wtime();
	MPI_Bcast(sub_a, block_size_row * N, MPI_INT, 0, row_comm);
	MPI_Bcast(sub_b, block_size_col * N, MPI_INT, 0, col_comm);

	MPI_Barrier(MPI_COMM_WORLD);
	communication_time = end_intern_time - start_intern_time;
	MPI_Barrier(MPI_COMM_WORLD);
	//Computation time
	start_intern_time = MPI_Wtime();
	for (int i = 0; i < block_size_row; i++)
	{
		for (int j = 0; j < N; j++)
		{
			for(int k = 0; k < block_size_col; k++)
			{
				sub_c[i * block_size_row + k] += sub_a[i * block_size_row + j] * sub_b[k * block_size_col + j];
			}

		}
	}
	end_intern_time = MPI_Wtime();
	MPI_Barrier(MPI_COMM_WORLD);
	//Computation time
	computation_time = end_intern_time - start_intern_time;
	MPI_Barrier(MPI_COMM_WORLD);
	end_time = MPI_Wtime();
	//Matrix Multiplication time
	time = end_time - start_time;	

	if (id_proc == root)
	{
		print_matrix(sub_c, block_size_row, block_size_col);
	}

	if (id_proc == root)
	{
		printf("id %d ,communicaton_time: %f, computation_time: %f, MM time: %f\n", id_proc, communication_time, computation_time, time);
	}
	MPI_Finalize();
	return 0;
}
