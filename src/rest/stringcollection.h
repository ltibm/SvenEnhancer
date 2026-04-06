#pragma once
#include <unordered_map>
#include <vector>
#include <string>
struct CaseInsensitiveHash
{
    size_t operator()(const std::string& s) const
    {
        size_t h = 0;
        for (char c : s)
            h ^= std::tolower(c) + 0x9e3779b9 + (h << 6) + (h >> 2);
        return h;
    }
};
struct CaseInsensitiveEqual
{
    bool operator()(const std::string& a, const std::string& b) const
    {
        if (a.size() != b.size()) return false;

        for (size_t i = 0; i < a.size(); i++)
            if (tolower(a[i]) != tolower(b[i]))
                return false;

        return true;
    }
};
class StringCollection
{
public:
    void Add(const std::string& key, const std::string& value);
    void Set(const std::string& key, const std::string& value);
    void Remove(const std::string& key);
    const std::string Get(const std::string& key) const;
    const std::vector<std::string>& GetAll(const std::string& key) const;
    bool Contains(const std::string& key) const;
    void Clear();
    void AddFromHeader(const std::string& line);
    size_t Size() const {
		return m_Data.size();
    }
    const std::unordered_map<
        std::string,
        std::vector<std::string>,
        struct CaseInsensitiveHash,
        struct CaseInsensitiveEqual
    >& GetAllData() const;

private:
    std::unordered_map<
        std::string,
        std::vector<std::string>,
        struct CaseInsensitiveHash,
        struct CaseInsensitiveEqual
    > m_Data;
};