/*
	Copyright (C) 2016 Federico Kircheis

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

// windows
#include <Windows.h>
#include <winnls.h>

// std
#include <string>
#include <vector>
#include <numeric>
#include <limits>
#include <stdexcept>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <locale>         // std::locale, std::isdigit
#include <regex>

// cstd
#include <cassert>

/*
inline std::string GetLastErrorAsString() {
	//Get the error message, if any.
	const DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0) {
		return ""; //No error message has been recorded
	}

	LPSTR messageBuffer = nullptr;
	const size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, nullptr);

	std::string message(messageBuffer, size);

	//Free the buffer.
	LocalFree(messageBuffer);

	return message;
}
*/

/// example of usage:
/// int* array = ...; size_t size = ...; assert(implies(size > 0, array != nullptr));
template<typename T1, typename T2>
bool implies(const T1& t1, const T2& t2) {
	return (!(t1) || (t2));
}

#if defined(NONLS)
#error "need NLS for CP_UTF8 and MB_ERR_INVALID_CHARS"
#endif

inline std::string trim(std::string s){
	s.erase(0, s.find_first_not_of(" \t\n\r\f\v"));
	s.erase(s.find_last_not_of(" \t\n\r\f\v") + 1);
	return s;
}

/// removes delimiter " at the begin and(!) at the end
inline std::string removedel(std::string s) {
	if (s.size() < 2) {
		return s; // not enough space for " at the begin and end
	}
	if (s.at(0) == '"' && s.at(s.size() - 1) == '"') {
		return s.substr(1, s.size() - 2);
	}
	return s;
}

/// possible alternative: http://www.cplusplus.com/reference/codecvt/codecvt_utf8_utf16/
inline std::wstring s2ws(const std::string& s) {
	if (s.empty()) {
		return L"";
	}
	constexpr auto flag = MB_ERR_INVALID_CHARS; // alternative to MB_ERR_INVALID_CHARS is 0
	// https://msdn.microsoft.com/en-us/library/windows/desktop/dd319072%28v=vs.85%29.aspx
	if (s.length() > (static_cast<size_t>((std::numeric_limits<int>::max)()))) {
		throw std::runtime_error("string " + s + " is too long");
	}
	int len = MultiByteToWideChar(CP_UTF8, flag, s.c_str(), static_cast<int>(s.length()), nullptr, 0);
	if (len <= 0) {
		throw std::runtime_error("error when converting" + s + "to wstring");
	}
	std::wstring buf = std::wstring(static_cast<size_t>(len), '\0');
	len = MultiByteToWideChar(CP_UTF8, flag, s.c_str(), static_cast<int>(s.length()), &buf[0], len);
	if (len <= 0) {
		throw std::runtime_error("error when converting" + s + "to wstring");
	}
	buf.resize(static_cast<size_t>(len));
	return buf;
}

inline std::string ws2s(const std::wstring& s) {
	if (s.empty()) {
		return "";
	}
	if (s.length() > (static_cast<size_t>((std::numeric_limits<int>::max)()))) {
		throw std::runtime_error("string is too long");
	}
	constexpr auto flag = WC_ERR_INVALID_CHARS; // alternative to WC_ERR_INVALID_CHARS is 0
	// https://msdn.microsoft.com/en-us/library/windows/desktop/dd374130%28v=vs.85%29.aspx
	int len = WideCharToMultiByte(CP_UTF8, flag, s.c_str(), static_cast<int>(s.length()), nullptr, 0, nullptr, nullptr);
	if (len <= 0) {
		throw std::runtime_error("error when converting to string");
	}
	std::string buf(static_cast<size_t>(len), '\0');
	len = WideCharToMultiByte(CP_UTF8, flag, s.c_str(), static_cast<int>(s.length()), &buf[0], len, nullptr, nullptr);
	if (len <= 0) {
		throw std::runtime_error("error when converting to string");
	}
	buf.resize(static_cast<size_t>(len));
	return buf;
}


// ----------------------------------------------------------- //

inline std::vector<std::string> trimandremovedelim(std::vector<std::string> v) {
	std::transform(v.begin(), v.end(), v.begin(), [](const std::string& s) { return removedel(trim(s)); });
	return v;
}
inline std::vector<std::string> explode(const std::string &s, const char delim) {
	std::vector<std::string> elems;
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

/// may change order!
inline std::vector<std::string> uniquify(std::vector<std::string> s) {
	std::sort(s.begin(), s.end());
	s.erase(std::unique(s.begin(), s.end()), s.end());
	return s;
}

inline std::string flatten(const std::vector<std::string>& in, const char sep) {
	if(in.empty()){
		return "";
	}
	const std::string toreturn = std::accumulate(in.begin(), in.end()-1, std::string(),
												 [sep](std::string& so_far, const std::string& cur){
			so_far += cur; so_far += sep;
			return so_far;
});
	return toreturn + in.at(in.size()-1);
}

inline std::wstring flatten(const std::vector<std::wstring>& in, const wchar_t sep) {
	if(in.empty()){
		return L"";
	}
	const std::wstring toreturn = std::accumulate(in.begin(), in.end()-1, std::wstring(),
												  [sep](std::wstring& so_far, const std::wstring& cur){
			so_far += cur; so_far += sep;
			return so_far;
});
	return toreturn + in.at(in.size()-1);
}


inline std::string to_multi_string(const std::vector<std::string>& in) {
	const std::string toreturn = flatten(in, '\0') + '\0';
	const std::string empty_multi_string(1, '\0');
	return toreturn.empty() ? empty_multi_string : toreturn;
}

inline std::wstring to_multi_string(const std::vector<std::wstring>& in) {
	const std::wstring toreturn = flatten(in, L'\0') + L'\0';
	const std::wstring empty_multi_string(1, L'\0');
	return toreturn.empty() ? empty_multi_string : toreturn;
}

/// complexity: O(N^2), use only on small containers, otherwise use a std::set/unordered_set
inline void add_if_unique(std::vector<std::string>& v, const std::string& value) {
	if (std::find(v.begin(), v.end(), value) == v.end()) {
		// someName not in name, add it
		v.push_back(value);
	}
}

/// first says if it matches or not
/// second is the optional number, empty if there was no number
inline std::pair<bool, std::string> matchwithoptionalnumber(const std::string& s, const std::string& rule) {
	if (s.size() < rule.size()) {
		return{ false, "" };
	}
	if (s.compare(0, rule.size(), rule) != 0) {
		return{ false, "" };
	}
	const std::locale loc;
	const auto& substr = s.substr(rule.size());
	for (const auto& i : substr) {
		if (!std::isdigit(i, loc)) {
			return{ false, "" };
		}
	}
	return{ true, substr };
}

inline std::vector<std::string> combineext(const std::vector<std::string>& ext1, const std::vector<std::string>& ext2) {
	assert(!ext1.empty());
	assert(!ext2.empty());
	std::vector<std::string> doubleext;
	doubleext.reserve(ext1.size()*ext2.size());
	for (const auto& e1 : ext1) {
		assert(e1.at(0) != '.');
		for (const auto& e2 : ext2) {
			assert(e2.at(0) != '.');
			doubleext.emplace_back("*." + e1 + "." + e2);
		}
	}
	return doubleext;
}
