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

#ifndef POLICYSETTING_HPP
#define POLICYSETTING_HPP

#include "policy.hpp"

#include <QWidget>
#include <singlepolicysheetinterface.hpp>

namespace Ui {
	class PolicySetting;
}

class PolicySetting final : public SinglePolicySheetInterface
{
	Q_OBJECT

public:
	explicit PolicySetting(bool isPcSetting, QWidget *parent = 0);
	~PolicySetting();

	virtual QString getName() const override{return "Settings";}
	virtual bool isPcSetting() const override{return m_isPcSetting;}

	void setExecutableTypes(const std::vector<std::string>& exec_types);
	void setSecuritylevel(const policy::securitylevel Securitylevel);
	void setPolicyScope(const policy::policyScope PolicyScope);
	void setEnforcementLevel(const policy::enforcementLevel EnforcementLevel);
	void setAdminInfoUrl(const std::string& AdminInfoUrl);

	void setDefaultValues();
private slots:
	void on_pushButton_apply_clicked();
	void on_pushButton_exportfile_clicked();

private:
	Ui::PolicySetting *ui;
	bool m_isPcSetting;
};

#endif // POLICYSETTING_HPP
