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


# move mouse over highest bidder button
xdotool mousemove --window $window 501 359

sleep 1

xdotool click 1


sleep 1



# tab to start price field
xdotool key Tab
sleep 1

# start price
# don't leave at 0, because that's what we use to detect whether it's currently
# for sale or not on tcd website
xdotool type "0.001"



# move mouse over auction # days drop-down
xdotool mousemove --window $window 813 751

sleep 1

xdotool click 1

sleep 1

# move mouse over one week
xdotool mousemove --window $window 753 839

sleep 1

xdotool click 1

sleep 1


# move mouse over post button
xdotool mousemove --window $window 969 501

sleep 1

# wait to click until done testing
xdotool click 1


# wait for sig window to pop up:
sleep 15


sigWindowID=`xdotool getactivewindow`


# move mouse over sign button
xdotool mousemove --window $sigWindowID 341 573

sleep 1

# click to sign
xdotool click 1
