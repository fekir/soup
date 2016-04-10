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
#include "common.hpp"

// windows
#include <Windows.h>
#include <Objbase.h> // Objbase.h

//std
#include <string>
#include <stdexcept>
#include <cassert>

namespace uid {

	// UUID and GUID are the same thing
	inline UUID createUUID() {
		UUID uid;
		const auto res2 = UuidCreate(&uid);
		if (res2 == RPC_S_OK || res2 == RPC_S_UUID_LOCAL_ONLY) {
			return uid;
		}
		throw std::runtime_error("unable to create UUID");
	}

	inline std::string to_string(const UUID& uid) {
		constexpr int maxsize = 60;
		OLECHAR szGuid[maxsize + 1] = { 0 };
		const auto nCount = ::StringFromGUID2(uid, szGuid, maxsize);
		if (nCount <= 0 || nCount > maxsize) {
			throw std::runtime_error("unable to convert UUID to string");
		}
		const std::wstring res(szGuid, static_cast<size_t>(nCount-1)); // -1 since it is the '\0'
		return ws2s(res);
	}

	class MyUUID{
		std::string uuid;
	public:
		MyUUID() : uuid(to_string(createUUID())){}
		explicit MyUUID(const UUID& uuid) : uuid(to_string(uuid)){}
		explicit MyUUID(const std::string& uuid) : uuid(uuid){} // FIXME: validate uuid param
		const std::string& str() const{
			return uuid;
		}
		friend bool operator==(const MyUUID& l, const MyUUID& r){
			return l.uuid == r.uuid;
		}
	};

}
