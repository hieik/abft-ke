#include<stdio.h>
#include<mpi.h>
#include<string.h>
#include<math.h>
#include<stdlib.h>

#define BER 1e-12

void bit_flop_int(int* integer, int message_size, double ber)
{
	srand((unsigned)(time(NULL)));
	long scope = (long)(1 / ber) * 10;
	long bit_num = message_size * sizeof(int) * 8;
	long p_num = 0;
	int err_count = 0;
	int bit_err_loc = 0;
	int msg_index = 0;
	for (long i = 0; i < bit_num; i++)
	{
		p_num = rand() % scope;
		if (p_num < 10)
		{
			msg_index = i / (sizeof(int) * 8);
			bit_err_loc = sizeof(int) * 8 - (i - msg_index * sizeof(int) * 8) - 1;
			err_count++;
			integer[msg_index] ^= 1 << (bit_err_loc);
		//	printf("error count is %d, msg_index is %d, bit_err_loc is %d\n", err_count, msg_index, bit_err_loc);
		}
	}
}

void bit_flop_double(double* double_float, int message_size, double ber)
{
	srand((unsigned)(time(NULL)));
	long scope = (long)(1 / ber) * 10;
	long bit_num = message_size * sizeof(double) * 8;
	long p_num = 0;
	int err_count = 0;
	int bit_err_loc = 0;
	int msg_index = 0;
	int* integer = (int*)double_float;
	for (long i = 0; i < bit_num; i++)
	{
		p_num = rand() % scope;
		if (p_num < 10)
		{
			msg_index = i / (sizeof(int) * 8);
			bit_err_loc = sizeof(int) * 8 - (i - msg_index * sizeof(int) * 8) - 1;
			err_count++;
			integer[msg_index] ^= 1 << (bit_err_loc);
//			printf("error count is %d, msg_int_index is %d, msg_double_index is %d, bit_err_loc_int is %d, bit_error_loc_double is %d\n", err_count, msg_index, msg_index / 2, bit_err_loc, msg_index % 2 == 0 ? bit_err_loc + sizeof(int) * 8 : bit_err_loc);
		}
	}
}

int hmc_lookup_table(int cs1, int cs2, int cs3)
{
		/*
		   hamming code generator matrix
		   1000110
		   0100101
		   0010011
		   0001111
		   */

	int err_loc = 0;

	if (cs1 == 0 && cs2 == 0 && cs3 == 0)
	{
		err_loc = 0;
	} else if (cs1 == 0 && cs2 == 0 && cs3 != 0)
	{
		err_loc = 7;
	} else if (cs1 == 0 && cs2 != 0 && cs3 == 0)
	{
		err_loc = 6;
	} else if (cs1 != 0 && cs2 == 0 && cs3 == 0)
	{
		err_loc = 5;
	} else if (cs1 == 0 && cs2 != 0 && cs3 != 0)
	{
		err_loc = 3;
	} else if (cs1 != 0 && cs2 == 0 && cs3 != 0)
	{
		err_loc = 2;
	} else if (cs1 != 0 && cs2 != 0 && cs3 == 0)
	{
		err_loc = 1;
	} else 
	{
		err_loc = 4;
	}
	return err_loc;
}

int hmc_err_loc_int(int* hmc_vector)
{
	int cs1 = 0, cs2 = 0, cs3 = 0;
	int err_loc = 0;
	cs1 = hmc_vector[0] + hmc_vector[1] + hmc_vector[3] - hmc_vector[4];	
	cs2 = hmc_vector[0] + hmc_vector[2] + hmc_vector[3] - hmc_vector[5];	
	cs3 = hmc_vector[1] + hmc_vector[2] + hmc_vector[3] - hmc_vector[6];	
	err_loc = hmc_lookup_table(cs1, cs2, cs3);
	
	return err_loc;
}
int hmc_err_cor_double()
{

}

int* hamming_checksum_matrix_translation(int* matrix, int row, int col)
{
	int hmc_row, hmc_col;
	hmc_col = (col + 3) / 4 * 7;
	hmc_row = row; 
	
	int* hamming_checksum_matrix = (int*)malloc(sizeof(int) * hmc_row * hmc_col);

	for (int i = 0; i < hmc_row; i++)
	{
		for (int j = 0; j < hmc_col; j++)
		{
			if (j % 7 < 4)
			{
				hamming_checksum_matrix[i * hmc_col + j] = matrix[i * col + (j / 7 * 4 + j % 7)];
			} else {
				switch (j % 7) 
				{
					case 4 : hamming_checksum_matrix[i * hmc_col + j] = hamming_checksum_matrix[i * hmc_col + j - 4] + hamming_checksum_matrix[i * hmc_col + j - 3] + hamming_checksum_matrix[i * hmc_col + j - 1]; break; 
					case 5 : hamming_checksum_matrix[i * hmc_col + j] = hamming_checksum_matrix[i * hmc_col + j - 5] + hamming_checksum_matrix[i * hmc_col + j - 2] + hamming_checksum_matrix[i * hmc_col + j - 3]; break; 
					case 6 : hamming_checksum_matrix[i * hmc_col + j] = hamming_checksum_matrix[i * hmc_col + j - 5] + hamming_checksum_matrix[i * hmc_col + j - 4] + hamming_checksum_matrix[i * hmc_col + j - 3]; break; 
				}
			}
		}
	}
	return hamming_checksum_matrix;
}

void init_matrix(int* matrix, int matrix_size_row, int matrix_size_col, int init_value)
{
	for (int i = 0; i < matrix_size_row; i++)
	{
		for (int j = 0; j < matrix_size_col; j++)
		{
			matrix[i * matrix_size_col + j] = init_value;
		}
	}
}

void print_matrix(int* matrix, int row, int col)
{
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			printf("%d ", matrix[i * col + j]);
		}
		printf("\n");
	}
}






int main(int argc, char** argv)
{

	long long N = 4096;
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
	int *a_col_checksum, *a_row_checksum;
	int *b_col_checksum, *b_row_checksum;
	int *c_col_checksum, *c_row_checksum;

		
	int sub_a_checksum = 0, sub_b_checksum = 0, sub_c_checksum = 0;


	struct timeval current_time;

	double time, computation_time = 0, communication_time = 0;
	double start_time, end_time, start_intern_time, end_intern_time;
	double err_time_start, err_time_end, err_time;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id_proc);
	MPI_Comm_size(MPI_COMM_WORLD, &num_proc);
	MPI_Status status;	

	int err_loc = 0;

	if (id_proc == 1)
	{
		//	err_loc = hmc_err_loc_int(hamming_checksum_vector);
	}

	int row = 3, col = 12;
	int hmc_row, hmc_col;
	hmc_col = (col + 3) / 4 * 7;
	hmc_row = row;
	int* matrix_test = (int*)malloc(sizeof(int) * row * col);
	int* matrix_hm_test;


	init_matrix(matrix_test, row, col, 1);
	//print_matrix(matrix_test, 8, 3);
	matrix_hm_test = hamming_checksum_matrix_translation(matrix_test, row, col);
	print_matrix(matrix_test, row, col);
	print_matrix(matrix_hm_test, hmc_row, hmc_col);
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
