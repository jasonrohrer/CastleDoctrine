if [ $# -ne 2 ]
then

	echo "Usage:  uploadStolenItem.sh image_file web_window_id"
	echo
	exit
fi


touch $1

info="Stolen"

id=`echo -n $1 | sed -e "s/[^0-9]*0*//" | sed -e "s/\.png//"`

artistName="Stolen"


echo "Uploading $1 (with id $id) to window id $2"

window=$2


xdotool windowfocus $window


windowHigh=`xdotool getwindowfocus getwindowgeometry | grep Geom | sed -e "s/.*x//"`
windowWide=`xdotool getwindowfocus getwindowgeometry | grep Geom | sed -e "s/x.*//" | sed -e "s/[^0-9]*//" `

echo "Window wide = $windowWide, window high = $windowHigh"


propertyPosY=$((windowHigh - 630))

buttonPosY=$((windowHigh - 200))


propertySavePosX=$((windowWide / 2))
propertySavePosY=$((windowHigh - 333))


# move mouse to address bar and click to focus it
xdotool mousemove --window $window 250 60

xdotool click 1

sleep 1

# select all in address bar
xdotool key "ctrl+a"


# type create URL
xdotool type "https://opensea.io/collection/thecastledoctrine/assets/create"

xdotool key KP_Enter

# wait for create page to load
sleep 10


# tab to file picking area

xdotool key Tab

xdotool key Tab

# enter to open file picker
xdotool key KP_Enter

sleep 2

# enter to pick most recent file in list

xdotool key KP_Enter


sleep 2


# tab to name field
xdotool key Tab
sleep 1

xdotool type "TCD #$id --- $info"

sleep 1

# tab to URL field
xdotool key Tab

sleep 1

xdotool type "http://thecastledoctrine.net/crypto.php"

sleep 1

# tab to description field
xdotool key Tab

sleep 1


xdotool type "The Castle Doctrine non-fungible painting token #$id

$info"

sleep 1


# scroll to bottom to reach property add button
xdotool key Page_Down Page_Down Page_Down

sleep 1


# move mouse over property add button
xdotool mousemove --window $window 1022 $propertyPosY

sleep 1

# click property add button
xdotool click 1

sleep 1

# tab to property name field
xdotool key Tab Tab

sleep 1

xdotool type "Artist"

sleep 1

# tab to property value field
xdotool key Tab

sleep 1

xdotool type "$artistName"

sleep 1

# move mouse overy property save button
xdotool mousemove --window $window $propertySavePosX $propertySavePosY

sleep 1

# click property save button
xdotool click 1

sleep 1


# scroll to bottom to reach create button
xdotool key Page_Down Page_Down Page_Down


sleep 1


# move mouse over create button
xdotool mousemove --window $window 300 $buttonPosY


sleep 1

# click create button
xdotool click 1
