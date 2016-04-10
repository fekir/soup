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

// qt
#include <QApplication>
#include <QStyleFactory>

#include <QFile>
#include <QDebug>
#include <QTextStream>

const auto blue    = QColor(42,  130, 218);
const auto violet  = QColor(142, 45,  197);
const auto gray    = QColor(53,  53,  53);
const auto darkgey = QColor(25,  25,  25);

QPalette fusionpalette(const QColor highlighcolor = blue){
	// https://gist.github.com/QuantumCD/6245215
	// colori descritt qua: https://doc.qt.io/qt-5/qt.html

	QPalette darkPalette;
	darkPalette.setColor(QPalette::Window, gray);
	darkPalette.setColor(QPalette::Button, gray);

	darkPalette.setColor(QPalette::Highlight, highlighcolor);

	darkPalette.setColor(QPalette::ButtonText, Qt::white);
	darkPalette.setColor(QPalette::WindowText, Qt::white);

	darkPalette.setColor(QPalette::Base, darkgey);
	darkPalette.setColor(QPalette::AlternateBase, gray);
	darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
	darkPalette.setColor(QPalette::ToolTipText, Qt::white);
	darkPalette.setColor(QPalette::Text, Qt::white);
	darkPalette.setColor(QPalette::BrightText, Qt::red);
	darkPalette.setColor(QPalette::Link, blue);
	darkPalette.setColor(QPalette::HighlightedText, Qt::black);
	return darkPalette;
}

int main(int argc, char *argv[]){

	QApplication a(argc, argv);

	if(true){// Dark theme
		a.setStyle(QStyleFactory::create("Fusion"));

		const auto pal = fusionpalette();
		a.setPalette(pal);
		// #2a82da = blue (= QColor(42, 130, 218)), #ffffff = black
		a.setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }"); // Tooltip impostato prima ... su bianco...
	}
	MainWindow w;
	w.show();

	return a.exec();
}
