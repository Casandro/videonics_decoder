#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define WIDTH (720)
#define HEIGHT (344)


int main(int argc, char *argv[])
{
	if (argc<3) return 1;
	int offset=atoi(argv[2]);
	uint8_t image[WIDTH*HEIGHT];
	FILE *f=fopen(argv[1], "r");
	fseek(f, -WIDTH*HEIGHT, SEEK_END);
	fread(image, sizeof(image), 1, f);
	int field=0;
	for (field=0; field<2; field++){
		int line;
		for (line=0; line<HEIGHT/2; line++) {
			printf("%d %3d: ", field, line);
			int il=line*2+field;
			int bit;
			int databits[24];
			for (bit=0; bit<24; bit++) {
				int xp=720/24*bit+offset;
				int databit=0;
				if (image[il*WIDTH+xp]>127) databit=1;
				printf("%d", databit);
				if (bit%8==7) printf(" ");
				databits[bit]=databit;
			}
			printf(" ");
			int o;
			for (o=0; o<3; o++) {
				int byte=0;
				int n;
				for (n=0; n<8; n++) {
					byte=(byte<<1) | databits[o*3+(7-n)];
				}
				printf("0x%02x ", byte); 
			}

			printf("\n");
		}
	}
}
