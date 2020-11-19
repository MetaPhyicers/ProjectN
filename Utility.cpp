#include "Utility.h"
#include <Windows.h>
#include <cstdlib>
#include <stdexcept>


std::string Unicode::UnicodeToMultibytes(const wchar_t* text)
{
	int num = WideCharToMultiByte(CP_ACP, 0, text, -1, nullptr, 0, nullptr, FALSE);
	LPSTR buffer = new char[num];
	WideCharToMultiByte(CP_ACP, 0, text, -1, buffer, num, nullptr, FALSE);

	std::string result(buffer);
	delete[] buffer;

	return result;
}

std::wstring Unicode::MultibytesToUnicode(const char* text)
{
	int num = MultiByteToWideChar(CP_UTF8, 0, text, -1, nullptr, 0);
	LPWSTR buffer = new wchar_t(sizeof(wchar_t) * num);
	MultiByteToWideChar(CP_UTF8, 0, text, num, buffer, sizeof(wchar_t) * num);

	std::wstring result(buffer);
	delete[] buffer;

	return result;
}

unsigned int HashString::FNV_1A_Unicode(const wchar_t* str, size_t length)
{
	const unsigned int primeHash = 2166136261;

	unsigned int hashValue = primeHash;
	size_t idx = 0;

	wchar_t c = *str;
	while (c != '\0') {
		hashValue = hashValue ^ c;
		hashValue = hashValue * 16777619;

		idx++;
		c = str[idx];
	}

	return hashValue;
}

unsigned int HashString::FNV_1A_Multibyte(const char* str, size_t length)
{
	const unsigned int primeHash = 2166136261;

	unsigned int hashValue = primeHash;
	size_t idx = 0;

	char c = *str;
	while (c != '\0') {
		hashValue = hashValue ^ c;
		hashValue = hashValue * 16777619;

		idx++;
		c = str[idx];
	}

	return hashValue;
}
