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
#include "DCountry.h"
#include "VCountry_private.h"
#include "SimpleListbox.h"
#include "Backgrounds.h"

#include "TCountry.h"

#include "../lkSQLite3/lkSQL3Exception.h"

#include <wx/tokenzr.h>
#include <wx/list.h>

#include "../lkControls/lkTransTextCtrl.h"
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/button.h>

#include <wx/valtext.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////
// class CountryData + class CountryList
////
class CountryData : public wxObject
{
public:
	CountryData();
	virtual ~CountryData();

	wxString	m_sISO; // Custom or the Real ISO -- this string we must get when asking an item's parameter

// used when searching
	wxUint64	m_nID; // this id not of real importance -- will get ActualRowID
	wxString	m_sISOdef; // if m_sISO is a custom ISO, here we'll set the default ISO
	wxString	m_sNameDef;
	wxString	m_sNameCustom;
private:
	wxDECLARE_ABSTRACT_CLASS(CountryData);
	wxDECLARE_NO_COPY_CLASS(CountryData);
};
wxIMPLEMENT_ABSTRACT_CLASS(CountryData, wxObject)

CountryData::CountryData() : wxObject(), m_sISO(), m_sISOdef(), m_sNameDef(), m_sNameCustom()
{
}
CountryData::~CountryData()
{
}

WX_DECLARE_LIST(CountryData, CountryList);

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(CountryList);


//////////////////////////////////////////////////////////////////////////////////////////////////////
// class LBCountry
////
class LBCountry : public CountryListbox
{
public:
	LBCountry(lkSQL3Database* pDB);
	virtual bool		BuildList(void) wxOVERRIDE;

protected:
	virtual wxString	GetOrder(void) const wxOVERRIDE;

	CountryList			m_BackupList;

private:
	wxDECLARE_ABSTRACT_CLASS(LBCountry);
	wxDECLARE_NO_COPY_CLASS(LBCountry);
};
wxIMPLEMENT_ABSTRACT_CLASS(LBCountry, CountryListbox)

LBCountry::LBCountry(lkSQL3Database* pDB) : CountryListbox(pDB, NULL), m_BackupList()
{
	m_ExpandFlag = lkExpandFlags::EXPAND_Center;
	ActAsCheckListbox(true);

	m_BackupList.DeleteContents(true);
}
//virtual
wxString LBCountry::GetOrder() const
{
	wxString order;
	order.Printf(rowset_COUNTRY_DEFORDER, t3Country_CUSTOM, t3Country_DEF);

	return order;
}

bool LBCountry::BuildList()
{
	wxASSERT(m_pDB != NULL);
	if ( (m_pDB == NULL) || !m_pDB->IsOpen() ) return false; // nothing to do

	StartOver();

	TCountry RS;
	RS.SetDatabase(m_pDB);
	wxUint64 rowCnt = 0;

	try
	{
		RS.SetOrder(GetOrder());
		RS.Open();
		rowCnt = RS.GetRowCount();
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return false;
	}

	if ( rowCnt == 0 )
		return true; // properly in ADD modus

	lkBoxInitList* pInitList = NULL;
	if ( !RS.IsEmpty() )
	{
		wxImage			flag;
		wxString		iso, country, s, nameFld;
		wxUint64		param;

		CountryData*	pObj;

		pInitList = new lkBoxInitList();
		pInitList->DeleteContents(true);

		RS.MoveFirst();
		while ( !RS.IsEOF() )
		{
			if ( !(s = RS.GetCustomIsoValue()).IsEmpty() )
				iso = s;
			else
				iso = RS.GetIsoValue();

			if ( !(s = RS.GetCustomCountryValue()).IsEmpty() )
				country = s;
			else
				country = RS.GetDefaultCountryValue();

			nameFld.Printf(wxT("%s (%s)"), country, iso);
			flag = RS.GetFlag();

			m_BackupList.Append(pObj = new CountryData());
			pObj->m_nID = RS.GetActualCurRow();
			pObj->m_sISO = iso;

			pObj->m_sISOdef = RS.GetIsoValue();
			pObj->m_sNameDef = RS.GetDefaultCountryValue();
			pObj->m_sNameCustom = RS.GetCustomCountryValue();

			param = (wxUint64)pObj;

			// fill initlist
			pInitList->Append(new lkBoxInit(nameFld, flag, param));
			RS.MoveNext();
		}
	}

	// populate listbox
	bool bRet = false;
	if ( pInitList != NULL )
		bRet = InitializeFromList(pInitList, true, m_ExpandFlag);

	// populate listbox
	if ( pInitList && bRet )
		Populate();

	delete pInitList;
	return bRet;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
// class DCountry
////
wxIMPLEMENT_ABSTRACT_CLASS2(DCountry, lkDialog, TCountry)

wxBEGIN_EVENT_TABLE(DCountry, lkDialog)
	EVT_INIT_DIALOG(DCountry::OnInitDialog)
wxEND_EVENT_TABLE()

DCountry::DCountry(const wxString& sCtr, lkSQL3Database* pDB) : lkDialog(), m_sCountries(sCtr)
{
	m_CListbox = NULL;
	m_srcBtn = NULL;
	m_pDB = pDB;
	m_CountryListBox = wxWindow::NewControlId(1);
	m_CountrySrearchBtn = wxWindow::NewControlId(1);
}
bool DCountry::Create(wxWindow* parent)
{
	if ( !lkDialog::Create(parent, GetImage_FBG_COUNTRY(), wxID_ANY, wxT("Select Countries"), wxDefaultPosition, wxSize(320,305), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER) )
		return false;
	SetMinClientSize(wxSize(304, 266));

	wxBoxSizer* szMain = new wxBoxSizer(wxVERTICAL);
	{
		wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
		szHor->Add(new wxStaticText(this, wxID_ANY, wxT("Countries :"), wxDefaultPosition, wxSize(63, -1), wxALIGN_RIGHT), 0, wxRIGHT | wxBOTTOM | wxFIXED_MINSIZE | wxALIGN_CENTER_VERTICAL, 5);
		szHor->Add(new lkTransTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN | wxTE_READONLY,
									   wxTextValidator(wxFILTER_NONE, &m_sCountries)), 1, wxBOTTOM | wxEXPAND, 5);
		szHor->Add(m_srcBtn = new wxButton(this, m_CountrySrearchBtn, wxT("Search")), 0, wxBOTTOM | wxLEFT, 5);

		szMain->Add(szHor, 0, wxLEFT | wxRIGHT | wxTOP | wxEXPAND, 10);
	}
	szMain->Add(m_CListbox = new LBCountry(m_pDB), 1, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, 10);
	(dynamic_cast<LBCountry*>(m_CListbox))->Create(this, m_CountryListBox);

	szMain->Add(CreateStdDialogButtonSizer(wxOK | wxCANCEL), 0, wxLEFT | wxBOTTOM | wxRIGHT | wxEXPAND, 10);

	SetSizer(szMain);
	Layout();

	Bind(wxEVT_CHECKLISTBOX, &DCountry::OnChekListbox, this, m_CountryListBox);
	Bind(wxEVT_BUTTON, &DCountry::OnSearchBtn, this, m_CountrySrearchBtn);

	return true;
}

wxString DCountry::GetCountryString() const
{
	return m_sCountries;
}

wxSortedArrayString DCountry::SplitCountries(const wxString& ctr)
{
	wxSortedArrayString list;
	if ( ctr.IsEmpty() )
		return list;

	wxString s;
	wxStringTokenizer tok(ctr);
	while ( tok.HasMoreTokens() )
		if ( !(s = tok.GetNextToken()).IsEmpty() )
			list.Add(s);

	return list;
}

// Event Handling
////////////////////////////////////////////////////////

void DCountry::OnInitDialog(wxInitDialogEvent& event)
{
	(dynamic_cast<LBCountry*>(m_CListbox))->BuildList();
	wxDialog::OnInitDialog(event);

	bool bEnable = (m_CListbox && !m_CListbox->IsEmpty());
	if ( !bEnable )
		m_srcBtn->Enable(bEnable);

	wxSortedArrayString ar = SplitCountries(m_sCountries);
	if ( !m_CListbox->IsEmpty() && !ar.IsEmpty() )
	{
		CountryData* cD;
		size_t first = wxNOT_FOUND;
		size_t i, j;
		for ( j = 0; j < m_CListbox->GetCount(); j++ )
			if ( (cD = (CountryData*)((dynamic_cast<LBCountry*>(m_CListbox))->GetLParam(j))) != NULL )
			{
				for ( i = 0; i < ar.GetCount(); i++ )
				{
					if ( ar[i].IsSameAs(cD->m_sISO, false) )
					{
						(dynamic_cast<LBCountry*>(m_CListbox))->SetCheck(j, true);
						if ( first == wxNOT_FOUND )
							first = j;
					}
				}

			}
		(dynamic_cast<LBCountry*>(m_CListbox))->SetSelection(first);
		if ( first != wxNOT_FOUND )
			(dynamic_cast<LBCountry*>(m_CListbox))->ScrollToRow(first);
	}
}

void DCountry::OnChekListbox(wxCommandEvent& event)
{
//	event.Skip();
	if ( static_cast<size_t>(event.GetInt()) < m_CListbox->GetCount() )
	{
		CountryData* cD;
		if ( (cD = (CountryData*)((dynamic_cast<LBCountry*>(m_CListbox))->GetLParam(event.GetInt()))) != NULL )
		{
			wxSortedArrayString ar = SplitCountries(m_sCountries);
			bool bChecked = (dynamic_cast<LBCountry*>(m_CListbox))->IsChecked(event.GetInt());

			if ( !bChecked )
			{
				if ( !ar.IsEmpty() )
					for ( size_t i = 0; i < ar.GetCount(); i++ )
						if ( ar[i].IsSameAs(cD->m_sISO) || ar[i].IsSameAs(cD->m_sISOdef))
						{
							ar.RemoveAt(i, 1);
							break;
						}
			}
			else
				ar.Add(cD->m_sISO);

			m_sCountries.Empty();
			for ( size_t j = 0; j < ar.GetCount(); j++ )
			{
				if ( !m_sCountries.IsEmpty() )
					m_sCountries += wxT(" ");
				m_sCountries += ar[j];
			}

			TransferDataToWindow();
		}
	}
}

void DCountry::OnSearchBtn(wxCommandEvent& event)
{
	if ( !m_CListbox || m_CListbox->IsEmpty() )
		return;

	wxString order;
	order.Printf(rowset_COUNTRY_DEFORDER, t3Country_CUSTOM, t3Country_DEF);

	TCountry RS;
	RS.SetDatabase(m_pDB);
	wxUint64 rowCnt = 0;

	try
	{
		RS.SetOrder(order);
		RS.Open();
		rowCnt = RS.GetRowCount();
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return;
	}

	if ( rowCnt == 0 )
		return;

	wxUint64 row = DlgCountryFind::FindCountry(this, &RS);

	if ( row > 0 )
	{
		RS.Move(row);
		row = RS.GetActualCurRow();

		CountryData* cD;
		for ( size_t j = 0; j < m_CListbox->GetCount(); j++ )
			if ( (cD = (CountryData*)((dynamic_cast<LBCountry*>(m_CListbox))->GetLParam(j))) != NULL )
				if ( cD->m_nID == row )
				{
					m_CListbox->SetSelection(j);
					m_CListbox->ScrollToRow(j);
					break;
				}
	}
}


