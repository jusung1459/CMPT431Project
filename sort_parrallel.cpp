#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include "core/cxxopts.h"
#include "core/get_time.h"
#include "binaryFileController.h"

using namespace std;

#define DEFAULT_N "10000000000"
#define DEFAULT_SPLIT "8"
#define DEFAULT_SEED 123
#define DEFAULT_RAM_SIZE "5"
#define DEFAULT_THREAD_SIZE "2"

// Node to store value and chunk file number
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

// checks if array is sorted
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

// reads in a chunk from the large file to vector
// then sorts the vector
// store sorted vector into sub file
void sort_K(unsigned int k, unsigned long long size, unsigned int start, unsigned int end, unsigned int threads) {
  unsigned long split_size = size/threads;
  printf("%d, %d\n", start, end);
  printf("%lu, %lu, %d\n", split_size, size, k);

  for (int i = start; i < end; i++) {
    vector<float> *readArr = new vector<float>(split_size);
    std::string file_name = "sortedFloats_" + std::to_string(i) + ".bin";
    printf("%d\n", i);
    binRead(readArr, "randomFloats.bin", split_size,i*split_size);
    std::sort((*readArr).begin(), (*readArr).begin()+split_size);
    binWrite(readArr, file_name, split_size, 0);
    delete readArr;
  }
}

// loads in chunks from sub files into vectors
// merges vectors into sorted vector
// by using min heap to compare values
void merge_K(unsigned int K, unsigned long long size, unsigned int ram, unsigned int threads) {
  std::vector<float> vecs[K*threads];
  std::vector<float> sorted_vec;
  int index_K[K];
  unsigned long current_MaxK[K*threads];
  unsigned long prev_MaxK[K*threads];
  unsigned long split_size = size/(K*threads);
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
      binWrite(&sorted_vec, "sortedfloats.bin", sorted_vec.size(), 1);
      sorted_vec.resize(0);
      sorted_vec.reserve(current_MaxK[chunk]);
    }
    
  }
  binWrite(&sorted_vec, "sortedfloats.bin", sorted_vec.size(), 1);
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
          {"nRam", "Gb or Ram",         
          cxxopts::value<unsigned int>()->default_value(DEFAULT_RAM_SIZE)},
          {"nThreads", "Number of threads",         
          cxxopts::value<unsigned int>()->default_value(DEFAULT_RAM_SIZE)}
      });
  auto cl_options = options.parse(argc, argv);
  unsigned long long n_size = cl_options["nSize"].as<unsigned long long>();
  unsigned long long n_split = cl_options["nSplit"].as<unsigned long long>();
  unsigned int n_ram = cl_options["nRam"].as<unsigned int>();
  unsigned int n_threads = cl_options["nThreads"].as<unsigned int>();

  std::cout << "Number of floating points : " << n_size << "\n";
  std::cout << "Number of split points : " << n_split << "\n";
  std::cout << "Ram Size : " << n_ram << "\n";
  std::cout << "Number of Threads : " << n_threads << std::endl;
  
  timer serial_timer;

  serial_timer.start();

  std::thread threads[n_threads];

  int start = 0;
  int end = n_split;
  printf("main %d %d\n", start, end);
  for (unsigned int i = 0; i < n_threads; i++) {
    threads[i] = std::thread(sort_K, 
                            n_split, n_size, start, end, n_threads);
    start = end;
    end += n_split;          
    printf("main %d %d\n", start, end);
  } 
  for (int i = 0; i < n_threads; i++) {
    threads[i].join();
  } 

  merge_K(n_split, n_size, n_ram, n_threads);

  double time_taken = serial_timer.stop();
  printf("time: %f\n", time_taken);

  vector<float> sorted_array;

  for (int k = 0; k < 10; k++) {
    sorted_array.resize(0);
    sorted_array.resize(n_size/(n_split*n_threads));
    binRead(&sorted_array, "sortedfloats.bin", n_size/(n_split*n_threads), k*(n_size/(n_split*n_threads)));
    printf("%lu\n", sorted_array.size());
    isSort(sorted_array, n_size/(n_split*n_threads));
  }

  return 0;
}


// ./sort_serial --nSize 10000000000 --nSplit 10 --nCreate 0