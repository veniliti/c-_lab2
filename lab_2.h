#pragma once

#include <cstddef>
#include <vector>

bool is_positive(int x);
bool my_parallel_all_of(const std::vector<int>& v, std::size_t K);
void run_experiments();