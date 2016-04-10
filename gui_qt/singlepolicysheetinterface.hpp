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

#ifndef SINGLEPOLICYSHEETINTERFACE_H
#define SINGLEPOLICYSHEETINTERFACE_H

// qt
#include <QWidget>
#include <QString>


class SinglePolicySheetInterface : public QWidget
{
	Q_OBJECT
public:
	explicit SinglePolicySheetInterface(QWidget *parent = nullptr) : QWidget(parent){}
	virtual ~SinglePolicySheetInterface() = default;

	virtual QString getName() const = 0;
	virtual bool isPcSetting() const = 0;
};

#endif // SINGLEPOLICYSHEETINTERFACE_H
