/*this program filters the output of compactor.c and only passes packets with clean bits*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

//Result of wc -l of the input file
#define LCNT (1192247) 

#define MINLINE (45)
#define MAXLINE (225)

#define IMGWIDTH ((MAXLINE-MINLINE)*24)

#define IMGHEIGHT (6586)


int main(int argc, char *argv[])
{
	int olineno=0;
	int lineno=0;
	fprintf(stdout, "P5 %d %d 255\n", IMGWIDTH, IMGHEIGHT);
	char bits[64]; //24 would be enough
	uint8_t line[IMGWIDTH];
	int cnt=0;
	while (scanf("%d%s", &lineno, bits)==2) {
		if (lineno<olineno) {
			fwrite(line, sizeof(line), 1, stdout);
			cnt=cnt+1;
			memset(line, 0x7f7f7f7f, sizeof(line));
		}
		olineno=lineno;
		int l=lineno-MINLINE;
		if (l<0) l=0;
		int n;
		for (n=0; n<24; n++) {
			int p=l*24+n;
			if (p<0) p=0;
			if (p>=IMGWIDTH) p=IMGWIDTH-1;
			if (bits[n]=='0') line[p]=255; else line[p]=0;
		}
	}
	fprintf(stderr, "linecount=%d\n", cnt);
	return 0;
}
