/home/jcr13/makeBackups.sh


# make sure headless clients are still running
portA=5077
portB=5078




responseA=$(echo -n "check_alive\n[END_REQUEST]" | netcat localhost $portA)
responseB=$(echo -n "check_alive\n[END_REQUEST]" | netcat localhost $portB)

clientDown=0

if [[ "$responseA" != *OK* ]]; then
	echo "Headless client on port $portA down"
	clientDown=1
fi

if [[ "$responseB" != *OK* ]]; then
	echo "Headless client on port $portB down"
	clientDown=1
fi


if [[ $clientDown -eq 1 ]]; then
	echo "At least one client down, killing and rebuilding both."
	/home/jcr13/buildAndLaunchHeadlessClients.sh
fi
