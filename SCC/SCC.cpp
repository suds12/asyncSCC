// Copyright 2019-2021 Lawrence Livermore National Security, LLC and other YGM
// Project Developers. See the top-level COPYRIGHT file for details.
//
// SPDX-License-Identifier: MIT

#include "reader.cpp"

int main(int argc, char **argv) {
  ygm::comm world(&argc, &argv);

  // -------Init distributed containers------

  // ygm::container::set<std::pair<int, int>> me_set;
  ygm::container::map<int, int> color_map(world);
  // ygm::container::disjoint_set<int> connected_components(world);
  // auto cc_ptr = connected_components.get_ygm_ptr();
  auto color_map_ptr = color_map.get_ygm_ptr();

  read_partition(argv, world.rank());
  read_color_map(argv, world.rank(), color_map_ptr);
  // read_change_set(argv, mv_map_ptr, cc_ptr, world.rank());

  world.barrier();

  //-------Display results--------
  color_map.for_all([](auto k) {
    std::cout << "component : " << k.first << " -> " << k.second << std::endl;
  });

  return 0;
}