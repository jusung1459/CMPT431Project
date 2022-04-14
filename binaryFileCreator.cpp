#include <fstream>
#include <iostream>
#include "core/utils.h"
#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <vector>

using namespace std; 

int binRead(float array[], const string& file, unsigned long N, unsigned long start) {
	ifstream ifstream;
	ifstream.open(file);
	if (ifstream) {
        //https://stackoverflow.com/questions/18640001/read-several-bytes-jump-over-n-bytes-and-then-read-several-bytes-again-how
        ifstream.ignore(sizeof(float) * start);
		ifstream.read(reinterpret_cast<char *>(array), sizeof(float) * (N-start));
		ifstream.close();
		return 0;
	}
	else
		return -1;
}

int binWrite(float array[], const string& file, unsigned long N) {
	ofstream ofstream;
	ofstream.open(file);
	if (ofstream) {
		ofstream.write(reinterpret_cast<const char *>(array), sizeof(float) * N);
		ofstream.close();
		return 0;
	}
	else
		return -1;
}

float *createFloatingPoints(unsigned long N, int seed) {
    float *arr = new float[N];
    for (int i = 0; i < N; i++) {
        float random = ((float) rand()) / (float) RAND_MAX;
        arr[i] = random;
        // printf("%f ", random);
    }
    return arr;
} 

int main()
{	
    unsigned long size = 250;
	// float *arr = createFloatingPoints(size, 123);
	// binWrite(arr, "randomFloats.bin", size);
    unsigned long pass = 1;
    float *readArr = new float[size-pass];
    binRead(readArr, "randomFloats.bin", size,pass);
    for (unsigned long i = 0; i < 5; i++) {
        printf("%f\n", readArr[i]);
    }
    return 0;
}
