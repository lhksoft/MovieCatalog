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
#include "CStorage.h"
#include "Backgrounds.h"
#include "lkSQL3Valid.h"
#include "DlgDate.h"

#include "TStorage.h"

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/font.h>
#include <wx/button.h>
#include <wx/config.h>
#include <wx/frame.h>
#include <wx/dialog.h>

#include <wx/log.h>

#include "../lkControls/lkColour.h"
#include "../lkSQLite3/lkDateTime.h"
#include "../lkControls/lkConfigTools.h"

#include "../lkSQLite3/lkSQL3Validators.h"
#include "../lkSQLite3/lkSQL3Exception.h"

#define conf_STORAGE_PATH			wxT("Storages")

///////////////////////////////////////////////////////////////////////////////////////////////
// class CStorage
////
CStorage::CStorage() : m_CreaDate()
{
	m_pParentFrame = NULL;
	m_pParentPanel = NULL;

	m_pStorageCtrl = NULL;
	m_pMedium = NULL;
	m_pLocation = NULL;
	m_pCreaDate = NULL;

	m_bReqAdd = false;

	m_ID_DateBtn = wxWindow::NewControlId(1);
}
CStorage::~CStorage()
{
}

//static
wxString CStorage::GetConfigKeyString()
{
	wxString s = conf_STORAGE_PATH;
	return s;
}

wxImage CStorage::GetBackground() const
{
	return GetImage_FBG_STORAGES();
}

void CStorage::SetStorageFocus()
{
	if ( m_pStorageCtrl )
		m_pStorageCtrl->SetFocus();
}

bool CStorage::stTransferDataToWindow(bool bInitial)
{
	if ( m_pMedium )
		if ( bInitial || IsMediumChanged() )
			m_pMedium->BuildList(!bInitial);

	if ( m_pLocation )
		if ( bInitial || IsLocationChanged() )
			m_pLocation->BuildList(!bInitial);

	return true;
}

bool CStorage::CreateCanvas(wxWindow* parentFrame, wxWindow* parentPanel)
{
	if ( !(parentFrame && (parentFrame->IsKindOf(wxCLASSINFO(wxFrame)) || parentFrame->IsKindOf(wxCLASSINFO(wxDialog)))) )
		return false;
	if ( !(parentPanel && (parentPanel->IsKindOf(wxCLASSINFO(wxPanel)) || parentPanel->IsKindOf(wxCLASSINFO(wxDialog)))) )
		return false;

	m_pParentFrame = parentFrame;
	m_pParentPanel = parentPanel;

	m_pParentPanel->SetForegroundColour(lkCol_White);

	wxBoxSizer* szMain = new wxBoxSizer(wxVERTICAL);
	wxFlexGridSizer* szGrid = new wxFlexGridSizer(2, 2, 10, 10);

	// 1st Line
	{
		// 1st item
		{
			wxBoxSizer* szVer = new wxBoxSizer(wxVERTICAL);
			szVer->Add(new wxStaticText(m_pParentPanel, wxID_ANY, wxT("Storage :"), wxDefaultPosition, wxSize(60, -1), wxALIGN_LEFT | wxST_NO_AUTORESIZE), 0, wxBOTTOM, 3);

			szVer->Add(m_pStorageCtrl = new lkTransTextCtrl(m_pParentPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(245, -1), wxTE_CENTER, lkSQL3TextValidator(GetFldStorage())), 0);
			wxFont f = m_pStorageCtrl->GetFont();
			f.SetFamily(wxFONTFAMILY_MODERN);
			f.MakeLarger();
#ifdef __WXMSW__
			f.MakeLarger();
#endif
			f.MakeBold();
			m_pStorageCtrl->SetFont(f);
			m_pStorageCtrl->SetForegroundColour(MAKE_RGB(0xC0, 0xFF, 0xC0)); // light kaki green
#ifndef __WXMSW__
			// transparant background only supported in wxMSW
			m_pStorageCtrl->SetBackgroundColour(MAKE_RGB(0x68, 0x88, 0x6F)); // dark-Kaki-Green
#endif
			szGrid->Add(szVer, 0, wxEXPAND);
		}
		// 2nd item
		{
			wxBoxSizer* szVer = new wxBoxSizer(wxVERTICAL);
			szVer->Add(new wxStaticText(m_pParentPanel, wxID_ANY, wxT("Medium :"), wxDefaultPosition, wxSize(60, -1), wxALIGN_LEFT | wxST_NO_AUTORESIZE), 0, wxBOTTOM, 3);
			szVer->Add(m_pMedium = new MediumBox(m_pParentPanel, 
#ifdef __WXMSW__
			wxSize(245, -1)
#else
			wxSize(265, -1)
#endif
			, lkSQL3ImgComboValidator(GetFldMedium(), wxT("Medium box"), &m_bReqAdd)), 0, wxFIXED_MINSIZE);
			m_pMedium->SetDatabase(GetDBase());
			m_pMedium->SetBackgroundColour(MAKE_RGB(0xBB, 0xDD, 0xBB)); // light kaki grayisch-green
			m_pMedium->SetForegroundColour(MAKE_RGB(0x00, 0x00, 0xC0)); // lighter dark-blue
			m_pMedium->SetSelectedBackColour(MAKE_RGB(0x7D, 0xAE, 0xFF)); // pastel dark-blue
			m_pMedium->SetSelectedForeColour(lkCol_White);

			szGrid->Add(szVer, 0, wxEXPAND);
		}
	}

	// 2nd Line
	{
		// 1st item
		{
#if 0
			wxLog* curLog = wxLog::GetActiveTarget();
			wxLog* logger = new wxLogStderr();
			logger->SetLogLevel( wxLogLevelValues::wxLOG_Debug );
			wxLog::SetActiveTarget(logger);

			wxSize butSize = wxButton::GetDefaultSize();
			wxLogInfo( wxT("button default size(%d, %d)\n"), butSize.GetWidth(), butSize.GetHeight());
			butSize.SetHeight(25);
			butSize.SetWidth(130);
			wxLogInfo( wxT("button new size(%d, %d)\n"), butSize.GetWidth(), butSize.GetHeight());
			wxLog::FlushActive();
			wxLog::SetActiveTarget(curLog);
			delete logger; logger=NULL;
#endif
			wxBoxSizer* szVer = new wxBoxSizer(wxVERTICAL);
#ifdef __WXMSW__
			szVer->Add(new wxButton(m_pParentPanel, m_ID_DateBtn, wxT("Creation Date")), 0, wxBOTTOM, 3);
#else
			szVer->Add(new wxButton(m_pParentPanel, m_ID_DateBtn, wxT("Creation Date"), wxDefaultPosition, wxSize(130,25)), 0, wxBOTTOM, 3);
#endif
			lkSQL3DateValidator* dVal = new lkSQL3DateValidator(&m_CreaDate, GetFldCreation(), GetBaseRecordset(), true, true);
			dVal->AddOnlyRequired(true);
			szVer->Add(m_pCreaDate = new lkTransTextCtrl(m_pParentPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(245, -1), wxTE_READONLY | wxTE_CENTER, *dVal), 0, wxFIXED_MINSIZE);
			delete dVal;
			m_pCreaDate->SetForegroundColour(lkCol_Yellow);
#ifndef __WXMSW__
			// transparant background only supported in wxMSW
			m_pCreaDate->SetBackgroundColour(MAKE_RGB(0x68, 0x88, 0x6F)); // dark-Kaki-Green
#endif
			szGrid->Add(szVer, 0, wxALIGN_TOP | wxEXPAND);
		}
		// 2nd item
		wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
		{
			wxBoxSizer* szVer = new wxBoxSizer(wxVERTICAL);
			szVer->Add(new wxStaticText(m_pParentPanel, wxID_ANY, wxT("Location :"), wxDefaultPosition, wxSize(60, -1), wxALIGN_LEFT | wxST_NO_AUTORESIZE), 0, wxBOTTOM, 3);
			szVer->Add(m_pLocation = new LocationBox(m_pParentPanel, 
#ifdef __WXMSW__
			wxSize(170, -1)
#else
			wxDefaultSize
#endif
			, lkSQL3ImgComboValidator(GetFldLocation(), wxT("Location box"), &m_bReqAdd)), 
#ifdef __WXMSW__
			0, wxFIXED_MINSIZE
#else
			1, wxEXPAND
#endif
			);
			m_pLocation->SetDatabase(GetDBase());
			m_pLocation->SetBackgroundColour(MAKE_RGB(0xBB, 0xDD, 0xBB)); // light kaki grayisch-green
			m_pLocation->SetForegroundColour(MAKE_RGB(0x00, 0x00, 0xC0)); // lighter dark-blue
			m_pLocation->SetSelectedBackColour(MAKE_RGB(0x7D, 0xAE, 0xFF)); // pastel dark-blue
			m_pLocation->SetSelectedForeColour(lkCol_White);
			szHor->Add(szVer, 1, wxRIGHT | wxEXPAND, 5);
		}
		// 3th item
		{
			wxBoxSizer* szVer = new wxBoxSizer(wxVERTICAL);
			szVer->Add(new wxStaticText(m_pParentPanel, wxID_ANY, wxT("Info :"), wxDefaultPosition, wxSize(60, -1), wxALIGN_LEFT | wxST_NO_AUTORESIZE), 0, wxBOTTOM, 3);
			lkTransTextCtrl* t;
			szVer->Add(t = new lkTransTextCtrl(m_pParentPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(70, -1), wxTE_RIGHT,
											   lkSQL3TextValidator(GetFldInfo(), wxFILTER_DIGITS)), 0, wxFIXED_MINSIZE);
			t->SetForegroundColour(MAKE_RGB(0xC0, 0xFF, 0xC0)); // light kaki green
#ifndef __WXMSW__
			// transparant background only supported in wxMSW
			t->SetBackgroundColour(MAKE_RGB(0x68, 0x88, 0x6F)); // dark-Kaki-Green
#endif
			szHor->Add(szVer);
		}
		szGrid->Add(szHor, 0, wxALIGN_BOTTOM | wxEXPAND);
	}
	szMain->Add(szGrid, 0, wxALL, 10);
	m_pParentPanel->SetSizer(szMain);
	m_pParentPanel->Layout();

	return true;

}

void CStorage::OnSelectDate(wxCommandEvent& WXUNUSED(event))
{
	DlgDate dlg(m_CreaDate);
	if ( dlg.Create(m_pParentFrame) )
		if ( dlg.ShowModal() == wxID_OK )
		{
			m_CreaDate = dlg.GetDate();

			if ( m_pCreaDate )
			{
				wxString szTxt = m_CreaDate.FormatFullDate_Dutch();
				if ( szTxt.IsEmpty() )
					szTxt = wxT("# unset #");
				m_pCreaDate->SetLabel(szTxt);
			}
		}
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// class DlgStorageFind
////
wxIMPLEMENT_ABSTRACT_CLASS(DlgStorageFind, DlgFind)

DlgStorageFind::DlgStorageFind(lkSQL3RecordSet* pRS, lkFindList* param) : DlgFind(pRS, param)
{
}

bool DlgStorageFind::Create(wxWindow* parent, long style)
{
	return DlgFind::Create(parent, wxT("Find a Storage"), style);
}

//static
wxUint64 DlgStorageFind::FindStorage(wxWindow* parent, lkSQL3RecordSet* pRS)
{
	if ( !parent || !pRS || !pRS->IsOpen() )
		return 0;

	wxUint64 row = 0;

	lkFindList paramList;
	paramList.DeleteContents(true);
	paramList.Append(new lkFindParam(t3Storage_NAME, wxT("Storage Label")));

	DlgStorageFind dlg(pRS, &paramList);
	if ( dlg.Create(parent) && (dlg.ShowModal() == wxID_OK) )
	{
		wxString szFind = dlg.GetFindString();
		row = pRS->FindInit(wxCLASSINFO(TStorage), szFind, dlg.IsForward());
	}

	return row;
}

//virtual
void DlgStorageFind::DoInit()
{
	wxInt64 ll = GetConfigInt(conf_STORAGE_PATH, conf_FIND_DIRECTION);
	if ( (ll > (wxInt64)lkSQL3RecordSet::FIND_DIRECTION::fndINVALID) && (ll <= (wxInt64)lkSQL3RecordSet::FIND_DIRECTION::fndLAST) )
		m_nSrcOn = (wxUint64)ll;

	m_bForward = GetConfigBool(conf_STORAGE_PATH, conf_FIND_FORWARD, true);

	if ( m_pParams && !m_pParams->IsEmpty() )
	{
		ll = GetConfigInt(conf_STORAGE_PATH, conf_FIND_FIELD);
		if ( ll <= 0 )
			ll = 1;

		wxInt64 i = 0;
		for ( lkFindList::iterator iter = m_pParams->begin(); iter != m_pParams->end(); ++iter )
		{
			lkFindParam* current = *iter;
			if ( ++i == ll )
			{
				m_nField = (wxUint64)current;
				break;
			}
		}
	}
}

//virtual
void DlgStorageFind::DoOK()
{
	wxInt64 ll = (wxInt64)m_nSrcOn;
	SetConfigInt(conf_STORAGE_PATH, conf_FIND_DIRECTION, ll, (wxInt64)lkSQL3RecordSet::FIND_DIRECTION::fndStart);

	SetConfigBool(conf_STORAGE_PATH, conf_FIND_FORWARD, m_bForward, true);

	if ( m_pParams && !m_pParams->IsEmpty() && (m_nField != 0) )
	{
		ll = 0;
		for ( lkFindList::iterator iter = m_pParams->begin(); iter != m_pParams->end(); ++iter )
		{
			lkFindParam* current = *iter;
			ll++;
			if ( m_nField == (wxUint64)current )
			{
				SetConfigInt(conf_STORAGE_PATH, conf_FIND_FIELD, ll, 1);
				break;
			}
		}
	}
}

