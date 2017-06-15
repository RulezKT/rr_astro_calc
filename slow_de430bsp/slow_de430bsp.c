//
// Created by RUSTAM RUDAKOV on 11/06/2017.
//
#include "slow_de430bsp.h"

//check already opened file
// for size, Ftpstring and etc.
// returns 1 if everything is ok, 0 if not
int is_de430bsp_ok_slow(FILE *file_ptr){

return 1;



}

//loads the header struct from de430.bsp
// to struct de430bsp_file_header
int load_de430header_slow(FILE *bsp_430_file,
                          struct de430bsp_file_header *file_header_ptr){


    rewind(bsp_430_file);
    fread(&file_header_ptr->file_record_struct, sizeof(file_header_ptr->file_record_struct), 1, bsp_430_file);

    // first summary record. we need to read nd doubles and ni integers starting at offset fward
    int first_summary_rec = (file_header_ptr->file_record_struct.fward - 1) * 1024;
    fseek(bsp_430_file, first_summary_rec, SEEK_SET);
    fread(&file_header_ptr->summary_record_struct, sizeof(file_header_ptr->summary_record_struct), 1, bsp_430_file);


    fseek(bsp_430_file, first_summary_rec + 24, SEEK_SET);
    //SEEK_SET- с начала файла,SEEK_CUR-тек.позиция, SEEK_END - с конца

    for (int i = 0; i <= (int)file_header_ptr->summary_record_struct.total_summaries_number; i++) {
        if (i ==0){
            //SOLAR SYSTEM BARYCENTER
            file_header_ptr->summaries_line_struct[0].segment_start_time = 0; //always
            file_header_ptr->summaries_line_struct[0].segment_last_time = 0; //always
            file_header_ptr->summaries_line_struct[0].target_code =0;
            file_header_ptr->summaries_line_struct[0].center_code =0;
            file_header_ptr->summaries_line_struct[0].ref_frame =0; //always
            file_header_ptr->summaries_line_struct[0].type_of_data =0; //always
            file_header_ptr->summaries_line_struct[0].record_start_adress = 0;
            file_header_ptr->summaries_line_struct[0].record_last_adress = 0;
        } else {
            fread(&file_header_ptr->summaries_line_struct[i], sizeof(file_header_ptr->summaries_line_struct[0]), 1, bsp_430_file);
        /*   printf("Summaries[%2d] = ", i);
           printf(" %.2f,", summaries_array[i].segment_start_time); // в сек от J2000
           printf(" %.2f,", summaries_array[i].segment_last_time); // в сек от J2000
           printf("%20s[%3d],  ", planet_name(summaries_array[i].target_code),summaries_array[i].target_code);
           printf("%20s[%2d],  ", planet_name(summaries_array[i].center_code),summaries_array[i].center_code);
           printf(" %d, ", summaries_array[i].ref_frame); // ??? always 1
           printf(" %d, ", summaries_array[i].type_of_data); // always 2 - planet
           printf("%d, ", summaries_array[i].record_start_adress);
           printf("%d, ", summaries_array[i].record_last_adress);
           printf("\n"); */
        }
    }


/*  Лишние для нас записи в файле не трогаем
 *
     int single_summary_size = file_header_ptr->file_record_struct.nd +
                              (int)((file_header_ptr->file_record_struct.ni + 1) / 2); // integer division
    printf("single_summary_size = %d \n", single_summary_size);
    // Получается single_summary_size = 5 , умножаем на 8 (так как размер double precision),
    // равно 40 байтам. Эти 40 байтов состоят из ND*8 + NI*4 = 2*8 + 6*4 = 40

    int first_name_rec = first_summary_rec + 1024;
    printf("first_name_rec = %d \n", first_name_rec);
    fseek(bsp_430_file, first_name_rec, SEEK_SET);
    //SEEK_SET- с начала файла,SEEK_CUR-тек.позиция, SEEK_END - с конца
    fread(&name_rec, sizeof(name_rec), 1, bsp_430_file);
    printf("%s ", name_rec);

    fseek(bsp_430_file, 1024, SEEK_SET);
    //SEEK_SET- с начала файла,SEEK_CUR-тек.позиция, SEEK_END - с конца
    fread(&comm_rec, sizeof(comm_rec), 1, bsp_430_file);
    printf("\n-------------\n");
    printf("%s ", comm_rec);
    printf("-------------\n");

*/

    return 1;

}