#include <stdio.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <time.h>

#define MSK 0x070707 //tolerance level for each colour. Lower is better as long
//as 100% static detection is maintained

#define RED 0xfb0d1b | MSK //the colours proper
#define RE2 0xf22d35 | MSK
#define ORA 0xec3a2d | MSK
#define YEL 0xfee133 | MSK
#define GRE 0x29be52 | MSK
#define GR2 0x56df7d | MSK //gradients are hard to 'spot', have aliases
#define CYA 0x37d3e3 | MSK
#define BLU 0x250ea6 | MSK
#define VIO 0xc41596 | MSK

#define CYATIM 0x36cfdf | MSK //time colours (as needed)
#define REDTIM 0xe93146 | MSK
#define BLUTIM 0x4e32e8 | MSK
#define YELTIM 0xfac747 | MSK
#define ORATIM 0xe96e2a | MSK
#define GRETIM 0x1da143 | MSK

#define BLUBON 0x4f32ca | MSK //bonus colours (as needed)
#define REDBON 0xfc0e27 | MSK
#define YELBON 0xfcc948 | MSK

#define CYASPA 0x1fb4fc | MSK //sparkly colours (as needed)
#define VIOSPA 0xcd25ca | MSK

#define RBW 0xc9f984 | MSK //rainbow colour (add more so it's seen more consistently)
#define UNK 0x010101 //generic unknown value. Should be something that's not going
// to be found 'naturally' (hence not 0x0). Preferably not in the colour map.

unsigned long colArr[64]; //array to hold colours - the detected colour for each 
//square going across the rows, or UnK

int auxHP[6] = {-2, -9, 7, -6, 10, 3}; //convenience arrays for movefinding
int auxVP[6] = {-9, 15, -16, 24, -7, 17};
int auxHD[2] = {-7, 9};
int auxVD[2] = {7, 9};

int moves[2] = {0, 0}; //the moves to be enacted next.

int tlx = 508; //top left corner of the grid
int tly = 264;

int dx = 71; //grid sizes. Should be equal due to hardcoding of search pattern
int dy = 71; //8x8 grid size also hardcoded. Deal.

int main()
{
	void updateColor();
	
	Display *d = XOpenDisplay( NULL );
	Window w = RootWindow(d, DefaultScreen(d));

	void parseField();
	int ix;
	for(ix = 0; ix < 110; ix++){ //main loop
		int j;  //index convention (0-63)
		printf("Loop %d/110. Colours updated:\n\n", ix);
		for(j = 63; j >= 0; j--){ //update from below to increase accuracy
			updateColor(j, d, w, tlx + dx*(j % 8), tly + dy*(j / 8), dx, dy);
			printf("%d:%x ", j + 1, colArr[j]);
			if(j % 8 == 0){printf("\n");}
		}
		parseField();
		system("sleep 0.6");
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
/* END MAIN */
////////////////////////////////////////////////////////////////////////////////
/*	updateColor - the big beef. Takes the x, y, width and height of an image, as
		well as some X-related parameters (display and window) and a sq index value. 
		Samples and searches, one by one, 625 pixels at absolute positions in this
		image, shortcircuiting when one of the pixels is recognized by findColor.
		Passes the masked value of the recognized pixel to the sq'th position in the
		array colArr. If no pixel in the square matches, passes directive-specified
		'unknown' value to the array instead.*/
////////////////////////////////////////////////////////////////////////////////
void updateColor(int sq, Display *disp, Window wind, int x, int y, int dx, int dy)
//sq :: 1 - 64. 
{
	XImage *i =
	XGetImage(disp, wind, x, y, dx, dy, AllPlanes, XYPixmap);
	
	int j;
	int k;

	unsigned long findColor();
	unsigned long px;
	unsigned long cl;

	for (j = 0; j < 26; j++){
		for(k = 0; k < 26; k++){	
	
			px = XGetPixel(i, 10 + 2*j, 10 + 2*k);
			cl = findColor(px);
			
			if(cl != 0){
				colArr[sq] = cl;
				XDestroyImage(i);
				return;
			}
		}
	}

	XDestroyImage(i);

	colArr[sq] = UNK;
	return;
}
////////////////////////////////////////////////////////////////////////////////
/*	findCOLOR - boring switch tower. Takes a raw pixel, masks it and compares it
		to directive-specified key values. Returns either the matching value or 0 if
		no values match.	*/
////////////////////////////////////////////////////////////////////////////////
unsigned long findColor(unsigned long px){
// takes a tPixArr index and finds the swapple colour at that index
	switch(px | MSK){
		case RED:
			return RED; case RE2: return RED;
		case ORA:
			return ORA;
		case YEL:
			return YEL;
		case GRE:
			return GRE; case GR2: return GRE;
		case CYA:
			return CYA;
		case BLU:
			return BLU;
		case VIO:
			return VIO;

		case CYATIM:
			return CYA;
		case REDTIM:
			return RED;
		case BLUTIM:
			return BLU;
		case YELTIM:
			return YEL;
		case ORATIM:
			return ORA;
		case GRETIM:
			return GRE;

		case BLUBON:
			return BLU;
		case REDBON:
			return RED;
		case YELBON:
			return YEL;

		case CYASPA:
			return CYA;
		case VIOSPA:
			return VIO;

		case RBW:
			return RBW;

		default:
			break;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
/*	checkHP - takes a pivot square, and tries to find a horizontal pair move
		with the h-pair at sq, sq + 1. Returns 0 if none found, 1 if it finds a move,
		which is actually propagated from sCheckHP. Make sure to use this on squares 
		in the 7th column to find	possible 6-7-8Hs */
////////////////////////////////////////////////////////////////////////////////
int checkHP(int sq){
	int sCheckHP();
	if(colArr[sq] == colArr[sq + 1]){
		
		printf("checkHP: h-pair at %d, %d: %x, %x!\n", sq + 1, sq + 2, colArr[sq], colArr[sq + 1]);

		if(sCheckHP(sq)){
			return 1;
		}
	}	
	return 0;
}
////////////////////////////////////////////////////////////////////////////////
/*	sCheckHP - takes a pivot and - assuming there is an h-pair at that pivot -
		checks if there is a full match. If there is, updates moves[], returns 1.
		This 1 is handled by and propagated by checkHP, from which this function is
		called. */
////////////////////////////////////////////////////////////////////////////////
int sCheckHP(int sq){ //sq index convention (0-63)
// propagates 1 if it finds a move, as well writes the moves.
	int z;
	int sqp = sq + 1; //all sqp numbers are square convention (1-64), as are moves
	int chk1; int chk2;

	printf(" sCheckHP entered: ");
	switch(sqp % 8){  //ensure no warping by restricting which possibilites get checked.
		case 1:
			chk1 = 3; chk2 = 6; break;
		case 2:
			chk1 = 1; chk2 = 6; break;
		case 6:
			chk1 = 0; chk2 = 5; break;
		case 7:
			chk1 = 0; chk2 = 3; break;
		case 0:
			return 0;
		default:
			chk1 = 0; chk2 = 6; break;
		}
	for(z = chk1; z < chk2; z++){
		
		printf("testing %d: %x, ", auxHP[z], colArr[sq + auxHP[z]]);

		if(		 (colArr[sq + auxHP[z]] == colArr[sq])
				&& (sqp + auxHP[z] > 0)
				&& (sqp + auxHP[z] < 65)
				&& !(colArr[sq + auxHP[z]] == UNK)
				&& !(colArr[
										(z < 3) ? sq - 1 : sq + 2
									 ] == UNK)
			)
		{
			printf("\n found completion at %d: ", sqp + auxHP[z]);
			
			moves[0] = (z < 3) ? sqp - 1 : sqp + 2; printf("wrote moves[0]:%d, ", (z < 3) ? sqp - 1 : sqp + 2);
			moves[1] = sqp + auxHP[z]; printf("wrote moves[1]:%d.\n\n", sqp + auxHP[z]);

			return 1;
		}
	}
	printf("nothing found!\n\n");
	return 0;
}
////////////////////////////////////////////////////////////////////////////////
/*	similar functions - checkVP, checkHD, checkVD and their associated s versions.
		functionality is nigh-identical. D-functions check for Disjoint X-O-X pairs. */
////////////////////////////////////////////////////////////////////////////////
int checkVP(int sq){
	int sCheckVP();	
	if(colArr[sq] == colArr[sq + 8]){

		printf("\ncheckHP: v-pair at %d, %d: %x, %x!\n", sq + 1, sq + 9, colArr[sq], colArr[sq + 8]);

		if(sCheckVP(sq)){
			return 1;
		}
	}	
	return 0;
}

int sCheckVP(int sq){
// propagates 1 if it find a move. expects to be passed on.
	int z;
	int sqp = sq + 1;
	int chk1; int chk2;

	printf(" sCheckVP: entered ");
	switch(sqp % 8){  //ensure no warping by restricting which possibilites get checked.
		case 1:
			chk1 = 2; chk2 = 6; break;
		case 0:
			chk1 = 0, chk2 = 4; break;
		default:
			chk1 = 0; chk2 = 6; break;
		}
	for(z = chk1; z < chk2; z++){
		
		printf("testing %d: %x, ", sqp, auxVP[z] + 1, colArr[sq + auxVP[z]]);

		if(    (colArr[sq + auxVP[z]] == colArr[sq])
				&& (sqp + auxVP[z] > 0)
				&& (sqp + auxVP[z] < 65)
				&& !(colArr[sq + auxVP[z]] == UNK)
				&& !(colArr[
										((z % 2) == 0) ? sq - 8 : sq + 16
									 ] == UNK)
			)
		{ 
			printf("\nsCheck: found completion at %d.\n", sqp + auxVP[z]);

			moves[0] = (z % 2 == 0) ? sqp - 8 : sqp + 16; printf("\n wrote moves[0]:%d", (z % 2 == 0) ? sqp - 8 : sqp + 16);
			moves[1] = sqp + auxVP[z]; printf("\n wrote moves[1]:%d", sqp + auxVP[z]);

			return 1;
		}
	}
	return 0;
}

int checkHD(int sq){
	int sCheckHD();	
	if(colArr[sq] == colArr[sq + 2]){

		printf("\ncheckVP: h-disj at %d, %d: %x, %x!\n", sq + 1, sq + 3, colArr[sq], colArr[sq + 2]);

		if(sCheckHD(sq)){
			return 1;
		}
	}	
	return 0;
}

int sCheckHD(int sq){
// propagates 1 if it find a move. expects to be passed on.
	printf(" sCheckHD: entered\n");

	int z;
	int sqp = sq + 1;

	if((sqp % 8 == 7) || (sqp % 8 == 0)){
		return 0;
	}
	for(z = 0; z < 2; z++){
		
		printf(" sCheck: testing %d + %d: %x\n", sqp, auxHD[z], colArr[sq + auxHD[z]]);

		if(	   (colArr[sq + auxHD[z]] == colArr[sq])
				&& (sqp + auxHD[z] > 0)
				&& (sqp + auxHD[z] < 65)
				&& !(colArr[sq + auxHD[z]] == UNK)
				&& !(colArr[sq + 1] == UNK)
			)
		{
			printf(" sCheck: found completion at %d.\n", sqp + auxHD[z]);
			
			moves[0] = sqp + 1; printf("\n wrote moves[0]:%d", sqp + 1);
			moves[1] = sqp + auxHD[z]; printf("\n wrote moves[1]:%d", sqp + auxHD[z]);

			return 1;
		}
	}
	printf(" sCheckHD: nothing found.\n");
	return 0;
}

int checkVD(int sq){
	int sCheckHP();	
	if(colArr[sq] == colArr[sq + 16]){

		printf("\ncheckHP: v-disj at %d, %d: %x, %x!\n", sq + 1, sq + 17, colArr[sq], colArr[sq + 16]);

		if(sCheckVD(sq)){
			return 1;
		}
	}	
	return 0;
}

int sCheckVD(int sq){
// propagates 1 if it find a move. expects to be passed on.
	int z;
	int sqp = sq + 1;
	int chk1; int chk2;

	printf("sCheckVD: entered\n");
	switch(sqp % 8){  //ensure no warping by restricting which possibilites get checked.
		case 1:
			chk1 = 1; chk2 = 2; break;
		case 0:
			chk1 = 0, chk2 = 1; break;
		default:
			chk1 = 0; chk2 = 2; break;
		}
	for(z = chk1; z < chk2; z++){
		
		printf("sCheck: testing %d + %d: %x\n", sqp, auxVD[z], colArr[sq + auxVD[z]]);

		if(	   (colArr[sq + auxVD[z]] == colArr[sq])
				&& (sqp + auxVD[z] > 0)
				&& (sqp + auxVD[z] < 65)
				&& !(colArr[sq + auxVD[z]] == UNK)
				&& !(colArr[sq + 8] == UNK)
			)
		{			
			printf("sCheck: found completion at %d.\n", sqp + auxVD[z]);
			
			moves[0] = sqp + 8; printf("\n wrote moves[0]:%d", sqp + 8);
			moves[1] = sqp + auxVD[z]; printf("\n wrote moves[1]:%d\n", sqp + auxVD[z]);

			return 1;
		}
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////
/*	enactMove - the end of the road. Tries to swap squares at moves[0] and
		moves[1] if neither is 0. */
////////////////////////////////////////////////////////////////////////////////
void enactMove(){
	int fx = dx*((moves[0] - 1) % 8) + 35;
	int fy = dy*((moves[0] - 1) / 8) + 35;
	int sx = dx*((moves[1] - 1) % 8) + 35;
	int sy = dy*((moves[1] - 1) / 8) + 35;

	printf("\n\nmoving %d %d\n\n", moves[0], moves[1]);

	char buff[512];
	int msg = sprintf(buff, "xdotool mousemove %d %d; xdotool click 1;\
											 xdotool mousemove %d %d; xdotool click 1;",\
											 tlx + fx, tly + fy, tlx + sx, tly + sy);

	system(buff);
	moves[0] = 0; moves[1] = 0;

	return;
}
////////////////////////////////////////////////////////////////////////////////
/*	parseField - putting it all together. Goes row by row across the game field,
		finding and enacting moves. Move after each find. After each move the "spent"
		elements of colArr[] are overwritten with garbage so that they're not found
		again. */
////////////////////////////////////////////////////////////////////////////////
void parseField(){
	int j; void moveRBW();
	for(j = 0; j < 64; j++){
		
		if(colArr[j] == UNK){
			continue;
		}
		else if(colArr[j] == (RBW)){

			printf("Rainbow at %d!\n", j + 1);

			moveRBW(j);
			enactMove();
			return;
		}
		else if(checkHP(j)){
			colArr[j] = UNK;
			colArr[j + 1] = UNK;
			colArr[moves[0] - 1] = UNK;
			colArr[moves[1] - 1] = UNK;

			enactMove();
			continue;
		}
		else if(checkHD(j)){
			colArr[j] = UNK;
			colArr[j + 2] = UNK;
			colArr[moves[0] - 1] = UNK;
			colArr[moves[1] - 1] = UNK;

			enactMove();
			continue;
		}
		else if((j / 8 < 8) && checkVP(j)){
			colArr[j] = UNK;
			colArr[j + 8] = UNK;
			colArr[moves[0] - 1] = UNK;
			colArr[moves[1] - 1] = UNK;
			
			enactMove();
			continue;
		}	
		else if((j / 8 < 7) && checkVD(j)){
			colArr[j] = UNK;
			colArr[j + 16] = UNK;
			colArr[moves[0] - 1] = UNK;
			colArr[moves[1] - 1] = UNK;

			enactMove();
			continue;
		}
		else continue;
	}
	return;
}	
////////////////////////////////////////////////////////////////////////////////
/*	moveRBW - function to handle rainbows. Swaps the rainbow with the block on
		the immediate right, unless the rainbow is in the 8th column, in which case
		the block on the immediate left is used. */
////////////////////////////////////////////////////////////////////////////////
void moveRBW(int sq){
	int sqp = sq + 1;
	switch(sqp % 8){
		case 0:
			moves[0] = sqp; moves[1] = sqp - 1;
			return;
		default:
			moves[0] = sqp; moves[1] = sqp + 1;
			return;
	}
}
		

		









