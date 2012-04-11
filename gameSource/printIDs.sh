findArray=(`find . -name "info.txt"`)


num=${#findArray[@]}

for ((i = 0; i < $num; i++)); do
	echo
	echo
    cat "${findArray[$i]}"
done


echo
echo