#include "common.h"
#include "update.cpp"
#include <cereal/archives/json.hpp>
#include <cereal/types/utility.hpp>
#include <fcntl.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unordered_set>
#include <utility>
#include <vector>
#include <ygm/comm.hpp>
#include <ygm/container/detail/hash_partitioner.hpp>
#include <ygm/container/disjoint_set.hpp>
#include <ygm/container/map.hpp>
#include <ygm/container/set.hpp>
#include <ygm/detail/ygm_ptr.hpp>

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
int read_change_set(char *argv[], auto &mv_map_ptr, auto &cc_ptr, int rank) {

  //---visitor lambda---------
  auto make_meta_edge = [](auto mv_pair, int from, auto &cc_ptr) {
    std::cout << "Making meta edge between " << from << " and "
              << mv_pair.second << std::endl;
    // auto meta_edge = std::make_pair(from, mv_pair.second);
    // me_set.
    cc_ptr->async_union(from, mv_pair.second);
  };
  //------------------
  std::ifstream ChangeFile;
  ChangeFile.open(argv[3]);
  std::string line;
  if (!ChangeFile.is_open()) {
    std::cerr << "Could not open the file - '" << argv[3] << "'" << std::endl;
    return EXIT_FAILURE;
  }
  for (int lineno = 0; std::getline(ChangeFile, line); lineno++) {
    std::istringstream ss(line);
    int node1, node2;
    ss >> node1;
    ss >> node2;
    if (meta_vertex_t.vertices.find(node1) != meta_vertex_t.vertices.end()) {
      mv_map_ptr->async_visit(node2, make_meta_edge, rank, cc_ptr);
    }
  }
  return 0;
}

int read_cc_map(char *argv[], int rank, ygm::container::map<int, int> &mv_map) {
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
      int x;
      while (ss >> x) {
        meta_vertex_t.vertices.insert(x);
        mv_map.async_insert(x, lineno);
      }
    }
    // if (rank == 0)
    //   std::cout << lineno << " : " << line << std::endl;
  }
  return 0;
}