#include "lab_2.h"
#include "file_processing.h"

#include <algorithm>
#include <chrono>
#include <execution>
#include <iostream>
#include <thread>

using namespace std;

bool is_positive(int x) { return x > 0; }

static volatile bool sink_;

static double time_it(auto&& f)
{
    auto t0 = chrono::steady_clock::now();
    f();
    auto t1 = chrono::steady_clock::now();
    return chrono::duration<double>(t1 - t0).count();
}

bool my_parallel_all_of(const vector<int>& v, size_t K)
{
    size_t n = v.size();
    if (n == 0 || K == 0)
        return true;

    size_t chunk = (n + K - 1) / K;

    vector<char> results(K, 1);
    vector<jthread> threads;
    threads.reserve(K);

    for (size_t i = 0; i < K; ++i)
    {
        size_t start = i * chunk;
        size_t end = min(start + chunk, n);
        if (start >= end)
            continue;

        threads.emplace_back([&v, &results, i, start, end]
            {
                results[i] = all_of(v.begin() + start, v.begin() + end, is_positive) ? 1 : 0;
            });
    }
    for (auto& t : threads)
        t.join();

    return all_of(results.begin(), results.end(), [](char r) { return r != 0; });
}

void run_experiments()
{
    cout << "Variant 1: all_of" << endl;
    cout << "Hardware concurrency = " << thread::hardware_concurrency() << endl;

    const vector<size_t> sizes = { 100'000, 1'000'000, 10'000'000, 100'000'000 };
    const vector<size_t> k_values = { 1, 2, 4, 8, 16, 32, 64 };

    FileProcessing store("data");

    for (size_t n : sizes)
    {
        cout << endl;
        cout << "Data size N = " << n << endl;

        store.ensure(n);
        auto data = store.load(n);

        cout << "Library all_of:" << endl;

        auto bench = [&](const char* name, auto&& f)
            {
                double t = time_it(f);
                cout << "  " << name << " took " << t << " seconds" << endl;
            };

        bench("no policy", [&] { sink_ = all_of(data.begin(), data.end(), is_positive); });
        bench("seq      ", [&] { sink_ = all_of(execution::seq, data.begin(), data.end(), is_positive); });
        bench("par      ", [&] { sink_ = all_of(execution::par, data.begin(), data.end(), is_positive); });
        bench("unseq    ", [&] { sink_ = all_of(execution::unseq, data.begin(), data.end(), is_positive); });
        bench("par_unseq", [&] { sink_ = all_of(execution::par_unseq, data.begin(), data.end(), is_positive); });

        cout << "Custom parallel all_of:" << endl;

        double best_time = 1e18;
        size_t best_k = 1;

        for (size_t k : k_values)
        {
            double t = time_it([&] { sink_ = my_parallel_all_of(data, k); });
            cout << "  K = " << k << " took " << t << " seconds" << endl;
            if (t < best_time)
            {
                best_time = t;
                best_k = k;
            }
        }

        cout << "Best K is " << best_k
            << ", it took " << best_time << " seconds" << endl;
        cout << "Hardware concurrency = " << thread::hardware_concurrency() << endl;
    }
}