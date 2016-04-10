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

#include "policy.hpp"

#ifdef MSVC
#pragma execution_character_set("utf-8")
#endif

// local
#include "common.hpp"
#include "registry.hpp"
#include "uuid.hpp"
#include "win_handles.hpp"

// windows
#include <Windows.h>

//std
#include <vector>
#include <string>
#include <stdexcept>
#include <sstream>

// FIXME: vedere se \\live.sysinternals.com\DavWWWRoot\Tools è eseguibuile quando disabilito tutti i drive! vedi: https://en.wikipedia.org/wiki/Path_%28computing%29#Representations_of_paths_by_operating_system_and_shell
namespace policy{

	// compared to the default values:
	// - removed .lst (links in start menu, taskbar and desktop will work)
	// - added .diagcab, .vsix
	// - what about .jar, .sh, .py, ps1 and other scripting languages?
	// cryptolocker in office macro exists!
	// http://pcsupport.about.com/od/tipstricks/a/execfileext.htm
	std::vector<std::string> ExecutableExtensions() {
		return{
			"ade", "adp", "bas", "bat", "chm", "cmd", "com", "cpl", "crt", "diagcab",
			"exe", "hlp", "hta", "inf", "ins", "isp", "mdb", "mde", "msc", "msi",
			"msp", "mst", "ocx", "pcd", "pif", "reg", "scr", "shs", "url", "vb", "vsix"
			"wsc"
			// test/check
			, "application", "gadget", "jar", "vbs", "vbe", "js", "jse", "ws", "wsf"
			, "wsc", "wsh", "ps1", "ps1xml", "ps2" , "ps2xml", "psc1", "psc2"
			, "msh", "msh1", "msh2", "mshxml", "msh1xml", "msh2xml", "inf", "scf", "rgs"
		};
	}

	std::vector<std::string> CommonExtensions() {
		return{
			// Documents
			"doc?", "odt", "rtf", "pdf", "txt", "?htm?", "epub", "mobi", "ppt?", "odp", "xls?",
			// Images
			"jp?g", "png", "bmp",
			// Audio/Video
			"mp?", "flac", "wav", "avi", "mk?", "divx",
			// Archives
			"zip", "7z", "rar",
		};
	}

	std::vector<std::string> SecureLocations() {
		return{
			"%ProgramFiles%",
			"%ProgramFiles(x86)%", // check what it does on 32 bit systems
			"%SystemRoot%", // normally c:/windows
			// described here: https://technet.microsoft.com/en-us/library/cc785321%28v=ws.10%29.aspx
			"%HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\SystemRoot%",
			"%HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\ProgramFilesDir%"
		};
	}

	std::vector<std::string> UnsecureLocations() {
		return{
			// Only current user. You can execute files as administrator for example
			"%UserProfile%", "%AppData%", "%LocalAppData%", "%TMP%", "%TEMP%",
			// should work for all users
			"*:\\$Recycle.Bin", "%Public%", "%AllUsersProfile%"
		};
	}

	const std::wstring CodeIdentifiers(L"SOFTWARE\\Policies\\Microsoft\\Windows\\Safer\\CodeIdentifiers\\");

	std::vector<bidi> getBidi() {
		return{ // windows compiler does complain if saved inside char
			{ u8"\u200E", "U+200E LEFT-TO-RIGHT MARK" }
			,{ u8"\u200F", "U+200F RIGHT-TO-LEFT MARK" }

			,{ u8"\u202A", "U+202A LEFT-TO-RIGHT EMBEDDING" }
			,{ u8"\u202B", "U+202B RIGHT-TO-LEFT EMBEDDING" }

			,{ u8"\u202D", "U+202D LEFT-TO-RIGHT OVERRIDE" }
			,{ u8"\u202E", "U+202E RIGHT-TO-LEFT OVERRIDE" }

			,{ u8"\u2066", "U+2066 LEFT-TO-RIGHT ISOLATE" }
			,{ u8"\u2067", "U+2067 RIGHT-TO-LEFT ISOLATE" }
			,{ u8"\u2068", "U+2068 FIRST STRONG ISOLATE" }

			,{ u8"\u061C", "U+061C ARABIC LETTER MARK" }

			,{ u8"\u202C", "U+202C POP DIRECTIONAL FORMATTING" }
		};
	}

	PolicyManager::PolicyManager() : hkeyCodeIdentifiers(registry::CreateKeyTransacted(HKEY_LOCAL_MACHINE, CodeIdentifiers, KEY_WRITE)) {}
	PolicyManager::PolicyManager(DWORD flags) : hkeyCodeIdentifiers(registry::CreateKeyTransacted(HKEY_LOCAL_MACHINE, CodeIdentifiers, flags)) {}

	void PolicyManager::setEnforcementLevel(const enforcementLevel e) const {
		registry::SetValue(hkeyCodeIdentifiers.key.get(), L"TransparentEnabled", static_cast<DWORD>(e));
	}

	void PolicyManager::setPolicyScope(const policyScope s) const {
		registry::SetValue(hkeyCodeIdentifiers.key.get(), L"PolicyScope", static_cast<DWORD>(s));
	}

	void PolicyManager::setSecurityLevel(const securitylevel l) const {
		registry::SetValue(hkeyCodeIdentifiers.key.get(), L"DefaultLevel", static_cast<DWORD>(l));
	}

	void PolicyManager::setExecutableTypes(const std::vector<std::string>& exec_ext) const {
		registry::SetValue(hkeyCodeIdentifiers.key.get(), L"ExecutableTypes", exec_ext);
	}

	std::vector<std::string> PolicyManager::getExecutableTypes() const {
		return registry::QueryMultiString(hkeyCodeIdentifiers.key.get(), L"ExecutableTypes");
	}

	// WARNING: not transactional! - I should create a handle to an upper level
	void PolicyManager::setAdminInfoUrl(const std::string& link) const {
		const auto hkeyAdminInfo(registry::CreateKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Policies\\Microsoft\\Windows\\Explorer", KEY_WRITE));
		registry::SetValue(hkeyAdminInfo.get(), L"AdminInfoUrl", link);
	}


	enforcementLevel PolicyManager::getEnforcementLevel() const {
		return to_enforcementLevel(registry::QueryDWORD(hkeyCodeIdentifiers.key.get(), L"TransparentEnabled"));
	}

	policyScope PolicyManager::getPolicyScope() const {
		return to_policyScope(registry::QueryDWORD(hkeyCodeIdentifiers.key.get(), L"PolicyScope"));
	}

	securitylevel PolicyManager::getSecurityLevel() const {
		return to_securitylevel(registry::QueryDWORD(hkeyCodeIdentifiers.key.get(), L"DefaultLevel"));
	}

	std::string PolicyManager::getAdminInfoUrl() const {
		const auto hkeyAdminInfo(registry::CreateKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Policies\\Microsoft\\Windows\\Explorer", KEY_READ));
		try {
			return registry::QueryString(hkeyAdminInfo.get(), L"AdminInfoUrl");
		} catch (const std::runtime_error& err) { // FIXME: create appropriate exception type
			return "";
		}
	}

	bool PolicyManager::SetPolicy(const policy_rule& p, const securitylevel sec, const std::string& uuid) {
		const std::string seclevel = std::to_string(static_cast<int>(sec));
		const auto key(registry::CreateKey(hkeyCodeIdentifiers.key.get(), seclevel + "\\Paths\\" + uuid, KEY_WRITE));
		registry::SetValue(key.get(), L"Description", p.Description);
		registry::SetValue(key.get(), L"SaferFlags", 0);
		registry::SetValue(key.get(), L"Name", p.name);
		registry::SetValue(key.get(), L"ItemData", p.ItemData, p.ItemDataType);

		return true;
	}
	bool PolicyManager::SetPolicy(const policy_rule& p, const securitylevel sec, const UUID& uuid) {
		const std::string guid = uid::to_string(uuid);
		return SetPolicy(p, sec, guid);
	}
	bool PolicyManager::SetPolicy(const policy_rule& p, const securitylevel sec) {
		return SetPolicy(p, sec, uid::createUUID());
	}

	bool PolicyManager::RemovePolicy(const securitylevel sec, const std::string& UUID) {
		const std::string seclevel = std::to_string(static_cast<int>(sec));
		const auto key(registry::OpenKeyOptional(hkeyCodeIdentifiers.key.get(), seclevel + "\\Paths\\", KEY_READ | KEY_WRITE));
		if(key){
			return registry::RemoveKey(key.get(), UUID);
		}
		return false;
	}

	bool PolicyManager::SetPolicyDisableDoubleExt(const std::vector<std::string>& doubleext) {

		setEnforcementLevel(enforcementLevel::SkipDLLs);

		setPolicyScope(policyScope::AllUsers);

		setSecurityLevel(securitylevel::Unrestricted);

		const std::string description("WinSec, disable double extensions");
		for (const auto& v : doubleext) {
			policy_rule p;
			p.Description = description;
			p.ItemData = v;
			SetPolicy(p, securitylevel::Disallowed);
		}
		return true;
	}

	bool PolicyManager::SetPolicyDisableInsecureLocations(const std::vector<std::string>& locations, const std::vector<std::string>& exec_ext) {

		setEnforcementLevel(enforcementLevel::SkipDLLs);

		setPolicyScope(policyScope::AllUsers);

		setSecurityLevel(securitylevel::Unrestricted);

		setExecutableTypes(exec_ext);

		const std::string description("WinSec, disable insecure locations");
		for (const auto& v : locations) {
			policy_rule p;
			p.name = "DisableInsecureLocations";
			p.Description = description;
			p.ItemData = v;
			SetPolicy(p, securitylevel::Disallowed);
		}

		return true;
	}

	bool PolicyManager::SetPolicyEnableOnlySecureLocations(const std::vector<std::string>& locations, const std::vector<std::string>& exec_ext) {

		setEnforcementLevel(enforcementLevel::SkipDLLs);

		setPolicyScope(policyScope::AllUsers);

		setSecurityLevel(securitylevel::Unrestricted);

		setExecutableTypes(exec_ext);

		const std::string description("WinSec, enable only secure locations");
		for (const auto& v : locations) {
			policy_rule p;
			p.name = "EnableOnlySecureLocations";
			p.Description = description;
			p.ItemData = v;
			SetPolicy(p, securitylevel::Unrestricted);
		}

		// we have registered the secure location, let set everything as insecure
		policy_rule p;
		p.name = "EnableOnlySecureLocations";
		p.Description = description;
		p.ItemData = "*:";
		SetPolicy(p, securitylevel::Disallowed);

		return true;
	}

	bool PolicyManager::SetPolicyDisableBiDiFiles() {

		setEnforcementLevel(enforcementLevel::SkipDLLs);

		setPolicyScope(policyScope::AllUsers);

		setSecurityLevel(securitylevel::Unrestricted);

		const std::string any("*");
		const std::string description("WinSec, disable BiDi files");
		const auto bidis = getBidi();
		for (const auto& v : bidis) {
			policy_rule p;
			p.name = "DisableBiDiFiles";
			p.Description = description + v.desc;
			p.ItemData = any + v.c + any;
			SetPolicy(p, securitylevel::Disallowed);
		}/**/

		return true;
	}

	bool PolicyManager::Apply() {
		return ::CommitTransaction(hkeyCodeIdentifiers.transaction.get()) != 0;
	}
}

