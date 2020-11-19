#pragma once

// C++ standard library
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <list>	
#include <set>
#include <stack>
#include <queue>
#include <memory>
#include <map>
#include <stdexcept>

// windows API
#include <Windows.h>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#pragma comment(lib, "glew32S.lib")
#pragma comment(lib, "opengl32.lib")

//GLFW
#include <GLFW/glfw3.h>

#pragma comment(lib, "glfw3.lib")

// glm math library
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Unicode
{
public:
	static std::string UnicodeToMultibytes(const wchar_t* text);
	static std::wstring MultibytesToUnicode(const char* text);
};

class HashString
{
public:
	static unsigned int FNV_1A_Unicode(const wchar_t* str, size_t length);
	static unsigned int FNV_1A_Multibyte(const char* str, size_t length);
};

template <typename _Ty>
void SafeDelete(_Ty*& ptr)
{
	if (ptr) {
		delete ptr;
		ptr = nullptr;
	}
}
template <typename _Ty>
void SafeDeleteArray(_Ty*& arr)
{
	if (arr) {
		delete[] arr;
		arr = nullptr;
	}
}

/**
*	python range...
*/
template<typename _type>
class Range
{
public:
	Range(_type begin, _type end, _type offset = 1)
		:start(begin, offset), finish(end, offset)
	{
	}
	~Range()
	{
	}

	class iterator
	{
	public:
		iterator()
			:index(0), offset(1)
		{
		}
		iterator(_type index, _type offset)
			:index(index), offset(offset)
		{
		}
		iterator(const iterator& iter)
			:index(iter.index), offset(iter.offset)
		{
		}
		~iterator()
		{
		}
		iterator& operator = (const iterator& iter)
		{
			this->index = iter.index;
		}
		bool operator == (const iterator& iter)
		{
			return this->index == iter.index;
		}
		bool operator != (const iterator& iter)
		{
			return this->index != iter.index;
		}
		bool operator > (const iterator& iter)
		{
			return this->index > iter.index;
		}
		bool operator < (const iterator& iter)
		{
			return this->index < iter.index;
		}
		bool operator >= (const iterator& iter)
		{
			return this->index >= iter.index;
		}
		bool operator <= (const iterator& iter)
		{
			return this->index <= iter.index;
		}
		iterator operator ++ ()
		{
			iterator iter = *this;
			index += offset;
			return iter;
		}
		iterator operator -- ()
		{
			iterator iter = *this;
			index -= offset;
			return iter;
		}
		iterator operator ++ (int)
		{
			(*this)++;
			return *this;
		}
		iterator operator -- (int)
		{
			(*this)--;
			return *this;
		}
		iterator operator += (_type off)
		{
			index += off;
			return this;
		}
		iterator operator -= (_type off)
		{
			index -= off;
			return this;
		}
		_type operator* ()
		{
			return index;
		}
		_type* operator->()
		{
			return &index;
		}

		_type index;
		_type offset;
	};

	iterator begin()
	{
		return start;
	}
	iterator end()
	{
		return finish;
	}

private:

	iterator start;
	iterator finish;
};