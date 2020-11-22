#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define WIDTH (24*8)
#define HEIGHT (480)

#define START (100)
#define DATALINES (2*172)



int main(int argc, char *argv[])
{
	if (argc<2) return 1;
	uint8_t image[WIDTH*HEIGHT];
	FILE *f=fopen(argv[1], "r");
	fseek(f, -WIDTH*HEIGHT, SEEK_END);
	fread(image, sizeof(image), 1, f);
	fclose(f);

	char fn[1024];
	memset(fn, 0, sizeof(fn));
	snprintf(fn, sizeof(fn)-1, "%s.a.bin", argv[1]); 
	FILE *fa=fopen(fn, "w");
	snprintf(fn, sizeof(fn)-1, "%s.b.bin", argv[1]); 
	FILE *fb=fopen(fn, "w");

	int field=0;
	for (field=0; field<2; field++){
		uint8_t buffer[DATALINES/2*3];
		int line;
		for (line=0; line<DATALINES/2; line++) {
//			printf("%d %3d: ", field, line);
			int il=line*2+field+START;
			int bit;
			int databits[24];
			for (bit=0; bit<24; bit++) {
				int xp=(bit+0.5)*WIDTH/23.5;
				if (xp>=WIDTH) xp=WIDTH-1;
				int databit=0;
				if (image[il*WIDTH+xp]<96) databit=1;
//				printf("%x", image[il*WIDTH+xp]/16);
//				if (bit%8==7) printf(" ");
				databits[bit]=databit;
			}
			//printf(" ");
			int o;
			for (o=0; o<3; o++) {
				int byte=0;
				int n;
				for (n=0; n<8; n++) {
					byte=(byte<<1) | databits[o*3+(7-n)];
				}
				buffer[line*3+o]=byte;
//				printf("0x%02x ", byte); 
			}
//			printf("\n");
		}
		if (field==0) 
			fwrite(buffer, sizeof(buffer), 1, fa); 
		else
			fwrite(buffer, sizeof(buffer), 1, fb); 

	}
	fclose(fa);
	fclose(fb);
}
