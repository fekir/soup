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
#include "unique_handle.hpp"

// windows
#include <Windows.h>
#include <Winuser.h>
#include <Userenv.h> // UnloadUserProfile
#include <winevt.h>  // event

// cstd
#include <cassert>

namespace details{
	struct sKeyDeleter {
		void operator()(const HKEY& hkey) const noexcept {
			const auto res = ::RegCloseKey(hkey); (void)res; assert(res == ERROR_SUCCESS);
		}
	};

	struct sKeyUnloader {
		std::wstring key;
		void operator()(const HKEY& hkey) const noexcept {
			const auto res = ::RegUnLoadKeyW(hkey, key.c_str()); (void)res; assert(res == ERROR_SUCCESS);
		}
	};

	struct sCloseHandle{
		void operator()(const HANDLE& h) const noexcept {
			const auto res = ::CloseHandle(h); (void)res; assert(res != 0);
		}
	};

	struct sFreeLibrary{
		void operator()(const HMODULE& h) const noexcept {
			const auto res = ::FreeLibrary(h); (void)res; assert(res != 0);
		}
	};
#ifndef NOUSER
	struct sCloseDesktop{
		void operator()(const HDESK& h) const noexcept {
			const auto res = ::CloseDesktop(h); (void)res; assert(res != 0);
		}
	};
	struct sCloseWindowStation{
		void operator()(const HWINSTA& h) const noexcept {
			const auto res = ::CloseWindowStation(h); (void)res; assert(res != 0);
		}
	};

	struct sDestroyIcon {
		void operator()(const HICON & h) const noexcept {
			const auto res = ::DestroyIcon(h); (void)res; assert(res != 0);
		}
	} ;
#endif

	struct TokenAndProfile {
		HANDLE      Token;
		PROFILEINFOW Profile;
	};
	struct sUnloadUserProfile {
		void operator()(const TokenAndProfile& h) const noexcept {
			const auto res = ::UnloadUserProfile(h.Token, h.Profile.hProfile); (void)res; assert(res == TRUE);
		}
	};

#if (WINVER >= _WIN32_WINNT_VISTA)
	struct sCloseEvent {
		void operator()(const EVT_HANDLE& h) const noexcept {
			const auto res = ::EvtClose(h); (void)res; assert(res == TRUE);
		}
	};
#endif

}


using RAII_HANDLE      = unique_handle2<HANDLE,     details::sCloseHandle>;
using RAII_HINSTANCE   = unique_handle2<HINSTANCE,  details::sFreeLibrary>;
using RAII_HKEY        = unique_handle2<HKEY,       details::sKeyDeleter>;
using RAII_LOADHKEY    = unique_handle2<HKEY,       details::sKeyUnloader>;
using RAII_HMODULE     = unique_handle2<HMODULE,    details::sFreeLibrary>;

#if (WINVER >= _WIN32_WINNT_VISTA)
using RAII_EVTHANDLE   = unique_handle1<EVT_HANDLE, details::sCloseEvent, nullptr>;
#endif

#ifndef NOUSER
using RAII_HDESKTOP    = unique_handle2<HDESK,      details::sCloseDesktop>;
using RAII_HWINSTA     = unique_handle2<HWINSTA,    details::sCloseWindowStation>;
using RAII_HICON       = unique_handle2<HICON,      details::sDestroyIcon>;
using RAII_UserProfile = unique_handle2<details::TokenAndProfile, details::sDestroyIcon>;
#endif


