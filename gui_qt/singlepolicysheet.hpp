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

#ifndef SINGLEPOLICYSHEET_H
#define SINGLEPOLICYSHEET_H

// local
#include "policy.hpp"
#include "singlepolicysheetinterface.hpp"

// windows

// qt
#include <QWidget>
#include <QModelIndex>

// std
#include <vector>

namespace Ui {
	class SinglePolicySheet;
}

// FIXME: Tab ordering does not work: https://stackoverflow.com/questions/17542136/qtableview-in-mainwindow-and-tab-ordering
class SinglePolicySheet final : public SinglePolicySheetInterface
{
	Q_OBJECT

public:
	explicit SinglePolicySheet(bool isPcSetting, const std::vector<policy::policy_s>& policies, QWidget *parent = nullptr);
	explicit SinglePolicySheet(bool isPcSetting, QWidget *parent = nullptr);
	virtual ~SinglePolicySheet();

	virtual QString getName() const override;
	virtual bool isPcSetting() const override{return m_isPcSetting;}
	void addElements(const std::vector<policy::policy_s>& policies);

private slots:
	void on_pushButton_apply_clicked();
	void on_pushButton_add_clicked();
	void on_pushButton_remove_clicked();
	void on_pushButton_exportfile_clicked();

private:
	Ui::SinglePolicySheet *ui;
	bool m_isPcSetting;
};

#endif // SINGLEPOLICYSHEET_H
