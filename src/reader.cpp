#include <fcntl.h>
#include <fstream>
#include <iostream>
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

void read_cc_map(char *argv[], int rank) {
  char letter = '\0'; // 47
  int char_count = 0; // 48 keeps track of number of characters in each file
  int lineno = 0;
  size_t num_bytes_partition = 0; // 14
  char *buffer = new char[64]();
  const char *partition_pointer =
      get_file_map_info(argv[2], num_bytes_partition, rank);
}