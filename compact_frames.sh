#!/bin/bash

for x in /daten_fokker/videonics/single_frames/*.png
do
	convert $x /tmp/tmp.pgm
	./compactor /tmp/tmp.pgm
done
