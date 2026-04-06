#include <rest.h>
RestRequest::RestRequest() {
	Url = *CreateString("");
    Method = *CreateString("GET");

}
RestRequest::~RestRequest(){
}
bool RestRequest::IsAbsoluteUrl() {
	std::string str(Url.c_str());
	return str.starts_with("http://") || str.starts_with("https://");
}
std::string RestRequest::BuildUrl(const std::string& baseUrl, CURL* curl)
{
    auto encode = [&](const std::string& s)
        {
            char* out = CurlFn._curl_easy_escape(curl, s.c_str(), (int)s.size());
            std::string res = out ? out : "";
            if (out) CurlFn._curl_free(out);
            return res;
        };

    auto join = [](const std::string& base, const std::string& path)
        {
            if (base.empty()) return path;

            bool bSlash = base.back() == '/';
            bool pSlash = !path.empty() && path.front() == '/';

            if (bSlash && pSlash)
                return base + path.substr(1);
            if (!bSlash && !pSlash)
                return base + "/" + path;

            return base + path;
        };

    std::string urlStr(Url.c_str());
    std::string path = urlStr;
    std::string query;

    size_t qpos = urlStr.find('?');
    if (qpos != std::string::npos)
    {
        path = urlStr.substr(0, qpos);
        query = urlStr.substr(qpos + 1);
    }
    std::string finalUrl;

    if (IsAbsoluteUrl())
        finalUrl = path;
    else
        finalUrl = join(baseUrl, path);
    std::string finalQuery = query;

    auto appendParam = [&](const std::string& k, const std::string& v)
        {
            if (!finalQuery.empty())
                finalQuery += "&";

            finalQuery += encode(k);
            finalQuery += "=";
            finalQuery += encode(v);
        };
    for (const auto& param : m_QueryParams)
    {
        appendParam(param.first, param.second);
    }
    if (!finalQuery.empty())
        finalUrl += "?" + finalQuery;
    return finalUrl;
}
void RestRequest::AddJsonBody(void* obj, int typeId)
{
    if (typeId & asTYPEID_OBJHANDLE)
        obj = *(void**)obj;

    auto type = GetASEngine()->GetTypeInfoById(typeId);
    if (!obj || !type)
        return;
    JValue* jv = new JValue();
    jv->FillFromObject(obj, type);
	this->Body = jv->ToString()->c_str();
    jv->Release();
	this->Headers.Remove("Content-Type");
    this->Headers.Add("Content-Type", "application/json");
}
RestRequest* RestRequest::Factory()
{
    RestRequest* obj = new RestRequest();
    CASServerManager* manager = ASEXT_GetServerManager();
    asIScriptEngine* engine = manager->scriptEngine;
    asITypeInfo* type = engine->GetTypeInfoByName("RestRequest");
    engine->NotifyGarbageCollectorOfNewObject(obj, type);
    return obj;
}
CString* RestRequest::GetQueryStringAS()
{
    std::string qs = this->GetQueryString();
    return CreateString(qs.c_str());
}
std::string RestRequest::GetQueryString() const
{
    std::vector<std::pair<std::string, std::string>> params = m_QueryParams;
    std::sort(params.begin(), params.end(),
        [](const auto& a, const auto& b)
        {
            if (a.first == b.first)
                return a.second < b.second;
            return a.first < b.first;
        });
    std::string result;

    for (const auto& [key, value] : params)
    {
        if (!result.empty())
            result += "&";
        result += key;
        result += "=";
        result += value;
    }
    return result;
}