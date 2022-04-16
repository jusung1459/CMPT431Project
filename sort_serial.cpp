#include "core/utils.h"
#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include "binaryFileController.cpp"

#define DEFAULT_N "10000000000"
#define DEFAULT_SPLIT "8"
#define DEFAULT_SEED 123
#define DEFAULT_CREATE "0"

bool isSort(std::vector<float> arr, unsigned long N) {
  for(int i = 1; i < N; i++) {
    if (arr[i-1] > arr[i]) {
      return false;
    }
  }
  return true;
}

void sort_K(unsigned int k, unsigned long size) {
  unsigned long split_size = size/k;

  for (int i = 0; i < k; i++) {
    vector<float> *readArr = new vector<float>(size);
    std::string file_name = "sortedFloats_" + std::to_string(i) + ".bin";
    binRead(readArr, "randomFloats.bin", split_size,i*split_size);
    std::sort((*readArr).begin(), (*readArr).begin()+split_size);
    binWrite(readArr, file_name, split_size);
  }
}
 
int main(int argc, char *argv[]) {
  // Initialize command line arguments
  cxxopts::Options options("Serial Sorter",
                           "Sort lots of floating points");
  options.add_options(
      "custom",
      {
          {"nSize", "Number of floating points",         
           cxxopts::value<unsigned long>()->default_value(DEFAULT_N)},
          {"nSplit", "Number of split points",         
           cxxopts::value<unsigned long>()->default_value(DEFAULT_SPLIT)},
          {"nCreate", "Number of split points",         
           cxxopts::value<unsigned int>()->default_value(DEFAULT_CREATE)} 
      });
  auto cl_options = options.parse(argc, argv);
  unsigned long n_size = cl_options["nSize"].as<unsigned long>();
  unsigned long n_split = cl_options["nSplit"].as<unsigned long>();
  unsigned int n_create = cl_options["nCreate"].as<unsigned int>();
  std::cout << "Number of floating points : " << n_size << "\n";
  std::cout << "Number of split points : " << n_split << "\n";
  std::cout << "Create floats : " << n_create << "\n";
  timer serial_timer;
  serial_timer.start();

  if (n_create == 1) {
    // create floats
    vector<float> *arr = new vector<float>(n_size);
    createFloatingPoints(arr, n_size, 123);
    binWrite(arr, "randomFloats.bin", n_size);
  }

  // read N points and sort
  // write sorted to file
  sort_K(n_split, n_size);




  double time_taken = serial_timer.stop();
  printf("time: %f\n", time_taken);

  return 0;
}
