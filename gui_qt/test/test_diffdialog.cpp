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
#include "../diffdialog.hpp"
#include "test_help.hpp"

// qt
#include <QApplication>

// std
#include <iostream>
#include <string>

TEST_CASE("TestDiffDialog1", "[qt][policy][sheet][single][hide]"){
	CREATE_FAKE_APPLICATION_WITH_NO_ARGS(a);

	SECTION("sec1"){
		DiffDialog d("diff");
		d.addTextToBeAdded();
		d.addTextTobeRemoved("text to be removed");
		d.addPlaintext("\n");
		d.addTextToBeAdded();
		d.addTextToBeAdded("text to be added");
		d.show();
		a.exec();
	}
}
