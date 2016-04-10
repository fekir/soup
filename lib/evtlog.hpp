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
#include "registry.hpp"
#include "common.hpp"
#include "IniParser.hpp"
#include "win_handles.hpp"

// windows
#include <Windows.h>
#include <winerror.h>

#include <conio.h>
#include <stdio.h>
#include <winevt.h>
#include <Sddl.h> // ConvertStringSidToSid
#include <Winnt.h>

//std
#include <vector>
#include <string>
#include <stdexcept>
#include <sstream>
#include <iterator>
#include <tuple>
#include <regex>
#include <memory>
#include <type_traits>
#include <typeinfo>
#include <typeindex>      // std::type_index

/*
	https://msdn.microsoft.com/en-us/library/windows/desktop/aa385577(v=vs.85).aspx
	https://en.wikipedia.org/wiki/XPath
	https://technet.microsoft.com/en-us/library/cc734040(v=ws.10).aspx
	kb.prismmicrosys.com/evtpass/evtpages/EventId_866_SoftwareRestrictionPolicy_51844.asp
	https://msdn.microsoft.com/en-us/library/windows/desktop/dd996910(v=vs.85).aspx
	https://msdn.microsoft.com/en-us/library/windows/desktop/aa385771(v=vs.85).aspx
	https://msdn.microsoft.com/en-us/library/windows/desktop/aa385487(v=vs.85).aspx
	https://msdn.microsoft.com/en-us/library/windows/desktop/aa385780(v=vs.85).aspx
	https://msdn.microsoft.com/en-us/library/windows/desktop/aa385771(v=vs.85).aspx
	https://stackoverflow.com/questions/3773786/get-event-viewer-logs-with-win-api-in-c
	https://technet.microsoft.com/en-us/library/cc774995(v=ws.10).aspx

	https://stackoverflow.com/questions/3446955/real-time-update-a-qt-textview
	http://www.qtcentre.org/threads/32416-Update-GUI-from-another-thread
*/
// https://msdn.microsoft.com/en-us/library/windows/desktop/aa385650%28v=vs.85%29.aspx

namespace evtlog{

	// return empty string if unable to get username
	inline std::pair<std::string , std::string> sid_to_username(const std::string& sid_){
		PSID psid={};
		if(ConvertStringSidToSidW(s2ws(sid_).c_str(), &psid)==0){
			return{};
		}
		const size_t MAX_NAME_LENGTH = 256; // FIXME: handle error if name is not long enough(?)
		std::wstring name(MAX_NAME_LENGTH, L'\1');
		DWORD size = name.size();
		std::wstring name2(MAX_NAME_LENGTH, L'\1');
		DWORD size2 = name2.size();
		SID_NAME_USE usename;
		if(LookupAccountSidW(nullptr, psid, &name.at(0), &size, &name2.at(0), &size2, &usename ) == 0){
			return{};
		}
		name.resize(size);
		name2.resize(size2);
		const auto nametoreturn = ws2s(name).c_str();
		const auto nametoreturn2 = ws2s(name2).c_str();
		return{ nametoreturn, nametoreturn2 };
	}

	inline std::string getuser_home(const std::string& ssid){
		//HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\ProfileList\S-1-5-21-13210259-1748602183-1043662369-1001
		// -> ProfileImagePath
		try{
			auto reg = registry::OpenKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList\\" + ssid);
			return registry::QueryString(reg.get(), "ProfileImagePath");
		} catch(const std::runtime_error&){
			return "";
		}
	}
	// gets size for saving XML data, inclusive one or more trailing '\0'
	inline size_t getRequiredSize_XML(const EVT_HANDLE hEvent) {
		DWORD dwBufferUsed = 0;
		if (EvtRender(nullptr, hEvent, EvtRenderEventXml, 0, nullptr, &dwBufferUsed, nullptr)) {
			return 0;
		}
		DWORD status = GetLastError();
		if (ERROR_INSUFFICIENT_BUFFER != status) {
			throw std::runtime_error("unexpected error while querying size for message:" + std::to_string(status));
		}
		return dwBufferUsed / 2;
	}


	inline std::string get_rendered_content(const EVT_HANDLE hEvent)
	{
		DWORD dwBufferUsed = 0;
		std::wstring renderedContent(getRequiredSize_XML(hEvent), L'\0');

		if (!EvtRender(nullptr, hEvent, EvtRenderEventXml, renderedContent.size() * sizeof(wchar_t), &renderedContent.at(0), &dwBufferUsed, nullptr)) {
			const DWORD status = GetLastError(); assert(ERROR_INSUFFICIENT_BUFFER != status);
			throw std::runtime_error("unexpected error while querying message:" + std::to_string(status));
		}
		std::string toreturn = ws2s(renderedContent);
		toreturn.erase(toreturn.find_last_not_of('\0') + 1);
		return toreturn;
	}

	// fixme: make subscription part as static function, and callback with template parameter where to pass static function and handle exception/deregistration(?)
	struct subscription {
		const std::size_t check = typeid(*this).hash_code();
		RAII_EVTHANDLE hSubscription;
	};


	inline DWORD WINAPI SubscriptionCallback(const EVT_SUBSCRIBE_NOTIFY_ACTION action, const PVOID pContext, const EVT_HANDLE hEvent){
		assert(pContext != nullptr);
		const auto sub = reinterpret_cast<subscription*>(pContext);
		assert(sub->check == typeid(subscription).hash_code());
		try {

			switch (action) {
				case EvtSubscribeActionError: {
					/*
					if (ERROR_EVT_QUERY_RESULT_STALE == reinterpret_cast<decltype(ERROR_EVT_QUERY_RESULT_STALE)>(hEvent)){
						// The subscription callback was notified that event records are missing.\n
						// Handle if this is an issue for your application.
					} else {
						// The subscription callback received the following Win32 error: %lu\n", reinterpret_cast<decltype(ERROR_EVT_QUERY_RESULT_STALE)>(hEvent));
					}
					*/
					break;
				}
				case EvtSubscribeActionDeliver: {
					const auto var = get_rendered_content(hEvent);
					break;
				}
				default: {
					assert(false && "unknown action");
					return ERROR_INVALID_PARAMETER;
				}
			}
			return ERROR_SUCCESS;
		}
		catch (const std::bad_alloc& err) {
			sub->hSubscription.release();
			return ERROR_OUTOFMEMORY;
		}
		catch (const std::runtime_error& err) {
			sub->hSubscription.release();
			return ERROR_INVALID_DATA; // fixme
		}
		catch (...) {
			sub->hSubscription.release();
			return ERROR_INVALID_DATA; // fixme
		}
	}

	static_assert(std::is_same<decltype(&SubscriptionCallback), EVT_SUBSCRIBE_CALLBACK>::value, "not same type");



	inline void test()
	{
		DWORD status = ERROR_SUCCESS; (void) status;
		const auto pwsPath = L"Application";// L"<channel name goes here>";
		const auto pwsQuery = L"*[System/EventID=866]";//  nullptr;// L"<xpath query goes here>";

		// Subscribe to events beginning with the oldest event in the channel. The subscription
		// will return all current events in the channel and any future events that are raised
		// while the application is active.
		subscription sub;
		sub.hSubscription.reset(EvtSubscribe(nullptr, nullptr, pwsPath, pwsQuery, nullptr, &sub, SubscriptionCallback, EvtSubscribeStartAtOldestRecord));

		//RAII_EVTHANDLE hSubscription( EvtSubscribe(nullptr, nullptr, pwsPath, pwsQuery, nullptr, nullptr, SubscriptionCallback, EvtSubscribeStartAtOldestRecord));
		if (!sub.hSubscription)
		{
			status = GetLastError();
/*
			if (ERROR_EVT_CHANNEL_NOT_FOUND == status)
				wprintf(L"Channel %s was not found.\n", pwsPath);
			else if (ERROR_EVT_INVALID_QUERY == status)
				// You can call EvtGetExtendedStatus to get information as to why the query is not valid.
				wprintf(L"The query \"%s\" is not valid.\n", pwsQuery == nullptr ? L"" : pwsQuery);
			else
				wprintf(L"EvtSubscribe failed with %lu.\n", status);
				*/
			return;
		}

		wprintf(L"Hit any key to quit\n\n");
		Sleep(3);
		sub.hSubscription.reset();/*
		while (!_kbhit()) {
			Sleep(10);
		}/**/
	}


}
