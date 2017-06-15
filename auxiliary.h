
#ifndef BSP_FILE_READER_AUXILIARY_H
#define BSP_FILE_READER_AUXILIARY_H


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <math.h>

#include "fast_de430bsp/fast_de430bsp.h"
#include "slow_de430bsp/slow_de430bsp.h"


#define SOLAR_SYS_BARY.    0
#define MERCURY_BARYCENTER 1
#define VENUS_BARYCENTER   2
#define EARTH_MOON_BARY    3
#define MARS_BARYCENTER    4
#define JUPITER_BARYCENTER 5
#define SATURN_BARYCENTER  6
#define URANUS_BARYCENTER  7
#define NEPTUNE_BARYCENTER 8
#define PLUTO_BARYCENTER   9
#define SUN               10
// the numbers were ordered so one can use planets in for loop
#define MOON             11  // instead of 301
#define EARTH            12  // instead of 399
#define MERCURY          13  //  instead of 199
#define VENUS            14  // instead of 299

#define MARS             499 // instead of 15 doesn't exist in de430.bsp
#define SATURN           699 // instead of 17 doesn't exist in de430.bsp
#define URANUS           799 // instead of 18 doesn't exist in de430.bsp
#define NEPTUNE          899 // instead of 19 doesn't exist in de430.bsp
#define PLUTO            999 // instead of 20 doesn't exist in de430.bsp





//глобальная переменная - указатель на файл de430.bsp
// extern FILE *bsp_430_file;

//глобальная структура для хранения всех хэдеров файла
// либо заполняется функцией load_de430header_fast() from fast_de430bsp.c
// либо грузится из файла функцией load_de430header_slow() from slow_de430bsp.c
extern struct de430bsp_file_header de430bsp_file_header_struct;


double chebyshev(int order, double x, double* data);
double der_chebyshev(int order, double x, double* data);
char* planet_name(int naif_id); // finds and returns planet name by naif_id number
long long int gregorian_date_to_sec_from_j2000(int year, int month, int day,
									int hour, int minutes, int seconds);


struct Coordinates {
    double x;
    double y;
    double z;
    double velocity_x;
    double velocity_y;
    double velocity_z;
};

struct array_inf {
    double init; //start time of the first record in array
    double intlen; // the length of one record (seconds)
    double rsize; // number of elements in one record
    double n; // number of records in segment
};

struct de430bsp_file_record {

	char locidw [8]; // 1. LOCIDW (8 characters, 8 bytes): An identification word (`DAF/xxxx').  [Address 0]
	int nd; // 2. ND(1 integer, 4 bytes) : The number of double prec. components in each array summary.[Address 8]
	int ni; //3. NI ( 1 integer, 4 bytes): The number of integer components in each array summary. [Address 12]
	char locifn[60]; //	4. LOCIFN(60 characters, 60 bytes) : 
					 // The internal name or description of the array file.[Address 16]
	int fward; // 5. FWARD(1 integer, 4 bytes) : 
			   // The record number of the initial summary record in the file.[Address 76]

	int bward; // 6. BWARD(1 integer, 4 bytes) : 
			   //The record number of the final summary record in the file.[Address 80]

	int free; // 7. FREE(1 integer, 4 bytes) : 
			  //The first free address in the file.This is the address at which 
			  //the first element of the next array to be added to the file will be stored.[Address 84]

	char locfmt[8]; //8. LOCFMT(8 characters, 8 bytes) :
					// The character string that indicates the numeric binary format of the DAF.
					// The string has value either "LTL-IEEE" or "BIG-IEEE."[Address 88]

	char prenul[603]; //9. PRENUL(603 characters, 603 bytes) : 
					  //A block of nulls to pad between the last character of LOCFMT and the first character 
					  //of FTPSTR to keep FTPSTR at character 700 (address 699) in a 1024 byte record.[Address 96]

	char ftpstr[28];  //10. FTPSTR(28 characters, 28 bytes) : The FTP validation string. [Address 699]

	char pstnul[297];  //11. PSTNUL(297 characters, 297 bytes) : A block of nulls to pad from the last character 
					   // of FTPSTR to the end of the file record.Note : 
					   // this value enforces the length of the file record as 1024 bytes.[Address 727]

};

//в de430.bsp всего одна summary_record состоящая из
// summary_record+ 14 summaries_line
struct de430bsp_summary_record {
	double next_record_number;
	double previous_record_number;
	double total_summaries_number;
};

struct summaries_line {
	// etbeg, etend, target, observer, frame, type, rbeg, rend
	double segment_start_time;
	double segment_last_time;
	int target_code;
	int center_code;
	int ref_frame; // always 1 in planet SPK
	int type_of_data; // always 2 in planet SPK
	int record_start_adress; //counted in elements, one need to multiply this by 8 to obtain adress in file
	int record_last_adress;
};


struct de430bsp_file_header{
    struct de430bsp_file_record file_record_struct;
    struct de430bsp_summary_record summary_record_struct;
    struct summaries_line summaries_line_struct[15]; //в файле 14 сегментов + solar system
};



// Planet's numbers according to NAIF
/*
For those planets without moons, Mercury and Venus, the barycenter location coincides with
the body center of mass. However do not infer you may interchange use of the planet barycenter ID and the planet ID.
A barycenter has no radii, right ascension/declination of the pole axis, etc.
Use the planet ID when referring to a planet or any property of that planet.
*/

enum planets {
	solar_b = 0,	// 'SOLAR_SYSTEM_BARYCENTER', 'SSB', 'SOLAR SYSTEM BARYCENTER'
	mercury_b = 1,  // 'MERCURY_BARYCENTER', 'MERCURY BARYCENTER'
	venus_b = 2,    // 'VENUS_BARYCENTER', 'VENUS BARYCENTER'
	earth_b = 3,	// 'EARTH_BARYCENTER', 'EMB', 'EARTH MOON BARYCENTER', 'EARTH-MOON BARYCENTER', 'EARTH BARYCENTER'
	mars_b = 4,     // 'MARS_BARYCENTER', 'MARS BARYCENTER',
	jupiter_b = 5, // 'JUPITER_BARYCENTER', 'JUPITER BARYCENTER'
	saturn_b = 6,  // 'SATURN_BARYCENTER', 'SATURN BARYCENTER'
	uranus_b = 7,   // 'URANUS_BARYCENTER', 'URANUS BARYCENTER'
	neptune_b = 8,   // 'NEPTUNE_BARYCENTER', 'NEPTUNE BARYCENTER'
	pluto_b = 9,    // 'PLUTO_BARYCENTER', 'PLUTO BARYCENTER'
	sun = 10,       // 'SUN'

	mercury = 199,  // 'MERCURY'
	venus = 299,    // 'VENUS'
	earth = 399,    // 'EARTH'
	moon = 301,     // 'MOON'
	mars = 499,     // 'MARS'
	jupiter = 599,  // 'JUPITER'
	saturn = 699,   // 'SATURN'
	uranus = 799,   // 'URANUS'
	neptune = 899,  // 'NEPTUNE'
	pluto = 999,    // 'PLUTO'
}planet_numbers;


#endif //BSP_FILE_READER_AUXILIARY_H
