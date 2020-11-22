/* This program tries to find the boundaries of the bits in the video signal and output the bits
 * Input image needs to be 720x480 8 bit grayscale PGM
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "bitborders.inc"

#define WIDTH (640)
#define HEIGHT (480)

//Determine white and black level with gimp
#define WHITE (161)
#define BLACK (16)

#define MAXDIFF (abs(WHITE-BLACK)/4*3)

#define BITS_PER_LINE (24)


int clampx(const int x)
{
	if (x<0) return 0;
	if (x>=WIDTH) return WIDTH-1;
	return x;
}

int main(int argc, char *argv[])
{
	if (argc<2) return 1;

	uint8_t image[WIDTH*HEIGHT];
	FILE *f=fopen(argv[1], "r");
	fseek(f, -WIDTH*HEIGHT, SEEK_END);
	fread(image, sizeof(image), 1, f);
	fclose(f);

	int vline;

	for (vline=0; vline < HEIGHT; vline++) {
		int iline=(vline%240)*2+vline/240; //calculate image line from "video" line
		if (iline<90) continue;
		if (iline>460) continue;
		printf("%03d ", iline/2);
		int ls=iline*WIDTH; //calculate start of the line
		int col;
		for (col=0; col<BITS_PER_LINE; col++) { //for every bit
			//read in the left and right border of the current bit
			int left_border =bitborders[col];
			int right_border=bitborders[col+1];
			//calculate sensible borders;
			int mid=(left_border+right_border)/2;
			int left =clampx((left_border+mid)/2);
			int right=clampx((right_border+mid)/2);

			int sum=0;
			int max=-1;
			int min=256;
			int n;
			for (n=left; n<right; n++){
				sum=sum+image[ls+n];
				if (max<image[ls+n]) max=image[ls+n];
				if (min>image[ls+n]) min=image[ls+n];

			}	
			if (abs(max-min)>MAXDIFF) printf("X"); 
			else {
				int avg=sum/(right-left);
				if (avg>(WHITE+BLACK)/2) printf("0"); else printf("1");
			}
		}
		printf("\n");
	}


}
