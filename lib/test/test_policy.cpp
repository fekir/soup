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
#include "../policy.hpp"

// test
#include "catch.hpp"

// windows
#include <Windows.h>

//std
#include <string>
#include <vector>
#include <regex>

TEST_CASE("TestPolicyDoubleExt", "[policy][DoubleExt][hide]") {
	const auto doubleext = combineext(policy::CommonExtensions(), policy::ExecutableExtensions());

	policy::PolicyManager manager;
	const auto res = manager.SetPolicyDisableDoubleExt(doubleext);
	const auto res1 = manager.Apply();
}

TEST_CASE("TestPolicyUnsercureLoc", "[policy][UnsercureLoc][hide]") {
	auto locs = policy::UnsecureLocations();

	const auto ext = policy::ExecutableExtensions();

	policy::PolicyManager manager;
	const auto res = manager.SetPolicyDisableInsecureLocations(locs, ext);
	const auto res1 = manager.Apply();
}

TEST_CASE("TestPolicySecureLoc", "[policy][SecureLoc][hide]") {
	auto locs = policy::SecureLocations();
	locs.emplace_back("C:\\cygwin64");
	locs.emplace_back("C:\\Work");

	const auto ext = policy::ExecutableExtensions();

	policy::PolicyManager manager;

	const auto res = manager.SetPolicyEnableOnlySecureLocations(locs, ext);
	REQUIRE(res == true);
	const auto res1 = manager.Apply();
	REQUIRE(res1 == true);
}

TEST_CASE("TestPolicyBidi", "[policy][Bidi][hide]") {
	policy::PolicyManager manager;

	const auto res = manager.SetPolicyDisableBiDiFiles();
	const auto res1 = manager.Apply();
}

TEST_CASE("regex", "[policy][regex]") {
	const auto ext1_ = policy::CommonExtensions();
	const auto ext2_ = policy::ExecutableExtensions();
	const auto doubleex = combineext(ext1_, ext2_);

	const std::string tomatch1("*.doc.exe");
	const std::string tomatch2("*.doc?.com");
	const std::string tomatch3("*.?htm?.diagcab");
	const std::string notomatch1("*.?htm?_diagcab");
	const std::string notomatch2("*.?htm?.diagcab.blabla");
	const std::string notomatch3(" *.?htm?.diagcab");
	const std::string notomatch4("_.?htm?.diagcab");

	const std::regex r1("^\\*\\.([^.]+)\\.([^.]+)$");
	REQUIRE(std::regex_match(tomatch1, r1));
	REQUIRE(std::regex_match(tomatch2, r1));
	REQUIRE(std::regex_match(tomatch3, r1));
	REQUIRE(!std::regex_match(notomatch1, r1));
	REQUIRE(!std::regex_match(notomatch2, r1));
	REQUIRE(!std::regex_match(notomatch3, r1));
	REQUIRE(!std::regex_match(notomatch4, r1));


	std::vector<std::string> ext1;
	std::vector<std::string> ext2;
	std::smatch sm;
	for (const auto& v : doubleex) {
		std::regex_match(v, sm, r1);
		if (sm.size() == 3) {
			add_if_unique(ext1, sm[1]);
			add_if_unique(ext2, sm[2]);
		}
	}
}


TEST_CASE("loadini", "[policy][ini]") {
	auto res = policy::loadrulesfromini(test_data_dir + "/policy1.ini");
	REQUIRE(res.policies.size() == 3);
	REQUIRE(res.doubleextpol.size() == 1);
	REQUIRE(res.settings.size() == 1);


	REQUIRE(res.doubleextpol.at(0).ext1.size() == 2);
	REQUIRE(res.doubleextpol.at(0).ext2.size() == 2);

}


TEST_CASE("loadini2", "[policy][ini][invalid]") {
	auto res = policy::loadrulesfromini( test_data_dir + "invalidpolicy1.ini");
	REQUIRE(res.policies.size() == 0);
	REQUIRE(res.doubleextpol.size() == 0);
	REQUIRE(res.settings.size() == 0);
}
