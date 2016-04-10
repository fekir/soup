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

#ifndef POLICYEVENTLOG_HPP
#define POLICYEVENTLOG_HPP

#include "evtlog.hpp"
#include "singlepolicysheetinterface.hpp"

#include <QWidget>
#include <QString>

namespace Ui {
	class PolicyEventLog;
}

class EmptyQObject : public QObject {
	Q_OBJECT
public:
	EmptyQObject(){}
signals:
	void updateText(QString str);
public:
   void emitMySignal(const QString& text){
	   emit updateText(text);
   }

};

const auto pwsPath = L"Application";// L"<channel name goes here>";
const auto pwsQuery = L"*[System/EventID=866]";//  nullptr;// L"<xpath query goes here>";

struct subscriptionQt {
	const std::size_t check = typeid(*this).hash_code();
	std::unique_ptr<EmptyQObject> obj;
	RAII_EVTHANDLE hSubscription;
};

inline DWORD WINAPI QtSubscriptionCallback(const EVT_SUBSCRIBE_NOTIFY_ACTION action, const PVOID pContext, const EVT_HANDLE hEvent){
	assert(pContext != nullptr);
	auto sub = reinterpret_cast<subscriptionQt*>(pContext);
	assert(sub->check == typeid(subscriptionQt).hash_code());
	try {

		switch (action) {
			case EvtSubscribeActionError: {
				/*
				if (ERROR_EVT_QUERY_RESULT_STALE == reinterpret_cast<decltype(ERROR_EVT_QUERY_RESULT_STALE)>(hEvent)){
					// The subscription callback was notified that event records are missing.\n
					// Handle if this is an issue for your application.
				} else {
					// The subscription callback received the following Win32 error: %lu\n", reinterpret_cast<decltype(ERROR_EVT_QUERY_RESULT_STALE)>(hEvent));
				}
				*/
				break;
			}
			case EvtSubscribeActionDeliver: {
				const auto var = evtlog::get_rendered_content(hEvent);
				sub->obj->emitMySignal(QString::fromStdString(var));
				break;
			}
			default: {
				assert(false);
				return ERROR_INVALID_PARAMETER;
			}
		}
		return ERROR_SUCCESS;
	}
	catch (const std::bad_alloc& err) {
		sub->hSubscription.release();
		return ERROR_OUTOFMEMORY;
	}
	catch (const std::runtime_error& err) {
		sub->hSubscription.release();
		return ERROR_INVALID_DATA; // fixme
	}
	catch (...) {
		sub->hSubscription.release();
		return ERROR_INVALID_DATA; // fixme
	}
}



class PolicyEventLog final : public SinglePolicySheetInterface
{
	Q_OBJECT

public:
	explicit PolicyEventLog(QWidget *parent = 0);
	~PolicyEventLog();

	virtual QString getName() const override{return "EventLog";}
	virtual bool isPcSetting() const override{return true;}
	subscriptionQt subscription;

private:
	Ui::PolicyEventLog *ui;

public slots:
	void on_text_updated(QString text);
};

#endif // POLICYEVENTLOG_HPP
