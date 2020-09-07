#include <napi.h>
#include <node.h>
#include <memory>
#include <iostream>
#include <string>
#include <cstdio>
#include "electron-share-memery.h"

using namespace Napi;
using namespace std;


// trim from end of string (right)
inline std::string& rtrim(std::string& s)
{
    s.erase(s.find_last_not_of(" \t\n\r\f\v") + 1);
    return s;
}

// trim from beginning of string (left)
inline std::string& ltrim(std::string& s)
{
    s.erase(0, s.find_first_not_of(" \t\n\r\f\v"));
    return s;
}

// trim from both ends of string (right then left)
inline std::string& trim(std::string& s)
{
    return ltrim(rtrim(s));
}

std::string GetLastErrorAsString()
{
    //Get the error message, if any.
    DWORD errorMessageID = ::GetLastError();
    if(errorMessageID == 0)
        return std::string(); //No error message has been recorded

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    std::string message(messageBuffer, size);

    //Free the buffer.
    LocalFree(messageBuffer);

    return trim(message);
}

#define failFormat(...) { \
		size_t size = snprintf(nullptr, 0, __VA_ARGS__) + 1; \
		unique_ptr<char[]> buf(new char[size]); \
		snprintf(buf.get(), size, __VA_ARGS__); \
		auto str = string(buf.get(), buf.get() + size - 1); \
		Napi::Error::New(env, str).ThrowAsJavaScriptException(); \
	}

#define failv(...) { failFormat(__VA_ARGS__); return Napi::Value(); }

Napi::Value SetShareMemery(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	if (info.Length() < 4)		failv("Needs name, maxSize, dataSize, data");
	if (!info[0].IsString())	failv("argument 0 name needs be a string");
	if (!info[1].IsNumber())	failv("argument 1 maxSize needs be a number");
	if (!info[2].IsNumber())	failv("argument 2 dataSize needs be a number");
	if (!info[3].IsArrayBuffer())	failv("argument 3 data needs be a ArrayBuffer");

	auto name = info[0].As<Napi::String>().Utf8Value();
	auto maxSize = info[1].As<Napi::Number>();
	auto dataSize = info[2].As<Napi::Number>();
	auto data = info[3].As<Napi::ArrayBuffer>();

	HANDLE mapping = nullptr;

	if (_ShareMemerySetMap.find(name) == _ShareMemerySetMap.end())
	{
		mapping = CreateFileMapping(
			INVALID_HANDLE_VALUE,    // use paging file
			NULL,                    // default security
			PAGE_READWRITE,          // read/write access
			0,                       // maximum object size (high-order DWORD)
			(int64_t)maxSize,        // maximum object size (low-order DWORD)
			name.c_str());           // name of mapping object

		if (mapping == nullptr) {
			auto err = GetLastErrorAsString();
			failv("[SetShareMemery] could not open \"%s\" (ERROR: %s)", name.c_str(), err.c_str());
		}

		void* ptr = MapViewOfFile(mapping, FILE_MAP_WRITE, 0, 0, (int64_t)maxSize);
		if (ptr == nullptr) {
			auto err = GetLastErrorAsString();
			CloseHandle(mapping);
			failv("[SetShareMemery] could not map: \"%s\" (ERROR: %s)", name.c_str(), err.c_str());
		}

		memcpy(ptr, data, dataSize);

		_ShareMemerySetMap[name].handle = mapping;
		_ShareMemerySetMap[name].ptr = ptr;
		_ShareMemerySetMap[name].size = dataSize;
	} 
	else
	{
		memcpy(_ShareMemerySetMap[name].ptr, data, dataSize);
	}

	return Napi::ArrayBuffer::New(info.Env(), true);
}

Napi::Value GetShareMemery(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	if (info.Length() < 1)		failv("Needs name");
	if (!info[0].IsString())	failv("argument 0 name needs be a string");

	auto name = info[0].As<Napi::String>().Utf8Value();

	HANDLE mapping = nullptr;

	if (_ShareMemeryGetMap.find(name) == _ShareMemeryGetMap.end())
	{
		mapping = OpenFileMapping(FILE_MAP_READ, false, name.c_str());

		if (mapping == nullptr) {
			auto err = GetLastErrorAsString();
			failv("[GetShareMemery] could not open \"%s\" (ERROR: %s)", name.c_str(), err.c_str());
		}

		void* ptr = MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0);
		if (ptr == nullptr) {
			auto err = GetLastErrorAsString();
			CloseHandle(mapping);
			failv("[GetShareMemery] could not map: \"%s\" (ERROR: %s)", name.c_str(), err.c_str());
		}

		_ShareMemerySetMap[name].handle = mapping;
		_ShareMemerySetMap[name].ptr = ptr;
		_ShareMemerySetMap[name].size = 1024 * 1024;
	}

	return Napi::ArrayBuffer::New(env, _ShareMemerySetMap[name].ptr, (size_t)_ShareMemerySetMap[name].size);
}

Napi::Value ClearShareMemery(const Napi::CallbackInfo& info) {
    for (auto item : _ShareMemerySetMap)
    {
		UnmapViewOfFile(item.second.ptr);
		CloseHandle(item.second.handle);
    }
	_ShareMemerySetMap.clear();

	for (auto item : _ShareMemeryGetMap)
	{
		UnmapViewOfFile(item.second.ptr);
		CloseHandle(item.second.handle);
	}
	_ShareMemeryGetMap.clear();

	return Napi::Boolean::New(info.Env(), true);
}