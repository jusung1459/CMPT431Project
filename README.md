# CMPT431Project

Steps to run program
1. Make sure you have 120GB of free memory on your computer if you want to sort 10billion floats
2. generate the binary file by executing ./generateFloats --nSize 10000000000
3. Example of commands to run
sorting serial
./sort_serial --nSize 10000000000 --nSplit 10
sorting parallel 
./sort_parallel --nSize 10000000000 --nSplit 2 --nThreads 4
./sort_parallel --nSize 10000000000 --nSplit 2 --nThreads 8