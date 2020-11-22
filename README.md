# videonics_decoder


Example usage for a file which has 720x486 resolution. The parameter 100 indicates that approximately 100% of the last bit are visible.

ffmpeg -i videofile.mov -f rawvideo -pix_fmt gray - | ./rawvideo_to_packets 720 486 100 > somefile.txt

The output is a hexdump of the packet including the 16 bit CRC. It is prefixed by the number of the field the packet was in.
