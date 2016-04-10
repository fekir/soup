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

#include "policyeventlog.hpp"
#include "ui_policyeventlog.h"


#include <QXmlStreamReader>

PolicyEventLog::PolicyEventLog(QWidget *parent) :
	SinglePolicySheetInterface(parent),
	ui(new Ui::PolicyEventLog)
{
	ui->setupUi(this);
}

PolicyEventLog::~PolicyEventLog()
{
	delete ui;
}

void PolicyEventLog::on_text_updated(QString text){
	QXmlStreamReader xml(text);
	QString attemptedpath;
	QString time;
	QString GUUID;
	QString rulepath;
	QString user;
	while(!xml.atEnd() && !xml.hasError()){
		//ui->textEdit->append(xml.name().toString() + " -||- " + xml.text().toString() + "-||-" + xml.tokenString());
		//xml.readNext();

		/**/
		if(xml.name()=="TimeCreated" && xml.tokenType() == QXmlStreamReader::TokenType::StartElement){
			time = xml.attributes().value("SystemTime").toString();
		}
		if(xml.name()=="AttemptedPath" && xml.tokenType() == QXmlStreamReader::TokenType::StartElement){
			xml.readNext();
			attemptedpath = xml.text().toString();
		}
		if(xml.name()=="SrpRuleGuid" && xml.tokenType() == QXmlStreamReader::TokenType::StartElement){
			xml.readNext();
			GUUID = xml.text().toString();
		}
		if(xml.name()=="RulePath" && xml.tokenType() == QXmlStreamReader::TokenType::StartElement){
			xml.readNext();
			rulepath = xml.text().toString();
		}
		if(xml.name()=="Security" && xml.tokenType() == QXmlStreamReader::TokenType::StartElement){
			user = xml.attributes().value("UserID").toString();
			const auto userpath = evtlog::sid_to_username(user.toStdString());
			if(!userpath.first.empty()){
				user = QString::fromStdString(userpath.first);
			}
		}
		xml.readNext();
		/**/
	}
	ui->textEdit->append("The access to \"" + attemptedpath +"\" has been limited by "+ user +" on " + time +", with the rule \""+rulepath+"\" (" + GUUID +")");
}
