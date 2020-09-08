#ifndef ELECTRON_SHARED_MEMERY_H
#define ELECTRON_SHARED_MEMERY_H

#include <napi.h>
#include <windows.h>
#include <map>
typedef HANDLE HANDLE;

struct ShareMemeryInfo
{
    HANDLE handle;
	void* ptr;
};

static std::map<std::string, ShareMemeryInfo> _ShareMemerySetMap; //共享内存设置列表
static std::map<std::string, ShareMemeryInfo> _ShareMemeryGetMap; //共享内存获取列表

Napi::Value SetShareMemery(const Napi::CallbackInfo& info);
Napi::Value GetShareMemery(const Napi::CallbackInfo& info);
Napi::Value ClearShareMemery(const Napi::CallbackInfo& info);

#endif