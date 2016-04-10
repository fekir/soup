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

#include "registry.hpp"

#include "win_handles.hpp"

// local
#include "common.hpp"

// windows
#include <winreg.h>
#include <KtmW32.h> // >= windows vista, CreateTransaction
#include <WinDNS.h> // qword

// std
#include <string>
#include <cassert>
#include <algorithm>

#ifdef DONOTSAFEREGKEY
constexpr auto flag_volatile = REG_OPTION_VOLATILE;
#else
constexpr auto flag_volatile = REG_OPTION_NON_VOLATILE;
#endif

namespace registry {

	// return false if both KEY_WOW64_32KEY and KEY_WOW64_64KEY are set
	constexpr bool checkWOWflags(const DWORD dwOptions) {
		return !((dwOptions & KEY_WOW64_32KEY) && (dwOptions & KEY_WOW64_64KEY));
	}

	RAII_HKEY OpenKey(const HKEY hk, const std::wstring& subkey, const REGSAM samDesired, DWORD dwOptions) {
		assert(!subkey.empty() && subkey.at(0) != '\\' && "stupid error, path will be invalid");
		assert(checkWOWflags(dwOptions) && "does not make any sense");
		HKEY hkey;
		const auto res = RegOpenKeyExW(hk, subkey.c_str(), dwOptions | flag_volatile, samDesired, &hkey);
		if (res != ERROR_SUCCESS) {
			throw std::runtime_error("unable to open key");
		}
		RAII_HKEY k(hkey);
		return k;
	}

	RAII_HKEY OpenKey(const HKEY hk, const std::string& subkey, const REGSAM samDesired, const DWORD dwOptions) {
		return OpenKey(hk, s2ws(subkey), samDesired, dwOptions);
	}

	RAII_HKEY OpenKeyOptional(const HKEY hk, const std::wstring& subkey, const REGSAM samDesired, DWORD dwOptions) {
		assert(!subkey.empty() && subkey.at(0) != '\\' && "stupid error, path will be invalid");
		assert(checkWOWflags(dwOptions) && "does not make any sense");
		HKEY hkey;
		const auto res = RegOpenKeyExW(hk, subkey.c_str(), dwOptions | flag_volatile, samDesired, &hkey);
		if(res == ERROR_FILE_NOT_FOUND || res == ERROR_PATH_NOT_FOUND){
			RAII_HKEY k;
			return k;
		}
		if (res != ERROR_SUCCESS) {
			throw std::runtime_error("unable to open key");
		}
		RAII_HKEY k(hkey);
		return k;
	}

	RAII_HKEY OpenKeyOptional(const HKEY hk, const std::string& subkey, const REGSAM samDesired, const DWORD dwOptions) {
		return OpenKeyOptional(hk, s2ws(subkey), samDesired, dwOptions);
	}

	RAII_HKEY CreateKey(const HKEY hk, const std::wstring& subkey, const REGSAM samDesired, DWORD dwOptions) {
		assert(!subkey.empty() && subkey.at(0) != '\\' && "stupid error, path will be invalid");
		assert(checkWOWflags(dwOptions) && "does not make any sense");
		HKEY hkey;
		const auto res = RegCreateKeyExW(hk, subkey.c_str(), 0, nullptr, dwOptions | flag_volatile, samDesired, nullptr, &hkey, nullptr);
		if (res != ERROR_SUCCESS) {
			throw std::runtime_error("unable to create key");
		}
		RAII_HKEY k(hkey);
		return k;
	}

	RAII_HKEY CreateKey(const HKEY hk, const std::string& subkey, const REGSAM samDesired, const DWORD dwOptions) {
		return CreateKey(hk, s2ws(subkey), samDesired, dwOptions);
	}

	RAII_HANDLE CreateTransaction() {
		const HANDLE handle = ::CreateTransaction(nullptr, nullptr, 0, 0, 0, 0, nullptr);
		if (handle == nullptr || handle == INVALID_HANDLE_VALUE) {
			throw std::runtime_error("unable to create transaction handle");
		}
		return RAII_HANDLE(handle);
	}

	TransactionKey CreateKeyTransacted(const HKEY hk, const std::wstring& subkey, const REGSAM samDesired, DWORD dwOptions) {
		assert(!subkey.empty() && subkey.at(0) != L'\\' && "stupid error, path will be invalid");
		assert(checkWOWflags(dwOptions) && "does not make any sense");
		auto transhadle(CreateTransaction());
		HKEY hkey;
		const auto res = RegCreateKeyTransactedW(hk, subkey.c_str(), 0, nullptr, dwOptions | flag_volatile, samDesired,
												 nullptr, &hkey, nullptr, transhadle.get(), nullptr);
		if (res != ERROR_SUCCESS) {
			throw std::runtime_error("unable to open key");
		}
		RAII_HKEY k(hkey);
		return TransactionKey{ std::move(k), std::move(transhadle) };
	}

	TransactionKey CreateKeyTransacted(const HKEY hk, const std::string& subkey, const REGSAM samDesired, const DWORD dwOptions) {
		return CreateKeyTransacted(hk, s2ws(subkey), samDesired, dwOptions);
	}

	bool SetValue(const HKEY hk, const std::wstring& valuename, DWORD value) {
		const auto res = RegSetValueExW(hk, valuename.c_str(), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&value), sizeof(value));
		if (res != ERROR_SUCCESS) {
			return false;
		}
		return true;
	}

	bool SetValue(const HKEY hk, const std::string& valuename, DWORD value) {
		return SetValue(hk, s2ws(valuename), value);
	}

	bool SetValue(const HKEY hk, const std::wstring& valuename, const std::wstring& value, const regtype rt) {
		if (value.length() > ((std::numeric_limits<DWORD>::max)() - 1) / sizeof(wchar_t)) {
			throw std::runtime_error("value to save in the registry is too long");
		}
		// cbData must include the size of the terminating null
		const auto res = RegSetValueExW(hk, valuename.c_str(), 0, static_cast<DWORD>(rt), reinterpret_cast<const BYTE*>(value.c_str()), static_cast<DWORD>((value.size() + 1)*sizeof(wchar_t)));
		if (res != ERROR_SUCCESS) {
			return false;
		}
		return true;
	}

	bool SetValue(const HKEY hk, const std::string& valuename, const std::string& value, const regtype rt) {
		return SetValue(hk, s2ws(valuename), s2ws(value), rt);
	}

	bool SetValue(const HKEY hk, const std::wstring& valuename, const std::string& value, const regtype rt) {
		return SetValue(hk, valuename, s2ws(value), rt);
	}

	bool SetValue(const HKEY hk, const std::string& valuename, const std::wstring& value, const regtype rt) {
		return SetValue(hk, s2ws(valuename), value, rt);
	}

	bool SetValue(const HKEY hk, const std::wstring& valuename, const std::vector<std::wstring>& values) {
		const auto value_multi_sz = to_multi_string(values);
		return SetValue(hk, valuename, value_multi_sz, regtype::multi_sz);
	}

	bool SetValue(const HKEY hk, const std::string& valuename, const std::vector<std::string>& values) {
		const auto value_multi_sz = to_multi_string(values);
		return SetValue(hk, s2ws(valuename), s2ws(value_multi_sz), regtype::multi_sz);
	}

	bool SetValue(const HKEY hk, const std::wstring& key, const std::vector<std::string>& values) {
		const auto value_multi_sz = to_multi_string(values);
		return SetValue(hk, key, s2ws(value_multi_sz), regtype::multi_sz);
	}

	bool SetValue(const HKEY hk, const std::string& valuename, const std::vector<std::wstring>& values) {
		return SetValue(hk, s2ws(valuename), values);
	}


	bool RemoveValue(const HKEY hk, const std::wstring& valuename) {
		const auto res = RegDeleteValueW(hk, valuename.c_str());
		if (res != ERROR_SUCCESS) {
			return false;
		}
		return true;
	}

	bool RemoveValue(const HKEY hk, const std::string& valuename) {
		return RemoveValue(hk, s2ws(valuename));
	}

	bool RemoveKey(const HKEY hk, const std::wstring& valuename, const bool removesubkeys) {
		if(removesubkeys){
#if WINVER < _WIN32_WINNT_VISTA
#warning "need to link to Shlwapi.lib"
			const auto res = SHDeleteKey (hk, valuename.c_str());
#else
			const auto res = RegDeleteTreeW(hk, valuename.c_str());
#endif
			if (res != ERROR_SUCCESS) {
				return false;
			}
			return true;
		}
		const auto res = RegDeleteKeyW(hk, valuename.c_str());
		if (res != ERROR_SUCCESS) {
			return false;
		}
		return true;
	}

	bool RemoveKey(const HKEY hk, const std::string& valuename, const bool removesubkeys) {
		return RemoveKey(hk, s2ws(valuename), removesubkeys);
	}

	// ERROR_ACCESS_DENIED --> open with KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE
	std::vector<std::string> EnumKey(const HKEY hk) {
		DWORD cSubKeys = 0;
		DWORD cbMaxSubKey = 0;
		// Get the class name and the value count.
		auto retCode = RegQueryInfoKeyW(hk, nullptr, nullptr, nullptr, &cSubKeys, &cbMaxSubKey, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
		if (retCode != ERROR_SUCCESS) {
			throw std::runtime_error("unable to RegQueryInfoKeyW key");
		}
		if (cbMaxSubKey >= ((std::numeric_limits<DWORD>::max)() - 1)) {
			throw std::runtime_error("value saved in the registry is too long");
		}
		std::vector<std::string> toreturn;
		if (cSubKeys == 0) {
			return toreturn;
		}
		cbMaxSubKey++;
		// # of keys may have changed, still it could be a good approximation, and in most cases it wont change --> also cbMaxSubKey may be wrong, still it may be a good approximation
		toreturn.reserve(cSubKeys);

		for (DWORD i = 0; ;  ++i ) {
			std::wstring buffer(cbMaxSubKey, '\0');
			DWORD size = cbMaxSubKey;
			retCode = RegEnumKeyExW(hk, i, &buffer[0], &size, nullptr, nullptr, nullptr, nullptr);
			while (retCode == ERROR_MORE_DATA) { // timing issue, cbMaxSubKey may not be accurate
				if (buffer.size() >= (std::numeric_limits<DWORD>::max)() / 2) { // cannot safely double the size
					buffer.resize((std::numeric_limits<DWORD>::max)());
				} else if (buffer.size() < (std::numeric_limits<DWORD>::max)() / 2) { // double the size
					buffer.resize(2* buffer.size());
				} else { // to big to resize..
					throw std::runtime_error("error during RegEnumKeyExW");
				}
				size = static_cast<DWORD>(buffer.size()); // no conversion loss, checked when resizing buffer
				retCode = RegEnumKeyExW(hk, i, &buffer[0], &size, nullptr, nullptr, nullptr, nullptr);
			}
			if (retCode == ERROR_NO_MORE_ITEMS) {
				break;
			}
			if (retCode != ERROR_SUCCESS) {
				throw std::runtime_error("error during RegEnumKeyExW");
			}
			if (size != 0) { // may happen if some key is changed during enumeration(? -> check), it doesn't make any sense to list those
				buffer.resize(size);
				toreturn.emplace_back(ws2s(buffer));
			}
		}
		return toreturn;
	}

	DWORD QueryType(const HKEY hk, const std::wstring& valuename) {
		DWORD type = 0;
		auto res = RegQueryValueExW(hk, valuename.c_str(), nullptr, &type, nullptr, nullptr);
		if (res != ERROR_SUCCESS) {
			throw std::runtime_error("error while querying type");
		}
		return type;
	}

	DWORD QueryType(const HKEY hk, const std::string& valuename) {
		return QueryType(hk, s2ws(valuename));

	}

	std::string QueryString(const HKEY hk, const std::wstring& valuename) {
		DWORD type = REG_SZ; //  or REG_EXPAND_SZ
		DWORD size = 0;
		auto res = RegQueryValueExW(hk, valuename.c_str(), nullptr, &type, nullptr, &size);
		if (res != ERROR_SUCCESS || (type != REG_SZ && type != REG_EXPAND_SZ)) {
			throw std::runtime_error("error while querying value");
		}
		std::wstring buffer(size/sizeof(wchar_t), '\0');
		res = RegQueryValueExW(hk, valuename.c_str(), nullptr, &type, reinterpret_cast<LPBYTE>(&buffer.at(0)), &size);
		if (res != ERROR_SUCCESS) {
			throw std::runtime_error("error while querying value");
		}
		assert(buffer.at(buffer.size() - 1) == '\0');
		buffer.resize(buffer.size() - 1);
		return ws2s(buffer);
	}

	std::string QueryString(const HKEY hk, const std::string& valuename) {
		return QueryString(hk, s2ws(valuename));
	}

	std::vector<std::string> QueryMultiString(const HKEY hk, const std::wstring& valuename) {
		DWORD type = REG_SZ; //  or REG_EXPAND_SZ
		DWORD size = 0;
		auto res = RegQueryValueExW(hk, valuename.c_str(), nullptr, &type, nullptr, &size);
		if (res != ERROR_SUCCESS || (type != REG_MULTI_SZ)) {
			//throw std::runtime_error("error while querying value");
		}
		std::wstring buffer(size / sizeof(wchar_t), '\0');
		res = RegQueryValueExW(hk, valuename.c_str(), nullptr, &type, reinterpret_cast<LPBYTE>(&buffer.at(0)), &size);
		if (res != ERROR_SUCCESS) {
			throw std::runtime_error("error while querying value");
		}
		assert(buffer.at(buffer.size() - 1) == '\0');
		buffer.resize(buffer.size() - 1);
		return explode(ws2s(buffer), '\0');
	}

	std::vector<std::string> QueryMultiString(const HKEY hk, const std::string& valuename) {
		return QueryMultiString(hk, s2ws(valuename));
	}

	std::string QueryAsString(const HKEY hk, const std::wstring& valuename) {
		const auto type = QueryType(hk, valuename);
		switch (type) {
		case REG_DWORD:
			return std::to_string(QueryDWORD(hk, valuename));
		case REG_QWORD:
			return std::to_string(QueryQWORD(hk, valuename));
		case REG_SZ:
		case REG_EXPAND_SZ:
			return QueryString(hk, valuename);
		case REG_MULTI_SZ:
			return flatten(QueryMultiString(hk, valuename), '\0');
		case REG_BINARY:
			//like querystring, but querystring checks type
		default:
			assert(false && "unexpected type to query");
			throw std::runtime_error("unexpected type to query: " + std::to_string(type));
		}
	}
	std::string QueryAsString(const HKEY hk, const std::string& valuename) {
		return QueryAsString(hk, s2ws(valuename));
	}

	QWORD QueryQWORD(const HKEY hk, const std::wstring& valuename) {
		QWORD buffer;
		DWORD type = REG_QWORD;
		DWORD size = sizeof(buffer);
		const auto res = RegQueryValueExW(hk, valuename.c_str(), nullptr, &type, reinterpret_cast<LPBYTE>(&buffer), &size);
		if (res != ERROR_SUCCESS || (type != REG_DWORD && type != REG_QWORD)) {
			throw std::runtime_error("error while querying value");
		}
		return buffer;
	}

	QWORD QueryQWORD(const HKEY hk, const std::string& valuename) {
		return QueryQWORD(hk, s2ws(valuename));
	}

	DWORD QueryDWORD(const HKEY hk, const std::wstring& valuename) {
		DWORD buffer;
		DWORD type = REG_QWORD;
		DWORD size = sizeof(buffer);
		const auto res = RegQueryValueExW(hk, valuename.c_str(), nullptr, &type, reinterpret_cast<LPBYTE>(&buffer), &size);
		if (res != ERROR_SUCCESS || type != REG_DWORD) {
			throw std::runtime_error("error while querying value");
		}
		return buffer;
	}

	DWORD QueryDWORD(const HKEY hk, const std::string& valuename) {
		return QueryDWORD(hk, s2ws(valuename));
	}

}
