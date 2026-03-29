#include <atomic>
#include <mutex>
class MySqlConnectionPool
{
public:
    struct ConnectionEntry
    {
        MYSQL* conn = nullptr;
        std::atomic<int> refCount = {0};
        std::chrono::steady_clock::time_point lastUsed;
        std::mutex entryMutex;
    };

public:
    MYSQL* Open(uint64_t hash);
    void Close(uint64_t hash);
    void Cleanup();

private:
    std::unordered_map<uint64_t, ConnectionEntry> pool;
    std::mutex poolMutex;
};
extern MySqlConnectionPool pools;
