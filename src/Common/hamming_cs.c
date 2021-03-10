#include<stdio.h>
#include<string.h>
#include<stdlib.h>

/*looup table
 e = a + b + d
 f = a + c + d
 g = b + c + d

 a b c d e f g
 1 2 3 4 5 6 7
 efg   loc
 ooo    0
 oox    7
 oxo    6
 xoo    5
 oxx    3
 xox    2
 xxo    1
 xxx    4
*/
//Hamming Checksum Lookup Table(err_loc = 0 represnets no error occurs)
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

//Detect Error Location(Hamming Checksum, error_loc = 0 represents no error occurs)
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

//Correct Error(Hamming Checksum Vector)
void hmc_err_cor_vector_int(int* hmc_vector, int err_loc)
{
	switch(err_loc)
	{
		case 0:break;
		case 1:hmc_vector[0] = hmc_vector[4] - hmc_vector[1] - hmc_vector[3]; break;
		case 2:hmc_vector[1] = hmc_vector[4] - hmc_vector[0] - hmc_vector[3]; break;
		case 3:hmc_vector[2] = hmc_vector[5] - hmc_vector[0] - hmc_vector[0]; break;
		case 4:hmc_vector[3] = hmc_vector[5] - hmc_vector[0] - hmc_vector[2]; break;
		case 5:hmc_vector[4] = hmc_vector[0] + hmc_vector[1] + hmc_vector[3]; break;
		case 6:hmc_vector[5] = hmc_vector[0] + hmc_vector[2] + hmc_vector[3]; break;
		case 7:hmc_vector[6] = hmc_vector[1] + hmc_vector[2] + hmc_vector[3]; break;
	}
}
//Check Hamming Vector
int check_hamming_vector_int(int* hmc_vector)
{
	int is_equal_flag = -1;
	int sum = 0;
	for (int i = 0; i < 7; i++)
	{
		sum += hmc_vector[i];
	}
	if (sum == hmc_vector[7])
	{
		is_equal_flag = 1;
	}
	return is_equal_flag;
}

//Correct Error(Hamming Checksum Matrix)
void hmc_err_cor_matrix_int(int* hmc_matrix, int hmc_row, int hmc_col)
{
	int hmc_vector_size = hmc_row * hmc_col / 7;
	int* hmc_vector = hmc_matrix;
	int err_loc = 0;
	for (int i = 0; i < hmc_vector_size; i++)
	{
		hmc_vector = hmc_matrix + i * 7;
		err_loc = hmc_err_loc_int(hmc_vector);
		hmc_err_cor_vector_int(hmc_vector, err_loc);
	}
}

//Correct Error(Hamming Checksum Matrix with parity check)
int hmc_err_cor_matrix_int_parityck(int* hmc_matrix, int hmc_row, int hmc_col)
{
	//If cor_success = 0, the # of errors greater than 1, the errors can not be corrected
	int cor_success = 1;
	int hmc_vector_size = hmc_row * hmc_col / 8;
	int* hmc_vector = hmc_matrix;
	int err_loc = 0;
	for (int i = 0; i < hmc_vector_size; i++)
	{
		hmc_vector = hmc_matrix + i * 8;
		err_loc = hmc_err_loc_int(hmc_vector);
		hmc_err_cor_vector_int(hmc_vector, err_loc);
		if (check_hamming_vector_int(hmc_vector) < 0)
		{
			cor_success = 0;
			break;
		}
	}
	return cor_success;
}

//Correct Error(Hamming Checksum Matrix with parity check)

// Create Hamming Checksum Matrix
int* create_hamming_checksum_matrix(int* matrix, int row, int col)
{
	int hmc_row, hmc_col;
	hmc_col = (col + 3) / 4 * 7;
	hmc_row = row; 
	int* hamming_checksum_matrix = (int*)malloc(sizeof(int) * hmc_row * hmc_col);
	return hamming_checksum_matrix;
}

// Create Hamming Checksum Matrix with parity check
int* create_hamming_checksum_matrix_parityck(int* matrix, int row, int col)
{
	int hmc_row, hmc_col;
	hmc_col = (col + 3) / 4 * 8;
	hmc_row = row; 
	int* hamming_checksum_matrix = (int*)malloc(sizeof(int) * hmc_row * hmc_col);
	return hamming_checksum_matrix;
}

//Matrix Translation(Matrix -> Hamming Checksum Matrix)
void* hamming_checksum_matrix_translation(int* matrix, int row, int col, int* hamming_checksum_matrix)
{
	int hmc_row, hmc_col;
	hmc_col = (col + 3) / 4 * 7;
	hmc_row = row; 

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
}

//Matrix Translation(Matrix -> Hamming Checksum Matrix) with parity check
void* hamming_checksum_matrix_translation_parityck(int* matrix, int row, int col, int* hamming_checksum_matrix)
{
	int hmc_row, hmc_col;
	hmc_col = (col + 3) / 4 * 8;
	hmc_row = row; 

	for (int i = 0; i < hmc_row; i++)
	{
		for (int j = 0; j < hmc_col; j++)
		{
			if (j % 8 < 4)
			{
				hamming_checksum_matrix[i * hmc_col + j] = matrix[i * col + (j / 8 * 4 + j % 8)];
			} else {
				switch (j % 8) 
				{
					case 4 : hamming_checksum_matrix[i * hmc_col + j] = hamming_checksum_matrix[i * hmc_col + j - 4] + hamming_checksum_matrix[i * hmc_col + j - 3] + hamming_checksum_matrix[i * hmc_col + j - 1]; break; 
					case 5 : hamming_checksum_matrix[i * hmc_col + j] = hamming_checksum_matrix[i * hmc_col + j - 5] + hamming_checksum_matrix[i * hmc_col + j - 2] + hamming_checksum_matrix[i * hmc_col + j - 3]; break; 
					case 6 : hamming_checksum_matrix[i * hmc_col + j] = hamming_checksum_matrix[i * hmc_col + j - 5] + hamming_checksum_matrix[i * hmc_col + j - 4] + hamming_checksum_matrix[i * hmc_col + j - 3]; break; 
					case 7 : hamming_checksum_matrix[i * hmc_col + j] = 0;
									 for (int k = 0; k < 7; k++) 
									 {
										 hamming_checksum_matrix[i * hmc_col + j] += hamming_checksum_matrix[i * hmc_col + k];
									 }
									 break;
				}
			}
		}
	}
}

//Map Hamming Checksum matrix to matrix
void hamming_matrix_map_matrix(int* hamming_checksum_matrix, int hc_matrix_row, int hc_matrix_col, int* matrix, int matrix_row, int matrix_col)
{
		int* hmc_vector = hamming_checksum_matrix;
		int* vector = matrix;
		for (int i = 0; i < hc_matrix_row * hc_matrix_col / 7; i++)
		{
			hmc_vector = hamming_checksum_matrix + i * 7;
			vector = matrix + i * 4;
			memcpy(vector, hmc_vector, 4 * sizeof(int));
		}
}	

//Map Hamming Checksum(with parity check) matrix to matrix
void hamming_matrix_map_matrix_parityck(int* hamming_checksum_matrix, int hc_matrix_row, int hc_matrix_col, int* matrix, int matrix_row, int matrix_col)
{
		int* hmc_vector = hamming_checksum_matrix;
		int* vector = matrix;
		for (int i = 0; i < hc_matrix_row * hc_matrix_col / 8; i++)
		{
			hmc_vector = hamming_checksum_matrix + i * 8;
			vector = matrix + i * 4;
			memcpy(vector, hmc_vector, 4 * sizeof(int));
		}
}
int hmc_matrix_row(int row)
{
	return row;
}

int hmc_matrix_row_parity_check(int row)
{
	return row;
}

int hmc_matrix_col(int col)
{
	int hmc_col = (col + 3) / 4 * 7;
	return hmc_col;
}

int hmc_matrix_col_parity_check(int col)
{
	int hmc_col = (col + 3) / 4 * 8;
	return hmc_col;
}
