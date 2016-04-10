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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H



#include "aboutdialog.hpp"


#include <QMainWindow>

#include <memory>

namespace Ui {
	class MainWindow;
}

class MainWindow final : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	virtual ~MainWindow();

private slots:
	void on_actionInfo_triggered();

private:
	Ui::MainWindow *ui;
	std::unique_ptr<AboutDialog> aboutdialog;
	//AboutDialog* aboutdialog = nullptr;
};

#endif // MAINWINDOW_H
