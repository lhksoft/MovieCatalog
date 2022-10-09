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
#include "DlgSubCat.h"
#include "lkSQL3Valid.h"
#include "ImageCtrl.h"
//#include "Defines.h"

#include <wx/image.h>
#include <wx/bitmap.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>

#include "../lkSQLite3/lkSQL3Validators.h"
#include "../lkSQLite3/lkSQL3Exception.h"

#include "Backgrounds.h"

// ------------------------------------------------------------------------------------------------------------ //
wxIMPLEMENT_DYNAMIC_CLASS(SubCatDlg, lkDialog)

wxBEGIN_EVENT_TABLE(SubCatDlg, lkDialog)
	EVT_BUTTON(wxID_OK, SubCatDlg::OnOK)
	EVT_BUTTON(wxID_CANCEL, SubCatDlg::OnCancel)
wxEND_EVENT_TABLE()


SubCatDlg::SubCatDlg() : lkDialog()
{
	Init();
}
SubCatDlg::SubCatDlg(lkSQL3Database* pDB, wxUint32 nCat, wxUint32 nSub) : lkDialog(), m_ConfigSection()
{
	Init();
	wxASSERT(pDB && pDB->IsOpen());
	wxASSERT(nCat > 0);
	m_pDB = pDB;
	m_nCat = nCat;
	m_nSub = nSub;

//	(void)Create(pParent);
}
SubCatDlg::~SubCatDlg()
{
	if ( m_pSet )
		delete m_pSet;
}

void SubCatDlg::Init()
{
	m_pSet = NULL;
	m_pDB = NULL;
	m_nCat = m_nSub = 0;
	m_ConfigSection.Empty();
}

void SubCatDlg::SetConfigSection(const wxString& k)
{
	m_ConfigSection = k;
}

bool SubCatDlg::Create(wxWindow* pParent)
{
	if ( !m_pDB || (m_nCat == 0) ) return false;

	// Open the Recordset first
	if ( m_nSub == 0 )
	{ // Add Modus
		wxUint32 f = TCategory::GetFreeSubID(m_pDB, m_nCat);
		if ( f == 0 ) return false;

		if ( !(m_pSet = new TCategory(m_pDB)) )
			return false;
		try
		{
			m_pSet->Open();
			m_pSet->AddNew();
			m_pSet->SetCatValue(m_nCat);
			m_pSet->SetSubValue(f);
		}
		catch ( const lkSQL3Exception& e )
		{
			((lkSQL3Exception*)&e)->ReportError();
			return false;
		}
	}
	else
	{ // Edit Modus
		if ( !(m_pSet = new TCategory(m_pDB)) )
			return false;
		{
			wxString qry;
			qry.Printf(wxT("([%s] = %u) AND ([%s] == %u)"), t3Category_catID, m_nCat, t3Category_subID, m_nSub);
			m_pSet->SetFilter(qry);
		}
		try
		{
			m_pSet->Open();
		}
		catch ( const lkSQL3Exception& e )
		{
			((lkSQL3Exception*)&e)->ReportError();
			return false;
		}
	}
	wxString ttl; ttl.Printf(wxT("%s SubCategory"), ((m_nSub == 0) ? wxT("Add") : wxT("Edit")));
	if ( !lkDialog::Create(pParent, GetImage_FBG_CATEGORY(), wxID_ANY, ttl, wxDefaultPosition, wxSize(350, 140), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER) )
		return false;
	SetMinClientSize(wxSize(334, 101));


	wxBoxSizer* szMain = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* szTop = new wxBoxSizer(wxHORIZONTAL);
	{
		ImageCtrl* iCtrl;
		szTop->Add(iCtrl = new ImageCtrl(this, wxSize(36, 36), wxID_ANY, lkSQL3ImageValidator(m_pSet->m_pFldImage)), 0, wxRIGHT | wxFIXED_MINSIZE, 10);
		iCtrl->SetConfigSection(m_ConfigSection);
	}
	/* *******************************************************************************
	 * Category Name (EditCtrl) + Sort-By checkbox
	 * ******************************************************************************* */
	wxBoxSizer* szVer = new wxBoxSizer(wxVERTICAL);
	szVer->AddStretchSpacer(1);
	szVer->Add(new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_LEFT,
							  lkSQL3TextValidator(m_pSet->m_pFldName, wxFILTER_NONE, &TCategory::IsUniqueName, m_pDB, &m_nCat)), 0, wxEXPAND);
	szVer->AddStretchSpacer(1);
	szTop->Add(szVer, 1, wxEXPAND);


	szMain->Add(szTop, 0, wxALL | wxEXPAND, 10);

	wxSizer* btSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL | wxOK_DEFAULT);
	if ( btSizer )
		szMain->Add(btSizer, 0, wxLEFT | wxBOTTOM | wxRIGHT | wxEXPAND, 10);

	SetSizer(szMain);
	Layout();

	return true;
}

void SubCatDlg::OnOK(wxCommandEvent& event)
{
	if ( Validate() )
		if ( TransferDataFromWindow() )
		{
			if ( m_pSet->IsDirty(NULL) )
			{
				try
				{
					m_pSet->Update();
				}
				catch ( const lkSQL3Exception& e )
				{
					((lkSQL3Exception*)&e)->ReportError();
					return;
				}
			}
			event.Skip();
		}
	// no good
/*
	wxSize size = GetSize();
	wxString s = s.Format(wxT("width = %d\nheight = %d"), size.GetWidth(), size.GetHeight());
	wxMessageBox(s, wxT("For your information"), wxOK | wxICON_INFORMATION, this);
*/
}
void SubCatDlg::OnCancel(wxCommandEvent& event)
{
	if ( m_pSet->IsAdding() )
		m_pSet->CancelAddNew();
	else
		m_pSet->CancelUpdate();

	event.Skip();
}
