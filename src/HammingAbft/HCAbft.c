#include<stdio.h>
#include<mpi.h>
#include<string.h>
#include<math.h>
#include<stdlib.h>
#include"../Common/func.h"
#define BER 4e-7
#define N 1024 // N is matrix size


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
	int sub_a_hc_row, sub_a_hc_col, sub_b_hc_row, sub_b_hc_col;
	int sub_a_row, sub_a_col, sub_b_row, sub_b_col;

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

	gettimeofday(&current_time, NULL);
	srand((current_time.tv_sec * 1000) + (current_time.tv_usec / 1000));
	int send_recv_tag = 1;
	MPI_Comm row_comm;
	MPI_Comm col_comm;

  int* sub_a_hc_corrected_array = (int*)malloc(sizeof(int) * num_proc);
  int* sub_b_hc_corrected_array = (int*)malloc(sizeof(int) * num_proc);
  int sub_a_hc_is_corrected = 0;
  int sub_b_hc_is_corrected = 0;

	//SUMMA
	int row_color;
	int col_color;
	num_row = num_col = (int)sqrt((double)num_proc);
	block_size_row = N / num_row;
	block_size_col = N / num_col;
	block_size = block_size_row * block_size_col;

	sub_a_row = block_size_row;
	sub_a_col = N;
	sub_b_row = N;
	sub_b_row = block_size_col;
	sub_a_hc_row = hmc_matrix_row_parity_check(block_size_row); 
	sub_a_hc_col = hmc_matrix_col_parity_check(N);
	sub_b_hc_row = hmc_matrix_row_parity_check(N);
	sub_b_hc_col = hmc_matrix_col_parity_check(block_size_col); 

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
	if (id_proc == root)
	{
		a = (int*)malloc(sizeof(int) * N * N);
		b = (int*)malloc(sizeof(int) * N * N);
		//matrix initialization
		init_matrix(a, N, N, 1);
		init_matrix(b, N, N, 1);
	}

	//create sub matrix for processor
	sub_a = (int*)malloc(sizeof(int) * block_size_row * N);
	sub_b = (int*)malloc(sizeof(int) * N * block_size_col);
	sub_c = (int*)malloc(sizeof(int) * block_size_row * block_size_col);
	//sub matrix initialzation
	init_matrix(sub_a, block_size_row, N, 0);
	init_matrix(sub_b, N, block_size_col, 0);
	init_matrix(sub_c, block_size_row, block_size_col, 0);
	//create sub hamming checksum matrix
	sub_a_hc = create_hamming_checksum_matrix_parityck(sub_a, block_size_row, N);
	sub_b_hc = create_hamming_checksum_matrix_parityck(sub_b, N, block_size_col);

	//distrubute submatrix, use MPI_Send and MPI_Recv
	if(id_proc == root)
	{
		for (int i = 1; i < num_row; i++)
		{
			memcpy(sub_a, a + i * block_size_row * N, block_size_row * N * sizeof(int));
			hamming_checksum_matrix_translation_parityck(sub_a, block_size_row, N, sub_a_hc);
			MPI_Send(sub_a_hc, sub_a_hc_row * sub_a_hc_col, MPI_INT, i * num_col, 1, MPI_COMM_WORLD);	
		}
		for (int j = 1; j < num_col; j++)
		{
			memcpy(sub_b, b + j * block_size_col * N, block_size_col * N * sizeof(int));
			hamming_checksum_matrix_translation_parityck(sub_b, N, block_size_col, sub_b_hc);
			MPI_Send(sub_b_hc, sub_b_hc_row * sub_b_hc_col, MPI_INT, j, 1, MPI_COMM_WORLD);
		} 
		memcpy(sub_a, a, block_size_row * N * sizeof(int));
		memcpy(sub_b, b, block_size_col * N * sizeof(int));
		hamming_checksum_matrix_translation_parityck(sub_a, block_size_row, N, sub_a_hc);
		hamming_checksum_matrix_translation_parityck(sub_b, N, block_size_col, sub_b_hc);
	}
	if (p_j == 0 && id_proc != root)
	{
		MPI_Recv(sub_a_hc, sub_a_hc_row * sub_a_hc_col, MPI_INT, root, 1, MPI_COMM_WORLD, &status);
		//Error Correction
	}
	if (p_i == 0 && id_proc != root)
	{
		MPI_Recv(sub_b_hc, sub_b_hc_row * sub_b_hc_col, MPI_INT, root, 1, MPI_COMM_WORLD, &status);
		//Error Correction
	}

	if (id_proc == root)
	{
	//	printf("sub_a_row = %d, sub_a_col = %d\n", sub_a_row, sub_a_col);
	//	printf("sub_a_hc_row = %d, sub_a_hc_col = %d\n", sub_a_hc_row, sub_a_hc_col);
	//	print_matrix(sub_a, sub_a_row, sub_a_col);
		//	print_matrix(sub_a_hc, sub_a_hc_row, sub_a_hc_col);
//		print_matrix(sub_b_hc, sub_b_hc_row, sub_b_hc_col);
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
	MPI_Bcast(sub_a_hc, sub_a_hc_row * sub_a_hc_col, MPI_INT, 0, row_comm);
	MPI_Bcast(sub_b_hc, sub_b_hc_row * sub_b_hc_col, MPI_INT, 0, col_comm);
	//Error Injection
	if (p_j != 0 && p_i != 0)
	{
		bit_flop_int(sub_a_hc, sub_a_hc_row * sub_a_hc_col, BER);
	//	bit_flop_int(sub_b_hc, sub_b_hc_row * sub_b_hc_col, BER);
	}	

	//Error Correction
	sub_a_hc_is_corrected = hmc_err_cor_matrix_int_parityck(sub_a_hc, sub_a_hc_row, sub_a_hc_col);
	//hmc_err_cor_matrix_int_parityck(sub_b_hc, sub_b_hc_row, sub_b_hc_col);

	MPI_Allgather(&sub_a_hc_is_corrected, 1, MPI_INT, sub_a_hc_corrected_array, 1, MPI_INT, MPI_COMM_WORLD);
	while (!all_ele_true(sub_a_hc_corrected_array, num_proc))
	{
		for (int i = 0; i < num_proc; i++)
		{
			if (sub_a_hc_corrected_array[i] == 0)
			{
				if (id_proc == i)
				{
					MPI_Recv(sub_a_hc, sub_a_hc_row * sub_a_hc_col, MPI_INT, i / num_row * num_row, 1, MPI_COMM_WORLD, &status);
//					printf("recv node id : %d\n", i / num_row * num_row);
				}
				if (id_proc == i / num_row * num_row)
				{
					MPI_Send(sub_a_hc, sub_a_hc_row * sub_a_hc_col, MPI_INT, i, 1, MPI_COMM_WORLD);	
//					printf("send node id : %d\n", i);
				}
			}
		}
		if (p_j != 0 && p_i != 0)
		{
			bit_flop_int(sub_a_hc, sub_a_hc_row * sub_a_hc_col, BER);
		}
		sub_a_hc_is_corrected = hmc_err_cor_matrix_int_parityck(sub_a_hc, sub_a_hc_row, sub_a_hc_col);
		MPI_Allgather(&sub_a_hc_is_corrected, 1, MPI_INT, sub_a_hc_corrected_array, 1, MPI_INT, MPI_COMM_WORLD);
//		print_matrix(sub_a_hc_corrected_array, 1, num_proc);
//		print_matrix(sub_a_hc, sub_a_hc_row, sub_a_hc_col);
	}
	
	//map hamming checksum matrix to matrix
	hamming_matrix_map_matrix_parityck(sub_a_hc, sub_a_hc_row, sub_a_hc_col, sub_a, block_size_row, N);	
	hamming_matrix_map_matrix_parityck(sub_b_hc, sub_b_hc_row, sub_b_hc_col, sub_b, N, block_size_col);	
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

	if (id_proc == 15)
	{
//		print_matrix(sub_a_hc, sub_a_hc_row, sub_a_hc_col);
//		print_matrix(sub_b_hc, sub_b_hc_row, sub_b_hc_col);
		//print_matrix(sub_a, block_size_row, N);
//		print_matrix(sub_c, block_size_row, block_size_col);
//		print_matrix(sub_a_hc_corrected_array, 1, num_proc);
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
		printf("id %d ,communicaton_time: %f, computation_time: %f, MM time: %f\n", id_proc, communication_time, computation_time, time);
	}

	MPI_Finalize();
	return 0;
}
