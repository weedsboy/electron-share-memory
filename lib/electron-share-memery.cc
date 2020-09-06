#include <napi.h>
#include "electron-share-memery.h"

Napi::Value SetShareMemery(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    int n1 = info[0].As<Napi::Number>();
    int n2 = info[1].As<Napi::Number>();

    return Napi::Number::New(env, n1 + n2);
}

Napi::Value GetShareMemery(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    int n1 = info[0].As<Napi::Number>();
    int n2 = info[1].As<Napi::Number>();

    return Napi::Number::New(env, n1 - n2);
}