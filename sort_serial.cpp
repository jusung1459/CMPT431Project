#include "core/utils.h"
#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <vector>

#define DEFAULT_N "10000000000"
#define DEFAULT_SPLIT "8"
#define DEFAULT_SEED 123

std::vector<float> createFloatingPoints(unsigned long N, int seed) {
  std::vector<float> arr(N);
  for (int i = 0; i < N; i++) {
    float random = ((float) rand()) / (float) RAND_MAX;
    arr[i] = random;
    // printf("%f ", random);
  }
  return arr;
} 

bool isSort(std::vector<float> arr, unsigned long N) {
  for(int i = 1; i < N; i++) {
    if (arr[i-1] > arr[i]) {
      return false;
    }
  }
  return true;
}

void splitVector(std::vector<float> arr_splits[], std::vector<float> *arr, unsigned long splits, unsigned long total) {
  unsigned long split_size = total / splits;
  for (int i = 0; i < splits; i++) {
    auto start_itr = std::next(arr->cbegin(), i*split_size);
    auto end_itr = std::next(arr->cbegin(), i*split_size + split_size);
    arr_splits[i].resize(split_size);
    std::copy(start_itr, end_itr, arr_splits[i].begin());
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
           cxxopts::value<unsigned long>()->default_value(DEFAULT_SPLIT)}
      });
  auto cl_options = options.parse(argc, argv);
  unsigned long n_size = cl_options["nSize"].as<unsigned long>();
  unsigned long n_split = cl_options["nSplit"].as<unsigned long>();
  std::cout << "Number of floating points : " << n_size << "\n";
  std::cout << "Number of split points : " << n_split << "\n";
  std::vector<float> arr = createFloatingPoints(n_size, DEFAULT_SEED);
  printf("done makeing floats\n");
  timer serial_timer;
  serial_timer.start();
  // std::vector<float> arr_splits[n_split];
  // splitVector(arr_splits, &arr, n_split, n_size);

  // sort splited vectors
  unsigned long split_size = n_size / n_split;
  for (int i = 0; i < n_split; i++) {
    std::sort(arr.begin()+(i*split_size), arr.begin()+((i+1)*split_size));
  }

  double time_taken = serial_timer.stop();
  printf("time: %f\n", time_taken);

  // for (int i = 0; i < n_split; i++) {
  //   printf("size: %lu, first: %f\n", arr_splits[i].size(), arr_splits[i][0]);
  //   bool sorted = isSort(arr_splits[i], arr_splits[i].size());
  //   if (sorted) {
  //     printf("sorted\n");
  // } else {
  //     printf("not sorted\n");
  // }
  // }
  return 0;
}
