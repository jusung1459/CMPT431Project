#include <fstream>
#include <iostream>
#include "core/cxxopts.h"
#include "core/get_time.h"
#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include "binaryFileController.h"

using namespace std; 

int binRead(vector<float> *array, const string& file, unsigned long N, unsigned long start) {
	ifstream ifstream(file, std::ios::binary);

	if (ifstream) {
        //https://stackoverflow.com/questions/18640001/read-several-bytes-jump-over-n-bytes-and-then-read-several-bytes-again-how
        ifstream.ignore(sizeof(float) * start);
		for (int i = 0; i < N; i++) {
			ifstream.read(reinterpret_cast<char*>(&array->at(i)), sizeof(float));
		}
		if (!ifstream) {
		}
		ifstream.close();
		return 0;
	}
	else
		return -1;
}

int binWrite(vector<float> *array, const string& file, unsigned long N, int append) {
	ofstream ofstream;
	if (append == 1)
        ofstream.open(file, std::ios_base::app);
    else
        ofstream.open(file);
	if (ofstream) {
		for (int i = 0; i < N; i++) {
			ofstream.write(reinterpret_cast<const char *>(&array->at(i)), sizeof(float));
		}
		ofstream.close();
		return 0;
	}
	else
		return -1;
}

void createFloatingPoints(vector<float> *arr, unsigned long N, int seed) {
    for (int i = 0; i < N; i++) {
        float random = ((float) rand()) / (float) RAND_MAX;
        arr->at(i) = random;
        // printf("%f ", random);
    }
} 

// int main()
// {	
//     unsigned long size = 100;
// 	vector<float> *arr = new vector<float>(size);
// 	// createFloatingPoints(arr, size, 123);
// 	// binWrite(arr, "randomFloats.bin", size);
	
//     unsigned long pass = 0;
//     vector<float> *readArr = new vector<float>(size);

//     binRead(readArr, "randomFloats.bin", size,pass);
//     for (unsigned long i = 0; i < 5; i++) {
//         printf("%f\n", readArr->at(i)); 
//     }
//     return 0;
// }
