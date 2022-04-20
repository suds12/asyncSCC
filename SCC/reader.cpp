#pragma once
#include "common.h"
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
#include <ygm/container/map.hpp>

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
int read_change_set(char *argv[], auto &color_map, auto &me_set_ptr, int rank) {

  //---visitor lambda---------
  auto visitor_get_info = [](auto mv_pair, int from, auto &me_set_ptr) {
    std::cout << "sending back info to " << from << std::endl;
    // auto meta_edge = std::make_pair(from, mv_pair.second);
    auto temp = std::make_pair(from, mv_pair.second);
    me_set_ptr->insert(mv_pair.second);
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
    if (rank_info.vertices.find(node2) != rank_info.vertices.end()) {
      color_map->async_visit(node1, visitor_get_info, rank, me_set_ptr);
      // world.async()
    }
  }
  return 0;
}

int read_partition(char *argv[], int rank) {
  std::ifstream PartFile;
  PartFile.open(argv[1]);
  std::string line;
  if (!PartFile.is_open()) {
    std::cerr << "Could not open the file - '" << argv[1] << "'" << std::endl;
    return EXIT_FAILURE;
  }
  for (int lineno = 0; std::getline(PartFile, line); lineno++) {
    if (lineno == rank) {
      std::istringstream ss(line);
      int x;
      while (ss >> x) {
        rank_info.owned_colors.insert(x);
      }
    }
  }
  return 0;
}

int read_color_map(char *argv[], int rank, auto &color_map) {
  std::ifstream MapFile;
  MapFile.open(argv[2]);
  std::string line;
  if (!MapFile.is_open()) {
    std::cerr << "Could not open the file - '" << argv[2] << "'" << std::endl;
    return EXIT_FAILURE;
  }
  for (int lineno = 0; std::getline(MapFile, line); lineno++) {
    if (rank_info.owned_colors.find(lineno) != rank_info.owned_colors.end()) {
      std::istringstream ss(line);
      int x;
      while (ss >> x) {
        rank_info.vertices.insert(x);
        color_map->async_insert_if_missing(x, lineno); // vertex : color mapping
      }
    }
    // if (rank == 0)
    //   std::cout << lineno << " : " << line << std::endl;
  }
  return 0;
}