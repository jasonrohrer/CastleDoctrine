echo
echo "Focus target window and then wait four seconds..."
echo

sleep 4s

windowID=`xdotool getactivewindow`

echo "Got window id $windowID"


while read url; do
	./startAuction.sh $url $windowID

	sleep 5s
done <testItemURLs.txt
