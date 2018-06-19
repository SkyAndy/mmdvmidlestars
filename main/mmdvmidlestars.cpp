#include "ArduiPi_OLED_lib.h"
#include "Adafruit_GFX.h"
#include "ArduiPi_OLED.h"
#include <stdlib.h>     /* srand, rand */
#include <sys/time.h> /* /curmillis */
#include <getopt.h>
#include <math.h>

#define PRG_NAME        "mmdvmidlestar"
#define PRG_VERSION     "1.0.6"

// Instantiate the display
ArduiPi_OLED display;

struct timeval tp;			// currentmillis
int hCenter = 64;                       // horizontal center of animation
int vCenter = 32;                       // vertical center of animation
int Hstar, Vstar;                       // star location currently
int Quantity = 50;                      // number of stars
int StarProgress[50];                   // array that tracks progress of each star
int StarAngle[50] = {};                 // array that tracks angle of each star
unsigned int interval[500] = {};


// Config Option
struct s_opts
{
	int oled;
	int verbose;
} ;

int sleep_divisor = 1 ;
	
// default options values
s_opts opts = {
	OLED_ADAFRUIT_SPI_128x32,	// Default oled
  false					// Not verbose
};

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2
unsigned long currentMillis = 0;

void drawstar(void) {
	gettimeofday(&tp, NULL);
	currentMillis = tp.tv_sec * 1000 + tp.tv_usec / 1000;
	display.clearDisplay();
	for (int i = 0; i < Quantity; i++) {			// loop to draw each star
		if (StarProgress[i] > 100) {                	// if statement to reset stars after going off screen
			StarProgress[i] = StarProgress[i] - 100;  // resets star progress
			StarAngle[i] = rand() % 360 +1;           // resets star angle
			interval[i] = (rand() % 5 +1) + 1;        // resets star speed    
		}else{
			StarProgress[i] = StarProgress[i] + interval[i];                            	// distance traveled for star
    		}
    		if (currentMillis - StarProgress[i] >= interval[i] && StarProgress[i] > 0) {		// if statement to exclude some star paths
			Hstar = (hCenter + (sin(StarAngle[i] / 57.296) * StarProgress[i] ));        	// calculates star horizontal coordinate
			Vstar = (vCenter + (cos(StarAngle[i] / 57.296) * StarProgress[i] ));        	// calculates star horizontal coordinate
			display.drawPixel(Hstar, Vstar, WHITE);                                     	// draws each star
    		}
  	}
	display.display();
}

/* ======================================================================
Function: usage
Purpose : display usage
Input 	: program name
Output	: -
Comments: 
====================================================================== */
void usage( char * name)
{
	printf("%s\n", name );
	printf("Usage is: %s --oled type [options]\n", name);
	printf("  --<o>led type\nOLED type are:\n");
	for (int i=0; i<OLED_LAST_OLED;i++)
		printf("  %1d %s\n", i, oled_type_str[i]);
	
	printf("Options are:\n");
	printf("  --<v>erbose  : speak more to user\n");
	printf("  --<h>elp\n");
	printf("<?> indicates the equivalent short option.\n");
	printf("Short options are prefixed by \"-\" instead of by \"--\".\n");
	printf("Example :\n");
	printf( "%s -o 1 use a %s OLED\n\n", name, oled_type_str[1]);
	printf( "%s -o 4 -v use a %s OLED being verbose\n", name, oled_type_str[4]);
}


/* ======================================================================
Function: parse_args
Purpose : parse argument passed to the program
Input 	: -
Output	: -
Comments: 
====================================================================== */
void parse_args(int argc, char *argv[])
{
	static struct option longOptions[] =
	{
		{"oled"	  , required_argument,0, 'o'},
		{"verbose", no_argument,0, 'v'},
		{"help"		, no_argument,0, 'h'},
		{0, 0, 0, 0}
	};

	int optionIndex = 0;
	int c;

	while (1) 
	{
		/* no default error messages printed. */
		opterr = 0;

    c = getopt_long(argc, argv, "vho:", longOptions, &optionIndex);

		if (c < 0)
			break;

		switch (c) 
		{
			case 'v': opts.verbose = true;	break;

			case 'o':
				opts.oled = (int) atoi(optarg);
				
				if (opts.oled < 0 || opts.oled >= OLED_LAST_OLED )
				{
						fprintf(stderr, "--oled %d ignored must be 0 to %d.\n", opts.oled, OLED_LAST_OLED-1);
						fprintf(stderr, "--oled set to 0 now\n");
						opts.oled = 0;
				}
			break;

			case 'h':
				usage(argv[0]);
				exit(EXIT_SUCCESS);
			break;
			
			case '?':
			default:
				fprintf(stderr, "Unrecognized option.\n");
				fprintf(stderr, "Run with '--help'.\n");
				exit(EXIT_FAILURE);
		}
	} /* while */

	if (opts.verbose)
	{
		printf("%s v%s\n", PRG_NAME, PRG_VERSION);
		printf("-- OLED params -- \n");
		printf("Oled is    : %s\n", oled_type_str[opts.oled]);
		printf("-- Other Stuff -- \n");
		printf("verbose is : %s\n", opts.verbose? "yes" : "no");
		printf("\n");
	}	
}


/* ======================================================================
Function: main
Purpose : Main entry Point
Input 	: -
Output	: -
Comments: 
====================================================================== */
int main(int argc, char **argv)
{
	for (int i = 0; i < Quantity; i++) {          // loop to setup initial stars
    		StarAngle[i] = rand() % 360 +1;       // generates random angle for each star
    		interval[i] = (rand() % 5 +1 ) +1;    // generates random speed for each star +1 keeps stars from being zero
  	}

	// Oled supported display in ArduiPi_SSD1306.h
	// Get OLED type
	parse_args(argc, argv);

	// SPI
	if (display.oled_is_spi_proto(opts.oled))
	{
		// SPI change parameters to fit to your LCD
		if ( !display.init(OLED_SPI_DC,OLED_SPI_RESET,OLED_SPI_CS, opts.oled) )
			exit(EXIT_FAILURE);
	}
	else
	{
		// I2C change parameters to fit to your LCD
		if ( !display.init(OLED_I2C_RESET,opts.oled) )
			exit(EXIT_FAILURE);
	}

	display.begin();
	
 	// init done
	display.clearDisplay();		// clears the screen  buffer
	display.display();		// display it (clear display)

	if (opts.oled == 5)
	{
		// showing on this display is very slow (the driver need to be optimized)
		sleep_divisor = 4;

		for(char i=0; i < 12 ; i++)
		{
			display.setSeedTextXY(i,0);		//set Cursor to ith line, 0th column
			display.setGrayLevel(i);		//Set Grayscale level. Any number between 0 - 15.
			display.putSeedString("Hello MMDVM");	//Print Hello World
		}
		sleep(2);
	}

	display.clearDisplay();
  	while (1) {
   		drawstar();
  	}
}


