# CMPT431Project

Steps to run program
1. Make sure you have 120GB of free memory on your computer if you want to sort 10billion floats
2. generate the binary file by executing ./generateFloats --nSize 10000000000
3. Example of commands to run on cload
sorting serial
./sort_serial --nSize 10000000000 --nSplit 10


sorting parallel 
./sort_parallel --nSize 10000000000 --nSplit 2 --nThreads 4
./sort_parallel --nSize 10000000000 --nSplit 2 --nThreads 8




-------------------
Example output
./sort_serial --nSize 1000000000 --nSplit 10
Number of floating points : 1000000000
Number of split points : 10
Create floats : 0
Ram Size : 5
Finished reading from large file, index: 0 
Finished writing array 0 to bin file sortedFloats_0.bin 
Finished reading from large file, index: 1 
Finished writing array 1 to bin file sortedFloats_1.bin 
Finished reading from large file, index: 2 
Finished writing array 2 to bin file sortedFloats_2.bin 
Finished reading from large file, index: 3 
Finished writing array 3 to bin file sortedFloats_3.bin 
Finished reading from large file, index: 4 
Finished writing array 4 to bin file sortedFloats_4.bin 
Finished reading from large file, index: 5 
Finished writing array 5 to bin file sortedFloats_5.bin 
Finished reading from large file, index: 6 
Finished writing array 6 to bin file sortedFloats_6.bin 
Finished reading from large file, index: 7 
Finished writing array 7 to bin file sortedFloats_7.bin 
Finished reading from large file, index: 8 
Finished writing array 8 to bin file sortedFloats_8.bin 
Finished reading from large file, index: 9 
Finished writing array 9 to bin file sortedFloats_9.bin 
Finished sorting 10 files
Merging all sorted files into one

***************
Finished merging all sorted files into one
Time taken to sort: 498.032410
***************

Validating the output. Checking if the array is sorted in ascending order.
Validation complete. Array was sorted correctly