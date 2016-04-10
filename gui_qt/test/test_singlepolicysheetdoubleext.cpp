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
#include "../singlepolicysheetdoubleext.hpp"
#include "test_help.hpp"

// qt
#include <QApplication>

// std
#include <iostream>
#include <string>

TEST_CASE("TestSinglePolicySheetDoubleExt1", "[qt][policy][sheet][single][doubleext][hide]"){
	CREATE_FAKE_APPLICATION_WITH_NO_ARGS(app);
	policy::doubleext de;
	de.ext1 = {"doc", "jpg"};
	de.ext2 = {"exe"};
	de.name = "TestPolicy1";
	de.description = "Description of " + de.name;
	de.sec = policy::securitylevel::Unrestricted;
	SinglePolicySheetDoubleExt pol(false, de);
	pol.show();
	app.exec();
}


TEST_CASE("TestSinglePolicySheetDoubleExt3", "[qt][policy][sheet][single][doubleext][hide]"){
	CREATE_FAKE_APPLICATION_WITH_NO_ARGS(app);
	policy::doubleext de;
	de.ext1 = {"doc", "jpg"};
	de.ext2 = {"exe"};
	de.name = "TestPolicy1";
	de.description = "Description of " + de.name;
	de.sec = policy::securitylevel::Unrestricted;
	SinglePolicySheetDoubleExt pol(true, de);
	pol.show();
	app.exec();
}
