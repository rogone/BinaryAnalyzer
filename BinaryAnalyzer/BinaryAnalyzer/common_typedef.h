#ifndef __BINARY_ANALYZER_COMMON_TYPEDEF_H__
#define __BINARY_ANALYZER_COMMON_TYPEDEF_H__

typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
#ifdef WIN32
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#else
typedef long long int int64_t;
typedef unsigned long long int uint64_t;
#endif

typedef unsigned char buffer_t;

#endif //
