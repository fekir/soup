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

#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include "aboutdialog.hpp"
#include "qtcommon.hpp"

#include <cassert>



MainWindow::MainWindow(QWidget *newparent) :
	QMainWindow(newparent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_actionInfo_triggered(){
	if(aboutdialog == nullptr){
		aboutdialog =  std::make_unique<AboutDialog>(nullptr);
		//aboutdialog =  new AboutDialog(nullptr);
	}
	assert(aboutdialog != nullptr);
	show_or_raise(*aboutdialog);
}
