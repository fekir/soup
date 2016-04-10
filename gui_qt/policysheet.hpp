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

#ifndef POLICYSHEET_H
#define POLICYSHEET_H

// local

// windows

// qt
#include <QWidget>
#include <QInputDialog>

// std
#include <vector>
#include <memory>

namespace Ui {
	class PolicySheet;
}

class PolicyEventLog;
class PolicySetting;

class PolicySheet final : public QWidget
{
	Q_OBJECT

public:
	std::unique_ptr<PolicyEventLog> evtlog2;
	explicit PolicySheet(QWidget *parent = nullptr);
	virtual ~PolicySheet();

private slots:
	void on_pushButton_load_policies_clicked();
	void on_pushButton_load_file_clicked();
	void on_pushButton_newpolicy_clicked();
	void on_tabWidget_tabCloseRequested(int index);

	void on_pushButton_loadlog_clicked();

	void on_pushButton_settings_clicked();

private:
	Ui::PolicySheet *ui;
	QString lastusedpath;
	// FIXME: when reopening dialog, last selected option is the proposed one
	// id could be a local variable, but there is a noticeable lag when creating it for the first time
	QInputDialog id;
	PolicyEventLog* evtlog = nullptr;
//	std::unique_ptr<PolicyEventLog> evtlog2 = nullptr;
	PolicySetting* pcsetting = nullptr;
};

#endif // POLICYSHEET_H
