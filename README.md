Reinventing the wheel.
Making ephemeris calculation library in pure C.


Велосипедю велосипед.
Делаю библиотеку для расчета эфемерид на чистом C.

Erfinde das Rad neu.
Mache Bibliothek für Ephemeris Berechnung in pur C.

ftp://naif.jpl.nasa.gov/pub/naif/toolkit_docs/FORTRAN/req/daf.html
ftp://naif.jpl.nasa.gov/pub/naif/toolkit_docs/FORTRAN/req/spk.html
ftp://ssd.jpl.nasa.gov/pub/eph/planets/README.txt
https://naif.jpl.nasa.gov/naif/
ftp://ssd.jpl.nasa.gov/pub/eph/planets/bsp
ftp://ssd.jpl.nasa.gov/pub/eph/planets/ascii/



/*
The File Record is always the first physical record in a DAF.
The record size is 1024 bytes (for platforms with one byte char size, and four bytes integer size).
The items listed in the File Record:

1. LOCIDW (8 characters, 8 bytes): An identification word (`DAF/xxxx').

The 'xxxx' substring is a string of four characters or less indicating the type of data stored in the DAF file. This is used by the SPICELIB subroutines to verify that a particular file is in fact a DAF and not merely a direct access file with the same record length. When a DAF is opened, an error signals if this keyword is not present. [Address 0]

2. ND ( 1 integer, 4 bytes): The number of double precision components in each array summary. [Address 8]

3. NI ( 1 integer, 4 bytes): The number of integer components in each array summary. [Address 12]

4. LOCIFN (60 characters, 60 bytes): The internal name or description of the array file. [Address 16]

5. FWARD ( 1 integer, 4 bytes): The record number of the initial summary record in the file. [Address 76]

6. BWARD ( 1 integer, 4 bytes): The record number of the final summary record in the file. [Address 80]

7. FREE ( 1 integer, 4 bytes): The first free address in the file. This is the address at which the first element of the next array to be added to the file will be stored. [Address 84]

8. LOCFMT ( 8 characters, 8 bytes): The character string that indicates the numeric binary format of the DAF. The string has value either "LTL-IEEE" or "BIG-IEEE." [Address 88]

9. PRENUL ( 603 characters, 603 bytes): A block of nulls to pad between the last character of LOCFMT and the first character of FTPSTR to keep FTPSTR at character 700 (address 699) in a 1024 byte record. [Address 96]

10. FTPSTR ( 28 characters, 28 bytes): The FTP validation string.

This string is assembled using components returned from the SPICELIB private routine ZZFTPSTR. [Address 699]

11. PSTNUL ( 297 characters, 297 bytes): A block of nulls to pad from the last character of FTPSTR to the end of the file record. Note: this value enforces the length of the file record as 1024 bytes. [Address 727]

*/





/*
	NAIF ID     NAME
	________    ____________________
	0           'SOLAR_SYSTEM_BARYCENTER','SSB','SOLAR SYSTEM BARYCENTER'
	1           'MERCURY_BARYCENTER','MERCURY BARYCENTER'
	2           'VENUS_BARYCENTER', 'VENUS BARYCENTER'
	3           'EARTH_BARYCENTER','EMB', 'EARTH MOON BARYCENTER', 'EARTH-MOON BARYCENTER', 'EARTH BARYCENTER'
	4           'MARS_BARYCENTER', 'MARS BARYCENTER',
	5           'JUPITER_BARYCENTER', 'JUPITER BARYCENTER'
	6           'SATURN_BARYCENTER', 'SATURN BARYCENTER'
	7           'URANUS_BARYCENTER', 'URANUS BARYCENTER'
	8           'NEPTUNE_BARYCENTER', 'NEPTUNE BARYCENTER'
	9           'PLUTO_BARYCENTER', 'PLUTO BARYCENTER'
	10          'SUN'

	199         'MERCURY'
	299         'VENUS'
	399         'EARTH'
	301         'MOON'
	499         'MARS'
	599         'JUPITER'
	699         'SATURN'
	799         'URANUS'
	899         'NEPTUNE'
	999         'PLUTO'

	For those planets without moons, Mercury and Venus, the barycenter location coincides with
	the body center of mass. However do not infer you may interchange use of the planet barycenter ID and the planet ID.
	A barycenter has no radii, right ascension/declination of the pole axis, etc.
	Use the planet ID when referring to a planet or any property of that planet.

	*/

	/*
	Type 2: Chebyshev (position only)

	The second SPK data type contains Chebyshev polynomial coefficients for the position
	of the body as a function of time. Normally, this data type is used for planet barycenters,
	and for satellites whose ephemerides are integrated. (The velocity of the body is obtained by
	differentiating the position.)

	Each segment contains an arbitrary number of logical records. Each record contains a set of Chebyshev
	coefficients valid throughout an interval of fixed length.
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
	A four-number `directory' at the end of the segment contains the information needed to determine the
	location of the record corresponding to a particular epoch.

	1. INIT is the initial epoch of the first record, given in ephemeris seconds past J2000.

	2. INTLEN is the length of the interval covered by each record, in seconds.

	3. RSIZE is the total size of (number of array elements in) each record.

	4. N is the number of records contained in the segment.

	Each record is structured as follows:
	+------------------+
	| MID              |
	+------------------+
	| RADIUS           |
	+------------------+
	| X  coefficients  |
	+------------------+
	| Y  coefficients  |
	+------------------+
	| Z  coefficients  |
	+------------------+
	The first two elements in the record, MID and RADIUS, are the midpoint and radius of the time interval
	covered by coefficients in the record. These are used as parameters to perform transformations between
	the domain of the record (from MID - RADIUS to MID + RADIUS) and the domain of Chebyshev polynomials (from -1 to 1 ).
	The same number of coefficients is always used for each component, and all records are the same size (RSIZE),
	so the degree of each polynomial is

	( RSIZE - 2 ) / 3 - 1
	*/


		/*
	The number of summaries (NS) that can fit in a single summary record depends on the size
	of a single summary (SS), a function of NI and ND:

	                  (NI + 1)
	SS       =  ND + --------         (Note that this is
	                    2             integer division.)

	SS * NS <=  125

	NS      <=  125/SS                (Note that NS must be an
	integer greater than or
	equal to one.)
	Using an SPK as an example with NI = 6, ND = 2, then SS = 5.

	*/

	
	/*
	The new name record is located in the record immediately following the new summary record.
	the number of summary records is equal to the number of name records.

	SPK as an example with NI = 6, ND = 2, then NC = 40.
	(NI + 1)
	NC  =   8 * ( ND + -------- )     (Note that this is
	2           integer division.)
	*/

		/*
	A summary record contains a maximum of 128 double precision words. The first three words of each summary record are reserved for the following control information:

	1. The record number of the next summary record in the file. (Zero if this is the final summary record.)

	2. The record number of the previous summary record in the file. (Zero if this is the initial summary record.)

	3. The number of summaries stored in this record.
	*/

