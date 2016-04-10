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

#include "singlepolicysheet.hpp"
#include "ui_singlepolicysheet.h"


// local
#include "policy.hpp"
#include "uuid.hpp"

#include "diffdialog.hpp"
#include "qtcommon.hpp"

// qt
#include <QMessageBox>
#include <QFileDialog>

// cstd
#include <cassert>

// std
#include <algorithm>
#include <functional>
#include <fstream>
#include <iostream>

namespace {

	std::vector<policy::policy_s> table_to_rules(const std::string& name, QTableWidget& table){
		std::vector<policy::policy_s> rules; rules.reserve(table.rowCount());
		for(int row = 0; row != table.rowCount(); ++row){
			const auto theItem0 = table.item(row, 0);
			if(theItem0 == nullptr || theItem0->text().isEmpty()){continue;}
			const auto ItemData = theItem0->text().toStdString();
			const auto theItem1 = table.item(row, 1);
			const auto Description = (theItem1 != nullptr) ? theItem1->text().toStdString() : "";
			const auto theItem2 = table.item(row, 2);
			if(theItem2 == nullptr){
				throw std::runtime_error("No SecurityLevel set");
			}
			const auto sec = policy::to_securitylevel(theItem2->text().toStdString());
			const auto theItem3 = table.item(row, 3);
			auto UUID = (theItem3 != nullptr) ? theItem3->text().toStdString() : "";
			policy::policy_s p_rule;
			p_rule.pol.name = name;
			p_rule.pol.Description = Description;
			p_rule.pol.ItemData = ItemData;
			p_rule.pol.ItemDataType = registry::regtype::expand_sz;
			if(UUID.empty()){
				UUID = uid::to_string(uid::createUUID());
				table.setItem(row,3,new QTableWidgetItem(QString::fromStdString(UUID)));
			}
			p_rule.UUID = UUID;
			p_rule.sec = sec;
			rules.push_back(p_rule);
		}
		return rules;
	}

	// string to be saved onf file
	std::string export_rules(const std::vector<policy::policy_s>& rules){
		if(rules.empty()){
			return "";
		}
		std::string ss = !rules.at(0).pol.name.empty() ? "[" + rules.at(0).pol.name + "]\n" : ";Unnamed Policy\n";
		ss.reserve(ss.size()+rules.size()*15*3); // estimate, 15 = min lenght of description
		int i = 0;
		for(const auto& v : rules){
			ss += policy::keys::rule + std::to_string(++i) + " = " + v.pol.ItemData + "\n";
			if(!v.pol.Description.empty()){
				ss += policy::keys::description + std::to_string( i ) + " = " + v.pol.Description + "\n";
			}
			if(!v.UUID.empty()){
				ss += policy::keys::uuid + std::to_string( i ) + " = " + v.UUID + "\n";
			}
			ss += policy::keys::security  + std::to_string( i ) + " = " + policy::to_string(v.sec) + "\n";
		}
		return ss;
	}
}

SinglePolicySheet::SinglePolicySheet(bool isPcPolicy, const std::vector<policy::policy_s>& policies, QWidget *parent) :
	SinglePolicySheet(isPcPolicy, parent)
{
	if(!policies.empty()){
		addElements(policies);
		//ui->lineEdit_appliesto->setText(QString::fromStdString(policy::to_string(policies....)));
	}
	assert(!ui->lineEdit_appliesto->isEnabled() && "this lineedit should be always disabled");

}

SinglePolicySheet::SinglePolicySheet(bool isPcSetting, QWidget *parent) :
	SinglePolicySheetInterface(parent),
	ui(new Ui::SinglePolicySheet),
	m_isPcSetting(isPcSetting)
{
	ui->setupUi(this);
	assert(ui->tableWidget->columnCount() == 4); // Rule, Description, SecurityLevel, (hidden) UUID
	ui->tableWidget->setHorizontalHeaderLabels({"Rule", "Description", "SecurityLevel", "UUID"});
	assert(ui->tableWidget->horizontalHeader()->stretchLastSection());

	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	// FIXME: does not autoresize and minimum size is not clear...

	// hide UUID bar, FIXME: add as an option
#ifndef NDEBUG
	ui->tableWidget->setColumnHidden(3, true);
#endif
}

SinglePolicySheet::~SinglePolicySheet() {
	delete ui;
}

QString SinglePolicySheet::getName() const {
	assert(ui->tableWidget->rowCount() >0 && "no policies has been added");
	return "Policy: " + ui->lineEdit_policy_name->text();
}

void SinglePolicySheet::addElements(const std::vector<policy::policy_s>& policies){
	assert(!policies.empty() && "never pass an empty list");
	ui->lineEdit_policy_name->setText(QString::fromStdString(policies.at(0).pol.name));
	auto insertRow = ui->tableWidget->rowCount();
	for(const auto& v : policies){
		ui->tableWidget->insertRow(insertRow);
		ui->tableWidget->setItem(insertRow,0,new QTableWidgetItem(QString::fromStdString(v.pol.ItemData)));
		ui->tableWidget->setItem(insertRow,1,new QTableWidgetItem(QString::fromStdString(v.pol.Description)));
		ui->tableWidget->setItem(insertRow,2,new QTableWidgetItem(QString::fromStdString(policy::to_string(v.sec))));
		if(!v.UUID.empty()){
			ui->tableWidget->setItem(insertRow,3,new QTableWidgetItem(QString::fromStdString(v.UUID)));
		}
		insertRow++;
	}
}

void SinglePolicySheet::on_pushButton_apply_clicked() {
	try{
		const auto name = ui->lineEdit_policy_name->text().toStdString();

		// get all elements from the tables and generate list<policy> (and create UUID if not present)
		auto rules = table_to_rules(name, *(ui->tableWidget));

		// get policies in the pc
		auto policies = policy::getLoadedRules(HKEY_LOCAL_MACHINE);

		// remove policies with different names -- FIXME if name is empty
		policies.erase(std::remove_if(policies.begin(), policies.end(), [&name](const policy::policy_s& p){
			return p.pol.name != name;
		}), policies.end());

		auto& policygroupfrompc = policies;

		const policy::Compare_policy_s comp;
		// sort (need for applying diff)
		std::sort(rules.begin(), rules.end(), comp);
		std::sort(policygroupfrompc.begin(), policygroupfrompc.end(), comp);


		// apply diff
		std::vector<policy::policy_s> toadd;
		std::set_difference(rules.begin(), rules.end(), policygroupfrompc.begin(), policygroupfrompc.end(),
							std::back_inserter(toadd), comp);

		std::vector<policy::policy_s> toremove;
		std::set_difference(policygroupfrompc.begin(), policygroupfrompc.end(), rules.begin(), rules.end(),
							std::back_inserter(toremove), comp);

		if(toadd.empty() && toremove.empty()){
			QMessageBox::information(nullptr, tr("Info"), tr("There are no changes to apply."));
			return;
		}


		policy::PolicyManager p;
		DiffDialog d("Single Policy diff");

		if(!toremove.empty()){
			d.addTextTobeRemoved();
			for(const auto& v : toremove){
				p.RemovePolicy(v.sec, v.UUID);
				d.addTextTobeRemoved(name + " | " + v.pol.ItemData + " | " + v.pol.Description + " | " + policy::to_string(v.sec) );
			}
			d.addPlaintext("\n");
		}

		if(!toadd.empty()){
			d.addTextToBeAdded();
			for(const auto& v : toadd){
				p.SetPolicy(v.pol, v.sec, v.UUID);
				d.addTextToBeAdded(name + " | " + v.pol.ItemData + " | " + v.pol.Description + " | " + policy::to_string(v.sec) );
			}
		}

		const auto res = d.exec();
		if(res != QDialog::Accepted){
			return;
		}

		if(!p.Apply()){
			QMessageBox::warning(nullptr, tr("Warning"), tr("Unable to apply policies."));
		}
	} catch(const std::runtime_error& err){
		show_warning(err);
	}
}


void SinglePolicySheet::on_pushButton_add_clicked(){
	auto insertRow = ui->tableWidget->rowCount();
	assert(insertRow>=0);
	ui->tableWidget->insertRow(insertRow);
}

void SinglePolicySheet::on_pushButton_remove_clicked() {
	const auto rowcount = ui->tableWidget->rowCount();
	if(rowcount <= 0){ // nothing to remove
		return;
	}

	const auto vec = tovectors(ui->tableWidget->selectedItems());
	if(vec.empty()){
		ui->tableWidget->removeRow(rowcount-1);
		return;
	}

	std::vector<int> rows; //we use a set to prevent doubles
	for(const auto& v : vec){
		rows.push_back(v->row());
	}

	rows.erase(std::unique(rows.begin(), rows.end()), rows.end());
	// sort to avoid index invalidation
	std::sort(rows.begin(), rows.end(), std::greater<int>());
	for(const auto& i : rows){
		ui->tableWidget->removeRow(i);
	}
}

void SinglePolicySheet::on_pushButton_exportfile_clicked(){
	try{
		const auto name = ui->lineEdit_policy_name->text().toStdString();

		// get all elements from the tables and generate list<policy> (and create UUID if not present)
		const auto rules = table_to_rules(name, *(ui->tableWidget));

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
		ofs << export_rules(rules);
	} catch (const std::runtime_error& err){
		show_warning(err);
	}
}
