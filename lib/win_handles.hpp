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
#include <Winuser.h>
#include <Userenv.h> // UnloadUserProfile
#include <winevt.h>  // event

// cstd
#include <cassert>

// std
#include <memory>
#include <string>

namespace details{
	struct sKeyDeleter {
		typedef HKEY pointer;
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
		typedef HANDLE pointer;
		void operator()(const HANDLE& h) const noexcept {
			const auto res = ::CloseHandle(h); (void)res; assert(res != 0);
		}
	};

	struct sFreeLibrary{
		typedef HMODULE pointer;
		void operator()(const HMODULE& h) const noexcept {
			const auto res = ::FreeLibrary(h); (void)res; assert(res != 0);
		}
	};
#ifndef NOUSER
	struct sCloseDesktop{
		typedef HDESK pointer;
		void operator()(const HDESK& h) const noexcept {
			const auto res = ::CloseDesktop(h); (void)res; assert(res != 0);
		}
	};
	struct sCloseWindowStation{
		typedef HWINSTA pointer;
		void operator()(const HWINSTA& h) const noexcept {
			const auto res = ::CloseWindowStation(h); (void)res; assert(res != 0);
		}
	};

	struct sDestroyIcon {
		typedef HICON pointer;
		void operator()(const HICON& h) const noexcept {
			const auto res = ::DestroyIcon(h); (void)res; assert(res != 0);
		}
	} ;
#endif

	struct TokenAndProfile {
		HANDLE      Token;
		PROFILEINFOW Profile;
	};
	struct sUnloadUserProfile {
		typedef TokenAndProfile pointer;
		void operator()(const TokenAndProfile& h) const noexcept {
			const auto res = ::UnloadUserProfile(h.Token, h.Profile.hProfile); (void)res; assert(res == TRUE);
		}
	};

#if (WINVER >= _WIN32_WINNT_VISTA)
	struct sCloseEvent {
		typedef EVT_HANDLE pointer;
		void operator()(const EVT_HANDLE& h) const noexcept {
			const auto res = ::EvtClose(h); (void)res; assert(res == TRUE);
		}
	};
#endif

}

using RAII_HANDLE      = std::unique_ptr<HANDLE,     details::sCloseHandle>;


using RAII_HINSTANCE   = std::unique_ptr<HINSTANCE,  details::sFreeLibrary>;
using RAII_HKEY        = std::unique_ptr<HKEY,       details::sKeyDeleter>;
using RAII_LOADHKEY    = std::unique_ptr<HKEY,       details::sKeyUnloader>;
using RAII_HMODULE     = std::unique_ptr<HMODULE,    details::sFreeLibrary>;

#if (WINVER >= _WIN32_WINNT_VISTA)
using RAII_EVTHANDLE   = std::unique_ptr<EVT_HANDLE, details::sCloseEvent>;
#endif

#ifndef NOUSER
using RAII_HDESKTOP    = std::unique_ptr<HDESK,      details::sCloseDesktop>;
using RAII_HWINSTA     = std::unique_ptr<HWINSTA,    details::sCloseWindowStation>;
using RAII_HICON       = std::unique_ptr<HICON,      details::sDestroyIcon>;
using RAII_UserProfile = std::unique_ptr<details::TokenAndProfile, details::sDestroyIcon>;
#endif


