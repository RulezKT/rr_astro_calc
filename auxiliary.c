
#include "auxiliary.h"

#define JD2000 2451545.0 //12:00 UT on January 1, 2000

//NAIF names and codes for planets
char* planets_names[21] = {
	"SOLAR SYS. BARY.",        // 0
	"MERCURY BARYCENTER",	   // 1
	"VENUS BARYCENTER",		   // 2
	"EARTH-MOON BARY.",        // 3
	"MARS BARYCENTER",         // 4
	"JUPITER BARYCENTER",      // 5
	"SATURN BARYCENTER",       // 6
	"URANUS BARYCENTER",       // 7
	"NEPTUNE BARYCENTER",      // 8
	"PLUTO BARYCENTER",        // 9
	"SUN",                     // 10
    "MOON",     // 301  instead of 11
	"EARTH",   // 399 instead of 12
    "MERCURY", // 199 instead of 13
    "VENUS",   // 299 instead of 14
};

double chebyshev(int order, double x, double* data) {
	// Evaluate a Chebyshev polynomial
	double bk;
	double two_x = 2 * x;
	double bkp2 = data[order];
	double bkp1 = two_x * bkp2 + data[order - 1];

	for (int n = order - 2; n > 0; n--) {
		bk = data[n] + two_x * bkp1 - bkp2;
		bkp2 = bkp1;
		bkp1 = bk;
	}
	return (data[0] + x * bkp1 - bkp2);
}

double der_chebyshev(int order, double x, double* data) {
	//Evaluate the derivative of a Chebyshev polynomial
    double bk;
    double two_x = 2 * x;
    double bkp2 = order * data[order];
    double bkp1 = two_x * bkp2 + (order - 1) * data[order - 1];

	for (int n = order - 2; n > 1; n--) {
		bk = n * data[n] + two_x * bkp1 - bkp2;
		bkp2 = bkp1;
		bkp1 = bk;
	}

	return (data[1] + two_x * bkp1 - bkp2);
}

// finds and returns planet name by naif_id number
char* planet_name(int naif_id) {

	if (naif_id >= 0 && naif_id <= 10) return planets_names[naif_id];

	int temp_id;
	switch (naif_id) {
	case 199:
    case 13:
		temp_id = 13;
		break;
	case 299:
    case 14:
		temp_id = 14;
		break;
	case 399:
    case 12:
		temp_id = 12;
		break;
	case 301:
    case 11:
		temp_id = 11;
		break;
/*	case 499:
		temp_id = 15;
		break;
	case 599:
		temp_id = 16;
		break;
	case 699:
		temp_id = 17;
		break;
	case 799:
		temp_id = 18;
		break;
	case 899:
		temp_id = 19;
		break;
	case 999:
		temp_id = 20;
		break;*/
	default:
		return "Unknown planet!!!";
		break;
	}
	return planets_names[temp_id];
}

long long int gregorian_date_to_sec_from_j2000(int year, int month, int day,
									 int hour, int minutes, int seconds) {
	//#define JD2000 2451545.0 //12:00 UT on January 1, 2000
	// https://ru.wikipedia.org/wiki/%D0%AE%D0%BB%D0%B8%D0%B0%D0%BD%D1%81%D0%BA%D0%B0%D1%8F_%D0%B4%D0%B0%D1%82%D0%B0

	//You must compute first the number of years (y) and months (m) since March 1 −4800 (March 1, 4801 BC)
    int a = floor((14 - month) / 12);
	long long int y = year + 4800 - a;
	int m = month + 12 * a - 3;

	static int n_of_calculations = 0;

	n_of_calculations++;
	printf("from greg_to_sec Calculation number %d\n", n_of_calculations);

	//All years in the BC era must be converted to astronomical years, 
	//so that 1 BC is year 0, 2 BC is year −1, etc. Convert to a negative number, then increment toward zero.
	//JDN — это номер юлианского дня (англ. Julian Day Number), 
	//который начинается в полдень числа, для которого производятся вычисления.
	//Then, if starting from a Gregorian calendar date compute:
	double jdn = day + floor((153 * m + 2) / 5) + 365 * y + floor(y / 4) - floor(y / 100) + floor(y / 400) - 32045;
	double temp_jdn = jdn;
	int temp_hour = hour;
	// теперь отталкиваясь от полдня корректируемся на часы минуты и секунды
	if (hour < 12) {
		jdn -= 0.5;
	}
	else {
		hour -= 12;
	}
	jdn += (hour * 60 * 60 + minutes * 60 + seconds) / 86400.0;
	printf("from greg_to_sec Julian Day = %.10f\n", jdn);

	double date_in_sec = (jdn - JD2000) * 86400;
	printf(" from greg_to_sec date in seconds =  %f\n", date_in_sec);

	double date_in_sec1 = (temp_jdn - JD2000) * 86400;
	if (temp_hour < 12) date_in_sec1 -= 43200;
	date_in_sec1 += temp_hour * 60 * 60 + minutes * 60 + seconds;
	printf(" from greg_to_secdate in seconds1 = %f\n", date_in_sec1);
	printf("\n");

	return  (long long int)date_in_sec1;
}
