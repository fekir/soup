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
#include "../policy.hpp"
#include "../evtlog.hpp"

// test
#include "catch.hpp"

// windows
#include <Windows.h>

//std
#include <string>
#include <vector>


TEST_CASE("evt", "") {
	evtlog::test();
}


TEST_CASE("sid", ""){
	auto username = evtlog::sid_to_username("S-1-5-21-13210259-1748602183-1043662369-1001");

	int a =1 ;
}
