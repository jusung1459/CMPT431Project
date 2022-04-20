#include "core/cxxopts.h"
#include "core/get_time.h"
#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <queue>
#include "binaryFileController.h"

#define DEFAULT_N "10000000000"
#define DEFAULT_SPLIT "8"
#define DEFAULT_SEED 123
#define DEFAULT_CREATE "0"
#define DEFAULT_RAM_SIZE "5"

struct Node {
  float value;
  int chunk;
};

struct compare {
  bool operator()(Node a,Node b)
  {
      return a.value>b.value;
  }
};

bool isSort(std::vector<float> arr, unsigned long N) {
  float prev, cur, next;
  for(int i = 1; i < N; i++) {
    if (arr[i-1] > arr[i]) {
      prev = arr[i-1];
      cur = arr[i];
      next = arr[i+1];
      printf("prev: %f, cur: %f, next: %f\n", prev, cur, next);
      return false;
    }
  }
  printf("is sorted\n");
  return true;
}

void sort_K(unsigned int k, unsigned long long size) {
  unsigned long split_size = size/k;

  for (int i = 0; i < k; i++) {
    vector<float> *readArr = new vector<float>(split_size);
    std::string file_name = "sortedFloats_" + std::to_string(i) + ".bin";
    binRead(readArr, "randomFloats.bin", split_size,i*split_size);
    printf("finished reading from large bin file, index: %d \n", i);
    std::sort((*readArr).begin(), (*readArr).begin()+split_size);
    binWrite(readArr, file_name, split_size, 0);
    printf("finished writing to bin file %s \n", file_name.c_str());
    delete readArr;
  }
}
 
void merge_K(unsigned int K, unsigned long long size, unsigned int ram) {
  std::vector<float> vecs[K];
  std::vector<float> sorted_vec;
  int index_K[K];
  unsigned long current_MaxK[K];
  unsigned long prev_MaxK[K];
  unsigned long split_size = size/K;
  unsigned long avaiable_floats = 10000000; //((ram * 1000000000)/4)/(K+1);
  sorted_vec.reserve(split_size);
  printf("%lu %lu\n", avaiable_floats, split_size);
  for (int i = 0; i < K; i++) {
    std::string file = "sortedFloats_" + std::to_string(i) + ".bin";
    index_K[i] = 0;
    if (avaiable_floats < split_size) {
      current_MaxK[i] = avaiable_floats;
    } else {
      current_MaxK[i] = split_size;
    }
    prev_MaxK[i] = 0;
    vecs[i].resize(current_MaxK[i]);
    binRead(&vecs[i], file, current_MaxK[i], index_K[i]);
  }
  priority_queue<Node,vector<Node>, compare> minh;
  unsigned long index = 0;
  for(int j=0; j<K; j++) {
    if(vecs[j].empty() != true) {
      struct Node node;
      node.value = vecs[j][index_K[j]];
      node.chunk = j;
      minh.push(node);
      index_K[j]++;
      // printf("%d %f ", j, vecs[j][index_K[j]]);
      index++;
    }
  }

  int chunk =0;
  while(minh.size() > 0) {
    // get min and pop off heap
    chunk = minh.top().chunk;
    sorted_vec.push_back(minh.top().value);
    minh.pop();

    if (index_K[chunk] >= current_MaxK[chunk]) {
      // read in next chunk from file and put into vector when empty
      if (index_K[chunk] < split_size) {
        prev_MaxK[chunk] = current_MaxK[chunk];
        if ((avaiable_floats+index_K[chunk]) < split_size) {
          current_MaxK[chunk] = avaiable_floats+index_K[chunk];
        } else {
          current_MaxK[chunk] = split_size;
        }
        vecs[chunk].resize(0);
        vecs[chunk].resize(current_MaxK[chunk]-index_K[chunk]);
        std::string file = "sortedFloats_" + std::to_string(chunk) + ".bin";
        binRead(&vecs[chunk], file, (current_MaxK[chunk]-index_K[chunk]), index_K[chunk]);
      }
    }

    if (index_K[chunk] < current_MaxK[chunk]) {
      struct Node node;
      // printf("before value, prev_max: %ld, current_maxK: %ld, index_K: %d, diff:%ld \n", prev_MaxK[chunk], current_MaxK[chunk], index_K[chunk], index_K[chunk]-prev_MaxK[chunk]);
      node.value = vecs[chunk][index_K[chunk]-prev_MaxK[chunk]];
      node.chunk = chunk;
      minh.push(node);
      index_K[chunk]++;
      index++;
    } 
      
    if(sorted_vec.size() > split_size) {
      binWrite(&sorted_vec, "sortedFloats.bin", sorted_vec.size(), 1);
      sorted_vec.resize(0);
      sorted_vec.reserve(current_MaxK[chunk]);
    }
    
  }
  binWrite(&sorted_vec, "sortedFloats.bin", sorted_vec.size(), 1);
  printf("indx: %lu\n", index);
} 

int main(int argc, char *argv[]) {
  // Initialize command line arguments
  cxxopts::Options options("Serial Sorter",
                           "Sort lots of floating points");
  options.add_options(
      "custom",
      {
          {"nSize", "Number of floating points",         
           cxxopts::value<unsigned long long>()->default_value(DEFAULT_N)},
          {"nSplit", "Number of split points",         
           cxxopts::value<unsigned long long>()->default_value(DEFAULT_SPLIT)},
          {"nCreate", "Number of split points",         
           cxxopts::value<unsigned int>()->default_value(DEFAULT_CREATE)},
           {"nRam", "Number of split points",         
           cxxopts::value<unsigned int>()->default_value(DEFAULT_RAM_SIZE)} 
      });
  auto cl_options = options.parse(argc, argv);
  unsigned long long n_size = cl_options["nSize"].as<unsigned long long>();
  unsigned long long n_split = cl_options["nSplit"].as<unsigned long long>();
  unsigned int n_create = cl_options["nCreate"].as<unsigned int>();
  unsigned int n_ram = cl_options["nRam"].as<unsigned int>();
  std::cout << "Number of floating points : " << n_size << "\n";
  std::cout << "Number of split points : " << n_split << "\n";
  std::cout << "Create floats : " << n_create << "\n";
  std::cout << "Ram Size : " << n_ram << "\n";
  timer serial_timer;

  serial_timer.start();

  // read N points and sort
  // write sorted to file
  sort_K(n_split, n_size);
  printf("finished sorting %lld files\n", n_split);
  printf("time: %f\n", serial_timer.total());

  merge_K(n_split, n_size, n_ram);
  double time_taken = serial_timer.stop();
  printf("total time: %f\n", time_taken);

  vector<float> sorted_array;
  printf("checking if sortedfloats.bin is sorted in iterations\n");
  for (int k = 0; k < 10; k++) {
    sorted_array.resize(0);
    sorted_array.resize(n_size/n_split);
    binRead(&sorted_array, "sortedFloats.bin", n_size/n_split, k*(n_size/n_split));
    isSort(sorted_array, n_size/n_split);
  }

  return 0;
}


// ./sort_serial --nSize 10000000000 --nSplit 10 --nCreate 0