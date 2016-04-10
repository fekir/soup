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
#include "../policysetting.hpp"
#include "test_help.hpp"

// qt
#include <QApplication>

// std
#include <iostream>
#include <string>

TEST_CASE("TestPolicySettings1", "[qt][policy][sheet][settings][hide]"){
	CREATE_FAKE_APPLICATION_WITH_NO_ARGS(a);


	PolicySetting ps(false);
	ps.setDefaultValues();
	ps.show();
	a.exec();

}
