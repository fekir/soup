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

#ifndef QT_COMMON_H
#define QT_COMMON_H

// qt
#include <QList>
#include <QVector>
#include <QMessageBox>
#include <QString>
#include <QCompleter>
#include <QStringList>

// std
#include <vector>
#include <string>


template<typename T>
std::vector<T> tovectors(const QList<T>& list){
	std::vector<T> vec; vec.reserve(list.size());
	for(int i = 0; i < list.size(); ++i){
		vec.push_back(list.at(i));
	}
	return vec;
}

template<typename T>
std::vector<T> tovectors(const QVector<T>& list){
	std::vector<T> vec; vec.reserve(list.size());
	for(int i = 0; i < list.size(); ++i){
		vec.push_back(list.at(i));
	}
	return vec;
}

inline QStringList tostringlist(const std::vector<std::string>& vec){
	QStringList stringlist;
	for(const auto& v: vec){
		stringlist << QString::fromStdString(v);
	}
	return stringlist;
}

inline void show_warning(const std::runtime_error& err){
	QString message("An error occured: ");
	message += err.what();
	QMessageBox::warning(nullptr, QObject::tr("Warning"),message);
}


inline QCompleter* createCompleter(QObject& parent, const QStringList& hints){
	QCompleter* completer = new QCompleter(hints, &parent);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
	return completer;
}

inline QCompleter* createCompleter(QObject& parent, const std::vector<std::string>& hints_){
	return createCompleter(parent, tostringlist(hints_));
}

// show the window, if already visible, raise
inline void show_or_raise(QDialog& diag){
	if(diag.isVisible()){
		diag.raise();
	}else{
		diag.show();
	}
}

#endif
