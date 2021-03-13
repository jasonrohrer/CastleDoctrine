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


# move mouse over set price button (default, but click 
# to set up tab-to-field later)
xdotool mousemove --window $window 257 352

sleep 1

xdotool click 1


sleep 1



# tab to start price field
xdotool key Tab
sleep 1

# start price
xdotool type "2.19"

# tab to end price switch
xdotool key Tab
sleep 1

# enable switch
xdotool type " "
sleep 1

# leave end price at 0


# tab to end date drop-down
xdotool key Tab Tab Tab Tab
sleep 1


# click on Sunday the 14
xdotool mousemove --window $window 645 368
sleep 1

xdotool click 1
sleep 1

# close date picker
xdotool key KP_Enter
sleep 1


# tab to end time
xdotool key Tab
sleep 1

# enter 12 noon
xdotool type "1200p"
sleep 1



# move mouse over post button
xdotool mousemove --window $window 962 476

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
