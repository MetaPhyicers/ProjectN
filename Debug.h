#pragma once
#include "Utility.h"

#define _CRTDBG_MAP_ALLOC

#define ASSERT(x) if(!(x)) __debugbreak();

#ifdef _DEBUG
#define GLCall(x) \
	Debug::GLClearError(); \
	x; \
	ASSERT(Debug::GLLogCall(#x, __FILE__, __LINE__));
#else
#define GLCall(x) x;
#endif

#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

#ifdef _DEBUG
#define new new
#endif


class Debug
{
public:
	Debug();
	~Debug();

	static void ShowMessage(const char* message);

	static void _stdcall GLDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity,
		GLsizei length, const char* message, const void* userParam);

	static int _cdecl AllocateHook(int nAllocType, void* pvData, size_t nSize, int nBlockUse, long lRequest, 
		const unsigned char* szFileName, int nLine);

	static void _cdecl NewHandler();

	static void GLClearError();
	static bool GLLogCall(const char* function, const char* file, int line);

private:

	// memory usage statistic
	static long long currentMemoryAllocate;
	static long long peekMemmoryAllocate;

	// crt memory check
	_CrtMemState memoryState;

};