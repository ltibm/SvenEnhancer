#include "rest.h"
std::vector<RestClient*> g_RestClients;
std::mutex g_RestMutex;
//Register angelscript bindings for RestClient
void RestClient::RegisterAngelScript(CASDocumentation* pASDoc, asIScriptEngine* engine)
{
    int r = 0;
    r = RegisterObject<RestResponse>("RestResponse", engine, asOBJ_REF | asOBJ_GC); assert(r >= 0);
	r = engine->RegisterObjectProperty("RestResponse", "string Body", asOFFSET(RestResponse, Body)); assert(r >= 0);
	r = engine->RegisterObjectProperty("RestResponse", "int StatusCode", asOFFSET(RestResponse, StatusCode)); assert(r >= 0);
	r = engine->RegisterObjectMethod("RestResponse", "string& GetHeader(string&in key)", asMETHOD(RestResponse, GetHeader), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("RestResponse", "string& GetHeader(uint index)", asMETHOD(RestResponse, GetHeaderValue), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("RestResponse", "string& GetHeaderKey(uint index)", asMETHOD(RestResponse, GetHeaderKey), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("RestResponse", "uint HeaderCount()", asMETHOD(RestResponse, HeaderCount), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("RestResponse", "bool IsSuccessStatusCode()", asMETHOD(RestResponse, IsSuccessStatusCode), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("RestResponse", "JValue@ GetJsonBody()", asMETHOD(RestResponse, GetJsonBody), asCALL_THISCALL); assert(r >= 0);
    r = RegisterObject<RestRequest>("RestRequest", engine, asOBJ_REF | asOBJ_GC); assert(r >= 0);
    r = engine->RegisterObjectProperty("RestRequest", "string Url", asOFFSET(RestRequest, Url)); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("RestRequest", asBEHAVE_FACTORY, "RestRequest@ RestRequest()", asFUNCTION(RestRequest::Factory), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterObjectMethod("RestRequest", "string& get_Body()", asMETHOD(RestRequest, GetBody), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("RestRequest", "void AddBody(string&in body, string&in contentType =\"text/plain\")", asMETHOD(RestRequest, SetBody), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("RestRequest", "void AddJsonBody(?&in obj)", asMETHOD(RestRequest, AddJsonBody), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectProperty("RestRequest", "string Method", asOFFSET(RestRequest, Method)); assert(r >= 0);
    r = engine->RegisterObjectMethod("RestRequest", "RestRequest@ AddHeader(string&in key, string&in value)", asMETHOD(RestRequest, AddHeader), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("RestRequest", "RestRequest@ AddQueryParam(string&in key, string&in value)", asMETHOD(RestRequest, AddQueryParam), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("RestRequest", "string& GetQueryString()", asMETHOD(RestRequest, GetQueryStringAS), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("RestRequest", "void ClearQueryParams()", asMETHOD(RestRequest, ClearQueryParams), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("RestRequest", "void ClearHeaders()", asMETHOD(RestRequest, ClearHeaders), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectProperty("RestRequest", "any@ UserData", asOFFSET(RestRequest, data)); assert(r >= 0);


	r = RegisterObject<RestContext>("RestContext", engine, asOBJ_REF | asOBJ_GC); assert(r >= 0);
    r = engine->RegisterObjectProperty("RestContext", "RestRequest@ Request", asOFFSET(RestContext, Request)); assert(r >= 0);
    r = engine->RegisterObjectProperty("RestContext", "RestResponse@ Response", asOFFSET(RestContext, Response)); assert(r >= 0);
    r = engine->RegisterObjectMethod("RestContext", "any@ get_UserData() const", asMETHOD(RestContext, GetUserData), asCALL_THISCALL); assert(r >= 0);

    ASEXT_RegisterFuncDef(pASDoc, "callback for ExecuteAsync", "void RestCallback(RestContext@ context)");

    r = RegisterObject<RestClient>("RestClient", engine, asOBJ_REF | asOBJ_GC); assert(r >= 0);
    r = engine->RegisterObjectBehaviour("RestClient", asBEHAVE_FACTORY, "RestClient@ RestClient()", asFUNCTION(RestClient::Factory), asCALL_CDECL); assert(r >= 0);
	r = engine->RegisterObjectMethod("RestClient", "RestResponse@ Execute(RestRequest@ request)", asMETHOD(RestClient, Execute), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("RestClient", "bool ExecuteAsync(RestRequest@ request, RestCallback@ callback = null)", asMETHOD(RestClient, ExecuteAsync), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectProperty("RestClient", "uint ConnectionTimeoutMs", asOFFSET(RestClient, ConnectionTimeoutMs)); assert(r >= 0);
    r = engine->RegisterObjectProperty("RestClient", "uint TimeoutMs", asOFFSET(RestClient, TimeoutMs)); assert(r >= 0);
}

size_t RestClient::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t total = size * nmemb;
    InternalRequest* job = (InternalRequest*)userp;
    if (job && job->Request) {
        job->ResponseBuffer.append((char*)contents, total);
    }
    return total;
}

size_t RestClient::HeaderCallback(char* buffer, size_t size, size_t nitems, void* userdata)
{
    size_t totalSize = size * nitems;
    InternalRequest* req = static_cast<InternalRequest*>(userdata);
    if (!req)
        return totalSize;
    std::string line(buffer, totalSize);
    if (!line.empty() && line.back() == '\n')
        line.pop_back();
    if (!line.empty() && line.back() == '\r')
        line.pop_back();
    if (line.empty())
        return totalSize;
    if (line.rfind("HTTP/", 0) == 0 || line.rfind("http/", 0) == 0)
        return totalSize;
    size_t sep = line.find(':');
    if (sep != std::string::npos)
    {
        std::string key = line.substr(0, sep);
        std::string value = line.substr(sep + 1);
        RestUtils::Trim(key);
        RestUtils::Trim(value);
        req->Headers.Add(key, value);
    }
    return totalSize;
}
void RestClient::RestFrame(void)
{
	RestClient::ProcessAll();
	RestClient::DispatchAll();
}
RestResponse* RestClient::Execute(RestRequest* request)
{
    CURL* curl = CurlPrepare(request, true);
    if (!curl)
        return nullptr;
    //this->AddRef();
    RestResponse* response = new RestResponse();
    std::string buffer;
    InternalRequest* req = new InternalRequest();
    req->Request = request;
    curl_slist* headerList = RestUtils::BuildHeaders(request->Headers);
    CurlFn._curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);
    CurlFn._curl_easy_setopt(curl, CURLOPT_WRITEDATA, req);
    CurlFn._curl_easy_setopt(curl, CURLOPT_HEADERDATA, req);

    CURLcode res = CurlFn._curl_easy_perform(curl);

    long response_code;
    CurlFn._curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
	response->StatusCode = (int)response_code;
    response->Body = *CreateString(req->ResponseBuffer.c_str());
	response->Headers = req->Headers;
    CurlFn._curl_easy_cleanup(curl);
    if(headerList)
        CurlFn._curl_slist_free_all(headerList);
    return response;
}

bool RestClient::ExecuteAsync(RestRequest* request, asIScriptFunction* callback) {
    if (!m_Multi) return false;

    InternalRequest* job = new InternalRequest();
    job->Request = request;
    job->Callback = callback;
    if (request->data)
    {
		job->UserData = request->data;
    }
    if (job->Callback) job->Callback->AddRef();

	CURL* curl = CurlPrepare(request, true);
    if (!curl)
        return false;

    //this->AddRef();
    job->Curl = curl;
    curl_slist* headerList = RestUtils::BuildHeaders(request->Headers);
	job->HeaderList = headerList;
    CurlFn._curl_easy_setopt(curl, CURLOPT_HEADERDATA, job);
    CurlFn._curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);
    CurlFn._curl_easy_setopt(curl, CURLOPT_WRITEDATA, job);
    CurlFn._curl_easy_setopt(curl, CURLOPT_PRIVATE, job);
    {
        std::lock_guard<std::recursive_mutex> lock(g_ClientMutex);
        CurlFn._curl_multi_add_handle(m_Multi, curl);
    }
 
    return true;
}
//For common options
CURL* RestClient::CurlPrepare(RestRequest* request, bool isAsync)
{
    if (!CurlFn._curl_easy_init)
        return nullptr;

    CURL* curl = CurlFn._curl_easy_init();
    if (!curl)
        return nullptr;
	std::string _bUrl(BaseUrl.c_str());
	std::string furl = request->BuildUrl(_bUrl, curl);
    CurlFn._curl_easy_setopt(curl, CURLOPT_URL, furl.c_str());
    CurlFn._curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
    CurlFn._curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    CurlFn._curl_easy_setopt(curl, CURLOPT_WRITEDATA, request);
    CurlFn._curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, static_cast<long>(TimeoutMs));
    CurlFn._curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, static_cast<long>(ConnectionTimeoutMs));
    if (request->Headers.Contains("content-length"))
    {
		request->Headers.Remove("content-length");
    }
    bool allowBody = true;
	if (request->GetMethod() == "post")
    {
        CurlFn._curl_easy_setopt(curl, CURLOPT_POST, 1L);
    }
    else if (request->GetMethod() == "put")
    {
        CurlFn._curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
    }
    else if (request->GetMethod() == "delete")
    {
        CurlFn._curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    }
    else if (request->GetMethod() == "patch")
    {
        CurlFn._curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
    }
    else 
    {
        allowBody = false;
        CurlFn._curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    }
    if (allowBody)
    {
        if (request->Body.length() > 0)
        {
			CurlFn._curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request->Body.c_str());
        }
        else {

            CurlFn._curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");
            CurlFn._curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, 0L);
        }

    }
    return curl;
}
void RestClient::ProcessAsync() {
	if (!m_Multi) 
        return;
    int running = 0;
    CurlFn._curl_multi_perform(m_Multi, &running);

    if (running > 0)
    {
        int numfds = 0;
        CurlFn._curl_multi_wait(m_Multi, NULL, 0, 1, &numfds);
    }
    CURLMsg* msg;
    int msgs_left;
    while ((msg = CurlFn._curl_multi_info_read(m_Multi, &msgs_left))) {
        if (msg->msg == CURLMSG_DONE) {
            CURL* curl = msg->easy_handle;
            InternalRequest* job = nullptr;
            CurlFn._curl_easy_getinfo(curl, CURLINFO_PRIVATE, &job);
            if (job) {
                long response_code;
                CurlFn._curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
                job->Response = new RestResponse();
                job->Response->StatusCode = (int)response_code;
                job->Response->Body = *CreateString(job->ResponseBuffer.c_str());
				job->Response->Headers = job->Headers;
                std::lock_guard<std::recursive_mutex> lock(g_ClientMutex);
                m_Completed.push(job);
                if (job->HeaderList)
                    CurlFn._curl_slist_free_all(job->HeaderList);
            }
            CurlFn._curl_multi_remove_handle(m_Multi, curl);
            CurlFn._curl_easy_cleanup(curl);
        }
    }
}

void RestClient::Dispatch() {

    while (!m_Completed.empty()) {
        InternalRequest* job = nullptr;
        {
            std::lock_guard<std::recursive_mutex> lock(g_ClientMutex);
            if (m_Completed.empty()) break;
            job = m_Completed.front();
            m_Completed.pop();
        }
        if (job && job->Callback) {

            auto engine = GetASEngine();
            if (engine) {

                asIScriptContext* ctx = engine->CreateContext();
                ctx->Prepare(job->Callback);

                auto context = RestContext::Factory();
                context->Request = job->Request;
                context->Response = job->Response;
				context->UserData = job->UserData;
                ctx->SetArgObject(0, context);
                ctx->Execute();
                ctx->Release();
                job->Callback->Release();

            }
        }
        if (job) {
            delete job;
        }
    }
}
void RestClient::ProcessAll()
{
    std::lock_guard<std::mutex> lock(g_RestMutex);
    RestClient::ProcessAsync();
}
void RestClient::DispatchAll()
{
    std::lock_guard<std::mutex> lock(g_RestMutex);
    RestClient::Dispatch();
}
RestClient* RestClient::Factory()
{
    RestClient* obj = new RestClient();
    CASServerManager* manager = ASEXT_GetServerManager();
    asIScriptEngine* engine = manager->scriptEngine;
    asITypeInfo* type = engine->GetTypeInfoByName("RestClient");
    engine->NotifyGarbageCollectorOfNewObject(obj, type);
    return obj;
}