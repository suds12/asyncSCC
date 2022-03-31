// Copyright 2019-2021 Lawrence Livermore National Security, LLC and other YGM
// Project Developers. See the top-level COPYRIGHT file for details.
//
// SPDX-License-Identifier: MIT

#include "reader.cpp"

int main(int argc, char **argv) {
  ygm::comm world(&argc, &argv);

  // Define the function to execute in async
  auto howdy = [](auto pcomm, int from, const std::string &str) {
    std::cout << "Howdy, I'm rank " << pcomm->rank()
              << ", and I received a message from rank " << from
              << " that read: \"" << str << "\"" << std::endl;
  };

  // Init distributed containers
  ygm::container::map<int, int> mv_map(world);
  ygm::container::disjoint_set<std::pair<int, int>> connected_components(world);

  auto favorites_lambda = [](auto mv_map, const int favorite_num) {
    std::cout << "My favorite animal is a " << mv_map.first << ". It says '"
              << mv_map.second << "!' My rank is " << favorite_num << std::endl;
  };

  read_cc_map(argv, world.rank(), mv_map);
  read_change_set(argv, mv_map, connected_components, world.rank());

  if (world.rank() == 2) {
    // mv_map.async_visit(4, favorites_lambda, world.rank());
  }

  return 0;
}