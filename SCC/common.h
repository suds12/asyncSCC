#include <iostream>
#include <set>
#include <vector>
#include <ygm/comm.hpp>
#include <ygm/container/detail/hash_partitioner.hpp>
#include <ygm/container/disjoint_set.hpp>
#include <ygm/container/map.hpp>
#include <ygm/container/set.hpp>
#include <ygm/detail/ygm_ptr.hpp>

class MetaVertex {
public:
  std::set<int> vertices;
  std::set<int> owned_colors;
  std::set<std::pair<int, int>> owned_outgoing_ME;

} rank_info;