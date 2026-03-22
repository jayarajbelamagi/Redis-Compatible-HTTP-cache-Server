# ⚡ RedisHTTP – Multithreaded HTTP Cache Server (C++)

RedisHTTP is a high-performance, in-memory HTTP cache server built in C++ that mimics core Redis-like caching behavior with support for **LRU eviction, TTL expiration, and concurrent request handling**.

---

## 🚀 Overview

This project implements a lightweight HTTP server that caches responses in memory and serves them efficiently using:

* LRU (Least Recently Used) cache eviction
* TTL (Time-To-Live) based expiration
* Multithreading using a thread pool

It is designed to demonstrate **systems programming, concurrency, and data structure optimization**.

---

## 🌟 Features

### ⚡ In-Memory Cache

* Fast key-value storage using `unordered_map`
* O(1) lookup and insertion
* Efficient memory usage

---

### 🔁 LRU Cache Eviction

* Uses doubly linked list (`std::list`)
* Automatically removes least recently used items when capacity is full

---

### ⏳ TTL Expiration

* Each cache entry has a time-to-live
* Expired entries are removed dynamically
* Prevents stale data

---

### 🧵 Multithreading (Thread Pool)

* Handles multiple clients concurrently
* Fixed-size worker thread pool
* Efficient task queue using condition variables

---

### 🌐 HTTP Server

* Built using Winsock (Windows sockets)
* Handles basic HTTP GET requests
* Returns cached or generated responses

---

### 📊 Metrics Endpoint

Access:

```
http://localhost:8080/metrics
```

Returns:

* Cache hits
* Cache misses
* Current cache size

---

## 🛠️ Tech Stack

* C++
* STL (`unordered_map`, `list`, `thread`, `mutex`)
* Winsock2 (Networking)
* Multithreading & Concurrency

---

## 📂 Project Structure

```id="rd1"
RedisHTTP/
│── server.cpp   (main server + cache implementation)
```

---

## ⚙️ How It Works

1. Client sends HTTP GET request
2. Server extracts key from URL
3. Cache is checked:

   * If found → return cached response (HIT)
   * If expired/missing → generate new response (MISS)
4. Cache updates using LRU + TTL
5. Response sent back to client

---

## ▶️ Usage

### 1️⃣ Compile

```bash id="rd2"
g++ server.cpp -o server -lws2_32
```

---

### 2️⃣ Run

```bash id="rd3"
./server
```

---

### 3️⃣ Test in Browser

```
http://localhost:8080/test
http://localhost:8080/hello
http://localhost:8080/metrics
```

---

## 📊 Example Responses

* `GET /hello`
  → `Generated data for hello` (first time)
  → `Cache HIT: Generated data for hello` (subsequent requests)

---

## 💡 Key Highlights

* Combines **systems design + networking + concurrency**
* Demonstrates real-world caching strategies
* Efficient handling of multiple client requests
* Clean implementation of LRU + TTL together

---

## 🚀 Future Improvements

* Support for POST/PUT requests
* Persistent storage (disk-based caching)
* Distributed caching (cluster mode)
* Linux socket support (POSIX)
* Benchmarking & load testing

---

## 📌 Code Reference

Core implementation includes:

* LRU Cache with TTL
* Thread Pool for concurrency
* HTTP request parsing

(See full source: )

---

## 🤝 Contributing

Pull requests are welcome!

---

## 👨‍💻 Author

Developed by **[Jayaraj Belamagi]**

---
