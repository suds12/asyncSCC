// Copyright 2019-2021 Lawrence Livermore National Security, LLC and other YGM
// Project Developers. See the top-level COPYRIGHT file for details.
//
// SPDX-License-Identifier: MIT

#include "reader.cpp"
#include <ygm/comm.hpp>

int main(int argc, char **argv) {
  ygm::comm world(&argc, &argv);

  // Define the function to execute in async
  auto howdy = [](auto pcomm, int from, const std::string &str) {
    std::cout << "Howdy, I'm rank " << pcomm->rank()
              << ", and I received a message from rank " << from
              << " that read: \"" << str << "\"" << std::endl;
  };

  read_cc_map(argv, world.rank());

  if (world.rank() == 2) {
    std::cout << meta_vertex_t.vertices[0] << " " << meta_vertex_t.vertices[1]
              << " " << meta_vertex_t.vertices[2] << std::endl;
  }

  return 0;
}