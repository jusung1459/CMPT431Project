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
  float value;  // floating-point value
  int chunk;    // which file the value is from
};

struct compare {
  bool operator()(Node a,Node b)
  {
      return a.value>b.value;
  }
};

// Checks if the input vector arr is sorted in ascending order
// @params
// arr = input vector of floating numbers
// N = size of vector arr
bool isSort(std::vector<float> arr, unsigned long N) {
  float prev, cur, next;
  for(int i = 1; i < N; i++) {
    if (arr[i-1] > arr[i]) {
      prev = arr[i-1];
      cur = arr[i];
      next = arr[i+1];
      printf("Mistake founded in array. Sequence not sorted [%f, %f, %f]\n", prev, cur, next);
      return false;
    }
  }
  return true;
}

// Separates an array of 10 billion numbers into smaller chunks
// and then sorts those chunks using quicksort
// Array chunks are stored in separate files
// @params
// k = number of splits (how many times the original array is partitioned)
// size = total size of the array (10 billion for this assignment)
// start = the start of the partition
// end = the end of the partition
// threads = total number of threads
void sort_K(unsigned int k, unsigned long long size, unsigned int start, unsigned int end, unsigned int threads) {
  unsigned long split_size = size/threads;

  for (int i = start; i < end; i++) {
    vector<float> *readArr = new vector<float>(split_size);
    std::string file_name = "sortedFloats_" + std::to_string(i) + ".bin";
    binRead(readArr, "randomFloats.bin", split_size,i*split_size);
    printf("Finished reading from large bin file, index: %d \n", i);
    std::sort((*readArr).begin(), (*readArr).begin()+split_size);
    binWrite(readArr, file_name, split_size, 0);
    printf("Finished writing to bin file %s \n", file_name.c_str());
    delete readArr;
  }

  printf("Finished sorting %lld files\n", k);
}

// Merge all sorted array chunks into one big array
// @params
// K = number of splits (how many times the original array is partitioned)
// size = total size of the array (10 billion for this assignment)
// ram = not used
// threads = total number of threads
void merge_K(unsigned int K, unsigned long long size, unsigned int ram, unsigned int threads) {
  printf("Merging all sorted files into one\n");

  std::vector<float> vecs[K*threads];
  std::vector<float> sorted_vec;
  int index_K[K];
  unsigned long current_MaxK[K*threads];
  unsigned long prev_MaxK[K*threads];
  unsigned long split_size = size/(K*threads);
  unsigned long avaiable_floats = 10000000; 

  sorted_vec.reserve(split_size);

  // Initialization step
  // Store the beginning of each sorted bin file in vecs
  // All numbers are separated by which file they came from
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

  // Initialize a min heap
  // using floating point values from vecs
  priority_queue<Node,vector<Node>, compare> minh;
  unsigned long index = 0;
  for(int j=0; j<K; j++) {
    if(vecs[j].empty() != true) {
      struct Node node;
      node.value = vecs[j][index_K[j]];
      node.chunk = j;
      minh.push(node);
      index_K[j]++;
      index++;
    }
  }

  // Combine smaller sorted bin files 
  // into one large sorted file
  int chunk =0;
  while(minh.size() > 0) {
    // Get min value from heap
    // and insert it into the sorted vector
    chunk = minh.top().chunk;
    sorted_vec.push_back(minh.top().value);
    minh.pop();

    // If we ran out of numbers stored in vecs for one file
    if (index_K[chunk] >= current_MaxK[chunk]) {
      if (index_K[chunk] < split_size) {
        prev_MaxK[chunk] = current_MaxK[chunk];
        if ((avaiable_floats+index_K[chunk]) < split_size) {
          current_MaxK[chunk] = avaiable_floats+index_K[chunk];
        } else {
          current_MaxK[chunk] = split_size;
        }
        // Load in more numbers from the file to vecs
        // so we have access to them in memory
        vecs[chunk].resize(0);
        vecs[chunk].resize(current_MaxK[chunk]-index_K[chunk]);
        std::string file = "sortedFloats_" + std::to_string(chunk) + ".bin";
        binRead(&vecs[chunk], file, (current_MaxK[chunk]-index_K[chunk]), index_K[chunk]);
      }
    }

    // If we still have numbers stored in vecs  
    if (index_K[chunk] < current_MaxK[chunk]) {
      // Determine which number we just took out of the heap
      // and which file it belonged to
      // And then, push a new number from that file (from vecs) into the heap
      struct Node node;
      node.value = vecs[chunk][index_K[chunk]-prev_MaxK[chunk]];
      node.chunk = chunk;
      minh.push(node);
      index_K[chunk]++;
      index++;
    } 
    
    // If the sorted vector is full
    if(sorted_vec.size() > split_size) {
      // Write the values in the sorted vector to a file
      binWrite(&sorted_vec, "sortedFloats.bin", sorted_vec.size(), 1);
      // Reset the sorted vector to take in more values
      sorted_vec.resize(0);
      sorted_vec.reserve(current_MaxK[chunk]);
    }
    
  }
  binWrite(&sorted_vec, "sortedFloats.bin", sorted_vec.size(), 1);
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

  // Read input arguments
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

  // Sort K different chunks of the big array
  // in different threads
  std::thread threads[n_threads];
  int start = 0;
  int end = n_split;
  for (unsigned int i = 0; i < n_threads; i++) {
    threads[i] = std::thread(sort_K, 
                            n_split, n_size, start, end, n_threads);
    start = end;
    end += n_split;          
  } 

  // Wait for threads to finish sorting their separate files
  for (int i = 0; i < n_threads; i++) {
    threads[i].join();
  } 

  // Merge separate files into one sorted array
  merge_K(n_split, n_size, n_ram, n_threads);

  double time_taken = serial_timer.stop();

  // Print statistics
  printf("\n***************\n");
  printf("Finished merging all sorted bin files into one\n");
  printf("Time taken to sort: %f\n", time_taken);
  printf("***************\n\n");

  vector<float> sorted_array;
  printf("Validating the output. Checking if the array is sorted in ascending order.\n");

  bool sorted = true;
  for (int k = 0; k < 10; k++) {
    sorted_array.resize(0);
    sorted_array.resize(n_size/(n_split*n_threads));
    binRead(&sorted_array, "sortedFloats.bin", n_size/(n_split*n_threads), k*(n_size/(n_split*n_threads)));
    sorted = sorted && isSort(sorted_array, n_size/(n_split*n_threads));
  }

  if (sorted) {
    printf("Validation complete. Array was sorted correctly\n");
  } else {
    printf("Validation complete. Array was not sorted correctly.\n");
  }

  return 0;
}
