#ifndef ELECTRON_SHARED_MEMORY_H
#define ELECTRON_SHARED_MEMORY_H

#include <napi.h>
#include <windows.h>
#include <map>
typedef HANDLE HANDLE;

struct ShareMemoryInfo
{
    HANDLE handle;
	void* ptr;
};

static std::map<std::string, ShareMemoryInfo> _ShareMemorySetMap; //共享内存设置列表
static std::map<std::string, ShareMemoryInfo> _ShareMemoryGetMap; //共享内存获取列表

Napi::Value SetShareMemory(const Napi::CallbackInfo& info);
Napi::Value GetShareMemory(const Napi::CallbackInfo& info);
Napi::Value ClearShareMemory(const Napi::CallbackInfo& info);

#endif