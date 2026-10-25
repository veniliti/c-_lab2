#include "file_processing.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>

using namespace std;
namespace fs = std::filesystem;

FileProcessing::FileProcessing(string dir) : dir_(move(dir)) {}

string FileProcessing::path_for(size_t n) const
{
    return dir_ + "/seq_" + to_string(n) + ".txt";
}

void FileProcessing::ensure(size_t n) const
{
    fs::create_directories(dir_);

    const string path = path_for(n);
    if (fs::exists(path))
        return;

    cout << "Generating " << path
        << " with " << n << " elements" << endl;

    ofstream out(path);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(1, 1000);

    for (size_t i = 0; i < n; ++i)
        out << dist(gen) << endl;
}

vector<int> FileProcessing::load(size_t n) const
{
    vector<int> v(n);
    ifstream in(path_for(n));
    for (size_t i = 0; i < n; ++i)
        in >> v[i];
    return v;
}