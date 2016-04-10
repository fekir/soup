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

// local
#include "win_handles.hpp"

// windows
#include <winreg.h>
#include <KtmW32.h> // >= windows vista
#include <WinDNS.h> // qword, should I really add it as a dependency?

// std
#include <string>
#include <cassert>
#include <algorithm>
#include <vector>

namespace registry {

	RAII_HKEY OpenKey(const HKEY hk, const std::wstring& subkey = L"", const REGSAM samDesired = KEY_READ, DWORD dwOptions = REG_OPTION_NON_VOLATILE);

	RAII_HKEY OpenKey(const HKEY hk, const std::string& subkey, const REGSAM samDesired = KEY_READ, const DWORD dwOptions = REG_OPTION_NON_VOLATILE);

	RAII_HKEY OpenKeyOptional(const HKEY hk, const std::wstring& subkey = L"", const REGSAM samDesired = KEY_READ, DWORD dwOptions = REG_OPTION_NON_VOLATILE);
	RAII_HKEY OpenKeyOptional(const HKEY hk, const std::string& subkey, const REGSAM samDesired = KEY_READ, DWORD dwOptions = REG_OPTION_NON_VOLATILE);

	RAII_HKEY CreateKey(const HKEY hk, const std::wstring& subkey = L"", const REGSAM samDesired = KEY_READ, DWORD dwOptions = REG_OPTION_NON_VOLATILE);

	RAII_HKEY CreateKey(const HKEY hk, const std::string& subkey, const REGSAM samDesired = KEY_READ, const DWORD dwOptions = REG_OPTION_NON_VOLATILE);

	RAII_HANDLE CreateTransaction();

	struct TransactionKey {
		RAII_HKEY key;
		RAII_HANDLE transaction;
	};
	TransactionKey CreateKeyTransacted(const HKEY hk, const std::wstring& subkey = L"", const REGSAM samDesired = KEY_QUERY_VALUE, DWORD dwOptions = REG_OPTION_NON_VOLATILE);

	TransactionKey CreateKeyTransacted(const HKEY hk, const std::string& subkey, const REGSAM samDesired = KEY_QUERY_VALUE, const DWORD dwOptions = REG_OPTION_NON_VOLATILE);

	bool SetValue(const HKEY hk, const std::wstring& valuename, DWORD value);

	bool SetValue(const HKEY hk, const std::string& valuename, DWORD value);

	enum class regtype : DWORD { sz = REG_SZ, expand_sz = REG_EXPAND_SZ, multi_sz = REG_MULTI_SZ, binary = REG_BINARY };
	bool SetValue(const HKEY hk, const std::wstring& valuename, const std::wstring& value, const regtype rt = regtype::sz);
	bool SetValue(const HKEY hk, const std::string& valuename, const std::string& value, const regtype rt = regtype::sz);
	bool SetValue(const HKEY hk, const std::wstring& valuename, const std::string& value, const regtype rt = regtype::sz);
	bool SetValue(const HKEY hk, const std::string& valuename, const std::wstring& value, const regtype rt = regtype::sz);

	bool SetValue(const HKEY hk, const std::wstring& valuename, const std::vector<std::wstring>& values);
	bool SetValue(const HKEY hk, const std::string& valuename, const std::vector<std::string>& values);
	bool SetValue(const HKEY hk, const std::wstring& valuename, const std::vector<std::string>& values);
	bool SetValue(const HKEY hk, const std::string& valuename, const std::vector<std::wstring>& values);


	bool RemoveValue(const HKEY hk, const std::wstring& valuename);
	bool RemoveValue(const HKEY hk, const std::string& valuename);

	bool RemoveKey(const HKEY hk, const std::wstring& valuename = L"", const bool removesubkeys = true);
	bool RemoveKey(const HKEY hk, const std::string& valuename, const bool removesubkeys = true);

	std::vector<std::string> EnumKey(const HKEY hk);

	DWORD QueryType(const HKEY hk, const std::wstring& valuename = L"");
	DWORD QueryType(const HKEY hk, const std::string& valuename);

	std::string QueryString(const HKEY hk, const std::wstring& valuename = L"");
	std::string QueryString(const HKEY hk, const std::string& valuename);

	std::vector<std::string> QueryMultiString(const HKEY hk, const std::wstring& valuename = L"");
	std::vector<std::string> QueryMultiString(const HKEY hk, const std::string& valuename);

	std::string QueryAsString(const HKEY hk, const std::wstring& valuename = L"");
	std::string QueryAsString(const HKEY hk, const std::string& valuename);

	DWORD QueryDWORD(const HKEY hk, const std::wstring& valuename);
	DWORD QueryDWORD(const HKEY hk, const std::string& valuename);

	QWORD QueryQWORD(const HKEY hk, const std::wstring& valuename);
	QWORD QueryQWORD(const HKEY hk, const std::string& valuename);

	// Any hive loaded using RegLoadAppKey is automatically unloaded when all handles to the keys inside the hive are closed using RegCloseKey. --> unclear, do i need to regclose also HKEY? I think yes
	inline RAII_HKEY loadhive(const std::string& filename) {
		HKEY hkey;
		// FIXME: SAM as parameter, alternatives for REG_PROCESS_APPKEY
		const auto ret = ::RegLoadAppKeyA(filename.c_str(), &hkey, KEY_ALL_ACCESS, 0, 0);
		if (ret != ERROR_SUCCESS) {
			throw std::runtime_error("unable to load key");
		}
		RAII_HKEY k(hkey);
		return k;
	}

}
