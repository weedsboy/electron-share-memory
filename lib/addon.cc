#include <napi.h>
#include "electron-share-memory.h"

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "SetShareMemory"), Napi::Function::New(env, SetShareMemory));
  exports.Set(Napi::String::New(env, "GetShareMemory"), Napi::Function::New(env, GetShareMemory));
  exports.Set(Napi::String::New(env, "ClearShareMemory"), Napi::Function::New(env, ClearShareMemory));
  return exports;
}

NODE_API_MODULE(electron_share_memory, Init)