# System-Design-Concepts

## Consistent Hashing
https://www.youtube.com/watch?v=jqUNbqfsnuw

Here's a `README.md` file for the **consistent hashing** implementation described above:

---

# Consistent Hashing in C++

This project demonstrates the implementation of **consistent hashing** in C++ using `std::map` to represent the hash ring. Consistent hashing is a technique used to distribute keys across a set of servers (or nodes) in a way that minimizes the need for rehashing when servers are added or removed, making it useful for distributed systems like caches or load balancers.

## Features
- Implements a **consistent hashing** mechanism with support for virtual nodes.
- Distributes keys evenly across multiple servers.
- Handles the circular nature of the hash ring.
- Allows adding or removing servers dynamically.

## Code Overview

### Consistent Hashing Class

The `ConsistentHashing` class is responsible for:
1. **Adding servers** to the hash ring using virtual nodes.
2. **Removing servers** from the hash ring.
3. **Finding the responsible server** for a given key.

#### Class Definition:

```cpp
class ConsistentHashing {
public:
    ConsistentHashing(int virtualNodes);
    void addServer(const std::string& server);
    void removeServer(const std::string& server);
    std::string getServer(const std::string& key) const;

private:
    int virtualNodes_;
    std::map<size_t, std::string> hashRing_;
    std::hash<std::string> hashFunction_;
};
```

### Key Functions

1. **addServer(const std::string& server)**:
   - Adds a server (node) to the hash ring.
   - Uses a configurable number of **virtual nodes** to spread the load evenly across servers.
   - For each server, multiple virtual nodes are created by appending `#<index>` to the server name.
   - Each virtual node is hashed and inserted into the `hashRing_`.

2. **removeServer(const std::string& server)**:
   - Removes all virtual nodes associated with a server from the hash ring.
   - It ensures that all key mappings to that server are removed when the server is decommissioned.

3. **getServer(const std::string& key)**:
   - Given a key, finds the server responsible for that key.
   - It hashes the key and uses `std::map::lower_bound` to find the first server in the hash ring with a hash greater than or equal to the key's hash.
   - Implements **circular hashing**: If the key's hash is larger than the largest server hash, the search wraps around and returns the first server in the ring.

### Circular Hashing with `std::map::lower_bound`

To simulate the circular nature of the hash ring:
- We use `std::map` to store the hash values of virtual nodes and their corresponding servers.
- The `lower_bound` function is used to find the first server whose hash is **greater than or equal to** the key's hash. If no such server exists (i.e., the key's hash is larger than any server's hash), the search wraps around to the first server in the map.

```cpp
auto it = hashRing_.lower_bound(hashValue);
if (it == hashRing_.end()) {
    it = hashRing_.begin();  // Wrap around to the beginning of the hash ring
}
```

## Code Example

```cpp
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <functional>  // for std::hash
#include <algorithm>   // for std::lower_bound

class ConsistentHashing {
public:
    ConsistentHashing(int virtualNodes) : virtualNodes_(virtualNodes) {}

    void addServer(const std::string& server) {
        for (int i = 0; i < virtualNodes_; ++i) {
            std::string virtualNode = server + "#" + std::to_string(i);
            size_t hashValue = hashFunction_(virtualNode);
            hashRing_[hashValue] = server;
        }
    }

    void removeServer(const std::string& server) {
        for (int i = 0; i < virtualNodes_; ++i) {
            std::string virtualNode = server + "#" + std::to_string(i);
            size_t hashValue = hashFunction_(virtualNode);
            hashRing_.erase(hashValue);
        }
    }

    std::string getServer(const std::string& key) const {
        if (hashRing_.empty()) {
            return "";
        }

        size_t hashValue = hashFunction_(key);
        auto it = hashRing_.lower_bound(hashValue);

        if (it == hashRing_.end()) {
            it = hashRing_.begin();  // Wrap around to the beginning
        }

        return it->second;
    }

private:
    int virtualNodes_;
    std::map<size_t, std::string> hashRing_;
    std::hash<std::string> hashFunction_;
};

int main() {
    ConsistentHashing ch(3);  // 3 virtual nodes per server

    // Add servers to the hash ring
    ch.addServer("Server1");
    ch.addServer("Server2");
    ch.addServer("Server3");

    // Distribute keys
    std::vector<std::string> keys = {"Key1", "Key2", "Key3", "Key4", "Key5"};

    for (const auto& key : keys) {
        std::cout << "Key: " << key << " is mapped to " << ch.getServer(key) << std::endl;
    }

    std::cout << "\nRemoving Server2\n";
    ch.removeServer("Server2");

    for (const auto& key : keys) {
        std::cout << "Key: " << key << " is now mapped to " << ch.getServer(key) << std::endl;
    }

    return 0;
}
```

### Example Output:
```
Key: Key1 is mapped to Server2
Key: Key2 is mapped to Server1
Key: Key3 is mapped to Server3
Key: Key4 is mapped to Server2
Key: Key5 is mapped to Server3

Removing Server2
Key: Key1 is now mapped to Server1
Key: Key2 is now mapped to Server1
Key: Key3 is now mapped to Server3
Key: Key4 is now mapped to Server1
Key: Key5 is now mapped to Server3
```

## How It Works
1. **Adding Servers**: Virtual nodes are created for each server and hashed using `std::hash`. These hashes are stored in `hashRing_` to simulate a hash ring.
   
2. **Key Distribution**: When a key is given, the key is hashed and the `lower_bound` function finds the first server whose virtual node hash is greater than or equal to the key's hash. If no such node exists, the function wraps around to the first server in the hash ring.

3. **Removing Servers**: All virtual nodes associated with the removed server are deleted, and the keys are automatically remapped to the remaining servers.

## Compilation and Execution
To compile and run this code:
```bash
g++ consistent_hashing.cpp -o consistent_hashing
./consistent_hashing
```

## Use Cases
- **Load Balancing**: Distribute requests evenly across multiple servers, with minimal disruption when servers are added or removed.
- **Distributed Caches**: Cache data across a cluster of servers, ensuring that when servers are added or removed, the cache is mostly retained on the remaining servers.
  
---

This README covers the structure, functionality, and usage of the consistent hashing code, explaining both its theoretical purpose and practical implementation.
