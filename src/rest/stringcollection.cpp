#include "stringcollection.h"
void StringCollection::Add(const std::string& key, const std::string& value)
{
    m_Data[key].push_back(value);
}
void StringCollection::Set(const std::string& key, const std::string& value)
{
    m_Data[key] = { value };
}
void StringCollection::Remove(const std::string& key)
{
    m_Data.erase(key);
}
const std::string StringCollection::Get(const std::string& key) const
{
    auto it = m_Data.find(key);
    if (it != m_Data.end() && !it->second.empty())
        return it->second[0];

    return "";
}

const std::vector<std::string>& StringCollection::GetAll(const std::string& key) const
{
    static const std::vector<std::string> empty;

    auto it = m_Data.find(key);
    if (it != m_Data.end())
        return it->second;

    return empty;
}
bool StringCollection::Contains(const std::string& key) const
{
    return m_Data.find(key) != m_Data.end();
}
void StringCollection::Clear()
{
    m_Data.clear();
}
void StringCollection::AddFromHeader(const std::string& line)
{
    if (line.empty())
        return;
    std::string clean = line;
    if (!clean.empty() && clean.back() == '\n') clean.pop_back();
    if (!clean.empty() && clean.back() == '\r') clean.pop_back();
    if (clean.empty())
        return;
    if (clean.rfind("HTTP/", 0) == 0 || clean.rfind("http/", 0) == 0)
        return;

    size_t pos = clean.find(':');
    if (pos == std::string::npos)
        return;
    std::string key = clean.substr(0, pos);
    std::string value = clean.substr(pos + 1);
    if (!value.empty() && value[0] == ' ')
        value.erase(0, 1);
    Add(key, value);
}
const std::unordered_map<
    std::string,
    std::vector<std::string>,
    struct CaseInsensitiveHash,
    struct CaseInsensitiveEqual
>& StringCollection::GetAllData() const
{
    return m_Data;
}