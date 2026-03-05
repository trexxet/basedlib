#pragma once

#include <string_view>

#if defined(__WIN32)
# include <windows.h>
#else
# include <cstdio>
# include <cwchar>
#endif

namespace Basedlib::Text {

// TODO: use zstring_view when available

/// @brief Print wide string view
/// @note Don't forget to check & set stdout orientation on Linux before printing
/// @warning Must be null-terminated
inline void print_wcs (std::wstring_view wcs) {
#ifdef __WIN32
	WriteConsoleW (GetStdHandle (STD_OUTPUT_HANDLE), wcs.data(), static_cast<DWORD> (wcs.size()), nullptr, nullptr);
#else
	std::wprintf (L"%ls", wcs.data());
#endif
}

constexpr bool is_high_surrogate (wchar_t wch) noexcept {
	return 0xD800 <= wch && wch <= 0xDBFF;
}

constexpr bool is_low_surrogate (wchar_t wch) noexcept {
	return 0xDC00 <= wch && wch <= 0xDFFF;
}

constexpr char32_t from_surrogate (wchar_t high, wchar_t low) noexcept {
	return 0x10000 + ((high & 0x03FF) << 10) + (low & 0x03FF);
}

}
