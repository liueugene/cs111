#!/bin/bash

mkdir test_dir
cd test_dir

#Test 1

echo "abc" > temporary_file.tmp
touch temporary_file2.tmp
touch temporary_file3.tmp
touch temporary_file4.tmp
../simpsh --rdonly temporary_file.tmp --wronly temporary_file2.tmp --wronly temporary_file3.tmp --command 0 1 2 cat
diff -u temporary_file.tmp temporary_file2.tmp > /dev/null || (echo "Test 1 Failed." && exit 1)
diff -u temporary_file3.tmp temporary_file4.tmp > /dev/null || (echo "Test 1 Failed." && exit 1)

#Test 2

echo "abc" > temporary_file5.tmp
echo "efg" > temporary_file6.tmp
touch temporary_file7.tmp
touch temporary_file8.tmp
touch temporary_file9.tmp
../simpsh --rdonly temporary_file5.tmp --wronly temporary_file7.tmp --wronly temporary_file8.tmp --command 0 1 2 tr abc efg
diff -u temporary_file7.tmp temporary_file6.tmp > /dev/null || (echo "Test 2 Failed." && exit 1)
diff -u temporary_file8.tmp temporary_file9.tmp > /dev/null || (echo "Test 2 Failed." && exit 1)

#Test 3

touch temporary_file10.tmp
touch temporary_file11.tmp
touch temporary_file12.tmp
../simpsh --rdonly temporary_file10.tmp --wronly temporary_file10.tmp --command 0 1 1 cat --verbose --rdonly temporary_file11.tmp --wronly temporary_file12.tmp --command 2 3 1 cat >temporary_file13.tmp
printf -- "--rdonly temporary_file11.tmp\n--wronly temporary_file12.tmp\n--command 2 3 1 cat\n" >temporary_file14.tmp
diff -u temporary_file13.tmp temporary_file14.tmp > /dev/null || (echo "Test 3 Failed." && exit 1)

#Test 4

touch temporary_file15.tmp
touch temporary_file16.tmp
touch temporary_file17.tmp
../simpsh --rdonly temporary_file15.tmp --rdonly temporary_file16.tmp --wronly temporary_file17.tmp --command 0 1 2 cat  
if [ -s temporary_file17.tmp ]
	then
		echo "Test 4 Failed."
		exit 1
fi

#Test 5

touch temporary_file18.tmp
printf "WORLD\nHELLO\nGOODBYE\nTHINGS" > temporary_file18.tmp
touch temporary_file19.tmp
../simpsh --rdonly temporary_file18.tmp --pipe --pipe --creat --trunc --wronly temporary_file20.tmp --creat --append --wronly temporary_file21.tmp --command 3 5 6 tr A-Z a-z --command 0 2 6 sort --command 1 4 6 cat temporary_file19.tmp - --wait > /dev/null
touch temporary_file22.tmp
touch temporary_file23.tmp
(sort < temporary_file18.tmp | cat temporary_file19.tmp - | tr A-Z a-z > temporary_file22.tmp) 2>> temporary_file23.tmp
diff -u temporary_file22.tmp temporary_file20.tmp > /dev/null || (echo "Test 5 Failed." && exit 1)

cd ..
rm -r test_dir