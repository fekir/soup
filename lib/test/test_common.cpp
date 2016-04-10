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
#include "common.hpp"
#include "policy.hpp"

// test
#include "catch.hpp"

// windows
#include <Windows.h>

//std
#include <string>


TEST_CASE("RuleCompare", "[common][matchwithoptionalnumber]") {
	{
		const std::string ok_1("Rule");
		auto ok1 = matchwithoptionalnumber(ok_1, "Rule");
		REQUIRE(ok1.first);
		REQUIRE(ok1.second == "");
	}
	{
		const std::string ok_2("Rule2");
		auto ok1 = matchwithoptionalnumber(ok_2, "Rule");
		REQUIRE(ok1.first);
		REQUIRE(ok1.second == "2");
	}
	{
		const std::string ok_3("Rule0");
		auto ok1 = matchwithoptionalnumber(ok_3, "Rule");
		REQUIRE(ok1.first);
		REQUIRE(ok1.second == "0");
	}
	{
		const std::string nok_4("Rule-1");
		auto ok1 = matchwithoptionalnumber(nok_4, "Rule");
		REQUIRE(!ok1.first);
	}
	{
		const std::string nok_4("Ruled");
		auto ok1 = matchwithoptionalnumber(nok_4, "Rule");
		REQUIRE(!ok1.first);
	}
	{
		const std::string ok_1("Description");
		auto ok1 = matchwithoptionalnumber(ok_1, "Description");
		REQUIRE(ok1.first);
		REQUIRE(ok1.second == "");
	}
	{
		const std::string ok_2("Description2");
		auto ok1 = matchwithoptionalnumber(ok_2, "Description");
		REQUIRE(ok1.first);
		REQUIRE(ok1.second == "2");
	}
	{
		const std::string ok_3("Description0");
		auto ok1 = matchwithoptionalnumber(ok_3, "Description");
		REQUIRE(ok1.first);
		REQUIRE(ok1.second == "0");
	}
	{
		const std::string nok_4("Description-1");
		auto ok1 = matchwithoptionalnumber(nok_4, "Description");
		REQUIRE(!ok1.first);
	}
	{
		const std::string nok_4("Description_");
		auto ok1 = matchwithoptionalnumber(nok_4, "Description");
		REQUIRE(!ok1.first);
	}
}


#if (defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL < 2) || !defined(_MSC_VER)
// does not work, additional debug function check if the comparator is valid and seems to have problem with overload resolution
struct comparator {
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

TEST_CASE("diff", "[common]") {
	std::vector<std::string> toadd;
	std::vector<std::string> exts = { "a", "b", "c", "e" };
	policy::policy_s p1; p1.pol.ItemData = "a";
	policy::policy_s p2; p2.pol.ItemData = "d";
	std::vector<policy::policy_s> policygroupfrompc = { p1, p2 };
	comparator comp;
	std::sort(exts.begin(), exts.end(), comp);
	std::sort(policygroupfrompc.begin(), policygroupfrompc.end(), comp);
	std::set_difference(exts.begin(), exts.end(), policygroupfrompc.begin(), policygroupfrompc.end(),
						std::back_inserter(toadd), comp);

	REQUIRE(toadd.size() == 3);


	std::vector<policy::policy_s> toremove;
	std::set_difference(policygroupfrompc.begin(), policygroupfrompc.end(), exts.begin(), exts.end(),
						std::back_inserter(toremove), comp);

	REQUIRE(toremove.size() == 1);
}

#endif

TEST_CASE("trim", "[common][trim]") {
	REQUIRE(trim("   ") == "");
	REQUIRE(trim("a ") == "a");
	REQUIRE(trim(" a") == "a");
	REQUIRE(trim("a a") == "a a");
}

TEST_CASE("removedel", "[common][removedel]") {
	REQUIRE(removedel("") == "");
	REQUIRE(removedel("a") == "a");
	REQUIRE(removedel("\"a") == "\"a");
	REQUIRE(removedel("a\"") == "a\"");
	REQUIRE(removedel("\"a\"") == "a");
	REQUIRE(removedel("a\"a\"") == "a\"a\"");
}

TEST_CASE("trimandremovedelim", "[common][removedel][trim]") {
	std::vector<std::string> input = { "a", " a", " \" a\"" };
	std::vector<std::string> output = { "a", "a", " a" };
	REQUIRE(trimandremovedelim(input) == output);
}
