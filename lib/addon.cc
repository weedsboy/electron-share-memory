#include <napi.h>
#include "electron-share-memery.h"

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "SetShareMemery"), Napi::Function::New(env, SetShareMemery));
  exports.Set(Napi::String::New(env, "GetShareMemery"), Napi::Function::New(env, GetShareMemery));
  return exports;
}

NODE_API_MODULE(electron_share_memery, Init)