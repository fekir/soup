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

#include "policysheet.hpp"
#include "ui_policysheet.h"

// local
#include "singlepolicysheet.hpp"
#include "singlepolicysheetdoubleext.hpp"
#include "policyeventlog.hpp"
#include "policysetting.hpp"
#include "registry.hpp"
#include "policy.hpp"
#include "IniParser.hpp"
#include "qtcommon.hpp"

// windows

// qt
#include <QFileDialog>    // choose file
#include <QMessageBox>    // messagebox
#include <QStandardPaths> // user path
#include <QInputDialog>

// std
#include <cassert>
#include <algorithm>
#include <memory>

// FIXME: update policy name in tab

const QStringList options = {"Single Policy", "Double Extension Policy", "Policy Settings"};

PolicySheet::PolicySheet(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PolicySheet), lastusedpath(QDir::homePath()),
	id(nullptr, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint) // do not set Qt::WindowContextHelpButtonHint
{
	ui->setupUi(this);
	assert(ui->tabWidget->tabsClosable() && "tabs needs to be closeable");

	id.setWindowTitle(tr("Choose Policy Sheet"));
	id.setLabelText(tr("Do you want to add a Single Policy sheet, or a Double Extension Policy sheet?\nIf unsure choose Single Policy sheet"));
	id.setComboBoxItems(options);
}

PolicySheet::~PolicySheet()
{
	delete ui;
}


void PolicySheet::on_pushButton_load_policies_clicked() {
	// need to load all the policies, create SinglePolicySheets, and add them to the tabWidget
	try{
		auto policies = policy::getLoadedRules(HKEY_LOCAL_MACHINE);
		auto groupedpolicies = policy::groupbyname(std::move(policies));


		const auto policiesdoubleext = policy::removedoubleext(groupedpolicies);

		std::vector<SinglePolicySheetInterface*> singlepolicysheets;
		singlepolicysheets.reserve(groupedpolicies.size() + policiesdoubleext.size());

		for(const auto& v : policiesdoubleext){
			auto sheet = new SinglePolicySheetDoubleExt(true, v, this);
			singlepolicysheets.push_back(sheet);
		}

		for(const auto& v : groupedpolicies){
			auto sheet = new SinglePolicySheet(true, v, this);
			singlepolicysheets.push_back(sheet);
		}


		// no error happened during reading and creating the sheets, i can swap the policies (hopefully noexcept)

		// NOTE: If policy did not change, and I've selected it, I'll remove focus... could make comparison and remove/replace... but can get really complicated
		// FIXME: using set_diff i could see if two groups are similar  (25% diff?) and perhaps update them
		for(int i = 0; i != ui->tabWidget->count();){
			const auto sheet = dynamic_cast<SinglePolicySheetInterface*>(ui->tabWidget->widget(i));
			assert(sheet != nullptr);
			if(sheet->isPcSetting() &&
					(dynamic_cast<SinglePolicySheet*>(sheet) != nullptr || dynamic_cast<SinglePolicySheetDoubleExt*>(sheet) != nullptr)){
				ui->tabWidget->removeTab(i);
			} else {
				++i;
			}
		}

		// add all the new policies
		int i = 1;
		for(const auto& v: singlepolicysheets){
			assert(v!=nullptr);
			const auto name = v->getName();
			const auto pos = ui->tabWidget->addTab(v, name.isEmpty() ? "Policy " + QString::number(i++) : name);
			if(name.isEmpty()){
				ui->tabWidget->tabBar()->setTabTextColor(pos,Qt::gray);
			}
		}
	} catch(const std::runtime_error& err){
		show_warning(err);
	}
}

void PolicySheet::on_pushButton_load_file_clicked() {
	const auto fileNames = QFileDialog::getOpenFileNames(this,
														 tr("Open Configuration file"), lastusedpath, tr("Config files (*.ini *.cfg)"));

	if (fileNames.isEmpty())
	{
		return;
	}
	lastusedpath = QFileInfo(fileNames[0]).path(); // store path for next time
	// create policies from every config file

	policy::policiesfromini polsfromini;
	for(const auto& v : fileNames){ // parse every file
		auto rules =  policy::loadrulesfromini(v.toStdString());
		polsfromini.policies.insert(polsfromini.policies.end(), rules.policies.begin(), rules.policies.end());
		polsfromini.doubleextpol.insert(polsfromini.doubleextpol.begin(), rules.doubleextpol.begin(), rules.doubleextpol.end());
		std::move(rules.settings.begin(), rules.settings.end(), std::back_inserter(polsfromini.settings)); // FIXME: need to move since myoptional is an unique_ptr
	}

	int i = 1; // NOTE: not really unique if open multiple files in different moments
	for(const auto& v : polsfromini.policies){
		auto sheet = new SinglePolicySheet(false, v, this);
		const auto name = v.at(0).pol.name;
		const auto pos = ui->tabWidget->addTab(sheet, !name.empty() ? QString::fromStdString(name) : "Policy " + QString::number(i++));
		if(name.empty()){
			ui->tabWidget->tabBar()->setTabTextColor(pos,Qt::gray);
		}
	}

	for(const auto& v : polsfromini.doubleextpol){
		auto sheet = new SinglePolicySheetDoubleExt(false, v, this);
		const auto name = v.name;
		const auto pos = ui->tabWidget->addTab(sheet, !name.empty() ? QString::fromStdString(name) : "Policy " + QString::number(i++));
		if(name.empty()){
			ui->tabWidget->tabBar()->setTabTextColor(pos,Qt::gray);
		}
	}

	for(const auto& v : polsfromini.settings){
		auto sheet = new PolicySetting(false, this);
		const auto pos = ui->tabWidget->addTab(sheet, "Settings " + QString::number(i++));
		ui->tabWidget->tabBar()->setTabTextColor(pos,Qt::gray);
		if(!v.executables.empty()){sheet->setExecutableTypes(v.executables);}
		if(v.SecurityLevel){sheet->setSecuritylevel(*(v.SecurityLevel.get()));}
		if(v.PolicyScope){sheet->setPolicyScope(*v.PolicyScope.get());}
		if(v.EnforcementLevel){sheet->setEnforcementLevel(*v.EnforcementLevel.get());}
		if(v.admininfourl){sheet->setAdminInfoUrl(*v.admininfourl.get());}
	}
}

void PolicySheet::on_pushButton_newpolicy_clicked(){

	const auto res = id.exec();
	if(!res){
		return;
	}
	const auto sel = id.textValue();
	if(sel == options[0]){
		auto sheet = new SinglePolicySheet(false, this);
		const auto pos = ui->tabWidget->addTab(sheet, "New Policy");
		ui->tabWidget->tabBar()->setTabTextColor(pos, Qt::gray);
	} else if(sel == options[1]){
		auto sheet = new SinglePolicySheetDoubleExt(false,{}, this);
		const auto pos = ui->tabWidget->addTab(sheet, "New Policy");
		ui->tabWidget->tabBar()->setTabTextColor(pos, Qt::gray);
	} else if(sel == options[2]){
		auto sheet = new PolicySetting(false, this);
		sheet->setDefaultValues();
		const auto pos = ui->tabWidget->addTab(sheet, "New Policy Setting");
		ui->tabWidget->tabBar()->setTabTextColor(pos, Qt::gray);
	} else {
		assert(false && "unhandled option");
	}
}

void PolicySheet::on_tabWidget_tabCloseRequested(const int index){
	// TODO: check if qobject_cast is a better alternative
	const auto sheet = dynamic_cast<SinglePolicySheetInterface*>(ui->tabWidget->widget(index));
	if(sheet == pcsetting){
		delete sheet;
		pcsetting = nullptr;
		return;
	}
	if(sheet == evtlog){
		delete sheet;
		evtlog = nullptr;
		return;
	}
	if(sheet->isPcSetting()){
		delete sheet;
		return;
	}

	// not a pc policy -> new or loaded from file


	// TODO: can i somehow detect if there are changes?
	// FIXME: add option to apply/save changes or stop close operation
	const auto ret = QMessageBox::information(nullptr ,tr("Removing tabs"), tr("Are you sure you want to remove the policy sheet? Unsaved changes (not applied or exported to file), will be lost"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if(ret != QMessageBox::Yes){
		return;
	}
	ui->tabWidget->removeTab(index);
	delete sheet;
}

void PolicySheet::on_pushButton_loadlog_clicked(){
	if(evtlog == nullptr){
		auto sheet = new PolicyEventLog;
		sheet->subscription.obj = std::make_unique<EmptyQObject>();

		sheet->subscription.hSubscription.reset(EvtSubscribe(nullptr, nullptr, pwsPath, pwsQuery, nullptr, &(sheet->subscription), QtSubscriptionCallback, EvtSubscribeStartAtOldestRecord));
		QObject::connect(sheet->subscription.obj.get(), &EmptyQObject::updateText, sheet, &PolicyEventLog::on_text_updated);

		evtlog = sheet;
		ui->tabWidget->addTab(evtlog, evtlog->getName());
		return;
	}

	assert(evtlog!=nullptr);
	for(int i = 0; i!= ui->tabWidget->size().width(); ++i){
		if(evtlog == ui->tabWidget->widget(i)){ // comparing pointers(!)
			ui->tabWidget->setCurrentIndex(i);
			return; // no need to add again
		}
	}
	// tabwidget not found, may have been added and closed before
	ui->tabWidget->addTab(evtlog, evtlog->getName());
}

void PolicySheet::on_pushButton_settings_clicked(){
	if(pcsetting == nullptr){
		pcsetting = new PolicySetting(true, this);
	}
	assert(pcsetting!=nullptr);

	try{
		policy::PolicyManager p(KEY_READ);
		// FIXME: throws if value is invalid or missing, I should really parse the data and add them to the dialog, with a warning about the errors
		const auto sec = p.getSecurityLevel();
		const auto enf = p.getEnforcementLevel();
		const auto scope = p.getPolicyScope();
		const auto url = p.getAdminInfoUrl();
		const auto execs = p.getExecutableTypes();

		pcsetting->setSecuritylevel(sec);
		pcsetting->setEnforcementLevel(enf);
		pcsetting->setPolicyScope(scope);
		pcsetting->setAdminInfoUrl(url);
		pcsetting->setExecutableTypes(execs);
	} catch(const std::runtime_error& err){
		show_warning(err);
	}

	for(int i = 0; i!= ui->tabWidget->size().width(); ++i){
		if(pcsetting == ui->tabWidget->widget(i)){ // comparing pointers(!)
			ui->tabWidget->setCurrentIndex(i);
			return; // already added
		}
	}
	ui->tabWidget->addTab(pcsetting, pcsetting->getName());
}
