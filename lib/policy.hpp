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

// windows
#include <Windows.h>

//std
#include <vector>
#include <string>
#include <stdexcept>
#include <sstream>
#include <iterator>
#include <tuple>
#include <regex>
#include <memory>

namespace policy{

	// compared to the default values:
	// - removed .lst (links in start menu, taskbar and desktop will work)
	// - added .diagcab, .vsix
	// - what about .jar, .sh, .py, ps1 and other scripting languages?
	std::vector<std::string> ExecutableExtensions();

	std::vector<std::string> CommonExtensions();

	std::vector<std::string> SecureLocations();

	std::vector<std::string> UnsecureLocations();

	struct bidi {
		std::string c;
		std::string desc;
	};
	std::vector<bidi> getBidi();


	/// You can set software restriction policies so that the operating system has one of two default security levels: Unrestricted and Disallowed. When you first create software restriction policies, a default setting of Unrestricted is automatically defined. You can then apply rules that restrict the execution of specified software programs. When you set the default security level to Disallowed, most software is restricted. There are four registry path rules that are automatically created to prevent you from completely locking down your system, including locking yourself out of it. You must then apply rules for all other software that you want to run.

	/// Software restriction policies are applied through the operating system. Certain software programs must be set to Unrestricted for the operating system to function at all. The following tips can help you decide which programs to set to Unrestricted if you decide to use a default security level of Disallowed:
	enum class securitylevel {
		Unrestricted = 262144 /// rules that allow execution
		//,Beginner = 131072  /// it's there in gpedit.msc, did not found any documentation on it -> I'll not consider it
		,Disallowed = 0       /// rules that disallow execution
	};
	namespace securitylevel_s {
		const std::string Unrestricted = "Unrestricted";
		const std::string Disallowed = "Disallowed";
	}
	inline std::string to_string(const securitylevel sec) {
		switch (sec) {
			case securitylevel::Unrestricted: return securitylevel_s::Unrestricted;
				//case securitylevel::Beginner: return "Beginner";
			case securitylevel::Disallowed: return securitylevel_s::Disallowed;
			default: assert(false && "missing enum"); return "";
		}
	}
	inline securitylevel to_securitylevel(const std::string& str) {
		if (str == securitylevel_s::Unrestricted) { return securitylevel::Unrestricted; }
		if (str == securitylevel_s::Disallowed) { return securitylevel::Disallowed; }
		//if (str == "Beginner") { return securitylevel::Beginner; }
		throw std::runtime_error("No valid securitylevel");
	}
	inline securitylevel to_securitylevel(const DWORD w) {
		if (w != static_cast<DWORD>(securitylevel::Disallowed) && w != static_cast<DWORD>(securitylevel::Unrestricted)) {
			throw std::runtime_error("No valid securitylevel");
		}
		return static_cast<securitylevel>(w);
	}
	inline int to_int(const securitylevel sec) { return static_cast<int>(sec); }

	// If the policy applies to all users, or all users excluded administrators
	enum class policyScope { AllUsers = 0, SkipAdministrators = 1};
	namespace policyScope_s {
		const std::string AllUsers = "AllUsers";
		const std::string SkipAdministrators = "SkipAdministrators";
	}
	inline std::string to_string(const policyScope scope) {
		switch (scope) {
			case policyScope::AllUsers: return policyScope_s::AllUsers;
			case policyScope::SkipAdministrators: return policyScope_s::SkipAdministrators;
			default: assert(false && "missing enum"); return "";
		}
	}
	inline policyScope to_policyScope(const std::string& str) {
		if (str == policyScope_s::AllUsers) { return policyScope::AllUsers; }
		if (str == policyScope_s::SkipAdministrators) { return policyScope::SkipAdministrators; }
		throw std::runtime_error("No valid policyScope");
	}
	inline policyScope to_policyScope(const DWORD w) {
		if (w != static_cast<DWORD>(policyScope::AllUsers) &&
			w != static_cast<DWORD>(policyScope::SkipAdministrators)) {
			throw std::runtime_error("No valid policyScope");
		}
		return static_cast<policyScope>(w);
	}
	inline int to_int(const policyScope scope) { return static_cast<int>(scope); }

	// If the policy applies to all files, all files excluded dll (performance reason), or no files
	enum class enforcementLevel { NoEnforcement = 0, SkipDLLs = 1, AllFiles =2 };
	namespace enforcementLevel_s {
		const std::string NoEnforcement = "NoEnforcement";
		const std::string SkipDLLs      = "SkipDLLs";
		const std::string AllFiles      = "AllFiles";
	}
	inline std::string to_string(const enforcementLevel scope) {
		switch (scope) {
			case enforcementLevel::AllFiles: return enforcementLevel_s::AllFiles;
			case enforcementLevel::SkipDLLs: return enforcementLevel_s::SkipDLLs;
			case enforcementLevel::NoEnforcement: return enforcementLevel_s::NoEnforcement;
			default: assert(false && "missing enum"); return "";
		}
	}
	inline enforcementLevel to_enforcementLevel(const std::string& str) {
		if (str == enforcementLevel_s::AllFiles) { return enforcementLevel::AllFiles; }
		if (str == enforcementLevel_s::SkipDLLs) { return enforcementLevel::SkipDLLs; }
		if (str == enforcementLevel_s::NoEnforcement) { return enforcementLevel::NoEnforcement; }
		throw std::runtime_error("No valid enforcementLevel");
	}
	inline enforcementLevel to_enforcementLevel(const DWORD w) {
		if (w != static_cast<DWORD>(enforcementLevel::AllFiles) &&
			w != static_cast<DWORD>(enforcementLevel::NoEnforcement) &&
			w != static_cast<DWORD>(enforcementLevel::SkipDLLs)) {
			throw std::runtime_error("No valid enforcementLevel");
		}
		return static_cast<enforcementLevel>(w);
	}
	inline int to_int(const enforcementLevel lev) { return static_cast<int>(lev); }

	struct policy_rule {
		std::string name;
		std::string Description;
		//DWORD SaferFlags = 0; // unused
		std::string ItemData;
		registry::regtype ItemDataType = registry::regtype::expand_sz;
	};

	struct policy_s {
		policy_rule pol; // the policy rule
		std::string UUID; // UUID, used to locate the policy
		HKEY hk; // -- user ID
		securitylevel sec; // securitylevel (also used to locate the policy)
	};

	class PolicyManager {
		registry::TransactionKey hkeyCodeIdentifiers;
	public:
		/// Changes are applied only when committing transaction (method Apply)
		PolicyManager();
		explicit PolicyManager(DWORD flags);

		void setEnforcementLevel(const enforcementLevel e) const;
		void setPolicyScope(const policyScope s) const;
		void setSecurityLevel(const securitylevel l) const;
		void setAdminInfoUrl(const std::string& link) const;

		// returns string and not enum since the value may be invalid!
		enforcementLevel getEnforcementLevel() const;
		policyScope getPolicyScope() const;
		securitylevel getSecurityLevel() const;
		std::string getAdminInfoUrl() const;

		void setExecutableTypes(const std::vector<std::string>& exec_ext) const;
		std::vector<std::string> getExecutableTypes() const;

		bool SetPolicy(const policy_rule& p, const securitylevel sec, const std::string& uuid); // FIXME: validate uuid
		bool SetPolicy(const policy_rule& p, const securitylevel sec, const UUID& uuid);
		bool SetPolicy(const policy_rule& p, const securitylevel sec);

		bool RemovePolicy(const securitylevel sec, const std::string& UUID);

		bool SetPolicyDisableDoubleExt(const std::vector<std::string>& doubleext);
		bool SetPolicyDisableInsecureLocations(const std::vector<std::string>& locations, const std::vector<std::string>& exec_ext);
		bool SetPolicyEnableOnlySecureLocations(const std::vector<std::string>& locations, const std::vector<std::string>& exec_ext);
		bool SetPolicyDisableBiDiFiles();

		bool Apply();

	};

	// just give local machine or user
	inline std::vector<policy_s> getLoadedRules(const HKEY hk) {
		std::vector<policy::policy_s> policies;
		const std::wstring CodeIdentifiers0(L"SOFTWARE\\Policies\\Microsoft\\Windows\\Safer\\CodeIdentifiers\\0\\Paths");
		auto key = registry::OpenKeyOptional(hk, CodeIdentifiers0);
		if(key){
			auto listofkeys = registry::EnumKey(key.get());
			policies.reserve(listofkeys.size());
			policy::policy_s pol;
			pol.hk = hk; // FIXME
			pol.sec = securitylevel::Disallowed; // FIXME
			for (const auto v : listofkeys) {
				const auto key2 = registry::OpenKey(key.get(), v);
				pol.pol.Description = registry::QueryString(key2.get(), L"Description");
				pol.pol.ItemData = registry::QueryString(key2.get(), L"ItemData");
				pol.pol.name = registry::QueryString(key2.get(), L"Name");
				pol.UUID = v; // FIXME
				auto saferflags = registry::QueryDWORD(key2.get(), L"SaferFlags"); // NOTE: ignore for the moment
				assert(saferflags == 0); (void)saferflags;
				policies.push_back(pol);
			}
		}

		const std::wstring CodeIdentifiers1(L"SOFTWARE\\Policies\\Microsoft\\Windows\\Safer\\CodeIdentifiers\\262144\\Paths");
		key.reset();
		key = registry::OpenKeyOptional(hk, CodeIdentifiers1);
		if(!key){
			return policies;
		}
		const auto listofkeys = registry::EnumKey(key.get());
		policies.reserve(policies.size() + listofkeys.size());
		policy::policy_s pol;
		pol.sec = securitylevel::Unrestricted; // FIXME
		for (const auto v : listofkeys) {
			const auto key2 = registry::OpenKey(key.get(), v);
			pol.pol.Description = registry::QueryString(key2.get(), L"Description");
			pol.pol.ItemData = registry::QueryString(key2.get(), L"ItemData");
			pol.pol.name = registry::QueryString(key2.get(), L"Name");
			pol.UUID = v; // FIXME
			auto saferflags = registry::QueryDWORD(key2.get(), L"SaferFlags"); // NOTE: ignore for the moment
			assert(saferflags == 0); (void)saferflags;
			policies.push_back(pol);
		}
		return policies;
	}

	// policies with different or no title are separated -> title is not unique!
	// lot of possible optimizations -> elements are copied and then removed
	// a) copy and do not remove (prevent reallocation) -> vector can be passed as (non constant) reference
	// b) move (prevent new allocation)
	inline std::vector<std::vector<policy::policy_s>> groupbyname(std::vector<policy::policy_s> pols, bool groupemtpy = false) {
		std::vector<std::vector<policy::policy_s>> toreturn;
		while (!pols.empty()) {
			const std::string name = pols.at(0).pol.name;
			if (name.empty() && !groupemtpy) { // elements without name are separated
				toreturn.push_back({ pols.at(0) });
				pols.erase(pols.begin());
				continue;
			}
			const auto lambda = [&name](const policy::policy_s& p) {return p.pol.name == name; };
			const auto lastel = std::partition(pols.begin(), pols.end(), lambda);
			const std::vector<policy::policy_s> tmp(pols.begin(), lastel); // could remember position of previous search?
			assert (!tmp.empty() && "if cannot be empty since pols.at(0) had the searched name!");
			pols.erase(pols.begin(), lastel);
			toreturn.push_back(tmp);
		}
		return toreturn;
	}

	struct doubleext{
		std::vector<std::string> ext1;
		std::vector<std::string> ext2;
		std::string name;
		std::string description;
		HKEY hk;
		securitylevel sec;
	};

	// They are a policy of double-ext if all properties (except itemdata and UUID) are equals and itemdata are all double-ext
	// This function may split the double-ext for different properties (user, security level, ...), but not yet
	inline doubleext getdoubleext(const std::vector<policy::policy_s>& pols) {
		if (pols.empty()) {
			return{};
		}

		const std::regex r1("^\\*\\.([^.]+)\\.([^.]+)$");

		std::vector<std::string> ext1;
		std::vector<std::string> ext2;
		const auto& name = pols.at(0).pol.name; // name
		const auto& description = pols.at(0).pol.Description; // description
		const auto& sec = pols.at(0).sec; // security settings
		const auto& hk = pols.at(0).hk; // user

		for (const auto& v : pols) {
			if (v.pol.name != name || v.pol.Description != description || v.sec != sec || v.hk != hk) {
				return{};
			}
			std::smatch sm;
			if(std::regex_match(v.pol.ItemData, sm, r1) && sm.size() == 3) {
				add_if_unique(ext1, sm[1]);
				add_if_unique(ext2, sm[2]);
			}
			else {
				return {};
			}
		}
		return{ ext1, ext2, name, description, hk, sec };
	}


	inline std::vector<doubleext> removedoubleext(std::vector<std::vector<policy::policy_s>>& groupedpols){
		std::vector<doubleext> toreturn;

		for(auto it = groupedpols.begin(); it != groupedpols.end(); ){
			assert(!it->empty());
			auto doubleextension = policy::getdoubleext(*it);
			if (!doubleextension.ext1.empty() && !doubleextension.ext2.empty()) {
				toreturn.push_back(doubleextension);
				it = groupedpols.erase(it);
			} else {
				++it;
			}
		}
		return toreturn;
	}

	// keywords that may be used in a configuration file
	namespace keys {
		// generic keys for policy
		const std::string rule        = "Rule";
		const std::string description = "Description";
		const std::string security    = "Security";
		const std::string who         = "Who";
		const std::string uuid        = "UUID";

		// keys for doubleext policy
		const std::string ext1 = "ext1";
		const std::string ext2 = "ext2";

		// keys for settings
		const std::string admin_info_url   = "AdminInfoUrl";
		const std::string securitylevel    = "SecurityLevel";
		const std::string policyscope      = "PolicyScope";
		const std::string enforcementlevel = "EnforcementLevel";
		const std::string executables      = "Executables";

	}

	// FIXME: using unique_ptr instead of some optional type (std::optional is not available yet), it is not the most performant solution, but the one that requires less code and no external dependencies -> notice: optional should be copyable, but unique_ptr it's not
	template<class T>
	using myoptional = std::unique_ptr<T>;

	struct policysettings {
		std::vector<std::string> executables;
		myoptional<securitylevel> SecurityLevel;
		myoptional<policyScope> PolicyScope;
		myoptional<enforcementLevel> EnforcementLevel;
		myoptional<std::string> admininfourl;
	};

	struct policiesfromini {
		std::vector<std::vector<policy::policy_s>> policies;
		std::vector<policy::doubleext> doubleextpol;
		std::vector<policysettings> settings;
	};
	inline policiesfromini loadrulesfromini(const std::string& inifile) {
		iniparser::IniParser parser(inifile);

		// policies are grouped together by name (if options are consistent)
		policiesfromini toreturn;
		for (const auto& v : parser.content) {
			const auto name = v.first;
			const std::string Who = (v.second.count(keys::who) ? v.second.at(keys::who) : "*"); //if nothing->everyone

			const bool hasext1 = v.second.count(keys::ext1) !=0;
			const bool hasext2 = v.second.count(keys::ext2) !=0;
			if ( (hasext1 && !hasext2) || (!hasext1 && hasext2)) {
				throw std::runtime_error("Invalid double ext configuration, you need to set ext1 and ext2");
			}

			if (hasext1) {
				doubleext d;
				d.ext1 = uniquify(trimandremovedelim(explode(v.second.at(keys::ext1), ',')));
				d.ext2 = uniquify(trimandremovedelim(explode(v.second.at(keys::ext2), ',')));
				d.description = v.second.count(keys::description) != 0 ? v.second.at(keys::description) : "";
				d.name = name;
				toreturn.doubleextpol.push_back(d);
				continue;
			}

			const bool hasexecutables = v.second.count(keys::executables) != 0;
			const bool hassecuritylevel = v.second.count(keys::securitylevel) != 0;
			const bool haspolicyScope = v.second.count(keys::policyscope) != 0;
			const bool hasenforcementLevel = v.second.count(keys::enforcementlevel) != 0;
			const bool hasadmininfourl = v.second.count(keys::admin_info_url) != 0;

			if (hasexecutables || hassecuritylevel || haspolicyScope || hasenforcementLevel || hasadmininfourl) {
				policysettings settings;
				if (hasexecutables) {
					settings.executables = uniquify(trimandremovedelim(explode(v.second.at(keys::executables), ',')));
				}
				settings.SecurityLevel = hassecuritylevel ? std::make_unique<securitylevel>( to_securitylevel(v.second.at(keys::securitylevel))) : nullptr;
				settings.PolicyScope = haspolicyScope ? std::make_unique<policyScope>(to_policyScope(v.second.at(keys::policyscope))) : nullptr;
				settings.admininfourl = hasadmininfourl ? std::make_unique<std::string>(v.second.at(keys::admin_info_url)) : nullptr;
				settings.EnforcementLevel = hasenforcementLevel ? std::make_unique<enforcementLevel>(to_enforcementLevel(v.second.at(keys::enforcementlevel))) : nullptr;
				toreturn.settings.push_back(std::move(settings));
			}

			const std::string Allow = (v.second.count(keys::security) ? v.second.at(keys::security) : ""); //maybe only given specific, if not throw
			auto description = v.second.count(keys::description) != 0 ? v.second.at(keys::description) : "";
			std::vector<policy::policy_s> tmppolicies;
			for (const auto& vv : v.second) {
				const auto match0 = matchwithoptionalnumber(vv.first, keys::rule);
				if (match0.first) {
					policy::policy_s tmp;
					tmp.pol.name = name;
					tmp.pol.ItemData = vv.second;
					const auto securitykey = keys::security + match0.second;
					tmp.sec = to_securitylevel( v.second.count(securitykey) != 0 ? v.second.at(securitykey) : Allow);
					const auto descriptionkey = keys::description + match0.second;
					tmp.pol.Description = v.second.count(descriptionkey) != 0 ? v.second.at(descriptionkey) : description;
					const auto uuidkey = keys::uuid + match0.second;
					tmp.UUID = v.second.count(uuidkey) != 0 ? v.second.at(uuidkey) : "";
					tmppolicies.push_back(tmp);
				}
			}
			if (!tmppolicies.empty()) {
				toreturn.policies.push_back(tmppolicies);
			}
		}
		return toreturn;
	}

	struct CompareByRule {
		bool operator()(const std::string& s, const std::string& s2) const {
			return s2 < s;
		}
		bool operator()(const std::string& s, const policy::policy_s& p) const {
			return p.pol.ItemData < s;
		}
		bool operator()(const policy::policy_s& p, const policy::policy_s& p2) const {
			return p2.pol.ItemData < p.pol.ItemData;
		}
		bool operator()(const policy::policy_s& p, const std::string& s) const {
			return s< p.pol.ItemData;
		}
	};

	struct CompareByUUID {
		bool operator()(const policy::policy_s& p, const policy::policy_s& p2) const {
			return p2.UUID < p.UUID;
		}
	};

	struct Compare_policy_s {
		bool operator()(const policy::policy_s& p, const policy::policy_s& p2) const {
			return std::tie(p.UUID, p.sec, p.pol.Description, p.pol.ItemData, p.pol.name ) < std::tie(p2.UUID, p2.sec, p2.pol.Description, p2.pol.ItemData, p2.pol.name );
		}
	};
}
