#include<stdio.h>
#include<mpi.h>
#include<string.h>
#include<math.h>
#include<stdlib.h>
#include"../Common/func.h"
#define BER 1e-5
#define N 4096 // N is matrix size


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

	//SUMMA
	int row_color;
	int col_color;
	num_row = num_col = (int)sqrt((double)num_proc);
	block_size_row = N / num_row;
	block_size_col = N / num_col;
	block_size = block_size_row * block_size_col;

	sub_a_hc_row = block_size_row;
	sub_a_hc_col = (N + 3) / 4 * 7;
	sub_b_hc_row = N;
	sub_b_hc_col = (block_size_col + 3) / 4 * 7;
	//hmc_col = (col + 3) / 4 * 7;
	//hmc_row = row;

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

	if (p_j == 0)
	{
		memcpy(sub_a, a + p_i * block_size_row * N, block_size_row * N * sizeof(int));
	} 
	
	if (p_i == 0)
	{
		memcpy(sub_b, b + p_j * block_size_col * N, block_size_col * N * sizeof(int));
	}
	
	sub_a_hc = hamming_checksum_matrix_translation(sub_a, block_size_row, N);
	sub_b_hc = hamming_checksum_matrix_translation(sub_b, N, block_size_col);

	if (id_proc == root)
	{
		//print_matrix(sub_a_hc, sub_a_hc_row, sub_a_hc_col);
		//print_matrix(sub_b_hc, sub_b_hc_row, sub_b_hc_col);
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
	hmc_err_cor_matrix_int(sub_a_hc, sub_a_hc_row, sub_a_hc_col);
	//map hamming checksum matrix to matrix
	hamming_matrix_map_matrix(sub_a_hc, sub_a_hc_row, sub_a_hc_col, sub_a, block_size_row, N);	
	hamming_matrix_map_matrix(sub_b_hc, sub_b_hc_row, sub_b_hc_col, sub_b, N, block_size_col);	
	
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

	if (id_proc == 5)
	{
	//	print_matrix(sub_a_hc, sub_a_hc_row, sub_a_hc_col);
	//	print_matrix(sub_b_hc, sub_b_hc_row, sub_b_hc_col);
//		print_matrix(sub_a, block_size_row, N);
	//	print_matrix(sub_c, block_size_row, block_size_col);
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

	//init_matrix(matrix_test, row, col, 1);
	//print_matrix(matrix_test, row, col);
	//matrix_hm_test = hamming_checksum_matrix_translation(matrix_test, row, col);
	//print_matrix(matrix_test, row, col);
	//print_matrix(matrix_hm_test, hmc_row, hmc_col);
	//set random seed
	/*	gettimeofday(&current_time, NULL);
		srand((current_time.tv_sec * 1000) + (current_time.tv_usec / 1000));

		int* corrected_array = (int*)malloc(sizeof(int) * num_proc);
		int msg_is_corrected = 0;

		int send_recv_tag = 1;
		MPI_Comm row_comm;
		MPI_Comm col_comm;
		int row_color;
		int col_color;


		num_row = num_col = (int)sqrt((double)num_proc);

		block_size_row = N / num_row;
		block_size_col = N / num_col;

		block_size = block_size_row * block_size_col;

		if (num_row * num_col != num_proc)
		{
			return 0;
		}

		p_i = id_proc / num_col;
		p_j = id_proc - p_i * num_col;
	//	printf("%d, %d, %d\n", id_proc, p_i, p_j);
		//if (id_proc == root)
		//{
		MPI_Barrier(MPI_COMM_WORLD);
		start_time = MPI_Wtime();
		a = (int*)malloc(sizeof(int) * N * N);
		b = (int*)malloc(sizeof(int) * N * N);
		for(int i = 0; i < N; i++)
		{
			for(int j = 0; j < N; j++)
			{
					a[i * N + j] = 1;
					b[i * N + j] = 1;
			}
		}

		//}

		if (id_proc == root)
		{
			printf("Matrix size is %lld\n", N * N);
			printf("num_row is %d\n", num_row);
			printf("num_col is %d\n", num_col);
			printf("block_size_row is %d\n", block_size_row);
			printf("block_size_col is %d\n", block_size_col);
		}

		sub_a = (int*)malloc(sizeof(int) * block_size_row * N);
		sub_a_col_checksum = (int*)malloc(sizeof(int) * 1 * block_size_row);
		sub_a_row_checksum = (int*)malloc(sizeof(int) * 1 * N);


		for(int i = 0; i < block_size_row; i++)
		{
			for(int j = 0; j < N; j++)
			{
				sub_a[i * N + j] = 0;
			}
		}
		

		sub_b = (int*)malloc(sizeof(int) * N * block_size_col);
		sub_b_col_checksum = (int*)malloc(sizeof(int) * 1 * N);
		sub_b_row_checksum = (int*)malloc(sizeof(int) * 1 * block_size_col);

		for(int i = 0; i < block_size_col; i++)
		{
			for(int j = 0; j < N; j++)
   		{
   			sub_b[i * N + j] = 0;
   		}
   	}

	sub_c = (int*)malloc(sizeof(int) * block_size_row * block_size_col);
	sub_c_col_checksum = (int*)malloc(sizeof(int) * 1 * block_size_row);
	sub_c_row_checksum = (int*)malloc(sizeof(int) * 1 * block_size_col);

	for(int i = 0; i < block_size_row; i++)
   	{
   		for(int j = 0; j < block_size_col; j++)
   		{
   			sub_c[i * block_size_row + j] = 0;
   		}
   	}


	if (p_j == 0)
	{
//		printf("%d %d\n", p_i, p_j);
		memcpy(sub_a, a + p_i * block_size_row * N, block_size_row * N * sizeof(int));
		calc_checksum(sub_a, block_size_row, N, sub_a_row_checksum, sub_a_col_checksum, &sub_a_checksum);
	
		//print_checksum(sub_a_row_checksum, block_size_row, sub_a_col_checksum, N, &sub_a_checksum);
		//print_matrix(sub_a, block_size_row, N);
		//printf("\n");
	}
	
	if (p_i == 0)
	{
		memcpy(sub_b, b + p_j * block_size_col * N, block_size_col * N * sizeof(int));
	//	calc_checksum(sub_b, N, block_size_col, sub_b_col_checksum, sub_b_row_checksum, &sub_b_checksum);
	//	print_checksum(sub_b_row_checksum, N, sub_a_col_checksum, block_size_col, &sub_a_checksum);
	//print_matrix(sub_b, block_size_col , N);
//		printf("\n");
	}
	end_time = MPI_Wtime();
	time = end_time - start_time;
	MPI_Barrier(MPI_COMM_WORLD);
	if (id_proc == root)
	{
		printf("init use time %f\n", time);
	}

//	Broadcast operation 

	row_color = id_proc / num_row;
	col_color = id_proc % num_col;
	MPI_Comm_split(MPI_COMM_WORLD, row_color, id_proc, &row_comm);
	MPI_Comm_split(MPI_COMM_WORLD, col_color, id_proc, &col_comm);

	MPI_Barrier(MPI_COMM_WORLD);

	start_time = MPI_Wtime();
	start_intern_time = MPI_Wtime();
	MPI_Bcast(sub_a, block_size_row * N, MPI_INT, 0, row_comm);
	MPI_Bcast(sub_a_row_checksum, N, MPI_INT, 0, row_comm);
	MPI_Bcast(sub_a_col_checksum, block_size_row, MPI_INT, 0, row_comm);
	MPI_Bcast(&sub_a_checksum, 1, MPI_INT, 0, row_comm);

	MPI_Bcast(sub_b, block_size_col * N, MPI_INT, 0, col_comm);
	if (p_j != 0 && p_i != 0)
	{
		bit_flop_int(sub_a, block_size_row * N, BER);
	}
	msg_is_corrected = abft(sub_a, block_size_row, N, sub_a_row_checksum, sub_a_col_checksum, &sub_a_checksum); 
	MPI_Allgather(&msg_is_corrected, 1, MPI_INT, corrected_array, 1, MPI_INT, MPI_COMM_WORLD);

//	printf("%d\n", all_ele_true(corrected_array, num_proc));
	int count = 0;
	while(!all_ele_true(corrected_array, num_proc))
	{
		for (int i = 0; i < num_proc; i++)
		{
			if (corrected_array[i] == 0)
			{
				if (id_proc == i)
				{
					//MPI_Recv
					MPI_Recv(sub_a, block_size_row * N, MPI_INT, i / num_row * num_row, send_recv_tag, MPI_COMM_WORLD, &status);
					//printf("%d recev meeeage from %d\n", i, i / num_row * num_row);
				}
				if (id_proc == i / num_row * num_row)
				{
					MPI_Send(sub_a, block_size_row * N, MPI_INT, i, send_recv_tag, MPI_COMM_WORLD );
					//printf("%d send meeeage to %d\n", id_proc, i);
				}
			}
		}
		if (p_j != 0 && p_i != 0)
		{
			bit_flop_int(sub_a, block_size_row * N, BER);
		}
	//	msg_is_corrected = abft_diag(sub_a, block_size_row, N, sub_a_row_checksum, sub_a_col_checksum, &sub_a_checksum, sub_a_left_checksum, sub_a_right_checksum); 
		msg_is_corrected = abft(sub_a, block_size_row, N, sub_a_row_checksum, sub_a_col_checksum, &sub_a_checksum); 
		MPI_Allgather(&msg_is_corrected, 1, MPI_INT, corrected_array, 1, MPI_INT, MPI_COMM_WORLD);
//		break;
		count++;
		if (id_proc == 0)
		{
//			printf("count %d\n", count);
		}

	}
	
//	print_matrix(sub_a, block_size_row, N);
//	print_checksum(sub_a_row_checksum, block_size_row, sub_a_col_checksum, N, &sub_a_checksum);
//	printf("\n");
	end_intern_time = MPI_Wtime();
//
	MPI_Barrier(MPI_COMM_WORLD);
	communication_time = end_intern_time - start_intern_time;
	MPI_Barrier(MPI_COMM_WORLD);
	start_intern_time = MPI_Wtime();
	//printf("%d\n", block_size);
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
//
	MPI_Barrier(MPI_COMM_WORLD);
	computation_time = end_intern_time - start_intern_time;
//	
	MPI_Barrier(MPI_COMM_WORLD);
	end_time = MPI_Wtime();
	time = end_time - start_time;	

	MPI_Barrier(MPI_COMM_WORLD);
	if (id_proc == root)
	{
		printf("id %d ,communicaton_time: %f, computation_time: %f, MM time: %f\n", id_proc, communication_time, computation_time, time);
	}

	if (id_proc == 0)
	{
		for (int i = 0; i < block_size_row; i++)
		{
			for (int j = 0; j < block_size_col; j++)
			{
//				printf("%d\n", sub_c[i * block_size_row + j]);
			}
		}

//		printf("count is %d\n", count);
	}

	MPI_Barrier(MPI_COMM_WORLD);
*/
	MPI_Finalize();
	return 0;
}
