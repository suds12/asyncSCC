#include "common.h"
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unordered_set>
#include <vector>

// This function is for memory mapping. Each reading function would call this
// with their respective files as arg1
const char *get_file_map_info(const char *fname, size_t &num_bytes, int rank) {
  int fd = open(fname, O_RDONLY); // 19
  if (fd == -1) {
    if (rank == 0) {
      std::cerr << "error opening the file" << std::endl;
    }
    return fname; // if there is an error, just returning the filename back
  }
  struct stat sb; // 20
  if (fstat(fd, &sb) == -1) {
    if (rank == 0) {
      std::cerr << " error in fstat " << std::endl;
    }
    return fname;
  }
  num_bytes = sb.st_size;
  const char *addr = static_cast<const char *>(
      mmap(NULL, num_bytes, PROT_READ, MAP_PRIVATE, fd, 0u));

  // addr[0], addr[1] give the first and 2nd character in file
  if (addr == MAP_FAILED) {
    if (rank == 0) {
      std::cerr << "mmap failed";
    }
    return fname;
  }

  return addr;
}

int read_cc_map(char *argv[], int rank) {
  std::ifstream MapFile;
  MapFile.open(argv[2]);
  std::string line;
  if (!MapFile.is_open()) {
    std::cerr << "Could not open the file - '" << argv[2] << "'" << std::endl;
    return EXIT_FAILURE;
  }
  for (int lineno = 0; std::getline(MapFile, line); lineno++) {
    if (lineno == rank) {
      std::istringstream ss(line);
      std::vector<int> temp_vec;
      int x;
      while (ss >> x) {
        temp_vec.push_back(x);
      }
      meta_vertex_t.vertices = temp_vec;
    }
    // if (rank == 0)
    //   std::cout << lineno << " : " << line << std::endl;
  }
  return 0;
}