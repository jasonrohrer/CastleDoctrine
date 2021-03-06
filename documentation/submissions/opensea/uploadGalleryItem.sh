if [ $# -ne 2 ]
then

	echo "Usage:  uploadGalleryItem.sh item_folder web_window_id"
	echo
	exit
fi


echo "Uploading $1 to window id $2"


xdotool windowfocus $2


windowHigh=`xdotool getwindowfocus getwindowgeometry | grep Geom | sed -e "s/.*x//"`



buttonPosY=$(($windowHigh - 200))



# move mouse to address bar and click to focus it
xdotool mousemove --window $2 250 60

xdotool click 1

sleep 1

# select all in address bar
xdotool key "ctrl+a"


# type create URL
xdotool type "https://opensea.io/collection/jason-rohrer/assets/create"

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

xdotool type "A really nice name"


# tab to URL field
xdotool key Tab

xdotool type "http://thecastledoctrine.net/crypto.php"


# tab to description field
xdotool key Tab

xdotool type "A really nice description"



# scroll to bottom to reach CREATE button
xdotool key Page_Down Page_Down Page_Down


# move mouse over create button
xdotool mousemove --window $2 250 $buttonPosY


sleep 2

# click create button
xdotool click 1
