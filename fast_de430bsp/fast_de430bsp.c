//
// Created by RUSTAM RUDAKOV on 11/06/2017.
//

/*
 * Since it is already known what is exactly the structure and size of de430.bsp,
 * we can make things much faster
 *
 * */
#include "fast_de430bsp.h"

// first we need to check if everything ok with the file
// function is_de430bsp_ok_fast returns 1 if file is ok and 0 if it's damaged or this is not de430.bsp
int is_de430bsp_ok(char* path_to_the_file){

    struct stat statistics; // from <sys/stat.h> - is used for computing file size
	stat(path_to_the_file, &statistics); // statistics.st_size now has the size of 430de.bsp
    if(statistics.st_size == 119741440) return 1; // the size of 430de.bsp must be 119 741 440 Bytes

    return 0;
}

// takes pointer to struct de430bsp_file_header and fills all fields
int load_de430header_fast(struct de430bsp_file_header *file_header_ptr){

    // 1. LOCIDW (8 characters, 8 bytes) An identification word (`DAF/SPK') 7+'\0'
    strcpy(file_header_ptr->file_record_struct.locidw, "DAF/SPK");

    // 2. ND(1 integer, 4 bytes) : The number of double prec. components in each array summary.[Address 8]
    file_header_ptr->file_record_struct.nd = 2;

    //3. NI ( 1 integer, 4 bytes): The number of integer components in each array summary. [Address 12]
    file_header_ptr->file_record_struct.ni = 6;

    //4. LOCIFN (60 characters, 60 bytes):
    // The internal name or description of the array file. 7+'\0'
    strcpy(file_header_ptr->file_record_struct.locifn, "NIO2SPK");


    //5. FWARD ( 1 integer, 4 bytes): The record number of the initial summary record in the file. [Address 76]
    file_header_ptr->file_record_struct.fward = 4;


    //6. BWARD ( 1 integer, 4 bytes): The record number of the final summary record in the file. [Address 80]
    file_header_ptr->file_record_struct.bward = 4;

    // 7. FREE(1 integer, 4 bytes) :
    //The first free address in the file.This is the address at which
    //the first element of the next array to be added to the file will be stored.
    file_header_ptr->file_record_struct.free = 14967465;


    //8. LOCFMT(8 characters, 8 bytes) :
    // The character string that indicates the numeric binary format of the DAF.
    // The string has value "LTL-IEEE" 8+'\0' одна буква не влезает
    // переделывать структуру не хочется. и так сойдет :)
    strcpy(file_header_ptr->file_record_struct.locfmt, "LTL-IEE");

    //10. FTPSTR(28 characters, 28 bytes) : The FTP validation string.
    strcpy(file_header_ptr->file_record_struct.ftpstr,
           "FTPSTR:\r:\n:\r\n:\r\x00:\x81:\x10\xce:ENDFTP");


    file_header_ptr->summary_record_struct.next_record_number = 0;
    file_header_ptr->summary_record_struct.previous_record_number = 0;
    file_header_ptr->summary_record_struct.total_summaries_number = 14;

    //                  (NI + 1)
    //SS       =  ND + --------         (Note that this is
    //                      2             integer division.)

    // Получается single_summary_size = 5 , умножаем на 8 (так как размер double precision),
    // равно 40 байтам. Эти 40 байтов состоят из ND*8 + NI*4 = 2ND*8 + 6NI*4 = 40


    //SOLAR SYSTEM BARYCENTER
    file_header_ptr->summaries_line_struct[0].segment_start_time = 0; //always
    file_header_ptr->summaries_line_struct[0].segment_last_time = 0; //always
    file_header_ptr->summaries_line_struct[0].target_code =0;
    file_header_ptr->summaries_line_struct[0].center_code =0;
    file_header_ptr->summaries_line_struct[0].ref_frame =0; //always
    file_header_ptr->summaries_line_struct[0].type_of_data =0; //always
    file_header_ptr->summaries_line_struct[0].record_start_adress = 0;
    file_header_ptr->summaries_line_struct[0].record_last_adress = 0;

    //MERCURY BARYCENTER
    file_header_ptr->summaries_line_struct[1].segment_start_time = -14200747200.00; //always
    file_header_ptr->summaries_line_struct[1].segment_last_time = 20514081600.00; //always
    file_header_ptr->summaries_line_struct[1].target_code =1;
    file_header_ptr->summaries_line_struct[1].center_code =0;
    file_header_ptr->summaries_line_struct[1].ref_frame =1; //always
    file_header_ptr->summaries_line_struct[1].type_of_data =2; //always
    file_header_ptr->summaries_line_struct[1].record_start_adress = 641;
    file_header_ptr->summaries_line_struct[1].record_last_adress = 2210500;

    //VENUS BARYCENTER
    file_header_ptr->summaries_line_struct[2].segment_start_time = -14200747200.00; //always
    file_header_ptr->summaries_line_struct[2].segment_last_time = 20514081600.00; //always
    file_header_ptr->summaries_line_struct[2].target_code =2;
    file_header_ptr->summaries_line_struct[2].center_code =0;
    file_header_ptr->summaries_line_struct[2].ref_frame =1; //always
    file_header_ptr->summaries_line_struct[2].type_of_data =2; //always
    file_header_ptr->summaries_line_struct[2].record_start_adress = 2210501;
    file_header_ptr->summaries_line_struct[2].record_last_adress = 3014088;

    //EARTH-MOON BARY.
    file_header_ptr->summaries_line_struct[3].segment_start_time = -14200747200.00; //always
    file_header_ptr->summaries_line_struct[3].segment_last_time = 20514081600.00; //always
    file_header_ptr->summaries_line_struct[3].target_code =3;
    file_header_ptr->summaries_line_struct[3].center_code =0;
    file_header_ptr->summaries_line_struct[3].ref_frame =1; //always
    file_header_ptr->summaries_line_struct[3].type_of_data =2; //always
    file_header_ptr->summaries_line_struct[3].record_start_adress = 3014089;
    file_header_ptr->summaries_line_struct[3].record_last_adress = 4043684;

    //MARS BARYCENTER
    file_header_ptr->summaries_line_struct[4].segment_start_time = -14200747200.00; //always
    file_header_ptr->summaries_line_struct[4].segment_last_time = 20514081600.00; //always
    file_header_ptr->summaries_line_struct[4].target_code =4;
    file_header_ptr->summaries_line_struct[4].center_code =0;
    file_header_ptr->summaries_line_struct[4].ref_frame =1; //always
    file_header_ptr->summaries_line_struct[4].type_of_data =2; //always
    file_header_ptr->summaries_line_struct[4].record_start_adress = 4043685;
    file_header_ptr->summaries_line_struct[4].record_last_adress = 4483148;


    //JUPITER BARYCENTER
    file_header_ptr->summaries_line_struct[5].segment_start_time = -14200747200.00; //always
    file_header_ptr->summaries_line_struct[5].segment_last_time = 20514081600.00; //always
    file_header_ptr->summaries_line_struct[5].target_code =5;
    file_header_ptr->summaries_line_struct[5].center_code =0;
    file_header_ptr->summaries_line_struct[5].ref_frame =1; //always
    file_header_ptr->summaries_line_struct[5].type_of_data =2; //always
    file_header_ptr->summaries_line_struct[5].record_start_adress = 4483149;
    file_header_ptr->summaries_line_struct[5].record_last_adress = 4809608;


    //SATURN BARYCENTER
    file_header_ptr->summaries_line_struct[6].segment_start_time = -14200747200.00; //always
    file_header_ptr->summaries_line_struct[6].segment_last_time = 20514081600.00; //always
    file_header_ptr->summaries_line_struct[6].target_code =6;
    file_header_ptr->summaries_line_struct[6].center_code =0;
    file_header_ptr->summaries_line_struct[6].ref_frame =1; //always
    file_header_ptr->summaries_line_struct[6].type_of_data =2; //always
    file_header_ptr->summaries_line_struct[6].record_start_adress = 4809609;
    file_header_ptr->summaries_line_struct[6].record_last_adress = 5098400;

    //URANUS BARYCENTER
    file_header_ptr->summaries_line_struct[7].segment_start_time = -14200747200.00; //always
    file_header_ptr->summaries_line_struct[7].segment_last_time = 20514081600.00; //always
    file_header_ptr->summaries_line_struct[7].target_code =7;
    file_header_ptr->summaries_line_struct[7].center_code =0;
    file_header_ptr->summaries_line_struct[7].ref_frame =1; //always
    file_header_ptr->summaries_line_struct[7].type_of_data =2; //always
    file_header_ptr->summaries_line_struct[7].record_start_adress = 5098401;
    file_header_ptr->summaries_line_struct[7].record_last_adress = 5349524;

    //NEPTUNE BARYCENTER
    file_header_ptr->summaries_line_struct[8].segment_start_time = -14200747200.00; //always
    file_header_ptr->summaries_line_struct[8].segment_last_time = 20514081600.00; //always
    file_header_ptr->summaries_line_struct[8].target_code =8;
    file_header_ptr->summaries_line_struct[8].center_code =0;
    file_header_ptr->summaries_line_struct[8].ref_frame =1; //always
    file_header_ptr->summaries_line_struct[8].type_of_data =2; //always
    file_header_ptr->summaries_line_struct[8].record_start_adress = 5349525;
    file_header_ptr->summaries_line_struct[8].record_last_adress = 5600648;

    //PLUTO BARYCENTER
    file_header_ptr->summaries_line_struct[9].segment_start_time = -14200747200.00; //always
    file_header_ptr->summaries_line_struct[9].segment_last_time = 20514081600.00; //always
    file_header_ptr->summaries_line_struct[9].target_code =9;
    file_header_ptr->summaries_line_struct[9].center_code =0;
    file_header_ptr->summaries_line_struct[9].ref_frame =1; //always
    file_header_ptr->summaries_line_struct[9].type_of_data =2; //always
    file_header_ptr->summaries_line_struct[9].record_start_adress = 5600649;
    file_header_ptr->summaries_line_struct[9].record_last_adress = 5851772;

    //SUN
    file_header_ptr->summaries_line_struct[10].segment_start_time = -14200747200.00; //always
    file_header_ptr->summaries_line_struct[10].segment_last_time = 20514081600.00; //always
    file_header_ptr->summaries_line_struct[10].target_code =10;
    file_header_ptr->summaries_line_struct[10].center_code =0;
    file_header_ptr->summaries_line_struct[10].ref_frame =1; //always
    file_header_ptr->summaries_line_struct[10].type_of_data =2; //always
    file_header_ptr->summaries_line_struct[10].record_start_adress = 5851773;
    file_header_ptr->summaries_line_struct[10].record_last_adress = 6730696;

    // MOON
    file_header_ptr->summaries_line_struct[11].segment_start_time = -14200747200.00; //always
    file_header_ptr->summaries_line_struct[11].segment_last_time = 20514081600.00; //always
    file_header_ptr->summaries_line_struct[11].target_code =301;
    file_header_ptr->summaries_line_struct[11].center_code =3;
    file_header_ptr->summaries_line_struct[11].ref_frame =1; //always
    file_header_ptr->summaries_line_struct[11].type_of_data =2; //always
    file_header_ptr->summaries_line_struct[11].record_start_adress = 6730697;
    file_header_ptr->summaries_line_struct[11].record_last_adress = 10849068;

    // EARTH
    file_header_ptr->summaries_line_struct[12].segment_start_time = -14200747200.00; //always
    file_header_ptr->summaries_line_struct[12].segment_last_time = 20514081600.00; //always
    file_header_ptr->summaries_line_struct[12].target_code =399;
    file_header_ptr->summaries_line_struct[12].center_code =3;
    file_header_ptr->summaries_line_struct[12].ref_frame =1; //always
    file_header_ptr->summaries_line_struct[12].type_of_data =2; //always
    file_header_ptr->summaries_line_struct[12].record_start_adress = 10849069;
    file_header_ptr->summaries_line_struct[12].record_last_adress = 14967440;

    //  MERCURY
    file_header_ptr->summaries_line_struct[13].segment_start_time = -14200747200.00; //always
    file_header_ptr->summaries_line_struct[13].segment_last_time = 20514081600.00; //always
    file_header_ptr->summaries_line_struct[13].target_code =199;
    file_header_ptr->summaries_line_struct[13].center_code =1;
    file_header_ptr->summaries_line_struct[13].ref_frame =1; //always
    file_header_ptr->summaries_line_struct[13].type_of_data =2; //always
    file_header_ptr->summaries_line_struct[13].record_start_adress = 14967441;
    file_header_ptr->summaries_line_struct[13].record_last_adress = 14967452;


    //  VENUS
    file_header_ptr->summaries_line_struct[14].segment_start_time = -14200747200.00; //always
    file_header_ptr->summaries_line_struct[14].segment_last_time = 20514081600.00; //always
    file_header_ptr->summaries_line_struct[14].target_code =299;
    file_header_ptr->summaries_line_struct[14].center_code =2;
    file_header_ptr->summaries_line_struct[14].ref_frame =1; //always
    file_header_ptr->summaries_line_struct[14].type_of_data =2; //always
    file_header_ptr->summaries_line_struct[14].record_start_adress = 14967453;
    file_header_ptr->summaries_line_struct[14].record_last_adress = 14967464;




    return 1;
}


//gets barycentric equatorial Cartesian positions and velocities
// relative to the equinox 2000/ICRS
struct Coordinates get_coordinates(long long int  date_in_seconds,
                                   int target_code, int center_code,
                                   struct de430bsp_file_header *file_header_ptr,
                                                              FILE *bsp_430_file) {
struct Coordinates pos;

long int array_info_offset;
struct array_inf array_info;

long int internal_offset;

for (int i = 0; i <= (int) file_header_ptr->summary_record_struct.total_summaries_number; i++) {
    if (file_header_ptr->summaries_line_struct[i].target_code == target_code &&
        file_header_ptr->summaries_line_struct[i].center_code == center_code &&
        file_header_ptr->summaries_line_struct[i].segment_start_time < date_in_seconds &&
        file_header_ptr->summaries_line_struct[i].segment_last_time > date_in_seconds) {
/*
        printf("get_coordinates Found: [%d] = ", i);
        printf(" %f,", file_header_ptr->summaries_line_struct[i].segment_start_time); // в сек от J2000
        printf(" %f,", file_header_ptr->summaries_line_struct[i].segment_last_time); // в сек от J2000
        printf(" %s[%d] , ", planet_name(file_header_ptr->summaries_line_struct[i].target_code), i);
        printf(" %s[%d], ", planet_name(file_header_ptr->summaries_line_struct[i].center_code), i);
        printf(" %d, ", file_header_ptr->summaries_line_struct[i].record_start_adress);
        printf(" %d, ", file_header_ptr->summaries_line_struct[i].record_last_adress);
        printf("\n");
*/
        /*


        The records within a segment are ordered by increasing initial epoch. All records contain the same number
        of coefficients. A segment of this type is structured as follows:

        +---------------+
        | Record 1      |
        +---------------+
        | Record 2      |
        +---------------+
        .
        .
        .
        +---------------+
        | Record N      |
        +---------------+
        | INIT          |
        +---------------+
        | INTLEN        |
        +---------------+
        | RSIZE         |
        +---------------+
        | N             |
        +---------------+
         */

        /*встаем на позицию 4 слова до конца summaries_line_struct[i]
        */
        array_info_offset =
                (file_header_ptr->summaries_line_struct[i].record_last_adress - 4) * 8;
        fseek(bsp_430_file, array_info_offset, SEEK_SET);
        //SEEK_SET- с начала файла,SEEK_CUR-тек.позиция, SEEK_END - с конца

        /*читаем double init - start time of the first record in array
        double intlen - the length of one record (seconds)
        double rsize - number of elements in one record
        double n - number of records in segment*/
        fread(&array_info, sizeof(array_info), 1, bsp_430_file);
/*
        printf(" %f, ", array_info.init);
        printf(" %f, ", array_info.intlen);
        printf(" %f, ", array_info.rsize);
        printf(" %f, ", array_info.n);
        printf("\n");

*/
        // находим смещение на нужную запись внутри массива
        internal_offset =
                floor((date_in_seconds - array_info.init) / array_info.intlen)
                * (int) array_info.rsize;

        // встаем на начало нужной записи
        int record = 8 * (int) (file_header_ptr->summaries_line_struct[i].record_start_adress + internal_offset);
        int order = (int) ((array_info.rsize - 2) / 3 - 1);

        //self._mem[record - 8:record + int(rsize) * 8]  ??? почему -8 ?
        fseek(bsp_430_file, (record-8), SEEK_SET);
        //SEEK_SET- с начала файла,SEEK_CUR-тек.позиция, SEEK_END - с конца

        double *array_of_coffs = malloc(array_info.rsize * 8);
        //double array_of_coffs[280];
        fread(array_of_coffs, (int) (array_info.rsize * 8), 1, bsp_430_file);


        //printf("%d, %d, %d, %d\n", internal_offset, summaries_array[i].record_start_adress, record, order);
        /*printf("%f \n", array_of_coffs[0]);
        printf("%f \n", array_of_coffs[1]);
        printf("%f \n", array_of_coffs[2]);*/


        double tau = (date_in_seconds - array_of_coffs[0]) / array_of_coffs[1];
        //float beg = 2;
        //float end = beg + order + 1;
        //order = (int) (order);
        int deg = order + 1;
        double factor = 1.0 / array_of_coffs[1]; // unscale time dimension

        double *coffs_ptr = &array_of_coffs[2];
        pos.x = chebyshev(order, tau, coffs_ptr); //array_of_coffs[2:2 + deg]);
        coffs_ptr = &array_of_coffs[2 + deg];
        pos.y = chebyshev(order, tau, coffs_ptr); //array_of_coffs[2 + deg:2 + 2 * deg]);
        coffs_ptr = &array_of_coffs[2 + 2 * deg];
        pos.z = chebyshev(order, tau, coffs_ptr); // array_of_coffs[2 + 2 * deg:2 + 3 * deg]);



        // type 2 uses derivative on the same polynomial
        coffs_ptr = &array_of_coffs[2];
        pos.velocity_x = der_chebyshev(order, tau, coffs_ptr) * factor; // array_of_coffs[2:2 + deg]) * factor;
        coffs_ptr = &array_of_coffs[2 + deg];
        pos.velocity_y =
                der_chebyshev(order, tau, coffs_ptr) * factor; // array_of_coffs[2 + deg:2 + 2 * deg]) * factor;
        coffs_ptr = &array_of_coffs[2 + 2 * deg];
        pos.velocity_z =
                der_chebyshev(order, tau, coffs_ptr) * factor; // array_of_coffs[2 + 2 * deg:2 + 3 * deg]) * factor;

        /*printf("---finally---\n");
        printf("x coordinate equals to %e \n", x_coord);
        printf("y coordinate equals to %e \n", y_coord);
        printf("z coordinate equals to %e \n", z_coord);*/

        /*printf("%f \n", x_coord1);
        printf("%f \n", y_coord1);
        printf("%f \n", z_coord1);*/

        free(array_of_coffs);
    }
}


return pos;

}

struct Coordinates calc_geocentric_equ_cartes_pos(long long int  date_in_seconds,
                                                  int target_code,
                                                  struct de430bsp_file_header *file_header_ptr,
                                                  FILE *bsp_430_file){


    struct Coordinates coordinates_of_object, temp_coordinates;

    int improved_target_code, imroved_center_code;

    if(target_code<=10){
        improved_target_code =target_code;
        imroved_center_code = 0;

    } else {
        switch(target_code){
            case 11: improved_target_code = 301; imroved_center_code =3; break;
            case 12: improved_target_code = 399; imroved_center_code =3; break;
            case 13: improved_target_code = 199; imroved_center_code =1; break;
            case 14: improved_target_code = 299; imroved_center_code =2; break;
            default: improved_target_code = -1;imroved_center_code =0;
        }

    }


    coordinates_of_object = get_coordinates(date_in_seconds, improved_target_code, imroved_center_code,
                                                                         file_header_ptr, bsp_430_file);

    printf("calc_geocentric_equ_cartes_pos\n");
    printf(" %s[%d] , \n", planet_name(improved_target_code), improved_target_code);
    printf(" center_code = [%d] , \n", imroved_center_code);
    printf("%e \n", coordinates_of_object.x);
    printf("%e \n", coordinates_of_object.y);
    printf("%e \n", coordinates_of_object.z);

    temp_coordinates = get_coordinates(date_in_seconds, 3, 0,
                                       file_header_ptr,
                                       bsp_430_file);

    coordinates_of_object.x -= temp_coordinates.x;
    coordinates_of_object.y -= temp_coordinates.y;
    coordinates_of_object.z -= temp_coordinates.z;

    temp_coordinates = get_coordinates(date_in_seconds, 399, 3,
                                       file_header_ptr,
                                       bsp_430_file);

    coordinates_of_object.x -= temp_coordinates.x;
    coordinates_of_object.y -= temp_coordinates.y;
    coordinates_of_object.z -= temp_coordinates.z;

    return coordinates_of_object;
}