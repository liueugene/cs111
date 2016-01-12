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

cd ..
rm -r test_dir