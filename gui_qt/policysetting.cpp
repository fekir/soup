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

#include "policysetting.hpp"
#include "ui_policysetting.h"


// local
#include "policy.hpp"
#include "common.hpp"

#include "diffdialog.hpp"
#include "qtcommon.hpp"


// qt
#include <QMessageBox>
#include <QFileDialog>
#include <QCompleter>

// cstd
#include <cassert>

// std
#include <fstream>
namespace {
	// string to be saved on file
	std::string export_rules(const std::string& name, const QLineEdit& seclevel, const QLineEdit& policyscope, const QLineEdit& enforcement, const QLineEdit& adminurl, const QTextEdit& execs){
		std::string ss = !name.empty() ? "[" + name + "]\n" : ";Unnamed Policy\n";
		const auto executables = uniquify(trimandremovedelim(explode(execs.toPlainText().toStdString(), ',')));
		if(!executables.empty()){
			ss += policy::keys::executables + " = " + flatten(executables, ',') + "\n";
		}

		const auto sec = seclevel.text().toStdString();
		if(!sec.empty()){
			ss += policy::keys::securitylevel + " = " + policy::to_string(policy::to_securitylevel(sec)) + "\n";
		}

		const auto scope = policyscope.text().toStdString();
		if(!scope.empty()){
			ss += policy::keys::policyscope + " = " + policy::to_string(policy::to_policyScope(scope)) + "\n";
		}

		const auto enf = enforcement.text().toStdString();
		if(!enf.empty()){
			ss += policy::keys::enforcementlevel + " = " + policy::to_string(policy::to_enforcementLevel(enf)) + "\n";
		}

		const auto adminurl_t = adminurl.text().toStdString();
		if(!adminurl_t.empty()){
			ss += policy::keys::admin_info_url + " = " + adminurl_t + "\n"; // FIXME: validate url
		}
		return ss;
	}

}

PolicySetting::PolicySetting(bool isPcSetting, QWidget *parent) :
	SinglePolicySheetInterface(parent),
	ui(new Ui::PolicySetting),
	m_isPcSetting(isPcSetting)
{
	using namespace  policy;
	ui->setupUi(this);
	using policy::to_string;
	// i need these settings value in multiple places as QString
	const QString securitylevel_Disallowed = QString::fromStdString(to_string(securitylevel::Disallowed));
	const QString securitylevel_Unrestricted = QString::fromStdString(to_string(securitylevel::Unrestricted));

	const QString enforcementLevel_AllFiles = QString::fromStdString(to_string(enforcementLevel::AllFiles));
	const QString enforcementLevel_SkipDLLs = QString::fromStdString(to_string(enforcementLevel::SkipDLLs));
	const QString enforcementLevel_NoEnforcement = QString::fromStdString(to_string(enforcementLevel::NoEnforcement));

	const QString policyScope_AllUsers = QString::fromStdString(to_string(policyScope::AllUsers));
	const QString policyScope_SkipAdministrators = QString::fromStdString(to_string(policyScope::SkipAdministrators));

	const QString possiblevalues("Possible values are: ");

	ui->lineEdit_security->setCompleter(
				createCompleter(*this, {securitylevel_Disallowed, securitylevel_Unrestricted}));
	ui->lineEdit_security->setPlaceholderText(possiblevalues + "\"" + securitylevel_Disallowed +"\"  or \"" + securitylevel_Unrestricted + "\"");

	ui->lineEdit_enforce->setCompleter(
				createCompleter(*this, {enforcementLevel_AllFiles, enforcementLevel_SkipDLLs, enforcementLevel_NoEnforcement}));
	ui->lineEdit_enforce->setPlaceholderText(possiblevalues + "\"" + enforcementLevel_AllFiles + "\", " + enforcementLevel_SkipDLLs + " or \"" + enforcementLevel_NoEnforcement + "\"");

	ui->lineEdit_PolicyScope->setCompleter(
				createCompleter(*this, {policyScope_AllUsers, policyScope_SkipAdministrators}));
	ui->lineEdit_PolicyScope->setPlaceholderText(possiblevalues + "\"" + policyScope_AllUsers +"\"  or \"" + policyScope_SkipAdministrators + "\"");

	ui->textEdit_exec->setPlaceholderText("For example: \"exe,com,reg\"");
}

PolicySetting::~PolicySetting()
{
	delete ui;
}

void PolicySetting::setExecutableTypes(const std::vector<std::string>& exec_types){
	ui->textEdit_exec->setText(QString::fromStdString(flatten(exec_types, ',')));
}

void PolicySetting::setSecuritylevel(const policy::securitylevel sec){
	ui->lineEdit_security->setText(QString::fromStdString(policy::to_string(sec)));
}

void PolicySetting::setPolicyScope(const policy::policyScope scope){
	ui->lineEdit_PolicyScope->setText(QString::fromStdString(policy::to_string(scope)));
}

void PolicySetting::setEnforcementLevel(const policy::enforcementLevel enf){
	ui->lineEdit_enforce->setText(QString::fromStdString(policy::to_string(enf)));
}

void PolicySetting::setAdminInfoUrl(const std::string& url){
	ui->lineEdit_AdminInfoUrl->setText(QString::fromStdString(url));
}

void PolicySetting::setDefaultValues(){
	setPolicyScope(policy::policyScope::AllUsers);
	setEnforcementLevel(policy::enforcementLevel::SkipDLLs);
	setSecuritylevel(policy::securitylevel::Unrestricted);
	setExecutableTypes(policy::ExecutableExtensions());
	setAdminInfoUrl("");
}

void PolicySetting::on_pushButton_apply_clicked(){
	try {
		policy::PolicyManager p;
		const auto executables = uniquify(trimandremovedelim(explode(ui->textEdit_exec->toPlainText().toStdString(), ',')));
		p.setExecutableTypes(executables);

		const auto sec = policy::to_securitylevel(ui->lineEdit_security->text().toStdString());
		p.setSecurityLevel(sec);

		const auto scope = policy::to_policyScope(ui->lineEdit_PolicyScope->text().toStdString());
		p.setPolicyScope(scope);

		const auto enf = policy::to_enforcementLevel(ui->lineEdit_enforce->text().toStdString());
		p.setEnforcementLevel(enf);

		const auto adminurl = ui->lineEdit_AdminInfoUrl->text().toStdString();
		p.setAdminInfoUrl(adminurl);

		if(!p.Apply()){
			QMessageBox::warning(nullptr, tr("Warning"), tr("Unable to apply policies."));
		}

	} catch(const std::runtime_error& err){
		show_warning(err);
	}

}

void PolicySetting::on_pushButton_exportfile_clicked(){
	try{
		const auto name = getName().toStdString();

		// get all elements from the tables and generate list<policy> (and create UUID if not present)
		QFileDialog dialog;
		dialog.setFileMode(QFileDialog::AnyFile);
		if (!dialog.exec()){
			return;
		}
		const auto fileNames = dialog.selectedFiles();
		std::ofstream ofs(fileNames.at(0).toStdString());
		if(!ofs.is_open()){
			QMessageBox::warning(nullptr, "Warning", "Unable to open " + fileNames.at(0) );
			return;
		}
		ofs.exceptions( std::ofstream::failbit | std::ofstream::badbit);
		ofs << export_rules(name, *(ui->lineEdit_security), *(ui->lineEdit_PolicyScope), *(ui->lineEdit_enforce), *(ui->lineEdit_AdminInfoUrl), *(ui->textEdit_exec));
	} catch (const std::runtime_error& err){
		show_warning(err);
	}
}
