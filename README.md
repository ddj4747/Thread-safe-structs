# 🧵 Thread-safe-structs

A lightweight C++ header-only library providing thread-safe wrappers around common STL containers using internal `std::mutex` locking.

## ✨ Features

- Header-only, no dependencies
- Thread-safe `vector` and `deque`
- STL-like interface
- Safe for concurrent access

## 📦 Included Containers

| Container        | STL Equivalent       | Description                      |
|------------------|----------------------|----------------------------------|
| `ts::vector<T>`  | `std::vector<T>`     | Thread-safe dynamic array        |
| `ts::deque<T>`   | `std::deque<T>`      | Thread-safe double-ended queue   |

## [```📚 Documentation```](https://github.com/ddj4747/Thread-safe-structs/wiki)
