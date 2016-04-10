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

#ifndef SINGLEPOLICYSHEETDOUBLEEXT_HPP
#define SINGLEPOLICYSHEETDOUBLEEXT_HPP

// local
#include "policy.hpp"
#include "singlepolicysheetinterface.hpp"

// windows

// qt
#include <QWidget>

// std
#include <vector>

namespace Ui {
	class SinglePolicySheetDoubleExt;
}


class SinglePolicySheetDoubleExt final : public SinglePolicySheetInterface
{
	Q_OBJECT

public:
	explicit SinglePolicySheetDoubleExt(bool isPcSetting, const policy::doubleext& policies, QWidget *parent = nullptr);
	virtual ~SinglePolicySheetDoubleExt();

	virtual QString getName() const override;
	virtual bool isPcSetting() const override{return m_isPcSetting;}

private slots:
	void on_pushButton_apply_clicked();
	void on_pushButton_exportfile_clicked();

private:
	Ui::SinglePolicySheetDoubleExt *ui;
	bool m_isPcSetting;
	std::string description; // FIXME: should add a lineedit or something like that
};

#endif // SINGLEPOLICYSHEETDOUBLEEXT_HPP
