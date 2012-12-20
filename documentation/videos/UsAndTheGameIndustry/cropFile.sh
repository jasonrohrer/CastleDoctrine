echo "Cropping $1"
mogrify  -crop 364x360+138+61 $1
mogrify  -bordercolor black -border 138x0 $1
mogrify -filter point -resize 1920x1080 -format png $1