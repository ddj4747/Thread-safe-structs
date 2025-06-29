#include <gtest/gtest.h>
#include <TSVector.h>
#include <TSDeque.h>
#include <thread>
#include <string>
#include <atomic>

// === ts::vector tests ===

TEST(TSVectorTest, PushBackAndSnapshot) {
    ts::vector<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);

    auto snap = v.snapshot();
    ASSERT_EQ(snap.size(), 3);
    EXPECT_EQ(snap[0], 1);
    EXPECT_EQ(snap[1], 2);
    EXPECT_EQ(snap[2], 3);
}

TEST(TSVectorTest, EmplaceAndResize) {
    ts::vector<std::string> v;
    v.emplace_back(5, 'x');
    EXPECT_EQ(v.snapshot()[0], "xxxxx");

    v.resize(3, "fill");
    EXPECT_EQ(v.size(), 3);
    EXPECT_EQ(v.snapshot()[2], "fill");
}

TEST(TSVectorTest, ThreadedPushBack) {
    ts::vector<int> v;
    std::thread t1([&] {
        for (int i = 0; i < 100; ++i) v.push_back(i);
    });
    std::thread t2([&] {
        for (int i = 100; i < 200; ++i) v.push_back(i);
    });

    t1.join();
    t2.join();

    EXPECT_EQ(v.size(), 200);
}

// === ts::deque tests ===

TEST(TSDequeTest, PushPopOrder) {
    ts::deque<std::string> d;
    d.push_back("A");
    d.push_back("B");
    d.push_front("C");

    EXPECT_EQ(d.pop_front(), "C");
    EXPECT_EQ(d.pop_front(), "A");
    EXPECT_EQ(d.pop_back(), "B");
    EXPECT_TRUE(d.empty());
}

TEST(TSDequeTest, EmplaceBackFront) {
    ts::deque<std::pair<int, int>> d;
    d.emplace_back(1, 2);
    d.emplace_front(3, 4);

    EXPECT_EQ(d.pop_front().first, 3);
    EXPECT_EQ(d.pop_back().second, 2);
}

TEST(TSDequeTest, ThreadedPushAndPop) {
    ts::deque<int> d;
    std::atomic<int> pushed = 0;

    std::thread producer([&] {
        for (int i = 0; i < 1000; ++i) {
            d.push_back(i);
            pushed++;
        }
    });

    std::thread consumer([&] {
        int popped = 0;
        while (popped < 1000) {
            if (!d.empty()) {
                d.pop_front();
                popped++;
            }
        }
    });

    producer.join();
    consumer.join();

    EXPECT_TRUE(d.empty());
}

TEST(TSVectorTest, ThreadSafeAccessAndSnapshot) {
    ts::vector<int> vec;
    constexpr int threadCount = 8;
    constexpr int itemsPerThread = 1000;

    std::vector<std::thread> threads;

    // Each thread pushes items
    for (int t = 0; t < threadCount; ++t) {
        threads.emplace_back([&vec, t] {
            for (int i = 0; i < itemsPerThread; ++i) {
                vec.push_back(t * itemsPerThread + i);
            }
        });
    }

    // One thread constantly snapshots
    std::atomic<bool> running = true;
    std::thread snapshotter([&vec, &running] {
        while (running) {
            auto snap = vec.snapshot();
            if (!snap.empty()) {
                EXPECT_GE(snap.size(), 1u);
            }
        }
    });

    for (auto& t : threads) t.join();
    running = false;
    snapshotter.join();

    EXPECT_EQ(vec.size(), threadCount * itemsPerThread);
}

TEST(TSDequeTest, ConcurrentPushPop) {
    ts::deque<int> dq;
    constexpr int pushCount = 10000;

    std::thread producer([&dq] {
        for (int i = 0; i < pushCount; ++i) {
            dq.push_back(i);
        }
    });

    std::thread consumer([&dq] {
        int popped = 0;
        while (popped < pushCount) {
            if (!dq.empty()) {
                dq.pop_front();
                popped++;
            }
        }
    });

    producer.join();
    consumer.join();
    EXPECT_TRUE(dq.empty());
}


TEST(TSVectorTest, ConcurrentEraseAndInsert) {
    ts::vector<int> vec;

    for (int i = 0; i < 1000; ++i)
        vec.push_back(i);

    std::atomic<bool> done = false;

    std::thread eraser([&vec, &done] {
        while (!done) {
            vec.erase_if([](int v) { return v % 10 == 0; });
        }
    });

    std::thread inserter([&vec, &done] {
        for (int i = 1000; i < 5000; ++i) {
            vec.push_back(i);
        }
        done = true;
    });

    eraser.join();
    inserter.join();

    auto snap = vec.snapshot();
    for (int val : snap) {
        EXPECT_NE(val % 10, 0);
    }
}

TEST(TSDequeTest, HighConcurrencyStress) {
    ts::deque<int> d;

    constexpr int threads = 10;
    constexpr int items = 10000;

    std::vector<std::thread> producers;
    for (int t = 0; t < threads; ++t) {
        producers.emplace_back([&d, t] {
            for (int i = 0; i < items; ++i) {
                d.push_back(t * items + i);
            }
        });
    }

    std::atomic<int> popped = 0;
    std::thread consumer([&] {
        while (popped < threads * items) {
            if (!d.empty()) {
                d.pop_front();
                ++popped;
            }
        }
    });

    for (auto& p : producers) p.join();
    consumer.join();
    EXPECT_TRUE(d.empty());
}