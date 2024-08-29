#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <functional>  // for std::hash
#include <algorithm>   // for std::lower_bound

class ConsistentHashing {
public:
    // Constructor to initialize the number of virtual nodes
    ConsistentHashing(int virtualNodes) : virtualNodes_(virtualNodes) {}

    // Add a server (node) to the hash ring
    void addServer(const std::string& server) {
        for (int i = 0; i < virtualNodes_; ++i) {
            std::string virtualNode = server + "#" + std::to_string(i);
            size_t hashValue = hashFunction_(virtualNode);
            hashRing_[hashValue] = server;
        }
    }

    // Remove a server from the hash ring
    void removeServer(const std::string& server) {
        for (int i = 0; i < virtualNodes_; ++i) {
            std::string virtualNode = server + "#" + std::to_string(i);
            size_t hashValue = hashFunction_(virtualNode);
            hashRing_.erase(hashValue);
        }
    }

    // Get the server responsible for a given key
    std::string getServer(const std::string& key) const {
        if (hashRing_.empty()) {
            return "";
        }

        size_t hashValue = hashFunction_(key);
        auto it = hashRing_.lower_bound(hashValue);

        // If we've gone past the last node, wrap around to the first node (circular behavior)
        if (it == hashRing_.end()) {
            it = hashRing_.begin();
        }

        return it->second;  // Return the server for this key
    }

private:
    int virtualNodes_;  // Number of virtual nodes per server
    std::map<size_t, std::string> hashRing_;  // Hash ring with virtual nodes
    std::hash<std::string> hashFunction_;  // Default string hash function
};

int main() {
    ConsistentHashing ch(3);  // Using 3 virtual nodes per server

    // Add servers to the hash ring
    ch.addServer("Server1");
    ch.addServer("Server2");
    ch.addServer("Server3");

    // Distribute some keys
    std::vector<std::string> keys = {"Key1", "Key2", "Key3", "Key4", "Key5"};

    for (const auto& key : keys) {
        std::cout << "Key: " << key << " is mapped to " << ch.getServer(key) << std::endl;
    }

    // Removing a server
    std::cout << "\nRemoving Server2\n";
    ch.removeServer("Server2");

    for (const auto& key : keys) {
        std::cout << "Key: " << key << " is now mapped to " << ch.getServer(key) << std::endl;
    }

    return 0;
}
