//Bit Error Rate -> Error Injection
void bit_flop_int(int* integer, int message_size, double ber);
void bit_flop_double(double* double_float, int message_size, double ber);

//Matirx Init
void init_matrix(int* matrix, int matrix_size_row, int matrix_size_col, int init_value);

//Matirx Print
void print_matrix(int* matrix, int row, int col);

//Hamming Checksum Lookup table
int hmc_lookup_table(int cs1, int cs2, int cs3);

//Detect Error Location(integer Hamming Checksum Vector)
int hmc_err_loc_int(int* hmc_vector);

//Correct Error Location(integer Hamming Checksum Vector)
void hmc_err_cor_vector_int(int* hmc_vector, int err_loc);

//Check Hamming Vector
int check_hamming_vector_int(int* hmc_vector);

//Correct Error(integer Hamming Checksum Matrix)
void hmc_err_cor_matrix_int(int* hmc_vector, int hmc_row, int hmc_col);

//Correct Error(Hamming Checksum Matrix with parity check)
int hmc_err_cor_matrix_int_parityck(int* hmc_matrix, int hmc_row, int hmc_col);

//Create Hamming Checksum Matrix 
int* create_hamming_checksum_matrix(int* matrix, int row, int col);

//Create Hamming Checksum Matrix with prity check
int* create_hamming_checksum_matrix_parityck(int* matrix, int row, int col);

//Matrix Translation(Matrix -> Hamming Checksum Matrix)
void* hamming_checksum_matrix_translation(int* matrix, int row, int col, int* hamming_checksum_matrix);

//Matrix Translation(Matrix -> Hamming Checksum Matrix) withparity check
void* hamming_checksum_matrix_translation_parityck(int* matrix, int row, int col, int* hamming_checksum_matrix);

//Map Hamming Checksum matrix to matrix
void hamming_matrix_map_matrix(int* hamming_checksum_matrix, int hc_matrix_row, int hc_matrix_col, int* matrix, int matrix_row, int matrix_col);

//Map Hamming Checksum matrix(with parity check) to matrix
void hamming_matrix_map_matrix_parityck(int* hamming_checksum_matrix, int hc_matrix_row, int hc_matrix_col, int* matrix, int matrix_row, int matrix_col);

//# of rows of Hamming checksum matrix
int hmc_matrix_row(int row);
//# of cols of Hamming checksum matrix
int hmc_matrix_col(int col);

//# of rows of Hamming checksum matrix with parity check
int hmc_matrix_row_parity_check(int row);

//# of cols of Hamming checksum matrix with parity check
int hmc_matrix_col_parity_check(int col);

//all elements of a array is 1
int all_ele_true(int *array, int size);
