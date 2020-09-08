#include <napi.h>
#include <node.h>
#include <memory>
#include <iostream>
#include <string>
#include <cstdio>
#include "electron-share-memory.h"

using namespace Napi;
using namespace std;

enum MemoryStatus
{
	MEMORY_FREE = 0,	//内存空闲
	MEMORY_WRITE = 1,	//内存写入
	MEMORY_READ = 1		//内存读取
};

Napi::Value SetShareMemory(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	if (info.Length() < 4)		return Napi::Boolean::New(env, false);
	if (!info[0].IsString())	return Napi::Boolean::New(env, false);
	if (!info[1].IsNumber())	return Napi::Boolean::New(env, false);
	if (!info[2].IsNumber())	return Napi::Boolean::New(env, false);
	if (!info[3].IsArrayBuffer())	return Napi::Boolean::New(env, false);

	auto name = info[0].As<Napi::String>().Utf8Value();
	auto maxSize = info[1].As<Napi::Number>();
	auto dataSize = info[2].As<Napi::Number>();
	auto data = info[3].As<Napi::ArrayBuffer>();

	HANDLE mapping = nullptr;

	if (_ShareMemorySetMap.find(name) == _ShareMemorySetMap.end())
	{
		mapping = CreateFileMapping(
			INVALID_HANDLE_VALUE,    
			NULL,                    
			PAGE_READWRITE,         
			0,                      
			(int64_t)maxSize,      
			name.c_str()); 

		if (mapping == nullptr) 
		{
			return Napi::Boolean::New(env, false);
		}

		void* ptr = MapViewOfFile(mapping, FILE_MAP_WRITE, 0, 0, (int64_t)maxSize);
		if (ptr == nullptr) 
		{
			CloseHandle(mapping);
			return Napi::Boolean::New(env, false);
		}

		//存储句柄、指针
		_ShareMemorySetMap[name].handle = mapping;
		_ShareMemorySetMap[name].ptr = ptr;

		//获取对应位置的指针
		unsigned int* memPtr = (unsigned int*)ptr;
		unsigned int* memFlagPtr = &memPtr[0];
		unsigned int* memMaxSizePtr = &memPtr[1];
		unsigned int* memDataSizePtr = &memPtr[2];
		unsigned int* memDataPtr = &memPtr[3];

		unsigned int uintLength = sizeof(unsigned int);

		//设置写状态
		unsigned int flag = MEMORY_FREE;
		memcpy(memFlagPtr, &flag, uintLength);

		//设置内存长度
		unsigned int maxMemSize = maxSize;
		memcpy(memMaxSizePtr, &maxMemSize, uintLength);

		//设置数据长度
		unsigned int dataMemSize = dataSize;
		memcpy(memDataSizePtr, &dataMemSize, uintLength);

		//设置数据内容
		memcpy(memDataPtr, data.Data(), dataMemSize);

		//还原写状态
		flag = 0;
		memcpy(memFlagPtr, &flag, uintLength);
	} 
	else
	{
		//获取对应位置的指针
		void* ptr = _ShareMemorySetMap[name].ptr;
		unsigned int* memPtr = (unsigned int*)ptr;
		unsigned int* memFlagPtr = &memPtr[0];
		unsigned int* memMaxSizePtr = &memPtr[1];
		unsigned int* memDataSizePtr = &memPtr[2];
		unsigned int* memDataPtr = &memPtr[3];

		unsigned int uintLength = sizeof(unsigned int);

		unsigned int flag;
		memcpy(&flag, memFlagPtr, uintLength);

		if (flag == MEMORY_FREE)
		{
			//设置写状态
			flag = MEMORY_WRITE;
			memcpy(memFlagPtr, &flag, uintLength);

			//设置数据长度
			unsigned int dataMemSize = dataSize;
			memcpy(memDataSizePtr, &dataMemSize, uintLength);

			//设置数据内容
			memcpy(memDataPtr, data.Data(), dataMemSize);

			//还原写状态
			flag = MEMORY_FREE;
			memcpy(memFlagPtr, &flag, uintLength);
		}
	}

	return Napi::Boolean::New(env, true);
}

Napi::Value GetShareMemory(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	if (info.Length() < 1)		return Napi::ArrayBuffer::New(env, nullptr, 0);
	if (!info[0].IsString())	return Napi::ArrayBuffer::New(env, nullptr, 0);

	auto name = info[0].As<Napi::String>().Utf8Value();

	HANDLE mapping = nullptr;

	if (_ShareMemoryGetMap.find(name) == _ShareMemoryGetMap.end())
	{
		mapping = OpenFileMapping(FILE_MAP_READ, false, name.c_str());

		if (mapping == nullptr) 
		{
			return Napi::ArrayBuffer::New(env, nullptr, 0);
		}

		void* ptr = MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0);
		if (ptr == nullptr) 
		{
			CloseHandle(mapping);
			return Napi::ArrayBuffer::New(env, nullptr, 0);
		}

		_ShareMemorySetMap[name].handle = mapping;
		_ShareMemorySetMap[name].ptr = ptr;
	}

	//获取对应位置的指针
	void* ptr = _ShareMemorySetMap[name].ptr;
	unsigned int* memPtr = (unsigned int*)ptr;
	unsigned int* memFlagPtr = &memPtr[0];
	unsigned int* memMaxSizePtr = &memPtr[1];
	unsigned int* memDataSizePtr = &memPtr[2];
	unsigned int* memDataPtr = &memPtr[3];

	unsigned int uintLength = sizeof(unsigned int);

	unsigned int flag;
	memcpy(&flag, memFlagPtr, uintLength);

	//如果当前正空闲，则读取数据
	if (flag == MEMORY_FREE)
	{
		//设置读状态
		flag = MEMORY_READ;
		memcpy(memFlagPtr, &flag, uintLength);

		//设置数据长度
		unsigned int dataMemSize;
		memcpy(&dataMemSize, memDataSizePtr, uintLength);

		auto data = Napi::ArrayBuffer::New(env, memDataPtr, dataMemSize);

		//设置空闲状态
		flag = MEMORY_FREE;
		memcpy(memFlagPtr, &flag, uintLength);

		return data;
	}
	else 
	{
		return Napi::ArrayBuffer::New(env, nullptr, 0);
	}
}

Napi::Value ClearShareMemory(const Napi::CallbackInfo& info) {
    for (auto item : _ShareMemorySetMap)
    {
		UnmapViewOfFile(item.second.ptr);
		CloseHandle(item.second.handle);
    }
	_ShareMemorySetMap.clear();

	for (auto item : _ShareMemoryGetMap)
	{
		UnmapViewOfFile(item.second.ptr);
		CloseHandle(item.second.handle);
	}
	_ShareMemoryGetMap.clear();

	return Napi::Boolean::New(info.Env(), true);
}