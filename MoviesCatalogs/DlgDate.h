/* ********************************************************************************
 * MoviesCatalog - a Programm to catalogue a private collection of Movies using SQLite3
 * Copyright (C) 2022 by Laurens Koehoorn (lhksoft)
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * ********************************************************************************/
#ifndef __DLG_DATE_H__
#define __DLG_DATE_H__
#pragma once

#include "../lkControls/lkDialog.h"
#include <wx/calctrl.h>
//#include <wx/textctrl.h>
#include <wx/stattext.h>

#include "../lkSQLite3/lkDateTime.h"

class DlgDate : public lkDialog
{
public:
	DlgDate(double dt = static_cast<double>(wxINT64_MIN));
	virtual ~DlgDate();

	// Create NOT called from C'tor !!
	bool					Create(wxWindow* pParent);

	lkDateTime				GetDate(void) const;


private:
	wxCalendarCtrl*			m_pCalendar;
	wxStaticText*			m_pDtTxtCtrl;

	wxWindowID				m_nToday, m_nNone;
	lkDateTime				m_dtCurrent;

	void					Init(void);

protected:
	void					OnToday(wxCommandEvent& event);
	void					OnNone(wxCommandEvent& event);

	void					OnChangeCalender(wxCalendarEvent& event);

	void					SetDateText(void);


	wxDECLARE_EVENT_TABLE();

	wxDECLARE_DYNAMIC_CLASS_NO_COPY(DlgDate);
};

#endif // !__DLG_DATE_H__
