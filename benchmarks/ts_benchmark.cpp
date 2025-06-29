#include <benchmark/benchmark.h>
#include <TSVector.h>
#include <TSDeque.h>

#include <thread>
#include <vector>
#include <deque>
#include <random>
#include <numeric>

// --- Clear Benchmarks ---

static void BM_TSVector_Clear(benchmark::State& state) {
    ts::vector<int> v;
    for (int i = 0; i < state.range(0); ++i)
        v.push_back(i);

    for (auto _ : state) {
        v.clear();
        for (int i = 0; i < state.range(0); ++i)
            v.push_back(i);
    }
}
BENCHMARK(BM_TSVector_Clear)->Range(1 << 10, 1 << 18);

static void BM_StdVector_Clear(benchmark::State& state) {
    std::vector<int> v;
    for (int i = 0; i < state.range(0); ++i)
        v.push_back(i);

    for (auto _ : state) {
        v.clear();
        for (int i = 0; i < state.range(0); ++i)
            v.push_back(i);
    }
}
BENCHMARK(BM_StdVector_Clear)->Range(1 << 10, 1 << 18);

static void BM_TSDeque_Clear(benchmark::State& state) {
    ts::deque<int> d;
    for (int i = 0; i < state.range(0); ++i)
        d.push_back(i);

    for (auto _ : state) {
        d.clear();
        for (int i = 0; i < state.range(0); ++i)
            d.push_back(i);
    }
}
BENCHMARK(BM_TSDeque_Clear)->Range(1 << 10, 1 << 18);

static void BM_StdDeque_Clear(benchmark::State& state) {
    std::deque<int> d;
    for (int i = 0; i < state.range(0); ++i)
        d.push_back(i);

    for (auto _ : state) {
        d.clear();
        for (int i = 0; i < state.range(0); ++i)
            d.push_back(i);
    }
}
BENCHMARK(BM_StdDeque_Clear)->Range(1 << 10, 1 << 18);

// --- Empty Benchmarks ---

static void BM_TSVector_Empty(benchmark::State& state) {
    ts::vector<int> v;
    for (auto _ : state) {
        benchmark::DoNotOptimize(v.empty());
    }
}
BENCHMARK(BM_TSVector_Empty);

static void BM_StdVector_Empty(benchmark::State& state) {
    std::vector<int> v;
    for (auto _ : state) {
        benchmark::DoNotOptimize(v.empty());
    }
}
BENCHMARK(BM_StdVector_Empty);

static void BM_TSDeque_Empty(benchmark::State& state) {
    ts::deque<int> d;
    for (auto _ : state) {
        benchmark::DoNotOptimize(d.empty());
    }
}
BENCHMARK(BM_TSDeque_Empty);

static void BM_StdDeque_Empty(benchmark::State& state) {
    std::deque<int> d;
    for (auto _ : state) {
        benchmark::DoNotOptimize(d.empty());
    }
}
BENCHMARK(BM_StdDeque_Empty);

// === Setup Helper ===
std::vector<int> generate_data(size_t size) {
    std::vector<int> data(size);
    std::iota(data.begin(), data.end(), 0);
    return data;
}

// === ts::vector vs std::vector push_back ===

static void BM_TSVector_PushBack(benchmark::State& state) {
    for (auto _ : state) {
        ts::vector<int> v;
        for (int i = 0; i < state.range(0); ++i)
            v.push_back(i);
    }
}
BENCHMARK(BM_TSVector_PushBack)->Range(1 << 10, 1 << 18); // 1K–256K

static void BM_StdVector_PushBack(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<int> v;
        for (int i = 0; i < state.range(0); ++i)
            v.push_back(i);
    }
}
BENCHMARK(BM_StdVector_PushBack)->Range(1 << 10, 1 << 18);

// === ts::deque vs std::deque push_back/pop_front cycle ===

static void BM_TSDeque_PushBackPopFront(benchmark::State& state) {
    for (auto _ : state) {
        ts::deque<int> d;
        for (int i = 0; i < state.range(0); ++i)
            d.push_back(i);
        while (!d.empty())
            d.pop_front();
    }
}
BENCHMARK(BM_TSDeque_PushBackPopFront)->Range(1 << 10, 1 << 18);

static void BM_StdDeque_PushBackPopFront(benchmark::State& state) {
    for (auto _ : state) {
        std::deque<int> d;
        for (int i = 0; i < state.range(0); ++i)
            d.push_back(i);
        while (!d.empty())
            d.pop_front();
    }
}
BENCHMARK(BM_StdDeque_PushBackPopFront)->Range(1 << 10, 1 << 18);

// === Snapshot of ts::vector during write ===

static void BM_TSVector_SnapshotWhilePushing(benchmark::State& state) {
    for (auto _ : state) {
        ts::vector<int> v;
        for (int i = 0; i < state.range(0); ++i) {
            v.push_back(i);
            if (i % 100 == 0) [[maybe_unused]] auto snap = v.snapshot();
        }
    }
}
BENCHMARK(BM_TSVector_SnapshotWhilePushing)->Range(1 << 10, 1 << 16);

// === Concurrent test: writer + snapshotter (realistic use case) ===

static void BM_TSVector_ConcurrentSnapshot(benchmark::State& state) {
    for (auto _ : state) {
        ts::vector<int> v;
        std::atomic<bool> done = false;

        std::thread writer([&] {
            for (int i = 0; i < state.range(0); ++i)
                v.push_back(i);
            done = true;
        });

        while (!done)
            [[maybe_unused]] auto snap = v.snapshot();

        writer.join();
    }
}

BENCHMARK(BM_TSVector_ConcurrentSnapshot)->Range(1 << 10, 1 << 16);

// --- Resize Benchmarks ---

static void BM_TSVector_Resize(benchmark::State& state) {
    for (auto _ : state) {
        ts::vector<int> v;
        v.resize(state.range(0));
    }
}
BENCHMARK(BM_TSVector_Resize)->Range(1 << 10, 1 << 18);

static void BM_StdVector_Resize(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<int> v;
        v.resize(state.range(0));
    }
}
BENCHMARK(BM_StdVector_Resize)->Range(1 << 10, 1 << 18);

// --- PopBack / PopFront Benchmarks ---

static void BM_TSVector_PopBack(benchmark::State& state) {
    ts::vector<int> v;
    for (int i = 0; i < state.range(0); ++i)
        v.push_back(i);

    for (auto _ : state) {
        while (!v.empty()) v.pop_back();
        for (int i = 0; i < state.range(0); ++i)
            v.push_back(i);
    }
}
BENCHMARK(BM_TSVector_PopBack)->Range(1 << 10, 1 << 18);

static void BM_StdVector_PopBack(benchmark::State& state) {
    std::vector<int> v;
    for (int i = 0; i < state.range(0); ++i)
        v.push_back(i);

    for (auto _ : state) {
        while (!v.empty()) v.pop_back();
        for (int i = 0; i < state.range(0); ++i)
            v.push_back(i);
    }
}
BENCHMARK(BM_StdVector_PopBack)->Range(1 << 10, 1 << 18);

// --- Swap Benchmarks ---

static void BM_TSVector_Swap(benchmark::State& state) {
    ts::vector<int> a, b;
    for (int i = 0; i < state.range(0); ++i) {
        a.push_back(i);
        b.push_back(state.range(0) - i);
    }
    for (auto _ : state) {
        a.swap(b);
    }
}
BENCHMARK(BM_TSVector_Swap)->Range(1 << 10, 1 << 18);

static void BM_StdVector_Swap(benchmark::State& state) {
    std::vector<int> a(state.range(0)), b(state.range(0));
    std::iota(a.begin(), a.end(), 0);
    std::iota(b.begin(), b.end(), 100);
    for (auto _ : state) {
        a.swap(b);
    }
}
BENCHMARK(BM_StdVector_Swap)->Range(1 << 10, 1 << 18);

// --- Erase_if Benchmarks ---

static void BM_TSVector_EraseIf(benchmark::State& state) {
    for (auto _ : state) {
        ts::vector<int> v;
        for (int i = 0; i < state.range(0); ++i)
            v.push_back(i);
        v.erase_if([](int x) { return x % 2 == 0; });
    }
}
BENCHMARK(BM_TSVector_EraseIf)->Range(1 << 10, 1 << 18);

// --- Snapshot (Large Copy) Benchmark ---

static void BM_TSVector_Snapshot(benchmark::State& state) {
    ts::vector<int> v;
    for (int i = 0; i < state.range(0); ++i)
        v.push_back(i);

    for (auto _ : state) {
        auto copy = v.snapshot();
        benchmark::DoNotOptimize(copy);
    }
}
BENCHMARK(BM_TSVector_Snapshot)->Range(1 << 10, 1 << 18);

// --- TSDeque PopBack ---

static void BM_TSDeque_PopBack(benchmark::State& state) {
    ts::deque<int> d;
    for (int i = 0; i < state.range(0); ++i)
        d.push_back(i);

    for (auto _ : state) {
        while (!d.empty()) d.pop_back();
        for (int i = 0; i < state.range(0); ++i)
            d.push_back(i);
    }
}
BENCHMARK(BM_TSDeque_PopBack)->Range(1 << 10, 1 << 18);

static void BM_StdDeque_PopBack(benchmark::State& state) {
    std::deque<int> d;
    for (int i = 0; i < state.range(0); ++i)
        d.push_back(i);

    for (auto _ : state) {
        while (!d.empty()) d.pop_back();
        for (int i = 0; i < state.range(0); ++i)
            d.push_back(i);
    }
}
BENCHMARK(BM_StdDeque_PopBack)->Range(1 << 10, 1 << 18);

static void BM_TSVector_PushBack_MultiThreaded(benchmark::State& state) {
    const size_t count = state.range(0);
    const int num_threads = state.threads();

    for (auto _ : state) {
        ts::vector<int> vec;
        std::atomic<int> index{0};

        benchmark::DoNotOptimize(vec);
        state.PauseTiming();

        // Resume timing after setup
        state.ResumeTiming();

        for (;;) {
            int i = index.fetch_add(1, std::memory_order_relaxed);
            if (i >= static_cast<int>(count)) break;
            vec.push_back(i);
        }

        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * count);
}

BENCHMARK(BM_TSVector_PushBack_MultiThreaded)
    ->Range(1024, 262144)
    ->Threads(2)
    ->Threads(4)
    ->Threads(8);


static void BM_TSVector_SnapshotWhileWriting_MultiThreaded(benchmark::State& state) {
    const size_t count = state.range(0);
    ts::vector<int> vec;
    std::atomic<bool> running{true};
    std::atomic<int> push_count{0};

    std::thread snapshot_thread([&] {
        while (running.load(std::memory_order_relaxed)) {
            auto snap = vec.snapshot();  // assumes snapshot is lock-free or safe
            benchmark::DoNotOptimize(snap);
        }
    });

    for (auto _ : state) {
        for (int i = 0; i < static_cast<int>(count); ++i) {
            vec.push_back(i);
            ++push_count;
        }
    }

    running = false;
    snapshot_thread.join();
    state.SetItemsProcessed(push_count);
}

BENCHMARK(BM_TSVector_SnapshotWhileWriting_MultiThreaded)
    ->Range(1024, 262144);

static void BM_TSVector_ConcurrentReaders(benchmark::State& state) {
    const size_t count = state.range(0);
    ts::vector<int> vec;
    for (size_t i = 0; i < count; ++i) vec.push_back(i);

    std::atomic<bool> running{true};
    std::thread writer_thread([&] {
        for (int i = 0; i < static_cast<int>(count); ++i) {
            vec.push_back(i);
        }
        running = false;
    });

    for (auto _ : state) {
        while (running.load(std::memory_order_relaxed)) {
            auto snap = vec.snapshot();
            benchmark::DoNotOptimize(snap.data());
            benchmark::DoNotOptimize(snap.size());
        }
    }

    writer_thread.join();
}

BENCHMARK(BM_TSVector_ConcurrentReaders)
    ->Range(1024, 262144)
    ->Threads(2)
    ->Threads(4)
    ->Threads(8);