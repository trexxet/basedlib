#pragma once

#include <string>
#include <string_view>

#if defined(__WIN32)
# include <windows.h>
#else
# include <cstdio>
# include <cwchar>
#endif

namespace Basedlib {

// TODO: use zstring_view when available

/// @brief Print wide string view
/// @note Don't forget to check & set stdout orientation on Linux before printing
/// @warning Must be null-terminated
void print_wcs (std::wstring_view wcs) {
#ifdef __WIN32
	WriteConsoleW (GetStdHandle (STD_OUTPUT_HANDLE), wcs.data(), static_cast<DWORD> (wcs.size()), nullptr, nullptr);
#else
	std::wprintf (L"%ls", wcs.data());
#endif
}

/// @brief Convert multibyte UTF-8 string view to wide string
/// @warning Must be null-terminated
std::wstring mbs_to_wcs (std::string_view mbs) {
	if (mbs.empty()) [[unlikely]] return L"";
#if defined(__WIN32)
	int len = MultiByteToWideChar (CP_UTF8, 0, mbs.data(), mbs.size(), nullptr, 0);
	if (len <= 0) [[unlikely]] return L"";
	std::wstring wcs (len, 0);
	MultiByteToWideChar (CP_UTF8, 0, mbs.data(), mbs.size(), wcs.data(), len);
#else
	const char* src = mbs.data();
	std::mbstate_t state {};
	size_t len = std::mbsrtowcs (nullptr, &src, 0, &state);
	if (len == static_cast<size_t> (-1)) [[unlikely]] return L"";
	std::wstring wcs (len + 1, 0);
	std::mbsrtowcs (wcs.data(), &src, wcs.size(), &state);
#endif
	return wcs;
}

/// @brief Convert wide string view to multibyte UTF-8 string
/// @warning Must be null-terminated
std::string wcs_to_mbs (std::wstring_view wcs) {
	if (wcs.empty()) [[unlikely]] return "";
#if defined(__WIN32)
	int len = WideCharToMultiByte (CP_UTF8, 0, wcs.data(), wcs.size(), nullptr, 0, nullptr, nullptr);
	if (len <= 0) [[unlikely]] return "";
	std::string mbs (len, 0);
	WideCharToMultiByte (CP_UTF8, 0, wcs.data(), wcs.size(), mbs.data(), len, nullptr, nullptr);
#else
	const wchar_t* wsrc = wcs.data();
	std::mbstate_t state {};
	size_t len = std::wcsrtombs (nullptr, &wsrc, 0, &state);
	if (len == static_cast<size_t> (-1)) [[unlikely]] return "";
	std::string mbs (len, 0);
	std::wcsrtombs (mbs.data(), &wsrc, mbs.size(), &state);
#endif
	return mbs;
}

}
