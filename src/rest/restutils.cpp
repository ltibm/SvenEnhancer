#include "rest.h"
#include "crypto.h"
#include <sha256.h>
#include <hmac.h>
#include <sha1.h>

curl_slist* RestUtils::BuildHeaders(const StringCollection& headers)
{
    curl_slist* list = nullptr;
    for (const auto& pair : headers.GetAllData())
    {
        const std::string& key = pair.first;
        const std::vector<std::string>& values = pair.second;
        for (const auto& value : values)
        {
            std::string header = key + ": " + value;
            list = CurlFn._curl_slist_append(list, header.c_str());
        }
    }
    return list;
}
std::string RestUtils::ToLower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return s;
}
std::string RestUtils::HMAC_SHA256(const std::string& key, const std::string& data)
{
    std::string result = hmac<SHA256>(data, key);
    return result;

}
std::string RestUtils::HMAC_SHA1(const std::string& key, const std::string& data)
{
    std::string result = hmac<SHA1>(data, key);
    return result;

}
std::string RestUtils::ToHex(const std::array<char, 16>& hash)
{
    static const char* hex = "0123456789abcdef";
    std::string out;
    out.reserve(32);
    for (unsigned char b : hash)
    {
        out.push_back(hex[(b >> 4) & 0xF]);
        out.push_back(hex[b & 0xF]);
    }
    return out;
}
std::string RestUtils::ToMD5(const std::string& data)
{
	MD5 md5;
	return md5(data);
}
