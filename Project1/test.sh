for entry in OS_PJ1_Test/*; do
	echo "Running $entry..."
	FILENAME=${entry#*/}
	sudo ./main < $entry 2> out/${FILENAME%.*}.out
	LINE=$(cat $entry | sed -n '2,2p')
	NUM=${LINE%?}
	echo $NUM
	dmesg | grep 'project1' | tail -n $NUM > out/${FILENAME%.*}.msg
done
