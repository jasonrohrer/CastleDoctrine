#!/bin/bash

filename="${1%.*}"

convert $1 -compress none $filename.ppm

./ppm2ps.pl $filename.ppm > $filename.ps