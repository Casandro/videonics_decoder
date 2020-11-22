#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>



int avg_pixels(const uint8_t line[], const int width, const int left, const int right)
{
	int l=left;
	if (l<0) l=0;
	if (l>width-2) l=width-2;
	int r=right;
	if (r<l) r=l+1;
	if (r>width-1) r=width-1;

	int sum=0;
	int n;
	for (n=l; n<r; n++) sum=sum+line[n];
	return sum/(r-l);
}

uint32_t line_to_bits(const uint8_t line[], const int width, const int fraction)
{
	double bit_width=((double)width)/(23.0+((double)fraction)/100);
	double left_border=0;
	uint32_t data=0;
	int n;
	for (n=0; n<24; n++) {
		//determine the right bit border
		double right_border=left_border+bit_width;
		double mid=(left_border+right_border)/2;
		//use only the middle half of the bit
		int l=round((left_border+mid)/2);
		int r=round((right_border+mid)/2);
		int avg=avg_pixels(line, width, l, r);
		if (avg>90) data=data | (1<<n);
		left_border=right_border;
	}
	return data;
}

void process_field(const uint8_t frame[], const int width, const int height, const int fraction, const int top, const int step, const int field_number)
{
	int lcnt=(height-top)/step;
	uint32_t fpixels[(lcnt)];
	int n;
	for (n=0; n<lcnt; n++) fpixels[n]=0;
	int c=0;
	int l;
	for (l=top; l<height; l=l+step)
	{
		fpixels[c]=line_to_bits(&(frame[l*width]), width, fraction);
		//printf("%03d %08x\n", c, fpixels[c]);
		if (c<lcnt) c=c+1;
	}

	/*for (l=0; l<lcnt; l++) {
		printf("%05d %03d ",field_number, l);
		for (n=0; n<24; n++) {
			if ((fpixels[l]&(1<<n))!=0) printf("1"); else printf("0");
		}
		printf("\n");
	}
	return;*/

	int ende=-1;
	for (l=lcnt-2; l>200; l--) 
	{
		if ( (fpixels[l+1]==0xffffff) &&
		     ((fpixels[l]&0xffff00) == 0x000000) ) {
			ende=l; //This line contains the last octet of the CRC
			break;
		}	
	}
	if (ende<0) return; //No data packet found
//	printf("Ende: %d\n", ende);
	uint8_t data[172*3];
	int anfang=ende-172+1;
//	printf("Anfang: %d %06x before: %06x end: %06x after: %06x\n", anfang, fpixels[anfang], fpixels[anfang-1], fpixels[ende], fpixels[ende+1]); 
	for (n=0; n<172; n++) {
		uint32_t d=fpixels[anfang+n];
		data[n*3+0]=(d & 0x0000ff) >> 0;
		data[n*3+1]=(d & 0x00ff00) >> 8;
		data[n*3+2]=(d & 0xff0000) >> 16;
	}
	printf("%05d ", field_number);
	for (n=0; n<514; n++) printf("%02x", data[n]);
	printf("\n");
	//fwrite(data, 512, 1, stdout);

}

void process_image(const uint8_t frame[], const int width, const int height, const int fraction, const int frame_number)
{
	if (height<300) { //if height < 300, video is not interlaced
		return process_field(frame, width, height, fraction, 0, 1, frame_number);
	}
	process_field(frame, width, height, fraction, 0, 2, frame_number*2);
	process_field(frame, width, height, fraction, 1, 2, frame_number*2+1);
}



int main(int argc, char *argv[])
{
	if (argc!=4) {
		printf("Usage: %s <width> <height> <fraction>\n\twidth: Width of video in pixels\n\theight: Height of video in pixels\n\tfraction: fraction of last bit still in picture (in percent)\nUse ffmpeg to transcode video to rawvideo rgb24", argv[0]);
		return 1;
	}
	int width=atoi(argv[1]);
	if (width<24*8) {
		printf("Width %s to small\n", argv[1]);
		return 1;
	}
	int height=atoi(argv[2]);
	if (height<240) {
		printf("Height %s to small\n", argv[2]);
		return 1;
	}
	int fraction=atoi(argv[3]);
	if (fraction>100) fraction=100;
	if (fraction<10) {
		printf("Faction %s to small\n", argv[3]);
	}

	size_t img_size=width*height*3;
	uint8_t *image=malloc(img_size);
	memset(image, 0, img_size);


	int frame_number=0;
	while (fread(image, img_size, 1, stdin)==1) {
		process_image(image, width, height, fraction, frame_number);
		frame_number=frame_number+1;
	}
	free(image);
	return 0;
}
