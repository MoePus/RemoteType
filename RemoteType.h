#pragma once
#include "iostream"
#include <tchar.h>
#include <memory>
#include "windows.h"

BOOL RPM(
	LPCVOID lpBaseAddress,
	LPVOID lpBuffer,
	SIZE_T nSize,
	SIZE_T* lpNumberOfBytesRead
);

template<typename T>
class ReflectPointer {
public:
	typedef T type;
	T addr;
	ReflectPointer(T _addr) :addr(_addr) {
		static_assert(std::is_pointer<T>::value, "RemoteType size must be le than an registers.");
	};
	T address()
	{
		return addr;
	}
	operator const T() const
	{
		T result = (T)malloc(sizeof(std::remove_pointer<T>::type));
		memset(result, 0, sizeof(std::remove_pointer<T>::type));
		size_t rb;
		RPM((void*)addr, result, sizeof(std::remove_pointer<T>::type), &rb);
		return result;
	};

	template<typename U = std::remove_pointer<T>::type>
	std::shared_ptr<U> share()
	{
		T result = (T)malloc(sizeof(std::remove_pointer<T>::type));
		memset(result, 0, sizeof(std::remove_pointer<T>::type));
		size_t rb;
		RPM((void*)addr, result, sizeof(std::remove_pointer<T>::type), &rb);
		return std::shared_ptr<U>(result, free);
	};

	template<typename W>
	typename std::enable_if<std::is_pointer<W>::value, ReflectPointer<W>>::type cast()
	{
		return ReflectPointer<W>((W)addr);
	}
};

template<typename T>
class RemoteType{
	size_t addr;
public:
	size_t address()
	{
		return addr;
	}

	RemoteType(size_t _addr) :addr(_addr) {
		static_assert(sizeof(T) <= sizeof(size_t), "RemoteType size must be le than an registers.");
	};

	template<typename U = std::remove_pointer<T>::type>
	typename std::enable_if<std::is_pointer<U>::value, RemoteType<U>>::type operator[] (const int index)
	{
		T result;
		size_t rb;
		RPM((void*)(addr + sizeof(size_t)*index), &result, sizeof(T), &rb);
		RemoteType<U> a((size_t)result);
		return a;
	};
	
	template<typename U = std::remove_pointer<T>::type>
	typename std::enable_if<
		!std::is_pointer<U>::value && (sizeof(U) <= sizeof(size_t)), U>::type
		operator[](const int index)
	{
		U result;
		size_t rb;
		RPM((void*)(addr + sizeof(U)*index), &result, sizeof(T), &rb);
		return result;
	};

	template<typename U = std::remove_pointer<T>::type>
	typename std::enable_if<std::is_pointer<U>::value, RemoteType<U>>::type operator*() const
	{
		T result;
		size_t rb;
		RPM((void*)(addr), &result, sizeof(T), &rb);
		RemoteType<U> a((size_t)result);
		return a;
	}

	template<typename U = std::remove_pointer<T>::type>
	typename std::enable_if<!std::is_pointer<U>::value, U>::type operator*() const
	{
		T result;
		size_t rb;
		RPM((void*)(addr), &result, sizeof(T), &rb);
		U a((size_t)result);
		return a;
	}

	template<typename U = std::remove_pointer<T>::type>
	ReflectPointer<T> refl(int index=0) const
	{
		return ReflectPointer<T>((T)((size_t)addr + sizeof(U)*index));
	}

	template<typename U = std::remove_pointer<T>::type>
	typename std::enable_if<std::is_pointer<U>::value, 
		std::shared_ptr<ReflectPointer<U>[]> >::type
	reflArray(int size) const
	{
		ReflectPointer<U>* result = (ReflectPointer<U>*)malloc(sizeof(U)*size);
		size_t rb;
		RPM((void*)addr,
			result, sizeof(U)*size, &rb);
		return std::shared_ptr<ReflectPointer<U>[]>(result, free);
	}
};
