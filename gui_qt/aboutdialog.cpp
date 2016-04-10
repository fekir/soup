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

#include "aboutdialog.hpp"
#include "ui_aboutdialog.h"

#include "qtcommon.hpp"

#include <QFile>
#include <QMessageBox>
#include <QTextStream>

#include <cassert>


AboutDialog::AboutDialog(QWidget *parent) :
	QDialog(parent, Qt::WindowTitleHint | Qt::WindowCloseButtonHint| Qt::WindowMinimizeButtonHint),
	ui(new Ui::AboutDialog)
{
	ui->setupUi(this);
	this->setFixedSize(this->size());

	const QString title("soup (stop unwanted programs)");
	const QString version_number(SOUP_VERSION_NUMBER);
	const QString shortdescription(
		"soup is a program for avoiding the execution of unwanted software like viruses and other malware.\n"
		"It can be used with any antivirus software, and does not need to run in background the whole time.");

	ui->label->setText(title + ", version " + version_number);
	ui->label_2->setText(shortdescription);
}

AboutDialog::~AboutDialog()
{
	delete ui;
}

void AboutDialog::on_pushButton_gpl_clicked(){
	if(license_gpl == nullptr){
		QFile file( ":/MyFiles/LICENSE.txt" );
		if(!file.open(QFile::ReadOnly | QFile::Text)){
			// FIXME: add link to gpl license
			QMessageBox::warning(nullptr, "License not found", "Unable to find the gpl license");
		}
		QTextStream in(&file);
		const QString text = in.readAll();
		license_gpl = CreateDialogForLicense("GPL License", text);
	}
	assert(license_gpl != nullptr);
	show_or_raise(*license_gpl);
}

