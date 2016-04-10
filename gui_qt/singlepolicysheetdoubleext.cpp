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

#include "singlepolicysheetdoubleext.hpp"
#include "ui_singlepolicysheetdoubleext.h"


// local
#include "policy.hpp"
#include "uuid.hpp"
#include "common.hpp"
#include "diffdialog.hpp"
#include "qtcommon.hpp"

// qt
#include <QMessageBox>
#include <QFileDialog>

// cstd
#include <cassert>

// std
#include <algorithm>
#include <iterator>
#include <fstream>

namespace {

	policy::doubleext to_rules(const std::string& name, const std::string& description, const QTextEdit& ted1, const QTextEdit& ted2){
		policy::doubleext d;
		d.name = name;
		// Warning: may throw if securitylevel value is not valid
		d.description = description; // needed for lambda capture
		d.sec = policy::securitylevel::Disallowed;
		// get all elements from the tables and generate list<policy>
		d.ext1 = uniquify(trimandremovedelim(explode(ted1.toPlainText().toStdString(), ',')));
		d.ext2 = uniquify(trimandremovedelim(explode(ted2.toPlainText().toStdString(), ',')));
		return d;
	}

	// string to be saved onf file
	std::string export_rules(const policy::doubleext& rules){
		std::string ss = !rules.name.empty() ? "[" + rules.name + "]\n" : ";Unnamed Policy\n";
		ss += policy::keys::ext1 + " = " + flatten(rules.ext1, ',') + "\n"; //ext1=doc //F
		ss += policy::keys::ext2 + " = " + flatten(rules.ext2, ',') + "\n";
		if(!rules.description.empty()){
			ss += policy::keys::description + " = " + rules.description + "\n";
		}
		return ss;
	}
}

SinglePolicySheetDoubleExt::SinglePolicySheetDoubleExt(bool isPcSetting, const policy::doubleext& policies, QWidget *parent) :
	SinglePolicySheetInterface(parent),
	ui(new Ui::SinglePolicySheetDoubleExt),
	m_isPcSetting(isPcSetting), description(policies.description)
{
	ui->setupUi(this);
	ui->lineEdit_policy_name->setText(QString::fromStdString(policies.name));
	ui->textEdit_ext1->setText(QString::fromStdString(flatten(policies.ext1, ',')));
	ui->textEdit_ext2->setText(QString::fromStdString(flatten(policies.ext2, ',')));
	//ui->lineEdit_appliesto->setText(QString::fromStdString(to_string(policies.sec)));
	assert(!ui->lineEdit_appliesto->isEnabled() && "this lineedit should be always disabled");
}

SinglePolicySheetDoubleExt::~SinglePolicySheetDoubleExt() {
	delete ui;
}

QString SinglePolicySheetDoubleExt::getName() const {
	return  "Policy: " + ui->lineEdit_policy_name->text();
}

// FIXME: does not work if changing name or security level (could save orig name and policy in separate variable)
void SinglePolicySheetDoubleExt::on_pushButton_apply_clicked() {
	try{
		const auto doublerules = to_rules(ui->lineEdit_policy_name->text().toStdString(), this->description, *(ui->textEdit_ext1), *(ui->textEdit_ext1));
		auto exts = combineext(doublerules.ext1, doublerules.ext2);

		// get policies in the pc
		auto policies = policy::getLoadedRules(HKEY_LOCAL_MACHINE);

		// remove policies with different names, description, and securitylevel
		policies.erase(std::remove_if(policies.begin(), policies.end(),
					   [&doublerules](const policy::policy_s p){
			return p.pol.name != doublerules.name || p.pol.Description != doublerules.description || p.sec != doublerules.sec;
		}),
		policies.end()
		);
		// FIXME: check that these policies are from the same group! (use smatch), separate them in groups

		auto& policygroupfrompc = policies;

		const policy::CompareByRule comp;
		// sort (need for applying diff)
		std::sort(exts.begin(), exts.end(), comp);
		std::sort(policygroupfrompc.begin(), policygroupfrompc.end(), comp);


		// apply diff
		std::vector<std::string> toadd;
		std::set_difference(exts.begin(), exts.end(), policygroupfrompc.begin(), policygroupfrompc.end(),
							std::back_inserter(toadd), comp);


		std::vector<policy::policy_s> toremove;
		std::set_difference(policygroupfrompc.begin(), policygroupfrompc.end(), exts.begin(), exts.end(),
							std::back_inserter(toremove), comp);

		if(toadd.empty() && toremove.empty()){
			QMessageBox::information(nullptr, tr("Info"), tr("There are no changes to apply."));
			return;
		}


		policy::PolicyManager p;
		DiffDialog diff("DoubleExtension diff");
		if(!toremove.empty()){
			diff.addTextTobeRemoved();
			for(const auto& v : toremove){
				p.RemovePolicy(v.sec, v.UUID);
				diff.addTextTobeRemoved(v.pol.name + " | " + v.pol.ItemData + " | " + v.pol.Description + " | " + policy::to_string(v.sec) );
			}
			diff.addPlaintext("\n");
		}

		if(!toadd.empty()){
			diff.addTextToBeAdded();
			for(const auto& v : toadd){
				policy::policy_rule rule;
				rule.name = doublerules.name;
				rule.ItemData = v;
				rule.Description = doublerules.description;
				p.SetPolicy( rule, doublerules.sec);
				diff.addTextToBeAdded(rule.name + " | " + rule.ItemData + " | " + rule.Description + " | " + policy::to_string(doublerules.sec) );
			}
		}

		const auto res = diff.exec();
		if(res != QDialog::Accepted){
			return;
		}

		if(!p.Apply()){
			QMessageBox::warning(nullptr, "Warning", "Unable to apply policies.");
		}
	} catch(const std::runtime_error& err){
		show_warning(err);
	}
}

void SinglePolicySheetDoubleExt::on_pushButton_exportfile_clicked(){
	try{
		const auto doublerules = to_rules(ui->lineEdit_policy_name->text().toStdString(), this->description, *(ui->textEdit_ext1), *(ui->textEdit_ext1));

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
		ofs << export_rules(doublerules);
	} catch (const std::runtime_error& err){
		show_warning(err);
	}
}
