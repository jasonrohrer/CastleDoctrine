echo
echo "Focus target window and then wait four seconds..."
echo

sleep 4s

windowID=`xdotool getactivewindow`

echo "Got window id $windowID"


for imageFile in tokenImages/*.png; do
	textFile=`echo -n $imageFile | sed -e "s/png/txt/"`
	./uploadGalleryItem.sh $imageFile $textFile $windowID

	sleep 5s
done
