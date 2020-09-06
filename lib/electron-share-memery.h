#ifndef ELECTRON_SHARED_MEMERY_H
#define ELECTRON_SHARED_MEMERY_H

#include <napi.h>

Napi::Value SetShareMemery(const Napi::CallbackInfo& info);
Napi::Value GetShareMemery(const Napi::CallbackInfo& info);

#endif