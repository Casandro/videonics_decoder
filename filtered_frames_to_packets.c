/*this program filters the output of compactor.c and only passes packets with clean bits*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


#define MINLINE (45)
#define MAXLINE (225)

#define DATALEN ((MAXLINE-MINLINE+1)*3)


void find_data(uint8_t data[], const int num)
{
	int ende=-1;
	int n;
	for (n=(DATALEN-50)/3; n<DATALEN/3-1; n++) {
		if ( (data[n*3+1]==0xff) &&
		     (data[n*3+2]==0xff) &&
		     (data[n*3+3]==0x00) &&
		     (data[n*3+4]==0x00) &&
		     (data[n*3+5]==0x00) ) {
			ende=n*3-2;
			break;
		}
	}
	if (ende<0) return;
	int anfang=ende-512+1;
	for (n=0;n<512+2; n++) printf("%02x", data[anfang+n]^0xff);
	printf("\n");
	int sum=0;	
	uint16_t xor=0;
	for (n=0; n<512; n++) {
		sum=sum+data[anfang+n];
		uint16_t x=data[anfang+n];
		if (n%2==0) x=x<<8;
		xor=xor^x;
		//sum=sum+x;
	}
	printf("Ende=%d sum=%08x xor=%04x %02x %02x   %02x\n", ende, sum, xor, data[ende+1], data[ende+2], data[anfang]);
}


int main(int argc, char *argv[])
{
	int olineno=0;
	int lineno=0;
	char bits[64]; //24 would be enough
	uint8_t data[DATALEN];
	int cnt=0;
	while (scanf("%d%s", &lineno, bits)==2) {
		if (lineno<olineno) {
			//fwrite(line, sizeof(line), 1, stdout);
			find_data(data, cnt);
			cnt=cnt+1;
			memset(data, 0, sizeof(data));
		}
		olineno=lineno;
		int l=lineno-MINLINE;
		if (l<0) l=0;
		int n;
		for (n=0; n<24; n++) {
			int p=l*24+n;
			if (p<0) p=0;
			int op=p/8;
			int bp=p%8;
			if (op>=DATALEN) op=DATALEN-1;
			if (bits[n]=='1') data[op]=data[op]|(1<<bp);
		}
	}
	fprintf(stderr, "linecount=%d\n", cnt);
	return 0;
}
