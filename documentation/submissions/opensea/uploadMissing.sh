echo
echo "Focus target window and then wait four seconds..."
echo

sleep 4s

windowID=`xdotool getactivewindow`

echo "Got window id $windowID"


for i in 009 016 074 089 091; do
	imageFile="tokenImages/$i.png";
	textFile=`echo -n $imageFile | sed -e "s/png/txt/"`
	./uploadGalleryItem.sh $imageFile $textFile $windowID

	sleep 5s
done
