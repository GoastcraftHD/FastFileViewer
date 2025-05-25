#pragma once

#include <memory>

typedef unsigned char U8;
typedef unsigned short int U16;
typedef unsigned int U32;
typedef unsigned long int U64;

typedef char I8;
typedef short int I16;
typedef int I32;
typedef long int I64;

typedef float F32;
typedef double F64;

template<typename T>
using UniquePtr = std::unique_ptr<T>;

template<typename T, typename... Args>
constexpr UniquePtr<T> MakeUnique(Args&&... args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using SharedPtr = std::shared_ptr<T>;

template<typename T, typename... Args>
constexpr SharedPtr<T> MakeShared(Args&&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}
