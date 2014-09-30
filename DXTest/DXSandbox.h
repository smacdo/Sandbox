#pragma once
#include <errno.h>

// Let VC++ know we are compiling for WinXP and up.  This let's us use
// API functions specific to WinXP (e.g., WM_INPUT API).
#ifndef _WIN32_WINNT
#define _WIN32_WINNT   0x0600 // Vista
#endif

// Enable extra D3D debugging in debug builds if using the debug DirectX runtime.  
// This makes D3D objects work well in the debugger watch window, but slows down 
// performance slightly.
#if defined(DEBUG) || defined(_DEBUG)
#	ifndef D3D_DEBUG_INFO
#		define D3D_DEBUG_INFO
#	endif
#endif

// Enable memory debug tracking.
#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

// Safely delete a pointer and set the pointer to null.
template<typename T>
void SafeDelete(T*& pointer)
{
	delete pointer;
	pointer = nullptr;
}

// Safely delete an array pointer and set it to zero.
template<typename T>
void SafeDeleteArray(T*& pointer)
{
	delete[] pointer;
	pointer = nullptr;
}

// Safely release a COM object and set it to zero.
template<typename T>
void SafeRelease(T*& pInterface)
{
	if (pInterface != nullptr)
	{
		pInterface->Release();
		pInterface = 0;
	}
}

// Safely delete a container of pointers.
template<typename T>
void SafeDeleteContainer(T& container)
{
    for (auto pPointer : container)
    {
        delete pPointer;
        pPointer = 0;
    }

    container.clear();
}

#define Assert(x) ThrowExceptionIfFalse((x), #x)
#define Verify(x) ThrowExceptionIfFalse((x), #x)
#define VerifyNotNull(x) ThrowExceptionIfNull((x), #x) 
#define AssertNotNull(x) ThrowExceptionIfNull((x), #x)
#define AssertIsFalse(x) ThrowExceptionIfFalse((x) == false, #x)
#define AssertIsTrue(x) ThrowExceptionIfFalse((x) == true, #x)
#define VerifyDXResult(x) VerifyDXResultImpl((x), __FILE__, __LINE__, #x)	// L#x

void ThrowExceptionIfNull(void* pointer, const char* pVariableName);
void ThrowExceptionIfFalse(bool result, const char* expression);
void VerifyCResult(errno_t error, const char* description);
void VerifyDXResultImpl(long result, char * pFileName, int line, char * pMessage);

const float PI = 3.14159265358979323f;
const float MATH_EPS = 0.0001f;