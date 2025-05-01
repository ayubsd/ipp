#include <vector>
#include <algorithm>
#include <omp.h>
#include <cstdlib>
#include <ctime>
#include<iostream>
using namespace std;

void distributedHistogramSort(std::vector<int>& data) {
    int n = data.size();
    if (n == 0) return;

    // Step 1: Find global min and max using OpenMP reductions
    int min_val = data[0];
    int max_val = data[0];
    #pragma omp parallel for reduction(min:min_val) reduction(max:max_val)
    for (int i = 0; i < n; ++i) {
        if (data[i] < min_val) min_val = data[i];
        if (data[i] > max_val) max_val = data[i];
    }

    // Define number of buckets and calculate bucket width
    const int B = 256;
    double range = max_val - min_val + 1; // +1 to avoid division by zero
    double bucket_width = range / B;

    // Containers for global histogram and prefix sums
    vector<int> global_hist(B, 0);
    vector<int> prefix_sum(B + 1, 0);
    vector<int> sorted_data(n);
    vector<vector<int>> shared_local_hists;

    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int num_threads = omp_get_num_threads();

        // Step 2: Each thread processes its chunk and fills local histogram and buckets
        vector<int> local_hist(B, 0);
        vector<vector<int>> local_buckets(B);

        #pragma omp for
        for (int i = 0; i < n; ++i) {
            int val = data[i];
            int b = static_cast<int>((val - min_val) / bucket_width);
            if (b >= B) b = B - 1; // Handle the max_val case
            local_hist[b]++;
            local_buckets[b].push_back(val);
        }

        // Step 3: Aggregate local histograms into global_hist using atomic operations
        for (int b = 0; b < B; ++b) {
            #pragma omp atomic
            global_hist[b] += local_hist[b];
        }

        #pragma omp barrier

        // Step 4: Compute prefix sums for global histogram (single thread)
        #pragma omp single
        {
            for (int b = 1; b <= B; ++b) {
                prefix_sum[b] = prefix_sum[b - 1] + global_hist[b - 1];
            }
        }

        // Step 5: Set up shared storage for local histograms
        #pragma omp single
        {
            shared_local_hists.resize(num_threads, std::vector<int>(B, 0));
        }

        // Each thread copies its histogram to the shared structure
        for (int b = 0; b < B; ++b) {
            shared_local_hists[tid][b] = local_hist[b];
        }

        #pragma omp barrier

        // Step 6: Calculate per-thread offsets for each bucket
        std::vector<int> thread_offset(B, 0);
        for (int b = 0; b < B; ++b) {
            int offset = prefix_sum[b];
            for (int t = 0; t < tid; ++t) {
                offset += shared_local_hists[t][b];
            }
            thread_offset[b] = offset;
        }

        // Step 7: Scatter elements into sorted_data using precomputed offsets
        for (int b = 0; b < B; ++b) {
            int pos = thread_offset[b];
            for (int val : local_buckets[b]) {
                sorted_data[pos++] = val;
            }
        }
    } // End of parallel scattering

    // Step 8: Sort each bucket in parallel
    #pragma omp parallel for
    for (int b = 0; b < B; ++b) {
        int start = prefix_sum[b];
        int end = prefix_sum[b + 1];
        if (start < end) {
            sort(sorted_data.begin() + start, sorted_data.begin() + end);
        }
    }

    data.swap(sorted_data);
}

int main() {
    const int N = 100000;
    std::vector<int> data;
    srand(time(0));
    for (int i = 0; i < N; ++i)
        data.push_back(rand() % 10000);  // Values in range 0–9999

    cout << "Sorting...\n";
    distributedHistogramSort(data);

    cout << "First 20 sorted elements:\n";
    for (int i = 0; i < 20; ++i)
        cout << data[i] << " ";
    cout <<endl;

    return 0;
}