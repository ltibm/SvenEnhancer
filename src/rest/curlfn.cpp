#include <svenenhancer.h>
CurlTable CurlFn;
#ifdef WIN32
HMODULE  curlHandle;
#else
void* curlHandle;
#endif
void InitCurl()
{
	#ifdef WIN32
	curlHandle = GetModuleHandleA("libcurl.dll");

	#else
	curlHandle = dlopen("libcurl.so.4", RTLD_NOLOAD);
	if (!curlHandle)
		curlHandle = dlopen("libcurl.so.4", RTLD_NOW | RTLD_GLOBAL);

	/*CurlFn._curl_easy_init = curl_easy_init;
	CurlFn._curl_easy_setopt = curl_easy_setopt;
	CurlFn._curl_easy_perform = curl_easy_perform;
	CurlFn._curl_easy_cleanup = curl_easy_cleanup;
	CurlFn._curl_multi_add_handle = curl_multi_add_handle;
	CurlFn._curl_multi_perform = curl_multi_perform;
	CurlFn._curl_multi_remove_handle = curl_multi_remove_handle;
	CurlFn._curl_multi_info_read = curl_multi_info_read;
	CurlFn._curl_global_init = curl_global_init;
	CurlFn._curl_multi_init = curl_multi_init;
	CurlFn._curl_multi_cleanup = curl_multi_cleanup;
	CurlFn._curl_easy_getinfo = curl_easy_getinfo;*/
	
	#endif 
	if (curlHandle)
	{
		CurlFn._curl_easy_init = GetSymbol<decltype(&curl_easy_init)>(curlHandle, "curl_easy_init");
		CurlFn._curl_easy_setopt = GetSymbol<decltype(&curl_easy_setopt)>(curlHandle, "curl_easy_setopt");
		CurlFn._curl_easy_perform = GetSymbol<decltype(&curl_easy_perform)>(curlHandle, "curl_easy_perform");
		CurlFn._curl_easy_cleanup = GetSymbol<decltype(&curl_easy_cleanup)>(curlHandle, "curl_easy_cleanup");
		CurlFn._curl_multi_add_handle = GetSymbol<decltype(&curl_multi_add_handle)>(curlHandle, "curl_multi_add_handle");
		CurlFn._curl_multi_perform = GetSymbol<decltype(&curl_multi_perform)>(curlHandle, "curl_multi_perform");
		CurlFn._curl_multi_remove_handle = GetSymbol<decltype(&curl_multi_remove_handle)>(curlHandle, "curl_multi_remove_handle");
		CurlFn._curl_multi_info_read = GetSymbol<decltype(&curl_multi_info_read)>(curlHandle, "curl_multi_info_read");
		CurlFn._curl_global_init = GetSymbol<decltype(&curl_global_init)>(curlHandle, "curl_global_init");
		CurlFn._curl_multi_init = GetSymbol<decltype(&curl_multi_init)>(curlHandle, "curl_multi_init");
		CurlFn._curl_multi_cleanup = GetSymbol<decltype(&curl_multi_cleanup)>(curlHandle, "curl_multi_cleanup");
		CurlFn._curl_easy_getinfo = GetSymbol<decltype(&curl_easy_getinfo)>(curlHandle, "curl_easy_getinfo");
		CurlFn._curl_slist_free_all = GetSymbol<decltype(&curl_slist_free_all)>(curlHandle, "curl_slist_free_all");
		CurlFn._curl_easy_escape = GetSymbol<decltype(&curl_easy_escape)>(curlHandle, "curl_easy_escape");
		CurlFn._curl_free = GetSymbol<decltype(&curl_free)>(curlHandle, "curl_free");
		CurlFn._curl_slist_append = GetSymbol<decltype(&curl_slist_append)>(curlHandle, "curl_slist_append");
		CurlFn._curl_multi_wait = GetSymbol<decltype(&curl_multi_wait)>(curlHandle, "curl_multi_wait");
	}
	else {

	}
}
