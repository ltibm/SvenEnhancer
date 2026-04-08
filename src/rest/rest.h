#pragma once    
#include <svenenhancer.h>
#include <queue>
#include <mutex>
#include "stringcollection.h"
#include "base64.h"

class RestRequest;
class RestResponse;
class RestClient;
class RestContext;
class RestUtils;
class InternalRequest;
extern std::vector<RestClient*> g_RestClients;
extern std::mutex g_RestMutex;
static std::recursive_mutex g_ClientMutex;
static CURLM* m_Multi;
static std::queue<InternalRequest*> m_Completed;
class RestUtils
{
public:
    static curl_slist* BuildHeaders(const StringCollection& headers);
    static std::string ToLower(std::string s);
    static std::string UrlEncode(const std::string& value) {
        CURL* curl = CurlFn._curl_easy_init();
        if (!curl) return "";
        char* output = CurlFn._curl_easy_escape(curl, value.c_str(), (int)value.length());
        std::string result(output);
        CurlFn._curl_free(output);
        CurlFn._curl_easy_cleanup(curl);
        return result;
    }
    static std::string UrlEncode(CURL* curl, const std::string& value) {
        if (!curl) return "";
        char* output = CurlFn._curl_easy_escape(curl, value.c_str(), (int)value.length());
        std::string result(output);
        CurlFn._curl_free(output);
        return result;
    }
    static void Trim(std::string& s)
    {
        size_t start = s.find_first_not_of(" \t");
        if (start == std::string::npos)
        {
            s.clear();
            return;
        }
        size_t end = s.find_last_not_of(" \t\r\n");
        s = s.substr(start, end - start + 1);
    }
    static std::string Base64Encode(const std::string& input) {
        return base64_encode(input);
	}
    static std::string Base64Decode(const std::string& input) {
        return base64_decode(input);
    }
    static std::string HMAC_SHA256(const std::string& key, const std::string& data);
    static std::string HMAC_SHA1(const std::string& key, const std::string& data);

    static std::string ToMD5(const std::string& data);
    static std::string ToHex(const std::array<char, 16>& hash);
};
class RestClient : public CASBaseGCObject
{
public:
	static void RegisterAngelScript(CASDocumentation* pASDoc, asIScriptEngine* engine);
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
    static size_t HeaderCallback(char* buffer, size_t size, size_t nitems, void* userdata);
    static void RestFrame(void);
    RestClient()
    {
        BaseUrl = *CreateString("");
        static bool initialized = false;
        if (!initialized)
        {
            CurlFn._curl_global_init(CURL_GLOBAL_DEFAULT);
            initialized = true;
        }
        //Singleton
		if (!m_Multi)
            m_Multi = CurlFn._curl_multi_init();
    }
    ~RestClient()
    {

    }
    RestResponse* Execute(RestRequest* request);
    bool ExecuteAsync(RestRequest* request, asIScriptFunction* callback);
    static void ProcessAsync();
    static void Dispatch();
    static void ProcessAll();
    static void DispatchAll();
    static RestClient* Factory();
    size_t ConnectionTimeoutMs = 2000;
    size_t TimeoutMs = 5000;
private:
    CURL* CurlPrepare(RestRequest* request, bool isAsync);
    CString BaseUrl;
};
struct InternalRequest
{
    CURL* Curl = nullptr;
    RestRequest* Request;
    RestResponse* Response;
    curl_slist* HeaderList = nullptr;
    asIScriptFunction* Callback = nullptr;
    std::string ResponseBuffer;
    StringCollection Headers;
    void* UserData = nullptr;
};

class RestResponse : public CASBaseGCObject
{
public:
    CString Body;
	int StatusCode = 0;
	StringCollection Headers;
    CString* GetHeader(CString& key) const {
        std::string _k(key.c_str());
		auto _v = Headers.Get(_k);
        return CreateString(_v.c_str());
    };
    size_t HeaderCount() const {
        return Headers.Size();
	}
    CString* GetHeaderKey(size_t index) const {
        auto& data = Headers.GetAllData();
        if (index >= data.size()) return CreateString("");
        auto it = data.begin();
        std::advance(it, index);
        return CreateString(it->first.c_str());
	}
    CString* GetHeaderValue(size_t index) const {
		auto v = GetHeaderKey(index);
		return this->GetHeader(*v);
    }
    bool IsSuccessStatusCode() const {
        return StatusCode >= 200 && StatusCode < 300;
	}
    JValue* GetJsonBody() const;
    RestResponse();
    ~RestResponse();
};
class RestRequest : public CASBaseGCObject {
public:
    RestRequest();
    ~RestRequest();
	CString Method;
    CString Url;
	std::string Body;
	StringCollection Headers;
    void* data = nullptr; // for user data
    static RestRequest* Factory();
    CString* GetQueryStringAS();
    void ClearQueryParams() {
        m_QueryParams.clear();
	}
    void ClearHeaders() {
        Headers.Clear();
	}
    std::string GetQueryString() const;
    bool IsAbsoluteUrl();
    std::string BuildUrl(const std::string& baseUrl, CURL* curl);
    std::string GetMethod() {
		return RestUtils::ToLower(std::string(Method.c_str(), Method.length()));
	}
	void SetBody(CString& body, CString& contentType) {
        Body = std::string(body.c_str(), body.length());
		this->Headers.Remove("content-type");
		this->Headers.Add("content-type", std::string(contentType.c_str()));
    }
    void AddJsonBody(void* obj, int typeId);
    CString* GetBody() const {
        return CreateString(Body.c_str());
    };
	RestRequest* AddHeader(CString& key, CString& value) {
        this->AddRef();
        this->Headers.Add(key.c_str(), value.c_str());
        return this;
    }
    RestRequest* AddQueryParam(CString& key, CString& value)
    {
		this->AddRef();
        std::string _key(key.c_str());
        std::string _value(value.c_str());
        m_QueryParams.emplace_back(_key, _value);
		return this;
    }
    std::vector<std::pair<std::string, std::string>> m_QueryParams;
};

class RestContext : public CASBaseGCObject
{
public:
    static RestContext* Factory();
    RestRequest* Request = nullptr;
    RestResponse* Response = nullptr;
    void* UserData = nullptr; // for user data
    void* GetUserData() const {
        return UserData;
	}
    RestContext();
    ~RestContext() {

    }
    
};



