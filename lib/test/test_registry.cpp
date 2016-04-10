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

// local
#include "settings.hpp"
#include "uuid.hpp"
#include "win_handles.hpp"

// test
#include "catch.hpp"

// local
#include "registry.hpp"

// windows
#include <windows.h>
#include <tchar.h>
#include <Shellapi.h>
#include <WinUser.h>


//std
#include <string>
#include <algorithm>
#include <utility>
#include <regex>
#include <array>

TEST_CASE("TestEnumKey", "[TestReg]") {
	const auto reg = registry::OpenKey(HKEY_LOCAL_MACHINE, "SOFTWARE");

	const auto res = registry::EnumKey(reg.get());

	REQUIRE(!res.empty());
}


TEST_CASE("TestLoadHive", "[TestReg]") {

	auto key = registry::loadhive(test_data_dir + "/hive/test.hive");

//	const auto res = ::RegLoadAppKeyA(test_data_dir + "\\hive2\\test.hive", &hk, KEY_READ, 0, 0);
//	const auto res2 = ::RegLoadKeyA(HKEY_USERS, "testhive", test_data_dir + "\\hive2\\test.hive");

}


/// convert HKEY abbreviations to long name
inline std::wstring get_full_HKEY_name(const std::wstring& path) {
	struct HKEYConv {
		std::wstring shortname;
		std::wstring fullname;
	};
	static const std::array<HKEYConv, 5> HKEYConvs = { {
		{ L"HKLM", L"HKEY_LOCAL_MACHINE" },
		{ L"HKU",  L"HKEY_USERS" },
		{ L"HKCU", L"HKEY_CURRENT_USER" },
		{ L"HKCC", L"HKEY_CURRENT_CONFIG" },
		{ L"HKCR", L"HKEY_CLASSES_ROOT" },
		}
	};

	// FIXME: should do case insensitive comparison
	for (size_t i = 0; i != HKEYConvs.size(); ++i) {
		if (path.compare(0, HKEYConvs[i].shortname.size(), HKEYConvs[i].shortname) == 0 || path.compare(0, HKEYConvs[i].fullname.size(), HKEYConvs[i].fullname) == 0) {
			return HKEYConvs[i].fullname;
		}
	}
	throw std::runtime_error("Invalid hkey");
}

HWND get_regedit_main_handle() {
	HWND regeditMainHwnd = FindWindowW(L"RegEdit_RegEdit", nullptr);
	if (regeditMainHwnd != nullptr) {
		return regeditMainHwnd;
	}

	SHELLEXECUTEINFOW info = {};
	info.cbSize = sizeof info;
	info.fMask = SEE_MASK_NOCLOSEPROCESS;
	info.lpVerb = L"open";
	info.lpFile = L"regedit.exe";
#if !defined(SW_SHOWNORMAL)
#error "!defined(SW_SHOWNORMAL), do not define NOSHOWWINDOW"
#endif
	info.nShow = SW_SHOWNORMAL;
	if (!ShellExecuteExW(&info)) {
		return nullptr;
	}
	WaitForInputIdle(info.hProcess, INFINITE);
	regeditMainHwnd = FindWindowW(L"RegEdit_RegEdit", nullptr);
	return regeditMainHwnd;
}

inline void OpenRegedit(const std::wstring& path){
	const std::wstring FullName = get_full_HKEY_name(path);
	const std::wstring RegPath = L"\\" + FullName + path.substr(path.find(L'\\'));

	// Open RegEdit
	const HWND regeditMainHwnd = get_regedit_main_handle();
	if (regeditMainHwnd == nullptr) {
		return;
	}
	ShowWindow(regeditMainHwnd, SW_SHOW);
	SetForegroundWindow(regeditMainHwnd);

	// Get treeview
	const HWND regeditHwnd = FindWindowExW(regeditMainHwnd, nullptr, L"SysTreeView32", nullptr);
	if (regeditHwnd == nullptr) {
		return;
	}
	SetForegroundWindow(regeditHwnd);
	SetFocus(regeditHwnd);

	DWORD ProcessID = 0;
	GetWindowThreadProcessId(regeditHwnd, &ProcessID);
	RAII_HANDLE hProcess(OpenProcess(SYNCHRONIZE, false, ProcessID));

	#if !defined(WM_KEYDOWN)
	#error "!defined(WM_KEYDOWN), do not define NOWINMESSAGES"
	#endif
	#if !defined(VK_LEFT)
	#error "!defined(VK_LEFT), do not define NOVIRTUALKEYCODES"
	#endif
	// Close it up
	for (int pos = 0; pos != 30; ++pos) {
		SendMessageW(regeditHwnd, WM_KEYDOWN, VK_LEFT, 0);
	}

	WaitForInputIdle(hProcess.get(), INFINITE); // wait for slow displays

	// Open path
	for (const auto& v : RegPath) {
		if (v == L'\\') {
			SendMessageW(regeditHwnd, WM_KEYDOWN, VK_RIGHT, 0);
			WaitForInputIdle(hProcess.get(), INFINITE); // wait for slow displays
		} else {
			SendMessageW(regeditHwnd, WM_CHAR, v, 0);
		}
	}

	// assure that regedit is in foreground
	SetForegroundWindow(regeditMainHwnd);
	SetFocus(regeditMainHwnd);
}

TEST_CASE("OpenRegedit", "[TestReg][hide]") {

	OpenRegedit(L"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList\\");
	HWND regeditMainHwnd = FindWindowW(L"RegEdit_RegEdit", nullptr);
	REQUIRE(regeditMainHwnd != nullptr);
}
