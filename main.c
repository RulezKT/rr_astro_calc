
#include "auxiliary.h"


/* the binary file with DE431 is so large that the following line is necessary
in standard Linux 32-bit environment */
//# define _FILE_OFFSET_BITS 64


double const SEC_TO_RAD = 4.8481368110953599359e-6; /* radians per arc second */
double const RAD_TO_DEG = 5.7295779513082320877e1;
double const PI = 3.14159265358979323846;




//глобальная переменная - указатель на файл de430.bsp
FILE *bsp_430_file = NULL;

//глобальная переменная - указатель на файл testpo.430
FILE *test_file = NULL;



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
FILE* open_bsp_file(char* file_path)
{

	FILE* bsp_file_ptr = NULL;
	char file_name_buffer[70];

	strcpy(file_name_buffer, file_path);

	printf("opening de430.bsp file...\n");
	bsp_file_ptr = fopen(file_path, "rb");

	while (!bsp_file_ptr) {
		printf("Enter DE file name ? ");
		fgets(file_name_buffer, sizeof(file_name_buffer), stdin);

		// вместо знака перевода строки ставим \0
		file_name_buffer[strcspn(file_name_buffer, "\n")] = 0;

		if (!(bsp_file_ptr = fopen(file_name_buffer, "rb")))
			printf("Can't find DE file <%s>\n", file_name_buffer);
	}
	printf("%s is opened\n", file_name_buffer);


	printf("fast checking file integrity...\n");
	if(is_de430bsp_ok(file_name_buffer)){
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


/*
def from_cartesian_to_polar(x, y, z):
    """
    conversion of cartesian coordinates x,y,z
    into polar r, theta, phi
    theta in [-90 deg, +90 deg]
    phi in [0 deg, +360 deg]
    """
    r = math.sqrt(math.pow(x,2)+math.pow(y,2)+math.pow(z,2))
    phi = atan2_in_degrees(y, x)
    if phi < 0:
        phi = phi +360
    theta = atan2_in_degrees(z, (math.sqrt(math.pow(x,2)+math.pow(y,2))))

    return r, theta, phi

T = my_birthday_in_sec/86400/36525
def equatorial_into_ecliptic(t, x, y, z):
    eps_angle = 23.43929111 - (46.8150 + (0.00059 - 0.001813 * t) * t) * t / 3600.0
    c = cos_in_degrees(eps_angle)
    s = sin_in_degrees(eps_angle)
    v = abs(c) * y + s * z
    z = -s * y + c * z
    y = v
    return x, y, z
*/
/*
struct Coordinates equatorial_into_ecliptic(double const T, struct Coordinates coordinates_of_object){

// Разобраться что такое конкретно Т
    double eps_angle = 23.43929111 - (46.8150 + (0.00059 - 0.001813 * T) * T) * T / 3600.0;
    double c = cos_in_degrees(eps_angle);
    double s = sin_in_degrees(eps_angle);
    double v = abs(c) * coordinates_of_object.y + s * coordinates_of_object.z;
    coordinates_of_object.z = -s * coordinates_of_object.y + c * coordinates_of_object.z;
    coordinates_of_object.y = v;

    return coordinates_of_object;
}

*/



int main()
{

	struct Coordinates coordinates_of_object;
	//struct Coordinates new_coordinates_of_object;

	long long int seconds_f_jd2000;

	char* file_path = "de430.bsp";

	bsp_430_file = open_bsp_file(file_path);
	if(!bsp_430_file) return 17;


	struct de430bsp_file_header *header_struct_fast_ptr =  &fh_struct_fast;
	struct de430bsp_file_header *header_struct_slow_ptr =  &fh_struct_slow;
	load_de430header_fast(header_struct_fast_ptr);
	load_de430header_slow(bsp_430_file, header_struct_slow_ptr);

	seconds_f_jd2000 = gregorian_date_to_sec_from_j2000(1978, 5, 17,0, 0, 0);

	//seconds_f_jd2000 = gregorian_date_to_sec_from_j2000(1978, 6, 25,0, 0, 0);

	//seconds_f_jd2000 = gregorian_date_to_sec_from_j2000(2015, 2, 8, 11, 52, 10);

	//seconds_f_jd2000 = gregorian_date_to_sec_from_j2000(2015, 2, 8, 0, 0, 0);
	printf("seconds_f_jd2000 = %lld\n", seconds_f_jd2000);

	//double T = seconds_f_jd2000/86400/36525;
	//int temp_i =0;

	double pp[3];
	double polar[3];
	int ofdate =1;//flag. if 1 the calculates precess
	for(int i=0;i<=14;i++){

		coordinates_of_object = calc_geocentric_equ_cartes_pos(seconds_f_jd2000,
								       i,
								       header_struct_fast_ptr,bsp_430_file);

		// You can see that the output of this ephemeris is in kilometers.
		printf("---finally--- \n");
		printf(" %s[%d] , \n", planet_name(i), i);
		//printf("%.20e \n", coordinates_of_object.x);
		//printf("%.20e \n", coordinates_of_object.y);
		//printf("%.20e \n", coordinates_of_object.z);


		pp[0] = coordinates_of_object.x/AU; //we need coords not in km but in AU
		pp[1] = coordinates_of_object.y/AU; //we need coords not in km but in AU
		pp[2] = coordinates_of_object.z/AU; //we need coords not in km but in AU

		//from swemini te = 2489933.501118
		//lonlat(pp, 2489933.501118, polar, ofdate);
		lonlat(pp, seconds_f_jd2000/SEC_IN_1_DAY+JD2000+0.5, polar, ofdate);
		// printf("polar coords. ecliptic long = %f, ecliptic lat = %f, r = %f\n\n", polar[0], polar[1], polar[2]);
		printf( "ecliptic long" );
		dms(polar[0]);
		printf("\n");
	}

	//Добавить знаки Зодиака

	//разобраться с неправильным расчетом скоростей
	//testpo_430_tests(bsp_430_file,header_struct_fast_ptr);

	//compare_bsp_swiss_files(header_struct_fast_ptr, bsp_430_file);
	//make_bsp_swiss_files(header_struct_fast_ptr, bsp_430_file);



	//getchar();
	fclose(bsp_430_file);
	return 0;
}