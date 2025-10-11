#include <benchmark/benchmark.h>
#include <fxt.hpp>

// Benchmark tuple creation with small types (3 ints)
static void BM_StdTuple_Creation_Small(benchmark::State& state) {
    for (auto _ : state) {
        auto t = fxt::make_tuple(1, 2, 3);
        benchmark::DoNotOptimize(t);
    }
}
BENCHMARK(BM_StdTuple_Creation_Small);

static void BM_FlatTuple_Creation_Small(benchmark::State& state) {
    for (auto _ : state) {
        auto t = fxt::make_flat_tuple(1, 2, 3);
        benchmark::DoNotOptimize(t);
    }
}
BENCHMARK(BM_FlatTuple_Creation_Small);

// Benchmark tuple creation with medium types (5 mixed types)
static void BM_StdTuple_Creation_Medium(benchmark::State& state) {
    for (auto _ : state) {
        auto t = fxt::make_tuple(1, 2.5, 3, 4.5f, 5);
        benchmark::DoNotOptimize(t);
    }
}
BENCHMARK(BM_StdTuple_Creation_Medium);

static void BM_FlatTuple_Creation_Medium(benchmark::State& state) {
    for (auto _ : state) {
        auto t = fxt::make_flat_tuple(1, 2.5, 3, 4.5f, 5);
        benchmark::DoNotOptimize(t);
    }
}
BENCHMARK(BM_FlatTuple_Creation_Medium);

// Benchmark tuple creation with large types (10 mixed types)
static void BM_StdTuple_Creation_Large(benchmark::State& state) {
    for (auto _ : state) {
        auto t = fxt::make_tuple(1, 2.5, 3, 4.5f, 5, 6.5, 7, 8.5f, 9, 10);
        benchmark::DoNotOptimize(t);
    }
}
BENCHMARK(BM_StdTuple_Creation_Large);

static void BM_FlatTuple_Creation_Large(benchmark::State& state) {
    for (auto _ : state) {
        auto t = fxt::make_flat_tuple(1, 2.5, 3, 4.5f, 5, 6.5, 7, 8.5f, 9, 10);
        benchmark::DoNotOptimize(t);
    }
}
BENCHMARK(BM_FlatTuple_Creation_Large);

// Benchmark element access (get) - Small tuple
static void BM_StdTuple_Access_Small(benchmark::State& state) {
    auto t = fxt::make_tuple(1, 2, 3);
    for (auto _ : state) {
        auto val = fxt::get<0>(t) + fxt::get<1>(t) + fxt::get<2>(t);
        benchmark::DoNotOptimize(val);
    }
}
BENCHMARK(BM_StdTuple_Access_Small);

static void BM_FlatTuple_Access_Small(benchmark::State& state) {
    auto t = fxt::make_flat_tuple(1, 2, 3);
    for (auto _ : state) {
        auto val = fxt::get<0>(t) + fxt::get<1>(t) + fxt::get<2>(t);
        benchmark::DoNotOptimize(val);
    }
}
BENCHMARK(BM_FlatTuple_Access_Small);

// Benchmark element access (get) - Medium tuple
static void BM_StdTuple_Access_Medium(benchmark::State& state) {
    auto t = fxt::make_tuple(1, 2.5, 3, 4.5f, 5);
    for (auto _ : state) {
        auto val = fxt::get<0>(t) + fxt::get<2>(t) + fxt::get<4>(t);
        benchmark::DoNotOptimize(val);
    }
}
BENCHMARK(BM_StdTuple_Access_Medium);

static void BM_FlatTuple_Access_Medium(benchmark::State& state) {
    auto t = fxt::make_flat_tuple(1, 2.5, 3, 4.5f, 5);
    for (auto _ : state) {
        auto val = fxt::get<0>(t) + fxt::get<2>(t) + fxt::get<4>(t);
        benchmark::DoNotOptimize(val);
    }
}
BENCHMARK(BM_FlatTuple_Access_Medium);

// Benchmark element access (get) - Large tuple
static void BM_StdTuple_Access_Large(benchmark::State& state) {
    auto t = fxt::make_tuple(1, 2.5, 3, 4.5f, 5, 6.5, 7, 8.5f, 9, 10);
    for (auto _ : state) {
        auto val = fxt::get<0>(t) + fxt::get<5>(t) + fxt::get<9>(t);
        benchmark::DoNotOptimize(val);
    }
}
BENCHMARK(BM_StdTuple_Access_Large);

static void BM_FlatTuple_Access_Large(benchmark::State& state) {
    auto t = fxt::make_flat_tuple(1, 2.5, 3, 4.5f, 5, 6.5, 7, 8.5f, 9, 10);
    for (auto _ : state) {
        auto val = fxt::get<0>(t) + fxt::get<5>(t) + fxt::get<9>(t);
        benchmark::DoNotOptimize(val);
    }
}
BENCHMARK(BM_FlatTuple_Access_Large);

// Benchmark combined creation and access - Small
static void BM_StdTuple_CreateAndAccess_Small(benchmark::State& state) {
    for (auto _ : state) {
        auto t = fxt::make_tuple(1, 2, 3);
        auto val = fxt::get<0>(t) + fxt::get<1>(t) + fxt::get<2>(t);
        benchmark::DoNotOptimize(val);
    }
}
BENCHMARK(BM_StdTuple_CreateAndAccess_Small);

static void BM_FlatTuple_CreateAndAccess_Small(benchmark::State& state) {
    for (auto _ : state) {
        auto t = fxt::make_flat_tuple(1, 2, 3);
        auto val = fxt::get<0>(t) + fxt::get<1>(t) + fxt::get<2>(t);
        benchmark::DoNotOptimize(val);
    }
}
BENCHMARK(BM_FlatTuple_CreateAndAccess_Small);

// Benchmark combined creation and access - Medium
static void BM_StdTuple_CreateAndAccess_Medium(benchmark::State& state) {
    for (auto _ : state) {
        auto t = fxt::make_tuple(1, 2.5, 3, 4.5f, 5);
        auto val = fxt::get<0>(t) + fxt::get<2>(t) + fxt::get<4>(t);
        benchmark::DoNotOptimize(val);
    }
}
BENCHMARK(BM_StdTuple_CreateAndAccess_Medium);

static void BM_FlatTuple_CreateAndAccess_Medium(benchmark::State& state) {
    for (auto _ : state) {
        auto t = fxt::make_flat_tuple(1, 2.5, 3, 4.5f, 5);
        auto val = fxt::get<0>(t) + fxt::get<2>(t) + fxt::get<4>(t);
        benchmark::DoNotOptimize(val);
    }
}
BENCHMARK(BM_FlatTuple_CreateAndAccess_Medium);

// Benchmark combined creation and access - Large
static void BM_StdTuple_CreateAndAccess_Large(benchmark::State& state) {
    for (auto _ : state) {
        auto t = fxt::make_tuple(1, 2.5, 3, 4.5f, 5, 6.5, 7, 8.5f, 9, 10);
        auto val = fxt::get<0>(t) + fxt::get<5>(t) + fxt::get<9>(t);
        benchmark::DoNotOptimize(val);
    }
}
BENCHMARK(BM_StdTuple_CreateAndAccess_Large);

static void BM_FlatTuple_CreateAndAccess_Large(benchmark::State& state) {
    for (auto _ : state) {
        auto t = fxt::make_flat_tuple(1, 2.5, 3, 4.5f, 5, 6.5, 7, 8.5f, 9, 10);
        auto val = fxt::get<0>(t) + fxt::get<5>(t) + fxt::get<9>(t);
        benchmark::DoNotOptimize(val);
    }
}
BENCHMARK(BM_FlatTuple_CreateAndAccess_Large);

// Benchmark pipe operator usage
static void BM_StdTuple_PipeOperator(benchmark::State& state) {
    auto t = fxt::make_tuple(1, 2, 3);
    auto accessor = [](auto& tuple) { return fxt::get<1>(tuple); };
    for (auto _ : state) {
        auto val = t | accessor;
        benchmark::DoNotOptimize(val);
    }
}
BENCHMARK(BM_StdTuple_PipeOperator);

static void BM_FlatTuple_PipeOperator(benchmark::State& state) {
    auto t = fxt::make_flat_tuple(1, 2, 3);
    auto accessor = [](auto& tuple) { return fxt::get<1>(tuple); };
    for (auto _ : state) {
        auto val = t | accessor;
        benchmark::DoNotOptimize(val);
    }
}
BENCHMARK(BM_FlatTuple_PipeOperator);

BENCHMARK_MAIN();

