//
// Created by RUSTAM RUDAKOV on 19/06/2017.
//

#include "tests.h"
#include "swephexp.h" 	/* this includes  "sweodef.h" */



//opens "testpo.430" file
FILE* open_test_file(char* test_file_path){

	FILE* test_file_ptr = NULL;
	char file_name_buffer[70];

	strcpy(file_name_buffer, test_file_path);

	printf("opening testpo.430 file...\n");
	test_file_ptr = fopen(test_file_path, "r");

	while (!test_file_ptr) {
		printf("Enter Test file name ? ");
		fgets(file_name_buffer, sizeof(file_name_buffer), stdin);
		// вместо знака перевода строки ставим \0
		file_name_buffer[strcspn(file_name_buffer, "\n")] = 0;
		if (!(test_file_ptr = fopen(file_name_buffer, "r")))
			printf("Can't find Test file <%s>\n", file_name_buffer);
	}
	printf("%s is opened\n", file_name_buffer);

	return test_file_ptr;
}



struct Coordinates position(long long int jed, int target,
							struct de430bsp_file_header *header_struct_fast_ptr, FILE *bsp_430_file)
{

	struct Coordinates coord1, coord2;

	switch(target){
		case 3:
			coord1 = get_coordinates(jed, 3, 0, header_struct_fast_ptr, bsp_430_file);
			coord2 = get_coordinates(jed, 399, 3, header_struct_fast_ptr, bsp_430_file);
			coord1.x += coord2.x;
			coord1.y += coord2.y;
			coord1.z += coord2.z;
			coord1.velocity_x += coord2.velocity_x;
			coord1.velocity_y += coord2.velocity_y;
			coord1.velocity_z += coord2.velocity_z;
			break;
		case 10:
			coord1 = get_coordinates(jed, 3, 0, header_struct_fast_ptr, bsp_430_file);
			coord2 = get_coordinates(jed, 301, 3, header_struct_fast_ptr, bsp_430_file);
			coord1.x += coord2.x;
			coord1.y += coord2.y;
			coord1.z += coord2.z;
			coord1.velocity_x += coord2.velocity_x;
			coord1.velocity_y += coord2.velocity_y;
			coord1.velocity_z += coord2.velocity_z;
			break;
		case 11:
			coord1 = get_coordinates(jed, 10, 0, header_struct_fast_ptr, bsp_430_file);
			break;
		case 12:
			coord1.x =0;
			coord1.y =0;
			coord1.z =0;
			coord1.velocity_x =0;
			coord1.velocity_y =0;
			coord1.velocity_z =0;
			break;
		case 13:
			coord1 = get_coordinates(jed, 3, 0, header_struct_fast_ptr, bsp_430_file);
			break;
		default:
			coord1 = get_coordinates(jed, target, 0, header_struct_fast_ptr, bsp_430_file);
			break;

	}


	return coord1;
}



int testpo_430_tests(FILE *bsp_430_file,
					 struct de430bsp_file_header *header_struct_fast_ptr)
{
	FILE *test_file;

	char buffer[255];

	char* test_file_path = "testpo.430";

	test_file = open_test_file(test_file_path);
	if(!test_file) return 17;


	do{
		fgets(buffer,255,test_file);
		buffer[3]='\0';

	}while(strcmp(buffer,"EOT")!=0);


	printf("LINE  JED    t# c# x#  --- JPL value ---  --- user value --   -- difference --");

	int total_lines =0;
	int skipped_lines =0;
	int error_lines=0;
	int success_lines=0;
	double test_jed =0;
	int test_target;
	int test_center;
	int test_number_of_coord;
	double test_coord;

	double coordinate_to_test;
	struct Coordinates target_coord;
	struct Coordinates center_coord;

	// 1550.01.01    2287195.5
	test_jed = gregorian_date_to_sec_from_j2000(1550, 1, 1, 0, 0, 0);
	printf("\n %f,     %f\n", test_jed, test_jed/86400+2451545.0);


	double first_de430_day =
		header_struct_fast_ptr->summaries_line_struct[1].segment_start_time
		/SEC_IN_1_DAY
		+JD2000;

	double last_de430_day =
		header_struct_fast_ptr->summaries_line_struct[1].segment_last_time
		/SEC_IN_1_DAY
		+JD2000;




	while(fgets(buffer, 255, test_file) != NULL)
	{
		total_lines++;
/*****  Read a value from the test case; Skip if not within the time-range
        of the present version of the ephemeris.                            */


		/* перепрыгиваем 15 char - номер эфемерид и грегорианскую дату
				 и считываем
				 julian ephemeris date                                       *
		     target number (1-mercury, ...,3-earth, ,,,9-pluto, 10-moon, 11-sun,    *
		                   12-solar system barycenter, 13-earth-moon barycenter    *
		                   14-nutations, 15-librations)                            *
		     center number (same codes as target number)                            *
		     coordinate number (1-x, 2-y, ... 6-zdot)                               *
		     coordinate  [au, au/day].
		 */
		sscanf(buffer+15," %lf %d %d %d %lf",
			   &test_jed,&test_target,&test_center,&test_number_of_coord,&test_coord);


		/* ephemeris starts later than test points */
		if(test_jed<first_de430_day){
			printf("test_jed to small = %f , \n",test_jed);
			continue;
		}

		/* ephemeris ends earlier than test points */
		if(test_jed>last_de430_day){
			printf("test_jed to BIG = %f , \n",test_jed);
			break;

		}

/*
	      1 = mercury           8 = neptune                             **
**            2 = venus             9 = pluto                               **
**            3 = earth            10 = moon                                **
**            4 = mars             11 = sun                                 **
**            5 = jupiter          12 = solar-system barycenter             **
**            6 = saturn           13 = earth-moon barycenter               **
**            7 = uranus           14 = nutations (longitude and obliq)     **
**                                 15 = librations, if on eph. file
*/

		long long int test_jed_in_seconds = floor((test_jed-JD2000)*SEC_IN_1_DAY);
		// 2688952.5  9 13  5        0.00003367832325310824
		// coordinate_to_test = :  0.00000000038979540802
		// неправильно считает скорости (координаты с 4 по 6)
		// не учитывает пробел в 10 дней в октябре 1582 года


		if(test_target == 14 || test_target == 15){
			target_coord = position(test_jed_in_seconds, test_target, header_struct_fast_ptr,bsp_430_file);
			switch(test_number_of_coord){
				case 1:
					coordinate_to_test = target_coord.x;
					break;
				case 2:
					coordinate_to_test = target_coord.y;
					break;
				case 3:
					coordinate_to_test = target_coord.z;
					break;
				case 4:
					coordinate_to_test = target_coord.velocity_x;
					break;
				case 5:
					coordinate_to_test = target_coord.velocity_y;
					break;
				case 6:
					coordinate_to_test = target_coord.velocity_z;
					break;
				default:
					coordinate_to_test =0;
					printf("This can not be! Default is impossible!");
					getchar();
			}


		} else{

			target_coord = position(test_jed_in_seconds, test_target, header_struct_fast_ptr,bsp_430_file);
			center_coord = position(test_jed_in_seconds, test_center, header_struct_fast_ptr,bsp_430_file);

			switch(test_number_of_coord){
				case 1:
					coordinate_to_test = (target_coord.x - center_coord.x)/AU;
					break;
				case 2:
					coordinate_to_test = (target_coord.y - center_coord.y)/AU;
					break;
				case 3:
					coordinate_to_test = (target_coord.z - center_coord.z)/AU;
					break;
				case 4:
					coordinate_to_test = (target_coord.velocity_x - center_coord.velocity_x)/AU;
					break;
				case 5:
					coordinate_to_test = (target_coord.velocity_y - center_coord.velocity_y)/AU;
					break;
				case 6:
					coordinate_to_test = (target_coord.velocity_z - center_coord.velocity_z)/AU;
					break;
				default:
					coordinate_to_test =0;
					printf("This can not be! Default is impossible!");
					getchar();
			}


		}
		//printf("coordinate_to_test = %f,\n", coordinate_to_test);



		double delta = coordinate_to_test - test_coord;

		if(coordinate_to_test == 0 && test_coord !=0) skipped_lines++;


		//printf("delta = %f,  test_coord = %f, \n", delta, test_coord);
		if(fabs(delta)>EPSILON && (fabs(coordinate_to_test) !=0)){
			printf("\nfound error !!!!!\n");
			printf("it's here : %lf %d %d %d !!!\n",
				   test_jed, test_target, test_center, test_number_of_coord);
			printf("test_coord = :        %.20f\ncoordinate_to_test = :  %.20f\n",
				   test_coord, coordinate_to_test);
			error_lines++;

		} else if(fabs(delta)<=EPSILON && coordinate_to_test !=0){
			printf("OK : %lf %d %d %d !!!\n",
				   test_jed, test_target, test_center, test_number_of_coord);
			printf("test_coord = :        %lf\ncoordinate_to_test = :  %lf\n",
				   test_coord, coordinate_to_test);
			success_lines++;
		}



	}

	printf("\nnumber of total_lines = %d\n", total_lines);
	printf("\nnumber of skipped_liness = %d\n", skipped_lines);
	printf("\nnumber of errors = %d\n", error_lines);
	printf("\nnumber of success_lines = %d\n", success_lines);
	printf("\nnumber = %d\n",skipped_lines+success_lines+error_lines);
	fclose(test_file);

	return 0;

}


int make_bsp_swiss_files(struct de430bsp_file_header *header_struct_fast_ptr, FILE *bsp_430_file) {

	FILE *file_to_write = fopen("my_bsp.tests", "w");
	char str_to_write[82]; // 10 планет по 8 знаков на каждую + символ перевода строки + символ конца строки
	char temp_string[9];
	int total_strings = 0;

	long long int jd_in_seconds;
	struct Coordinates coordinates_of_object;
	double pp[3];
	double polar[3];

	int ofdate = 1;//flag. if 1 the calculates precess

	// разобраться почему не работают экстремумы
	for (double i = 2287185.50; i <= 2688975.50; i += 0.5) {
		total_strings++;
		str_to_write[0] = 0;
		jd_in_seconds = (i - JD2000) * SEC_IN_1_DAY;

		for (int p = SUN; p <= MOON; p++) {

			coordinates_of_object = calc_geocentric_equ_cartes_pos(jd_in_seconds,
																   p,
																   header_struct_fast_ptr, bsp_430_file);

			pp[0] = coordinates_of_object.x / AU; //we need coords not in km but in AU
			pp[1] = coordinates_of_object.y / AU; //we need coords not in km but in AU
			pp[2] = coordinates_of_object.z / AU; //we need coords not in km but in AU
			lonlat(pp, i, polar, ofdate);

			/*
			 * print the longtitude to the file
			 * the order is Sun, Moon, Mercury, Venus, Mars, Jupiter, Saturn, Uranus,  Neptune, Pluto
			 */
			snprintf(temp_string, 9, "%3.3f ", polar[0] * RAD_TO_DEG);
			strcat(str_to_write, temp_string);
		}


		for (int p = 1; p <= 9; p++) {
			if (p == 3) continue; //we don't need Earth here

			coordinates_of_object = calc_geocentric_equ_cartes_pos(jd_in_seconds,
																   p,
																   header_struct_fast_ptr, bsp_430_file);

			pp[0] = coordinates_of_object.x / AU; //we need coords not in km but in AU
			pp[1] = coordinates_of_object.y / AU; //we need coords not in km but in AU
			pp[2] = coordinates_of_object.z / AU; //we need coords not in km but in AU
			lonlat(pp, i, polar, ofdate);

			/*
			 * print the longtitude to the file
			 * the order is Sun, Moon, Mercury, Venus, Mars, Jupiter, Saturn, Uranus,  Neptune, Pluto
			 */
			snprintf(temp_string, 9, "%3.3f ", polar[0] * RAD_TO_DEG);
			strcat(str_to_write, temp_string);
		}

		strcat(str_to_write, "\n");
		fputs(str_to_write, file_to_write);
	}
	printf("\ntotal_strings = %d\n", total_strings);
	fclose(file_to_write);



	char sdate[AS_MAXCH], snam[40], serr[AS_MAXCH];
	int jday = 1, jmon = 1, jyear = 2000;
	double jut = 0.0;
	double tjd, te, x2[6];
	int32 iflag, iflgret;
	int p;
	swe_set_ephe_path("/Users/rulez/swisseph/ephe"); //рекомендуемая к запуску функция, даже если путь указывать не надо
	iflag = SEFLG_SPEED;

	file_to_write = fopen("swis_orig.tests","w");
	str_to_write[0] =0; // 10 планет по 8 знаков на каждую + символ перевода строки + символ конца строки
	temp_string[0]= 0;
	total_strings=0;
	for(double i = 2287185.50; i <= 2688975.50; i+=0.5){
		total_strings++;
		str_to_write[0] = 0;
		for (int p = SE_SUN; p <= SE_PLUTO; p++) {
			if (p == SE_EARTH) continue;
			/*
			 * do the coordinate calculation for this planet p
			 */
			iflgret = swe_calc(i, p, iflag, x2, serr);
			/*
			 * if there is a problem, a negative value is returned and an
			 * errpr message is in serr.
			 */
			if (iflgret < 0)
				printf("error: %s\n", serr);
			else if (iflgret != iflag)
				printf("warning: iflgret != iflag. %s\n", serr);

			/*
			 * print the longtitude to the file
			 * the order is Sun, Moon, Mercury, Venus, Mars, Jupiter, Saturn, Uranus,  Neptune, Pluto
			 */
			snprintf(temp_string, 9, "%3.3f ",x2[0]);
			strcat(str_to_write, temp_string);
		}
		strcat(str_to_write,"\n");
		fputs(str_to_write, file_to_write);
	}
	printf("total_strings = %d\n", total_strings);
	fclose(file_to_write);


	return 0;

}



int compare_bsp_swiss_files(struct de430bsp_file_header *header_struct_fast_ptr, FILE *bsp_430_file)
{
//compare results


FILE *my_file = fopen("my_bsp.tests","r");
FILE *swiss_file = fopen("swis_orig.tests","r");
char my_buffer[82];
char swiss_buffer[82];

#define MIN_ERROR_LEVEL 0.025
#define MAX_ERROR_LEVEL 359.98

int strings_count =0;
int error_strings =0;

float my_sun, my_moon, my_mercury, my_venus, my_mars, my_jupiter,
	my_saturn, my_uranus, my_neptune, my_pluto;

float swiss_sun, swiss_moon, swiss_mercury, swiss_venus, swiss_mars, swiss_jupiter,
	swiss_saturn, swiss_uranus, swiss_neptune, swiss_pluto;


if(!my_file || !swiss_file) {
printf("coudn't open files...sorry\n");
return 0;
}
while(fgets(my_buffer, 82, my_file) != NULL && fgets(swiss_buffer, 82, swiss_file) != NULL) {

strings_count++;


sscanf(my_buffer," %f %f %f %f %f %f %f %f %f %f",
&my_sun, &my_moon, &my_mercury, &my_venus, &my_mars, &my_jupiter,
&my_saturn, &my_uranus, &my_neptune, &my_pluto);

sscanf(swiss_buffer," %f %f %f %f %f %f %f %f %f %f",
&swiss_sun, &swiss_moon, &swiss_mercury, &swiss_venus, &swiss_mars, &swiss_jupiter,
&swiss_saturn, &swiss_uranus, &swiss_neptune, &swiss_pluto);


if (fabs(my_sun-swiss_sun)>MIN_ERROR_LEVEL && fabs(my_sun-swiss_sun)<MAX_ERROR_LEVEL ||
fabs(my_moon-swiss_moon)>MIN_ERROR_LEVEL && fabs(my_moon-swiss_moon)<MAX_ERROR_LEVEL||
fabs(my_mercury-swiss_mercury)>MIN_ERROR_LEVEL && fabs(my_mercury-swiss_mercury)<MAX_ERROR_LEVEL ||
fabs(my_venus-swiss_venus)>MIN_ERROR_LEVEL && fabs(my_venus-swiss_venus)<MAX_ERROR_LEVEL ||
fabs(my_mars-swiss_mars)>MIN_ERROR_LEVEL && fabs(my_mars-swiss_mars)<MAX_ERROR_LEVEL ||
fabs(my_jupiter-swiss_jupiter)>MIN_ERROR_LEVEL && fabs(my_jupiter-swiss_jupiter)<MAX_ERROR_LEVEL ||
fabs(my_saturn-swiss_saturn)>MIN_ERROR_LEVEL && fabs(my_saturn-swiss_saturn)<MAX_ERROR_LEVEL ||
fabs(my_uranus-swiss_uranus)>MIN_ERROR_LEVEL && fabs(my_uranus-swiss_uranus)<MAX_ERROR_LEVEL ||
fabs(my_neptune-swiss_neptune)>MIN_ERROR_LEVEL && fabs(my_neptune-swiss_neptune)<MAX_ERROR_LEVEL ||
fabs(my_pluto-swiss_pluto)>MIN_ERROR_LEVEL && fabs(my_pluto-swiss_pluto)<MAX_ERROR_LEVEL){


	printf("Big difference found...\n");

	error_strings++;

	printf("mybuffer: %s\n", my_buffer);

	printf("swissbuffer: %s\n", swiss_buffer);

}



}

printf("processed %d strings\n",strings_count);
printf("wrong strings: %d\n",error_strings);


fclose(my_file);
fclose(swiss_file);

}

