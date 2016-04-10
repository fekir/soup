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

#include "catch.hpp"

// local
#include "../singlepolicysheet.hpp"
#include "test_help.hpp"

// qt
#include <QApplication>

// std
#include <iostream>
#include <string>

TEST_CASE("TestSinglePolicySheet1", "[qt][policy][sheet][single][hide]"){
	CREATE_FAKE_APPLICATION_WITH_NO_ARGS(a);

	std::vector<policy::policy_s> pols;
	for(int i = 0; i != 5; ++i){
		policy::policy_s p1;
		p1.pol.name = "TestPolicy1";
		p1.pol.ItemData = "rule" + std::to_string(i);
		p1.pol.Description = "Description of " + p1.pol.name;
		p1.sec = policy::securitylevel::Disallowed;
		pols.push_back(p1);
	}
	SinglePolicySheet pol(false, pols);
	pol.show();
	a.exec();
}

TEST_CASE("TestSinglePolicySheet2", "[qt][policy][sheet][single][hide]"){
	CREATE_FAKE_APPLICATION_WITH_NO_ARGS(a);
	SinglePolicySheet pol(false, {});
	pol.show();
	a.exec();
}

TEST_CASE("TestSinglePolicySheet3", "[qt][policy][sheet][single][hide]"){
	CREATE_FAKE_APPLICATION_WITH_NO_ARGS(a);
	policy::policy_s p1;
	p1.pol.name = "TestPolicy1";
	p1.pol.ItemData = "*.doc.exe";
	p1.pol.Description = "Description of " + p1.pol.name;
	p1.sec = policy::securitylevel::Unrestricted;
	SinglePolicySheet pol(true, {p1});
	pol.show();
	a.exec();
}
