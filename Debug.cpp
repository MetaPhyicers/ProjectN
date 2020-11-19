#include "Debug.h"
//#include <DBGINT.H>

Debug debug;

long long Debug::currentMemoryAllocate = 0L;
long long Debug::peekMemmoryAllocate = 0L;

Debug::Debug()
	:memoryState()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtSetAllocHook(reinterpret_cast<_CRT_ALLOC_HOOK>(AllocateHook));

	std::set_new_handler(NewHandler);

	///_CrtMemCheckpoint(&memoryState);
}

Debug::~Debug()
{
	std::string statisticInfo = "Debug Report:\nPeek heap memeory cost: ";
	if (peekMemmoryAllocate < 0x400) {
		statisticInfo += std::to_string(peekMemmoryAllocate);
		statisticInfo.append(" Byte");
	}
	else if (peekMemmoryAllocate < 0x100000) {
		statisticInfo += std::to_string(peekMemmoryAllocate >> 10);
		statisticInfo.append(" KB");
	}
	else {
		statisticInfo += std::to_string(peekMemmoryAllocate >> 20);
		statisticInfo.append(" MB");
	}

	ShowMessage(statisticInfo.c_str());
	//_CrtDumpMemoryLeaks();

	//_CrtMemDumpAllObjectsSince(&memoryState);
}

void Debug::ShowMessage(const char* message)
{
#ifdef _DEBUG
    // To output the debug message into std stream
    std::cout << message << std::endl;
#endif // _DEBUG

}

void _stdcall Debug::GLDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam)
{
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
    case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
    case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
    case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
    case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
    case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
    case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
    case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}

int _cdecl Debug::AllocateHook(int nAllocType, void* pvData, size_t nSize, int nBlockUse, long lRequest, const unsigned char* szFileName, int nLine)
{
	// ignore crt allocs because we might be calling into the crt from this function
	if (nBlockUse == _CRT_BLOCK)
	{
		return 1;
	}

	switch (nAllocType)
	{
	case _HOOK_ALLOC:
		currentMemoryAllocate += nSize;
		if (currentMemoryAllocate > peekMemmoryAllocate) {
			peekMemmoryAllocate = currentMemoryAllocate;
		}
		break;

	case _HOOK_REALLOC:
		if (pvData) {
			currentMemoryAllocate -= _msize_dbg(pvData, nBlockUse);
		}
		break;

	case _HOOK_FREE:
		currentMemoryAllocate -= _msize_dbg(pvData, nBlockUse);
		break;
	}


	return 1;
}

void _cdecl Debug::NewHandler()
{
	while (1);
}

void Debug::GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

bool Debug::GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum err = glGetError()) {

		std::string errorText;

		switch (err)
		{
		case GL_INVALID_ENUM:
			errorText = "GL_INVALID_ENUM";
			break;

		case GL_INVALID_VALUE:
			errorText = "GL_INVALID_VALUE";
			break;

		case GL_INVALID_OPERATION:
			errorText = "GL_INVALID_OPERATION";
			break;

		case GL_STACK_OVERFLOW:
			errorText = "GL_STACK_OVERFLOW";
			break;

		case GL_STACK_UNDERFLOW:
			errorText = "GL_STACK_UNDERFLOW";
			break;

		case GL_OUT_OF_MEMORY:
			errorText = "GL_OUT_OF_MEMORY";
			break;

		}

		std::string errorMessage = "OpenGL Error: " + errorText + " in ";
		errorMessage.append(file);
		errorMessage += " line " + std::to_string(line);

		std::cout << errorMessage << std::endl;


		return false;
	}

	return true;
}