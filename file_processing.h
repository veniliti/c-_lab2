#pragma once

#include <cstddef>
#include <string>
#include <vector>

class FileProcessing
{
public:
    explicit FileProcessing(std::string dir);

    void ensure(std::size_t n) const;
    std::vector<int> load(std::size_t n) const;

private:
    std::string dir_;
    std::string path_for(std::size_t n) const;
};