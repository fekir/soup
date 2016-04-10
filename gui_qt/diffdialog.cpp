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

#include "diffdialog.hpp"
#include "ui_diffdialog.h"

DiffDialog::DiffDialog(QString title, QWidget *parent) :
	QDialog(parent, Qt::WindowTitleHint | Qt::WindowCloseButtonHint |/* Qt::WindowMinimizeButtonHint |*/ Qt::WindowMaximizeButtonHint ),
	ui(new Ui::DiffDialog)
{
	ui->setupUi(this);
	this->setWindowTitle(title);
	QObject::connect(ui->pushButton_Apply, &QPushButton::clicked, this, &QDialog::accept);
	QObject::connect(ui->pushButton_Cancel, &QPushButton::clicked, this, &QDialog::reject);
}

DiffDialog::~DiffDialog()
{
	delete ui;
}


void DiffDialog::addTextTobeRemoved(const std::string& text){
	/// using <code> for having monospace
	QString text_ = "<font color='red'><code> - </code>" + QString::fromStdString(text) + "</font><br>";
	ui->textBrowser->insertHtml(text_);
}
void DiffDialog::addTextToBeAdded(const std::string& text){
	/// using <code> for having monospace
	QString text_ = "<font color='green'><code> + </code>" + QString::fromStdString(text) + "</font><br>";
	ui->textBrowser->insertHtml(text_);
}
void DiffDialog::addTextToBeAdded(){
	ui->textBrowser->insertPlainText("Following rules will be added:\n");
}
void DiffDialog::addTextTobeRemoved(){
	ui->textBrowser->insertPlainText("Following rules will be removed:\n");
}
void DiffDialog::addPlaintext(const std::string& text){
	ui->textBrowser->insertPlainText(QString::fromStdString(text));
}
