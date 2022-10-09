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
#include "DlgDate.h"
#include "Backgrounds.h"

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>

#include "../lkControls/lkTransTextCtrl.h"

#include <wx/msgdlg.h>


//////////////////////////////////////////////////////////////////////////////////////////////
// class DlgDate
////

wxIMPLEMENT_DYNAMIC_CLASS(DlgDate, lkDialog)

wxBEGIN_EVENT_TABLE(DlgDate, lkDialog)
	EVT_CALENDAR(wxID_ANY, DlgDate::OnChangeCalender)
wxEND_EVENT_TABLE()


DlgDate::DlgDate(double dt) : lkDialog(), m_dtCurrent(dt)
{
	Init();
}
DlgDate::~DlgDate()
{
}

void DlgDate::Init()
{
	m_pCalendar = NULL;
	m_pDtTxtCtrl = NULL;

	m_nToday = wxWindow::NewControlId(1);
	m_nNone = wxWindow::NewControlId(1);
}

bool DlgDate::Create(wxWindow* pParent)
{
	long style = wxDEFAULT_DIALOG_STYLE;
#ifdef __WXDEBUG__
	style |= wxRESIZE_BORDER;
#endif // __WXDEBUG__

	if ( !lkDialog::Create(pParent, GetImage_FBG_DATEPICKER(), wxID_ANY, wxT("Date Selector"), wxDefaultPosition, 
#ifdef __WXMSW__	
	wxSize(287, 250)
#else
	wxSize(345, 265)
#endif
	, style) )
		return false;
	SetMinClientSize(
#ifdef __WXMSW__		
		wxSize(271, 211)
#else
		wxSize(345, 242)
#endif
		);

	wxBoxSizer* szMain = new wxBoxSizer(wxVERTICAL);
	{
		wxBoxSizer* szVer = new wxBoxSizer(wxVERTICAL);

		szVer->Add(new wxStaticText(this, wxID_ANY, wxT("Selected Date :"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER | wxST_NO_AUTORESIZE), 0, wxEXPAND | wxBOTTOM, 5);
		szVer->Add(m_pDtTxtCtrl = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER | wxBORDER_SUNKEN | wxST_NO_AUTORESIZE), 0, wxEXPAND | wxBOTTOM, 5);

		szMain->Add(szVer, 0, wxLEFT | wxTOP | wxRIGHT | wxEXPAND, 10);
	}

	{
		wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
		szHor->Add(m_pCalendar = new wxCalendarCtrl(this, wxID_ANY, wxDefaultDateTime, wxDefaultPosition, wxDefaultSize, 0), 1, wxEXPAND | wxRIGHT, 5);

		wxBoxSizer* szV = new wxBoxSizer(wxVERTICAL);
		wxBoxSizer* szVer = new wxBoxSizer(wxVERTICAL);
		szVer->Add(new wxButton(this, m_nToday, wxT("Today")), 0, wxBOTTOM, 5);
		szVer->Add(new wxButton(this, m_nNone, wxT("None")), 0);
		szV->Add(szVer, 0);

		szV->AddStretchSpacer(1);

		szVer = new wxBoxSizer(wxVERTICAL);
		wxButton* ok;
		szVer->Add(ok = new wxButton(this, wxID_OK), 0, wxBOTTOM, 5);
		ok->SetDefault();
		szVer->Add(new wxButton(this, wxID_CANCEL), 0);
		szV->Add(szVer, 0);

		szHor->Add(szV, 0, wxEXPAND);
//		szHor->Add(CreateStdDialogButtonSizer(wxOK | wxCANCEL | wxOK_DEFAULT), 0, wxALIGN_BOTTOM);

		szMain->Add(szHor, 0, wxLEFT | wxBOTTOM | wxRIGHT | wxEXPAND, 10);
	}

	SetSizer(szMain);
	Layout();

	Bind(wxEVT_BUTTON, &DlgDate::OnToday, this, m_nToday);
	Bind(wxEVT_BUTTON, &DlgDate::OnNone, this, m_nNone);

	if ( m_dtCurrent.IsValid() )
		m_pCalendar->SetDate(m_dtCurrent);

	SetDateText();

	return true;
}

void DlgDate::OnToday(wxCommandEvent& event)
{
	if ( m_pCalendar )
	{
		m_pCalendar->SetDate(wxDateTime::Today());
		*(dynamic_cast<wxDateTime*>(&m_dtCurrent)) = m_pCalendar->GetDate();
		SetDateText();
	}
}

void DlgDate::OnNone(wxCommandEvent& event)
{
	m_dtCurrent.SetInvalid();
	SetDateText();
}

void DlgDate::OnChangeCalender(wxCalendarEvent& event)
{
	wxDateTime dt = event.GetDate();
	if ( dt.IsValid() )
	{
		m_dtCurrent.Set(dt.GetTm());
//		*(dynamic_cast<wxDateTime*>(&m_dtCurrent)) = dt;
//		m_pDtTxtCtrl->SetLabel(dt.Format(wxT("%#x")));
	}
	else
	{
		m_dtCurrent.SetInvalid();
	}
	SetDateText();
}

void DlgDate::SetDateText()
{
	wxString s;
	if ( !m_dtCurrent.IsValid() )
		s = wxT("# None #");
	else
		s = m_dtCurrent.FormatFullDate_Dutch();

	m_pDtTxtCtrl->SetLabel(s);
}

lkDateTime DlgDate::GetDate() const
{
	return m_dtCurrent;
}
