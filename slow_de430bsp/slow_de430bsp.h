//
// Created by RUSTAM RUDAKOV on 11/06/2017.
//

#ifndef BSP_FILE_READER_SLOW_DE430BSP_H
#define BSP_FILE_READER_SLOW_DE430BSP_H

#include "../auxiliary.h"

int is_de430bsp_ok_slow(FILE *file_ptr);
int load_de430header_slow(FILE *file_ptr, struct de430bsp_file_header *file_header_ptr);

#endif //BSP_FILE_READER_SLOW_DE430BSP_H
