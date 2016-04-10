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
#include "../aboutdialog.hpp"
#include "test_help.hpp"

// qt
#include <QApplication>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QTextEdit>
#include <QGridLayout>

// std
#include <iostream>
#include <string>

TEST_CASE("TestAboutDialog1", "[qt][policy][sheet][single][hide]"){
	CREATE_FAKE_APPLICATION_WITH_NO_ARGS(a);


	AboutDialog dialog;
	dialog.exec();
	//a.exec();
}


TEST_CASE("TestAboutDialog2", "[qt][policy][sheet][single][hide]"){
	CREATE_FAKE_APPLICATION_WITH_NO_ARGS(a);

	const bool postcard = false;
	QFile file( postcard ? ":/MyFiles/license_postcard.txt" : ":/MyFiles/license_gpl.txt" );
	if(!file.open(QFile::ReadOnly | QFile::Text)){
		FAIL("unable to open file");
	}

	QTextStream in(&file);
	QString text = in.readAll();

	REQUIRE(!text.isEmpty());

	auto diag = CreateDialogForLicense(postcard ? "Postcard License" : "GPL License", text);
	diag->show();

	INFO("finished");
	a.exec();
}
