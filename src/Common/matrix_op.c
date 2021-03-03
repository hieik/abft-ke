#include<stdio.h>

//Matirx_Init
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

//Matrix Print		
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
