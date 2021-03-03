#include<stdlib.h>
#include<time.h>

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
