#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <queue>
#include "binaryFileController.h"

#define DEFAULT_N "10000000000"

int main(int argc, char *argv[]) {
  // Initialize command line arguments
  cxxopts::Options options("Serial Sorter",
                           "Sort lots of floating points");
  options.add_options(
      "custom",
      {
          {"nSize", "Number of floating points",         
           cxxopts::value<unsigned long long>()->default_value(DEFAULT_N)}
      });
  auto cl_options = options.parse(argc, argv);
  unsigned long long n_size = cl_options["nSize"].as<unsigned long long>();
  std::cout << "Number of floating points : " << n_size << "\n";
  timer serial_timer;

  serial_timer.start();

  // Generate an input file with random floats of size n_size
  vector<float> *arr = new vector<float>(n_size/10);
  arr->resize(n_size/10);
  createFloatingPoints(arr, n_size/10, 123);
  binWrite(arr, "randomFloats.bin", n_size/10, 0);
  arr->resize(0);

  for (int i = 1; i < 10; i++) {
    arr->resize(n_size/10);
    createFloatingPoints(arr, n_size/10, 123);
    binWrite(arr, "randomFloats.bin", n_size/10, 1);
    arr->resize(0);
  }
  delete arr;

  double time_taken = serial_timer.stop();
  printf("Time taken: %f\n", time_taken);
  printf("Finished creating %llu random floats\n", n_size);

  return 0;
}
