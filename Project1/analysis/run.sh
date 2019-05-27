rm result
for i in OS_PJ1_Test/*;do
	FILE=${i#*/}
	TEST=${FILE%.*}
	echo $TEST
	python3 analyze.py $TEST >> result
done
