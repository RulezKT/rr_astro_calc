//
// Created by RUSTAM RUDAKOV on 11/06/2017.
//

#ifndef BSP_FILE_READER_FAST_DE430BSP_H
#define BSP_FILE_READER_FAST_DE430BSP_H

#include "../auxiliary.h"

int is_de430bsp_ok(char* path_to_the_file);
int load_de430header_fast(struct de430bsp_file_header *file_header_ptr);
struct Coordinates get_coordinates(long long int  date_in_seconds,
                                   int target_code, int center_code,
                                   struct de430bsp_file_header *file_header_ptr,
                                   FILE *bsp_430_file);

struct Coordinates calc_geocentric_equ_cartes_pos(long long int  date_in_seconds,
                                                  int target_code,
                                                  struct de430bsp_file_header *file_header_ptr,
                                                  FILE *bsp_430_file);








#endif //BSP_FILE_READER_FAST_DE430BSP_H
