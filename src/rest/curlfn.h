#pragma once
#ifndef WIN32
#include <curl/curl.h>
#else
#include <curl.h>
#endif
struct CurlTable
{
public:
    decltype(&curl_easy_init) _curl_easy_init;
    decltype(&curl_easy_setopt) _curl_easy_setopt;
    decltype(&curl_easy_perform) _curl_easy_perform;
    decltype(&curl_easy_cleanup) _curl_easy_cleanup;
    decltype(&curl_multi_add_handle) _curl_multi_add_handle;
    decltype(&curl_multi_perform) _curl_multi_perform;
    decltype(&curl_multi_remove_handle) _curl_multi_remove_handle;
    decltype(&curl_multi_info_read) _curl_multi_info_read;
    decltype(&curl_global_init) _curl_global_init;
    decltype(&curl_multi_init) _curl_multi_init;
    decltype(&curl_multi_cleanup) _curl_multi_cleanup;
	decltype(&curl_easy_getinfo) _curl_easy_getinfo;
	decltype(&curl_slist_free_all) _curl_slist_free_all;
    decltype(&curl_easy_escape) _curl_easy_escape;
    decltype(&curl_free) _curl_free;
    decltype(&curl_slist_append) _curl_slist_append;
    decltype(&curl_multi_wait) _curl_multi_wait;
};
extern CurlTable CurlFn;
#ifdef WIN32
extern HMODULE  curlHandle;
#else
extern void* curlHandle;
#endif
void InitCurl();