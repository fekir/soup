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

#ifndef ABOUTDIALOG_HPP
#define ABOUTDIALOG_HPP

#include <QDialog>
#include <QString>
#include <QDialog>
#include <QTextEdit>
#include <QGridLayout>

#include <memory>


inline std::unique_ptr<QDialog> CreateDialogForLicense(const QString& title, const QString& license_text){
	auto diag= std::make_unique<QDialog>(nullptr, Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint);
	diag->setWindowTitle(title);
	diag->resize(550, 400);

	auto te = std::make_unique<QTextEdit>(nullptr); // using unique_ptr for avoiding leak if 'new QGridLayout' throws
	te->setReadOnly(true);
	te->setPlainText(license_text);
	te->setLineWrapMode(QTextEdit::NoWrap);
	{
		QGridLayout *gridLayout = new QGridLayout(nullptr);
		diag->setLayout(gridLayout);
		gridLayout->addWidget(te.release());
	}
	return diag;
}

namespace Ui {
	class AboutDialog;
}

class AboutDialog : public QDialog
{
	Q_OBJECT

public:
	explicit AboutDialog(QWidget *parent = nullptr);
	~AboutDialog();

private slots:
	void on_pushButton_gpl_clicked();

private:
	Ui::AboutDialog *ui;
	std::unique_ptr<QDialog> license_gpl;
	std::unique_ptr<QDialog> license_postcard;
};

#endif // ABOUTDIALOG_HPP
