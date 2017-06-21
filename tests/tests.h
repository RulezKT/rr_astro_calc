//
// Created by RUSTAM RUDAKOV on 19/06/2017.
//

#ifndef BSP_FILE_READER_TESTS_H
#define BSP_FILE_READER_TESTS_H

#include "../auxiliary/auxiliary.h"

int testpo_430_tests(FILE *bsp_430_file, struct de430bsp_file_header *header_struct_fast_ptr);
int make_bsp_swiss_files(struct de430bsp_file_header *header_struct_fast_ptr, FILE *bsp_430_file);
int compare_bsp_swiss_files(struct de430bsp_file_header *header_struct_fast_ptr, FILE *bsp_430_file);


#endif //BSP_FILE_READER_TESTS_H
