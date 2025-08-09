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
#include "DlgStorageFilter.h"
#include "CStorage.h"
#include "SimpleBox.h"

#include <wx/sizer.h>
#include <wx/valgen.h>
#include <wx/valtext.h>
#include <wx/stattext.h>
#include <wx/radiobut.h>
#include <wx/button.h>
#include <wx/statbox.h>

#include "../lkControls/lkTransTextCtrl.h"

#include "FilterCtrls.h"

#include "../lkControls/lkColour.h"
#include "../lkControls/lkConfigTools.h"

#include "../lkSQLite3/lkSQL3Field.h"
#include "TStorage.h"

#include "../lkSQLite3/lkSQL3Exception.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class StorageFilterDlg
////
wxIMPLEMENT_CLASS(StorageFilterDlg, DlgFilter)

StorageFilterDlg::StorageFilterDlg(lkSQL3Database* DB) : DlgFilter(DB)
{
	m_eSortBy = Sorting::Storage;
}

bool StorageFilterDlg::Create(wxWindow* parent)
{
	wxSize size =
#ifdef __WXMSW__
		wxSize(520, 245)
#else
		wxSize(520, 270)
#endif
		;
	bool b = DlgFilter::Create(parent, wxT("Storage Filter"), size);

	size =
#ifdef __WXMSW__
		wxSize(504, 206)
#else
		wxSize(520, 246)
#endif
		;
	SetMinClientSize(size);
	return b;
}

//virtual
wxString StorageFilterDlg::GetConfigSection() const
{
	return CStorage::GetConfigKeyString();
}

//virtual
void StorageFilterDlg::AddCanvas()
{
	m_nID_Storage = wxID_HIGHEST + wxWindowID(IDS::Storage);
	m_nID_Location = wxID_HIGHEST + wxWindowID(IDS::Location);
	m_nID_Medium = wxID_HIGHEST + wxWindowID(IDS::Medium);

	wxBoxSizer* szMain = new wxBoxSizer(wxVERTICAL);
	wxSize rdSize(95, -1);

	/* ***********************************************************************************************
	 * First Group
	 * *********************************************************************************************** */
	{
		wxBoxSizer* szTop = new wxBoxSizer(wxHORIZONTAL);
		/* *******************************************************************************************
		 * Sorting
		 * ******************************************************************************************* */
		{
			wxBoxSizer* szLeft = new wxBoxSizer(wxVERTICAL);
			szLeft->AddStretchSpacer(1);
			szLeft->Add(new wxStaticText(this, wxID_ANY, wxT("Sorting"), wxDefaultPosition, wxDefaultSize, 0), 0, wxBOTTOM | wxEXPAND, 3);
			szLeft->Add(m_pSorting = new SortComboBox(this, lkImgComboBoxValidator(&m_nSort)), 0, wxRIGHT | wxEXPAND, 5);
			szLeft->AddStretchSpacer(1);
			szTop->Add(szLeft, 1, wxRIGHT | wxEXPAND, 5);
		}
		/* *******************************************************************************************
		 * 
		 * ******************************************************************************************* */
		{
			szTop->AddStretchSpacer(1);
		}
		/* *******************************************************************************************
		 * OK / Cancel
		 * ******************************************************************************************* */
		{
			wxBoxSizer* szRight = new wxBoxSizer(wxVERTICAL);
			wxButton* ok;
			szRight->Add(ok = new wxButton(this, wxID_OK), 0, wxBOTTOM, 5);
			ok->SetDefault();
			szRight->Add(new wxButton(this, wxID_CANCEL), 0);
			szTop->Add(szRight, 0, wxLEFT | wxEXPAND, 5);
		}
		szMain->Add(szTop, 0, wxLEFT | wxTOP | wxRIGHT | wxEXPAND, 10);
	}
	szMain->Add(0, 0, 0, wxBOTTOM | wxEXPAND, 5);
	/* ***********************************************************************************************
	 * Criteria Group
	 * *********************************************************************************************** */
	wxStaticBoxSizer* szBox = new wxStaticBoxSizer(wxVERTICAL, this, wxT(" Criteria "));


	/* ***********************************************************************************************
	 * Storages Group
	 * *********************************************************************************************** */
	wxWindow* parent = szBox->GetStaticBox();
	/* *******************************************************************************************
	 * by Storage
	 * ******************************************************************************************* */
	{
		wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
		/* ***************************************************************************************
		 * radio-button
		 * *************************************************************************************** */
		{
			wxBoxSizer* szVer = new wxBoxSizer(wxVERTICAL);
			szVer->AddStretchSpacer(1);
			szVer->Add(m_pRd_Storage = new wxRadioButton(parent, m_nID_Storage, wxT("Storage"), wxDefaultPosition, rdSize,
														 wxALIGN_RIGHT | wxRB_GROUP, wxGenericValidator(&m_bStorage)), 0, wxFIXED_MINSIZE);
			szHor->Add(szVer, 0, wxRIGHT | wxEXPAND, 10);
		}
		/* ***************************************************************************************
		 * string to filter
		 * *************************************************************************************** */
		{
			wxBoxSizer* szVer = new wxBoxSizer(wxVERTICAL);
			szVer->Add(m_pFilterStrStat = new wxStaticText(parent, wxID_ANY, wxT("Filter-String :")), 0, wxBOTTOM | wxFIXED_MINSIZE, 3);
			szVer->Add(m_pFilterStr = new lkTransTextCtrl(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200, -1), 0, wxTextValidator(wxFILTER_EMPTY, &m_sFilter)), 0, wxEXPAND);
			m_pFilterStr->SetForegroundColour(lkCol_Yellow);
			szHor->Add(szVer, 1, wxRIGHT | wxEXPAND, 5);
		}
		/* ***************************************************************************************
		 * occurance
		 * *************************************************************************************** */
		{
			wxBoxSizer* szVer = new wxBoxSizer(wxVERTICAL);
			szVer->Add(m_pOccStat = new wxStaticText(parent, wxID_ANY, wxT("Occurance :")), 0, wxBOTTOM | wxFIXED_MINSIZE, 3);
			szVer->Add(m_pOccCombo = new lkImgComboBox(parent, wxID_ANY, wxDefaultSize, wxCB_READONLY, lkImgComboBoxValidator(&m_nOcc)), 0, wxEXPAND);
			szHor->Add(szVer, 0, wxEXPAND);
		}

		szBox->Add(szHor, 0, wxALL | wxEXPAND, 5);
	}
	/* *******************************************************************************************
	 * by Location
	 * ******************************************************************************************* */
	{
		wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
		/* ***************************************************************************************
		 * radio-button
		 * *************************************************************************************** */
			szHor->Add(m_pRd_Location = new wxRadioButton(parent, m_nID_Location, wxT("Location"), wxDefaultPosition, rdSize,
														 wxALIGN_RIGHT, wxGenericValidator(&m_bLocation)), 0, wxRIGHT | wxFIXED_MINSIZE | wxALIGN_CENTER_VERTICAL, 10);
		/* ***************************************************************************************
		 * Location Combo
		 * *************************************************************************************** */
		szHor->Add(m_pLocation = new LocationBox(parent, wxDefaultSize, lkImgComboBoxValidator(&m_nLocation)), 1, wxEXPAND);
		(dynamic_cast<LocationBox*>(m_pLocation))->SetDatabase(m_pDB);

		szBox->Add(szHor, 0, wxLEFT | wxBOTTOM | wxRIGHT | wxEXPAND, 5);
	}
	/* *******************************************************************************************
	 * by Medium
	 * ******************************************************************************************* */
	{
		wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
		/* ***************************************************************************************
		 * radio-button
		 * *************************************************************************************** */
		szHor->Add(m_pRd_Medium = new wxRadioButton(parent, m_nID_Medium, wxT("Medium"), wxDefaultPosition, rdSize,
													wxALIGN_RIGHT, wxGenericValidator(&m_bMedium)), 0, wxRIGHT | wxFIXED_MINSIZE | wxALIGN_CENTER_VERTICAL, 10);
		/* ***************************************************************************************
		 * Medium Combo
		 * *************************************************************************************** */
		szHor->Add(m_pMedium = new MediumBox(parent, wxDefaultSize, lkImgComboBoxValidator(&m_nMedium)), 1, wxEXPAND);
		(dynamic_cast<MediumBox*>(m_pMedium))->SetDatabase(m_pDB);

		szBox->Add(szHor, 0, wxLEFT | wxBOTTOM | wxRIGHT | wxEXPAND, 5);
	}

	szMain->Add(szBox, 1, wxLEFT | wxBOTTOM | wxRIGHT | wxEXPAND, 10);
	SetSizer(szMain);
}


//virtual
bool StorageFilterDlg::TestFilter()
{
	wxASSERT(m_pDB && m_pDB->IsOpen());
	bool bRet = false;

	wxString qry;
	qry.Printf(wxT("SELECT COUNT([ROWID]) FROM [%s] WHERE %s;"), t3Storage, GetFilter());
	try
	{
		bRet = (((wxUint64)m_pDB->ExecuteSQLAndGet(qry)) > 0);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		bRet = false;
	}

	return bRet;
}

//static
void StorageFilterDlg::CorrectConfig(lkSQL3Database* dbTmp)
{
	if ( !dbTmp || !dbTmp->IsOpen() )
		return;

	wxString section = CStorage::GetConfigKeyString();
	DlgFilter::DoCorrectLocConfig(section, dbTmp);
	DlgFilter::DoCorrectMedConfig(section, dbTmp);
}

