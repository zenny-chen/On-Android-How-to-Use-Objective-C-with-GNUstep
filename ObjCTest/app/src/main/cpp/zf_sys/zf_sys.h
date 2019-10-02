//
//  zf_sys.h
//  ZennyFoundationTest
//
//  Created by Zenny Chen on 2019/3/22.
//  Copyright © 2019 Zenny Chen. All rights reserved.
//

#ifndef zf_sys_h
#define zf_sys_h

#ifdef _WIN32
// Windows platforms
#include <Windows.h>
#include <intrin.h>

static inline void zf_nap(void)
{
    Sleep(1);
}

#else
// Unix-like platforms
#include <unistd.h>

/// Let the current thread briefly sleep and make another thread run
static inline void zf_nap(void)
{
    usleep(10);
}

#endif // #ifdef _WIN32


#define ZF_SPIN_LOCK_TRY_COUNT       100


#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef _MSC_VER
// Compilers that support most of C11

#include <stdalign.h>
#include <stdnoreturn.h>

#define thread_local        _Thread_local

/// CPU Pause. This is a hint that indicates CPU to temporarily
/// pause the current running thread.
static inline void zf_cpu_pause(void)
{
#if defined(__x86__) || defined(__i386__) || defined(__x86_64__)
    asm("pause");
#elif defined(__arm__)

#if defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
    asm("yield");
#endif

#elif defined(__aarch64__)
    asm("yield");
#else
    // For other CPU architectures, do nothing here...
#endif
}

#else
// For MSVC

#define alignof             _Alignof
#define alignas(nBytes)     __declspec(align(nBytes))

#define thread_local        __declspec(thread)

#define noreturn            __declspec(noreturn)

/// signed size_t type which may be used in this foundation library
typedef ptrdiff_t           ssize_t;

/// CPU Pause. This is a hint that indicates CPU to temporarily
/// pause the current running thread.
static inline void zf_cpu_pause(void)
{
#if defined(_M_IX86) || defined(_M_X64)
    _mm_pause();

#elif defined(_M_ARM64)
    __yield();

#endif
}

#endif  // #ifndef _MSC_VER


#ifdef __GNUC__
// GNU C extensions

#ifndef let
#define let                 __auto_type
#endif

#define type_cast(expr)     (typeof(expr))

#else

#define type_cast(expr)

#endif  // #ifdef __GNUC__


// MARK: stdio relevant

#ifdef __ANDROID__

#include <syslog.h>

#define printf(...)     syslog(LOG_INFO, __VA_ARGS__)

#define puts(cstr)      printf("%s\n", (cstr))

#endif  // #ifdef __ANDROID__

/// Gets a whole line from the stdin stream, excluding the last newline character.
/// @param buffer the buffer that stores the input string
/// @param maxBufferSize the maximum size of the buffer
/// @return if successful, return the number of bytes the buffer stores; Otherwise, return -1.
static inline ssize_t zf_get_console_line(char* buffer, size_t maxBufferSize)
{
    if (buffer == NULL)
        return -1;

#ifdef _WIN32

    char* result = gets_s(buffer, maxBufferSize);
    return result == NULL ? -1 : strlen(result);
    
#else
    
    char *tmpBuf = NULL;
    size_t nBytes = 0;
    ssize_t result = getline(&tmpBuf, &nBytes, stdin);
    if(result > 0)
    {
        int nIters = result < 2 ? (int)result : 2;
        for(ssize_t index = result - 1; nIters > 0; nIters--, index--)
        {
            if(tmpBuf[index] == '\r' || tmpBuf[index] == '\n')
                result--;
        }
        
        if(result >= maxBufferSize)
            result = maxBufferSize - 1;
        
        strncpy(buffer, tmpBuf, result);
    }
    
    if(tmpBuf != NULL)
        free(tmpBuf);
    
    if(result >= 0)
        buffer[result] = '\0';
    
    return result;

#endif
}

#endif /* zf_sys_h */

