
#include "auxiliary.h"

double eps = 0.0; /* The computed obliquity in radians */
double coseps = 0.0; /* Cosine of the obliquity */
double sineps = 0.0; /* Sine of the obliquity */
double static jdeps = -1.0; /* Date for which obliquity was last computed */



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

/*
 * def chebyshev(order, x, data):
    """Evaluate a Chebyshev polynomial"""
    two_x = 2 * x
    bkp2 = data[order]
    bkp1 = two_x * bkp2 + data[order - 1]
    for n in range(order - 2, 0, -1):
        bk = data[n] + two_x * bkp1 - bkp2
        bkp2 = bkp1
        bkp1 = bk

    return data[0] + x * bkp1 - bkp2

def der_chebyshev(order, x, data):
    """Evaluate the derivative of a Chebyshev polynomial"""
    two_x = 2 * x
    bkp2 = order * data[order]
    bkp1 = two_x * bkp2 + (order - 1) * data[order - 1]
    for n in range(order - 2, 1, -1):
        bk = n * data[n] + two_x * bkp1 - bkp2
        bkp2 = bkp1
        bkp1 = bk
    return data[1] + two_x * bkp1 - bkp2
 */


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




/*							atan2()
 *
 *	Quadrant correct inverse circular tangent
 *
 *
 *
 * SYNOPSIS:
 *
 * double x, y, z, atan2();
 *
 * z = atan2( x, y );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns radian angle between 0 and +2pi whose tangent
 * is y/x.
 *
 *
 *
 * ACCURACY:
 *
 * See atan.c.
 *
 */


/*
Cephes Math Library Release 2.0:  April, 1987
Copyright 1984, 1987 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
Certain routines from the Library, including this one, may
be used and distributed freely provided this notice is retained
and source code is included with all distributions.
*/

double zatan2(double x, double y)
{
	double z, w;
	short code;


	code = 0;

	if( x < 0.0 )
		code = 2;
	if( y < 0.0 )
		code |= 1;

	if( x == 0.0 )
	{
		if( code & 1 )
			return( 1.5*PI );
		if( y == 0.0 )
			return( 0.0 );
		return( 0.5*PI );
	}

	if( y == 0.0 )
	{
		if( code & 2 )
			return( PI );
		return( 0.0 );
	}


	switch( code )
	{
		default:
		case 0: w = 0.0; break;
		case 1: w = 2.0 * PI; break;
		case 2:
		case 3: w = PI; break;
	}

	z = atan( y/x );

	return( w + z );
}

/* Radians to degrees, minutes, seconds
 */
int dms(double x)
{
	double s;
	int d, m;

	s = x * RAD_TO_DEG;
	if( s < 0.0 )
	{
		printf( " -" );
		s = -s;
	}
	else
	{
		printf( "  " );
	}
	d = (int) s;
	s -= d;
	s *= 60;
	m = (int) s;
	s -= m;
	s *= 60;
	printf( "%3dd %02d\' %05.2f\"  ", d, m, s );
	return(0);
}






/* J. L. Simon, P. Bretagnon, J. Chapront, M. Chapront-Touze', G. Francou,
   and J. Laskar, "Numerical Expressions for precession formulae and
   mean elements for the Moon and the planets," Astronomy and Astrophysics
   282, 663-683 (1994)  */

/* IAU Coefficients are from:
 * J. H. Lieske, T. Lederle, W. Fricke, and B. Morando,
 * "Expressions for the Precession Quantities Based upon the IAU
 * (1976) System of Astronomical Constants,"  Astronomy and Astrophysics
 * 58, 1-16 (1977).
 *
 * Before or after 200 years from J2000, the formula used is from:
 * J. Laskar, "Secular terms of classical planetary theories
 * using the results of general theory," Astronomy and Astrophysics
 * 157, 59070 (1986).
 *
 *  See precess.c and page B18 of the Astronomical Almanac.
 *
 */
/* The results of the program are returned in these
 * global variables:
 */

int epsiln(double J) {
/* Julian date input */

	double T;

	if( J == jdeps )
		return(0);


	T = (J - 2451545.0)/365250.0;

	eps = ((((((((( 2.45e-10*T + 5.79e-9)*T + 2.787e-7)*T
				 + 7.12e-7)*T - 3.905e-5)*T - 2.4967e-3)*T
			  - 5.138e-3)*T + 1.9989)*T - 0.0152)*T - 468.0927)*T
		  + 84381.412;



	eps *= SEC_TO_RAD;
	coseps = cos( eps );
	sineps = sin( eps );
	jdeps = J;

	return(0);
}


/* Precession of the equinox and ecliptic
 * from epoch Julian date J to or from J2000.0
 *
 * Program by Steve Moshier.  */

#if (DE200CD | DE200 | DE102 | SSYSTEM)
#define IAU 1
#endif

#define WILLIAMS 1
/* James G. Williams, "Contributions to the Earth's obliquity rate,
   precession, and nutation,"  Astron. J. 108, 711-724 (1994)  */

#define SIMON 0
/* J. L. Simon, P. Bretagnon, J. Chapront, M. Chapront-Touze', G. Francou,
   and J. Laskar, "Numerical Expressions for precession formulae and
   mean elements for the Moon and the planets," Astronomy and Astrophysics
   282, 663-683 (1994)  */

#ifndef IAU
#define IAU 0
#endif
/* IAU Coefficients are from:
 * J. H. Lieske, T. Lederle, W. Fricke, and B. Morando,
 * "Expressions for the Precession Quantities Based upon the IAU
 * (1976) System of Astronomical Constants,"  Astronomy and
 * Astrophysics 58, 1-16 (1977).
 */

#define LASKAR 0
/* Newer formulas that cover a much longer time span are from:
 * J. Laskar, "Secular terms of classical planetary theories
 * using the results of general theory," Astronomy and Astrophysics
 * 157, 59070 (1986).
 *
 * See also:
 * P. Bretagnon and G. Francou, "Planetary theories in rectangular
 * and spherical variables. VSOP87 solutions," Astronomy and
 * Astrophysics 202, 309-315 (1988).
 *
 * Laskar's expansions are said by Bretagnon and Francou
 * to have "a precision of about 1" over 10000 years before
 * and after J2000.0 in so far as the precession constants p^0_A
 * and epsilon^0_A are perfectly known."
 *
 * Bretagnon and Francou's expansions for the node and inclination
 * of the ecliptic were derived from Laskar's data but were truncated
 * after the term in T**6. I have recomputed these expansions from
 * Laskar's data, retaining powers up to T**10 in the result.
 *
 * The following table indicates the differences between the result
 * of the IAU formula and Laskar's formula using four different test
 * vectors, checking at J2000 plus and minus the indicated number
 * of years.
 *
 *   Years       Arc
 * from J2000  Seconds
 * ----------  -------
 *        0	  0
 *      100	.006
 *      200     .006
 *      500     .015
 *     1000     .28
 *     2000    6.4
 *     3000   38.
 *    10000 9400.
 */

#define DOUBLE double
#define COS cos
#define SIN sin

//extern DOUBLE SEC_TO_RAD; /* = 4.8481368110953599359e-6 radians per arc second */

/* In WILLIAMS and SIMON, Laskar's terms of order higher than t^4
   have been retained, because Simon et al mention that the solution
   is the same except for the lower order terms.  */
#if WILLIAMS
static DOUBLE pAcof[] = {
#if (DE403 | LIB403 | DE404 | DE405 | DE406 | DE406CD )
-8.66e-10, -4.759e-8, 2.424e-7, 1.3095e-5, 1.7451e-4, -1.8055e-3,
 -0.235316, 0.076, 110.5414, 50287.91959
#else
	-8.66e-10, -4.759e-8, 2.424e-7, 1.3095e-5, 1.7451e-4, -1.8055e-3,
	-0.235316, 0.076, 110.5407, 50287.70000
#endif /* not DE403 */
};
#endif
#if SIMON
/* Precession coefficients from Simon et al: */
static DOUBLE pAcof[] = {
 -8.66e-10, -4.759e-8, 2.424e-7, 1.3095e-5, 1.7451e-4, -1.8055e-3,
 -0.235316, 0.07732, 111.2022, 50288.200 };
#endif
#if LASKAR
/* Precession coefficients taken from Laskar's paper: */
static DOUBLE pAcof[] = {
 -8.66e-10, -4.759e-8, 2.424e-7, 1.3095e-5, 1.7451e-4, -1.8055e-3,
 -0.235316, 0.07732, 111.1971, 50290.966 };
#endif
#if WILLIAMS
/* Pi from Williams' 1994 paper, in radians.  No change in DE403.  */
static DOUBLE nodecof[] = {
	6.6402e-16, -2.69151e-15, -1.547021e-12, 7.521313e-12, 1.9e-10,
	-3.54e-9, -1.8103e-7,  1.26e-7,  7.436169e-5,
	-0.04207794833,  3.052115282424};
/* pi from Williams' 1994 paper, in radians.  No change in DE403.  */
static DOUBLE inclcof[] = {
	1.2147e-16, 7.3759e-17, -8.26287e-14, 2.503410e-13, 2.4650839e-11,
	-5.4000441e-11, 1.32115526e-9, -6.012e-7, -1.62442e-5,
	0.00227850649, 0.0 };
#endif
#if SIMON
static DOUBLE nodecof[] = {
6.6402e-16, -2.69151e-15, -1.547021e-12, 7.521313e-12, 1.9e-10,
-3.54e-9, -1.8103e-7, 2.579e-8, 7.4379679e-5,
-0.0420782900, 3.0521126906};

static DOUBLE inclcof[] = {
1.2147e-16, 7.3759e-17, -8.26287e-14, 2.503410e-13, 2.4650839e-11,
-5.4000441e-11, 1.32115526e-9, -5.99908e-7, -1.624383e-5,
 0.002278492868, 0.0 };
#endif
#if LASKAR
/* Node and inclination of the earth's orbit computed from
 * Laskar's data as done in Bretagnon and Francou's paper.
 * Units are radians.
 */
static DOUBLE nodecof[] = {
6.6402e-16, -2.69151e-15, -1.547021e-12, 7.521313e-12, 6.3190131e-10,
-3.48388152e-9, -1.813065896e-7, 2.75036225e-8, 7.4394531426e-5,
-0.042078604317, 3.052112654975 };

static DOUBLE inclcof[] = {
1.2147e-16, 7.3759e-17, -8.26287e-14, 2.503410e-13, 2.4650839e-11,
-5.4000441e-11, 1.32115526e-9, -5.998737027e-7, -1.6242797091e-5,
 0.002278495537, 0.0 };
#endif


/* Subroutine arguments:
 *
 * R = rectangular equatorial coordinate vector to be precessed.
 *     The result is written back into the input vector.
 * J = Julian date
 * direction =
 *      Precess from J to J2000: direction = 1
 *      Precess from J2000 to J: direction = -1
 * Note that if you want to precess from J1 to J2, you would
 * first go from J1 to J2000, then call the program again
 * to go from J2000 to J2.
 */

int precess(DOUBLE R[],DOUBLE J, int direction)
{
	DOUBLE A, B, T, pA, W, z, TH;
	DOUBLE x[3];
	DOUBLE *p;
	int i;
#if IAU
	DOUBLE sinth, costh, sinZ, cosZ, sinz, cosz, Z;
#endif

	if( J == JD2000 )
		return(0);
/* Each precession angle is specified by a polynomial in
 * T = Julian centuries from J2000.0.  See AA page B18.
 */
	T = (J - JD2000)/36525.0;

#if IAU
	/* Use IAU formula only for a few centuries, if at all.  */
if( fabs(T) > 2.0 )
	goto laskar;

Z =  (( 0.017998*T + 0.30188)*T + 2306.2181)*T*STR;
z =  (( 0.018203*T + 1.09468)*T + 2306.2181)*T*STR;
TH = ((-0.041833*T - 0.42665)*T + 2004.3109)*T*STR;

sinth = SIN(TH);
costh = COS(TH);
sinZ = SIN(Z);
cosZ = COS(Z);
sinz = SIN(z);
cosz = COS(z);
A = cosZ*costh;
B = sinZ*costh;

if( direction < 0 )
	{ /* From J2000.0 to J */
	x[0] =    (A*cosz - sinZ*sinz)*R[0]
	        - (B*cosz + cosZ*sinz)*R[1]
	                  - sinth*cosz*R[2];

	x[1] =    (A*sinz + sinZ*cosz)*R[0]
	        - (B*sinz - cosZ*cosz)*R[1]
	                  - sinth*sinz*R[2];

	x[2] =              cosZ*sinth*R[0]
	                  - sinZ*sinth*R[1]
	                       + costh*R[2];
	}
else
	{ /* From J to J2000.0 */
	x[0] =    (A*cosz - sinZ*sinz)*R[0]
	        + (A*sinz + sinZ*cosz)*R[1]
	                  + cosZ*sinth*R[2];

	x[1] =   -(B*cosz + cosZ*sinz)*R[0]
	        - (B*sinz - cosZ*cosz)*R[1]
	                  - sinZ*sinth*R[2];

	x[2] =             -sinth*cosz*R[0]
	                  - sinth*sinz*R[1]
	                       + costh*R[2];
	}
goto done;

laskar:
#endif /* IAU */

/* Implementation by elementary rotations. */

/* Obliquity of the equator at initial date.  */
	if( direction == 1 )
		epsiln( J ); /* To J2000 */
	else
		epsiln( JD2000 ); /* From J2000 */

/* Precession in longitude
 */
	T /= 10.0; /* thousands of years */
	p = pAcof;
	pA = *p++;
	for( i=0; i<9; i++ )
		pA = pA * T + *p++;
	pA *= SEC_TO_RAD * T;

/* Node of the moving ecliptic on the J2000 ecliptic.
 */
	p = nodecof;
	W = *p++;
	for( i=0; i<10; i++ )
		W = W * T + *p++;

/* Inclination of the ecliptic of date to the J2000 ecliptic.  */
	p = inclcof;
	TH = *p++;
	for( i=0; i<10; i++ )
		TH = TH * T + *p++;

/* First rotate about the x axis from the initial equator
 * to the initial ecliptic. (The input is equatorial.)
 */
	x[0] = R[0];
	z = coseps*R[1] + sineps*R[2];
	x[2] = -sineps*R[1] + coseps*R[2];
	x[1] = z;

/* Rotate about z axis to the node.
 */
	if( direction == 1 )
		z = W + pA;
	else
		z = W;
	B = COS(z);
	A = SIN(z);
	z = B * x[0] + A * x[1];
	x[1] = -A * x[0] + B * x[1];
	x[0] = z;

/* Rotate about new x axis by the inclination of the moving
 * ecliptic on the J2000 ecliptic.
 */
	if( direction == 1 )
		TH = -TH;
	B = COS(TH);
	A = SIN(TH);
	z = B * x[1] + A * x[2];
	x[2] = -A * x[1] + B * x[2];
	x[1] = z;

/* Rotate about new z axis back from the node.
 */
	if( direction == 1 )
		z = -W;
	else
		z = -W - pA;
	B = COS(z);
	A = SIN(z);
	z = B * x[0] + A * x[1];
	x[1] = -A * x[0] + B * x[1];
	x[0] = z;

/* Rotate about x axis to final equator.
 */
	if( direction == 1 )
		epsiln( JD2000 );
	else
		epsiln( J );
	z = coseps * x[1] - sineps * x[2];
	x[2] = sineps * x[1] + coseps * x[2];
	x[1] = z;

#if IAU
	done:
#endif

	for( i=0; i<3; i++ )
		R[i] = x[i];
	return(0);
}


/* Display ecliptic longitude and latitude (for
 * equinox of date, if ofdate nonzero).
 * Input pp is equatorial rectangular
 * coordinates for equinox J2000.
 */

int lonlat( double pp[], double J, double polar[], int ofdate)
{
	double s[3], x, y, z, yy, zz, r;
	int i;
/*
	printf("lonlat is called J= %f\n", J);
	printf("x= %f\n", pp[0]);
	printf("y= %f\n", pp[1]);
	printf("z= %f\n", pp[2]);
*/
/* Make local copy of position vector
 * and calculate radius.
 */
	r = 0.0;
	for( i=0; i<3; i++ ) {
		x = pp[i];
		s[i] = x;
		r += x * x;
	}
	r = sqrt(r);

/* Precess to equinox of date J
 * if flag is set
 */
	if( ofdate )
	{
		precess( s, J, -1 );
		epsiln(J);
	}
	else
		epsiln(JD2000);

/* Convert from equatorial to ecliptic coordinates
 */
	//printf("coseps = %f\n", coseps);
	//printf("sineps = %f\n", sineps);


	yy = s[1];
	zz = s[2];
	x  = s[0];
	y  =  coseps * yy  +  sineps * zz;
	z  = -sineps * yy  +  coseps * zz;

	//printf("zz = %f\n", zz);
	//printf("yy = %f\n", yy);

	yy = zatan2( x, y );
	zz = asin( z/r );

	polar[0] = yy;
	polar[1] = zz;
	polar[2] = r;


	//printf("polar[0] = %f\n", polar[0]);
	//printf("polar[1] = %f\n", polar[1]);

	//printf( "ecliptic long" );
	//dms( yy );
	//printf( " lat" );
	//dms( zz );
	//printf( " rad %.9E\n", r );
	return(0);
}



