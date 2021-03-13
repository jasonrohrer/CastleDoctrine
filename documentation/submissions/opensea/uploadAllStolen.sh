echo
echo "Focus target window and then wait four seconds..."
echo

sleep 4s

windowID=`xdotool getactivewindow`

echo "Got window id $windowID"


for imageFile in stolenImages/*.png; do
	./uploadStolenItem.sh $imageFile $windowID

	sleep 5s
done
