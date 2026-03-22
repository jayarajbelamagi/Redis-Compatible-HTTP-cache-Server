#include <iostream>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <unordered_map>
#include <list>
#include <thread>
#include <mutex>
#include <ctime>
#include <tuple>
#include <queue>
#include <condition_variable>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

// -------- LRU Cache with TTL --------
class LRUCache {
private:
    int capacity;

    list<tuple<string, string, time_t>> lruList;
    unordered_map<string, list<tuple<string,string,time_t>>::iterator> cacheMap;

public:
    int hits = 0;
    int misses = 0;

    LRUCache(int cap) {
        capacity = cap;
    }

    string get(string key) {

        auto it = cacheMap.find(key);
        if(it == cacheMap.end()) {
            misses++;
            return "EXPIRED";
        }

        time_t now = time(0);

        if(std::get<2>(*(it->second)) < now) {
            lruList.erase(it->second);
            cacheMap.erase(it);
            misses++;
            return "EXPIRED";
        }

        lruList.splice(lruList.begin(), lruList, it->second);
        hits++;

        return std::get<1>(*(it->second));
    }

    void put(string key, string value, int ttl = 10) {

        time_t expiry = time(0) + ttl;

        auto it = cacheMap.find(key);

        if(it != cacheMap.end()) {
            *(it->second) = make_tuple(key, value, expiry);
            lruList.splice(lruList.begin(), lruList, it->second);
        }
        else {
            if(lruList.size() == capacity) {
                auto last = lruList.back();
                cacheMap.erase(std::get<0>(last));
                lruList.pop_back();
            }

            lruList.push_front(make_tuple(key, value, expiry));
            cacheMap[key] = lruList.begin();
        }
    }

    int size() {
        return lruList.size();
    }
};

// -------- Global --------
LRUCache cache(100);   // increased capacity
mutex cache_mutex;

// -------- Client Handler --------
void handle_client(int client_socket) {

    char buffer[3000] = {0};
    recv(client_socket, buffer, 3000, 0);

    string request(buffer);

    string key = "default";

    size_t start = request.find("GET /");
    if(start != string::npos)
    {
        start += 5;
        size_t end = request.find(" ", start);
        key = request.substr(start, end - start);
    }

    string body;

    {
        lock_guard<mutex> lock(cache_mutex);

        if(key == "metrics") {
            body =
                "cache_hits: " + to_string(cache.hits) + "\n" +
                "cache_miss: " + to_string(cache.misses) + "\n" +
                "current_size: " + to_string(cache.size());
        }
        else {
            string val = cache.get(key);

            if(val != "EXPIRED") {
                body = "Cache HIT: " + val;
            }
            else {
                body = "Generated data for " + key;
                cache.put(key, body, 10);
            }
        }
    }

    string response =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/plain\r\n\r\n" + body;

    send(client_socket, response.c_str(), response.size(), 0);

    closesocket(client_socket);
}

// -------- Thread Pool --------
class ThreadPool {
private:
    vector<thread> workers;
    queue<int> tasks;

    mutex queue_mutex;
    condition_variable cv;
    bool stop;

public:
    ThreadPool(int num_threads) {
        stop = false;

        for(int i = 0; i < num_threads; i++) {
            workers.emplace_back([this]() {
                while(true) {
                    int client_socket;

                    {
                        unique_lock<mutex> lock(queue_mutex);

                        cv.wait(lock, [this]() {
                            return !tasks.empty() || stop;
                        });

                        if(stop && tasks.empty())
                            return;

                        client_socket = tasks.front();
                        tasks.pop();
                    }

                    handle_client(client_socket);
                }
            });
        }
    }

    void enqueue(int client_socket) {
        {
            unique_lock<mutex> lock(queue_mutex);
            tasks.push(client_socket);
        }
        cv.notify_one();
    }

    ~ThreadPool() {
        {
            unique_lock<mutex> lock(queue_mutex);
            stop = true;
        }

        cv.notify_all();

        for(thread &worker : workers)
            worker.join();
    }
};

// -------- Main --------
int main(){

    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    bind(server_fd, (sockaddr*)&address, sizeof(address));
    listen(server_fd, 10);

    cout << "Server running on port 8080\n";

    ThreadPool pool(4);  // 4 worker threads

    while(true){

        int addrlen = sizeof(address);
        int client_socket = accept(server_fd, (sockaddr*)&address, &addrlen);

        pool.enqueue(client_socket);
    }

    WSACleanup();
}