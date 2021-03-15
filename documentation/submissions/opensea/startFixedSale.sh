if [ $# -ne 2 ]
then

	echo "Usage:  startAuction.sh item_url web_window_id"
	echo
	exit
fi

echo "Starting sale for $1 to window id $2"

url="$1/sell"
window=$2


xdotool windowfocus $window
xdotool windowraise $window

sleep 1

# move mouse to address bar and click to focus it
xdotool mousemove --window $window 250 60

xdotool click --window $window 1

sleep 1

# select all in address bar
xdotool key "ctrl+a"


# type create URL
xdotool type "$url"

xdotool key KP_Enter

# wait for create page to load
sleep 15


# tab to start price field
xdotool key Tab Tab Tab
sleep 1


# start price
xdotool type "0.01"

sleep 1


# move mouse over post button
xdotool mousemove --window $window 962 476

sleep 1

# wait to click until done testing
xdotool click 1


# wait for sig window to pop up:
sleep 20


sigWindowID=`xdotool getactivewindow`


# move mouse over sign button
xdotool mousemove --window $sigWindowID 341 573

sleep 1

# click to sign
xdotool click 1
