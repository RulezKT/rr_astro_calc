
#include "auxiliary.h"


/* the binary file with DE431 is so large that the following line is necessary
in standard Linux 32-bit environment */
//# define _FILE_OFFSET_BITS 64



//глобальная переменная - указатель на файл de430.bsp
FILE *bsp_430_file = NULL;

//глобальная структура для хранения всех хэдеров файла
// либо заполняется функцией load_de430header_fast() from fast_de430bsp.c
// либо грузится из файла функцией load_de430header_slow() from slow_de430bsp.c
struct de430bsp_file_header fh_struct_fast;
struct de430bsp_file_header fh_struct_slow;

// 1024 is the size of one record, offset to first array record 5 * 1024
#define FIRST_ARRAY_RECORD  5120L

// the first record in de430.bsp must be "DAF/SPK"
#define LOCIDW "DAF/SPK"

//FTP string is located at the address 699:727(not inclusive) in de430.bsp
#define FTPSTR  "FTPSTR:\r:\n:\r\n:\r\x00:\x81:\x10\xce:ENDFTP"



//opens and checks de430.bsp file
FILE* open_bsp_file(char* file_path){

    FILE* bsp_file_ptr = NULL;

    printf("opening de430.bsp file...\n");
    bsp_file_ptr = fopen(file_path, "rb");

    while (!bsp_file_ptr) {
        printf("Enter DE file name ? ");
        gets(&file_path[0]);
        if (!(bsp_file_ptr = fopen(&file_path[0], "rb")))
            printf("Can't find DE file <%s>\n", &file_path[0]);
    }
    printf("%s is opened\n", file_path);


    printf("fast checking file integrity...\n");
    if(is_de430bsp_ok(file_path)){
        printf("check passed\n");
    } else {
        printf("the file is damaged!");
        getchar();
        return 0;

    }

    printf("slow checking file integrity...\n");
    if(is_de430bsp_ok_slow(bsp_file_ptr)){
        printf("check passed\n");
    } else {
        printf("the file is damaged!");
        getchar();
        return 0;

    }

    return bsp_file_ptr;
}


struct Coordinates bsp_file_info(long long int  date_in_seconds, int center_code, int target_code) {

	//char* file_path = "de430.bsp";

	double x_coord, y_coord, z_coord;

	struct de430bsp_file_record de430bsp_file_record_struct;
	struct de430bsp_summary_record de430bsp_summary_record_struct;
	// кол-во сегментов, не знаю как вставить сюда
	// summary_record_struct.total_summaries_number, поэтому вручную вписал 14
	struct summaries_line summaries_array[14];

	int first_name_rec;
	char name_rec[1024];

	char comm_rec[2048];

	long long int  date_recieved_in_sec = date_in_seconds; //-682516800.0; // 17.05.1978 00:00 -> -682516800.0
	int target_code_recieved = target_code;
	int center_code_recieved = center_code;

    // struct stat statistics;  from <sys/stat.h> - is used for computing file size
/*
	bsp_430_file = fopen(file_path, "rb"); //заглушка чтобы поменьше писать при выборе файла
	while (!bsp_430_file) {
		printf("Enter DE file name ? ");
		gets(&file_path[0]);
		if (!(bsp_430_file = fopen(&file_path[0], "rb")))
			printf("Can't find DE file <%s>\n", &file_path[0]);
	}
	printf("==========================\n");
	printf("Opened %s\n", file_path);
	printf("==========================\n");
*/



    fseek(bsp_430_file, 0, SEEK_SET);
	//rewind(bsp_430_file);
	fread(&de430bsp_file_record_struct, sizeof(de430bsp_file_record_struct), 1, bsp_430_file);
/*
    printf("de430.bsp struct:\nLOCIDW = %s, \nND = %d, \nNI = %d, \n",
		de430bsp_file_record_struct.locidw, de430bsp_file_record_struct.nd,
		de430bsp_file_record_struct.ni);
	printf("FWARD = %d, \n", de430bsp_file_record_struct.fward);
	printf("BWARD = %d, \n", de430bsp_file_record_struct.bward);
	printf("end of struct File Record\n");

*/
	// first summary record. we need to read nd doubles and ni integers starting at offset fward
	int first_summary_rec = (de430bsp_file_record_struct.fward - 1) * 1024;
	//printf("first_summary_rec = %d \n", first_summary_rec);
	fseek(bsp_430_file, first_summary_rec, SEEK_SET);
	//SEEK_SET- с начала файла,SEEK_CUR-тек.позиция, SEEK_END - с конца
	fread(&de430bsp_summary_record_struct, sizeof(de430bsp_summary_record_struct), 1, bsp_430_file);


    // printf(" next_record_number = %.0f, \n", summary_record_struct.next_record_number);
    //printf(" previous_record_number = %.0f, \n", summary_record_struct.previous_record_number);
    //printf(" total_summaries_number = %.0f, \n", summary_record_struct.total_summaries_number);

    /*
	int single_summary_size = de430bsp_file_record_struct.nd +
		(int)((de430bsp_file_record_struct.ni + 1) / 2); // integer division
	printf("single_summary_size = %d \n", single_summary_size);
     */
	// Получается single_summary_size = 5 , умножаем на 8 (так как размер double precision),
	// равно 40 байтам. Эти 40 байтов состоят из ND*8 + NI*4 = 2*8 + 6*4 = 40 

    // пропускаем уже прочитанные первые 3 слова и считываем
	fseek(bsp_430_file, first_summary_rec + 24, SEEK_SET);
	//SEEK_SET- с начала файла,SEEK_CUR-тек.позиция, SEEK_END - с конца


   for (int i = 0; i < (int)de430bsp_summary_record_struct.total_summaries_number; i++) {
       fread(&summaries_array[i], sizeof(summaries_array[0]), 1, bsp_430_file);
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

/*
   first_name_rec = first_summary_rec + 1024;
   //printf("first_name_rec = %d \n", first_name_rec);
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


	for (int i = 0; i < (int)de430bsp_summary_record_struct.total_summaries_number; i++) {
		if (summaries_array[i].target_code == target_code_recieved && 
			summaries_array[i].center_code == center_code_recieved &&
			summaries_array[i].segment_start_time < date_recieved_in_sec &&
			summaries_array[i].segment_last_time > date_recieved_in_sec) {
			printf("Found: [%d] = ", i);
			printf(" %f,", summaries_array[i].segment_start_time); // в сек от J2000
			printf(" %f,", summaries_array[i].segment_last_time); // в сек от J2000
			printf(" %s[%d] , ", planet_name(summaries_array[i].target_code), i);
			printf(" %s[%d], ", planet_name(summaries_array[i].center_code), i);
			//printf(" %d, ", summaries_array[i].ref_frame);
			//printf(" %d, ", summaries_array[i].type_of_data);
			printf(" %d, ", summaries_array[i].record_start_adress);
			printf(" %d, ", summaries_array[i].record_last_adress);
			printf("\n");


			int array_info_offset = (summaries_array[i].record_last_adress - 4) * 8; // last 4 words
			struct array_inf {
				// etbeg, etend, target, observer, frame, type, rbeg, rend
				double init; //start time of the first record in array
				double intlen; // the length of one record (seconds)
				double rsize; // number of elements in one record
				double n; // number of records in segment
			};
			struct array_inf array_info;

			fseek(bsp_430_file, array_info_offset, SEEK_SET);
			//SEEK_SET- с начала файла,SEEK_CUR-тек.позиция, SEEK_END - с конца
			fread(&array_info, sizeof(array_info), 1, bsp_430_file);

			printf(" init = %f, ", array_info.init);
			printf(" intlen = %f, ", array_info.intlen);
			printf(" rsize = %f, ", array_info.rsize);
			printf("array_info.n = %f, ", array_info.n);
			printf("\n");


			// находим смещение на нужную запись внутри массива
			int internal_offset = floor((date_recieved_in_sec - array_info.init) / array_info.intlen) * (int)array_info.rsize;

			// встаем на начало нужной записи
			int record = 8 * (int)(summaries_array[i].record_start_adress + internal_offset);
			int order = (int)((array_info.rsize - 2) / 3 - 1);

			//self._mem[record - 8:record + int(rsize) * 8]  ??? почему -8 ?

			fseek(bsp_430_file, (record - 8), SEEK_SET);
			//SEEK_SET- с начала файла,SEEK_CUR-тек.позиция, SEEK_END - с конца

			double* array_of_coffs = malloc(array_info.rsize * 8);
			//double array_of_coffs[280];
			fread(array_of_coffs, (int)(array_info.rsize * 8), 1, bsp_430_file);


			//printf("%d, %d, %d, %d\n", internal_offset, summaries_array[i].record_start_adress, record, order);
			/*printf("%f \n", array_of_coffs[0]);
			printf("%f \n", array_of_coffs[1]);
			printf("%f \n", array_of_coffs[2]);*/


			float tau = (date_recieved_in_sec - array_of_coffs[0]) / array_of_coffs[1];
			float beg = 2;
			float end = beg + order + 1;
			order = (int)(order);
			int deg = order + 1;
			float factor = 1.0 / array_of_coffs[1]; // unscale time dimension

			double* coffs_ptr = &array_of_coffs[2];
			x_coord = chebyshev(order, tau, coffs_ptr); //array_of_coffs[2:2 + deg]);
			coffs_ptr = &array_of_coffs[2 + deg];
			y_coord = chebyshev(order, tau, coffs_ptr); //array_of_coffs[2 + deg:2 + 2 * deg]);
			coffs_ptr = &array_of_coffs[2 + 2 * deg];
			z_coord = chebyshev(order, tau, coffs_ptr); // array_of_coffs[2 + 2 * deg:2 + 3 * deg]);

															  // type 2 uses derivative on the same polynomial
			coffs_ptr = &array_of_coffs[2];
			float x_coord1 = der_chebyshev(order, tau, coffs_ptr)* factor; // array_of_coffs[2:2 + deg]) * factor;
			coffs_ptr = &array_of_coffs[2 + deg];
			float y_coord1 = der_chebyshev(order, tau, coffs_ptr)* factor; // array_of_coffs[2 + deg:2 + 2 * deg]) * factor;
			coffs_ptr = &array_of_coffs[2 + 2 * deg];
			float z_coord1 = der_chebyshev(order, tau, coffs_ptr)* factor; // array_of_coffs[2 + 2 * deg:2 + 3 * deg]) * factor;

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

	/*
	stat(file_path, &statistics);
	printf("\nfile size according to statistics = %d \n", statistics.st_size);
	fseek(bsp_430_file, 0, SEEK_END);
	int f_size = ftell(bsp_430_file);
	printf("file size according to ftell =	    %d \n", f_size); // надо 119 741 440 Bytes
	printf("number of records in de430.bsp =       %d \n", statistics.st_size / 1024);
	*/



/*
    printf("file_record_struct:\n");

    printf("LOCIDW = %s\n", de430bsp_file_record_struct.locidw);
    printf("nd= %d\n", de430bsp_file_record_struct.nd);
    printf("ni = %d\n", de430bsp_file_record_struct.ni);
    printf("locifn = %s\n", de430bsp_file_record_struct.locifn);
    printf("fward = %d\n", de430bsp_file_record_struct.fward);
    printf("bward= %d\n", de430bsp_file_record_struct.bward);
    printf("free = %d\n", de430bsp_file_record_struct.free);

    // printf("ftpstr = %x\n", file_record_struct.ftpstr);

*/

    struct Coordinates pos;
    pos.x = x_coord;
    pos.y = y_coord;
    pos.z = z_coord;

    return pos;
}



int main() {

	struct Coordinates coordinates_of_object;

	long long int seconds_f_jd2000;

    char* file_path = "de430.bsp";

    bsp_430_file = open_bsp_file(file_path);
    if(!bsp_430_file) return 17;

	//seconds_f_jd2000 = gregorian_date_to_sec_from_j2000(1978, 5, 17,0, 0, 0);
	
	//seconds_f_jd2000 = gregorian_date_to_sec_from_j2000(2015, 2, 8, 11, 52, 10);
	
	seconds_f_jd2000 = gregorian_date_to_sec_from_j2000(2015, 2, 8, 0, 0, 0);
	printf("seconds_f_jd2000 = %lld\n", seconds_f_jd2000);

    struct de430bsp_file_header *header_struct_fast_ptr =  &fh_struct_fast;
    struct de430bsp_file_header *header_struct_slow_ptr =  &fh_struct_slow;
    load_de430header_fast(header_struct_fast_ptr);
    load_de430header_slow(bsp_430_file, header_struct_slow_ptr);


    //430  1563.07.01 2292124.5 12  3  4       -0.01546545065456724000
    // 430  1733.11.01 2354329.5 12  3  6       -0.00504575050976365800
    // 430  2027.11.01 2461710.5 12  3  3       -0.24142010306108470000
    seconds_f_jd2000 = gregorian_date_to_sec_from_j2000(2027, 11, 1, 0, 0, 0);
    printf("seconds_f_jd2000 = %lld\n", seconds_f_jd2000);

    coordinates_of_object = calc_geocentric_equ_cartes_pos(seconds_f_jd2000,
                                                           EARTH,
                                                           header_struct_fast_ptr,bsp_430_file);

    // You can see that the output of this ephemeris is in kilometers.
    printf("---finally--- \n");
    printf(" %s[%d] , \n", planet_name(EARTH), EARTH);
    printf("%e \n", coordinates_of_object.x);
    printf("%e \n", coordinates_of_object.y);
    printf("%e \n", coordinates_of_object.z);



    coordinates_of_object = calc_geocentric_equ_cartes_pos(seconds_f_jd2000,
                                                           VENUS,
                                                           header_struct_slow_ptr,bsp_430_file);

    // You can see that the output of this ephemeris is in kilometers.
    printf("---finally--- \n");
    printf(" %s[%d] , \n", planet_name(14), 14);
    printf("%e \n", coordinates_of_object.x);
    printf("%e \n", coordinates_of_object.y);
    printf("%e \n", coordinates_of_object.z);


    int temp_i =0;
    for(int i=MERCURY_BARYCENTER;i<=14;i++){

        if(i<=10){
            temp_i =i;
        } else {
            switch(i){
                case 11: temp_i = 301; break;
                case 12: temp_i = 399; break;
                case 13: temp_i = 199; break;
                case 14: temp_i = 299; break;
                default: temp_i = -1;
            }

        }

/*
        struct Coordinates temp_coordinates;
        temp_i = 7;

        coordinates_of_object = bsp_file_info(seconds_f_jd2000, 0, temp_i);
        printf("---not very finally---\n");
        printf(" %s[%d] , ", planet_name(temp_i), temp_i);
        printf("%f \n", coordinates_of_object.x);
        printf("%f \n", coordinates_of_object.y);
        printf("%f \n", coordinates_of_object.z);


        temp_coordinates = bsp_file_info(seconds_f_jd2000, 0, 3);
        coordinates_of_object.x -= temp_coordinates.x;
        coordinates_of_object.y -= temp_coordinates.y;
        coordinates_of_object.z -= temp_coordinates.z;

        // происходит ли вызов ???
        temp_coordinates = bsp_file_info(seconds_f_jd2000, 3, 399);
        coordinates_of_object.x -= temp_coordinates.x;
        coordinates_of_object.y -= temp_coordinates.y;
        coordinates_of_object.z -= temp_coordinates.z;


        // You can see that the output of this ephemeris is in kilometers.
        printf("---finally--- \n");
        printf(" %s[%d] , \n", planet_name(temp_i), temp_i);
        printf("%e \n", coordinates_of_object.x);
        printf("%e \n", coordinates_of_object.y);
        printf("%e \n", coordinates_of_object.z);

*/




        /*

 URANUS BARYCENTER[7] ,
-2.037119e+09
6.638236e+08
2.503946e+08
!!!
 2.99247897e+09   6.63823591e+08   2.50394557e+08

---finally---
 NEPTUNE BARYCENTER[8] ,
-2.037119e+09
-1.676314e+09
-7.909693e+08
!!!
4.23564851e+09  -1.67631442e+09  -7.90969345e+08


---finally---
 PLUTO BARYCENTER[9] ,
1.235212e+09
-2.236791e+09
-1.767728e+09
!!!
1.23521190e+09  -4.54383957e+09  -1.76772807e+09

1. Доделать прогу на Python и сверить результаты. готово!
2. Разобраться с testpo.430
3. Какие даты хранит jpl ? tdb ?
Time stamps in kernel files, and time inputs to and outputs from SPICE routines
reading kernel data and computing derived geometry, are double precision numbers
representing epochs in these two time systems: – Numeric Ephemeris Time (TDB),
expressed as ephemeris seconds past J2000



         */





    }


/*
    coordinates_of_object = calc_geocentric_equ_cartes_pos(seconds_f_jd2000,
                                                           MARS_BARYCENTER,
                                   header_struct_fast_ptr,bsp_430_file);
    printf("---MARS_BARYCENTER---\n");
    printf("%e \n", coordinates_of_object.x);
    printf("%e \n", coordinates_of_object.y);
    printf("%e \n", coordinates_of_object.z);

    coordinates_of_object = calc_geocentric_equ_cartes_pos(seconds_f_jd2000,
                                                           MOON,
                                                           header_struct_fast_ptr,bsp_430_file);

    // You can see that the output of this ephemeris is in kilometers.
    printf("---MOON--- \n");
    printf("%e \n", coordinates_of_object.x);
    printf("%e \n", coordinates_of_object.y);
    printf("%e \n", coordinates_of_object.z);


    coordinates_of_object = calc_geocentric_equ_cartes_pos(seconds_f_jd2000,
                                                           SUN,
                                                           header_struct_fast_ptr,bsp_430_file);
    printf("---SUN---\n");
    printf("%e \n", coordinates_of_object.x);
    printf("%e \n", coordinates_of_object.y);
    printf("%e \n", coordinates_of_object.z);
*/

/*
    printf("fh_struct_fast:\n");
    printf("nd= %d\n", fh_struct_fast.file_record_struct.nd);
    printf("ni = %d\n", fh_struct_fast.file_record_struct.ni);
    printf("fward = %d\n", fh_struct_fast.file_record_struct.fward);
    printf("bward= %d\n", fh_struct_fast.file_record_struct.bward);
    printf("free = %d\n", fh_struct_fast.file_record_struct.free);


    for (int i = 0;
         i < (int)fh_struct_fast.summary_record_struct.total_summaries_number; i++) {

        printf("Summaries[%2d] = ", i);
        printf(" %.2f,",fh_struct_fast.summaries_line_struct[i].segment_start_time); // в сек от J2000
        printf(" %.2f,", fh_struct_fast.summaries_line_struct[i].segment_last_time); // в сек от J2000
        printf("%20s[%3d],  ", planet_name(fh_struct_fast.summaries_line_struct[i].target_code),fh_struct_fast.summaries_line_struct[i].target_code);
        printf("%20s[%2d],  ", planet_name(fh_struct_fast.summaries_line_struct[i].center_code),fh_struct_fast.summaries_line_struct[i].center_code);
        printf(" %d, ", fh_struct_fast.summaries_line_struct[i].ref_frame); // ??? always 1
        printf(" %d, ", fh_struct_fast.summaries_line_struct[i].type_of_data); // always 2 - planet
        printf("%d, ", fh_struct_fast.summaries_line_struct[i].record_start_adress);
        printf("%d, ", fh_struct_fast.summaries_line_struct[i].record_last_adress);
        printf("\n");
    }


        for (int i = 0; i < (int)file_header_ptr->summary_record_struct.total_summaries_number; i++) {
        fread(&file_header_ptr->summaries_line_struct[i], sizeof(file_header_ptr->summaries_line_struct[0]), 1, bsp_430_file);
        printf("Summaries[%2d] = ", i);
        printf(" %.2f,",file_header_ptr->summaries_line_struct[i].segment_start_time); // в сек от J2000
        printf(" %.2f,", file_header_ptr->summaries_line_struct[i].segment_last_time); // в сек от J2000
        printf("%20s[%3d],  ", planet_name(file_header_ptr->summaries_line_struct[i].target_code),
               file_header_ptr->summaries_line_struct[i].target_code);
        printf("%20s[%2d],  ", planet_name(file_header_ptr->summaries_line_struct[i].center_code),
               file_header_ptr->summaries_line_struct[i].center_code);
        // printf(" %d, ", file_header_ptr->summaries_line_struct[i].ref_frame); // ??? always 1
        // printf(" %d, ", file_header_ptr->summaries_line_struct[i].type_of_data); // always 2 - planet
        printf("%d, ", file_header_ptr->summaries_line_struct[i].record_start_adress);
        printf("%d, ", file_header_ptr->summaries_line_struct[i].record_last_adress);
        printf("\n");
    }


*/

    getchar();
    fclose(bsp_430_file);
	return 0;
}