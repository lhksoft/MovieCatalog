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
#include "DlgFind.h"
#include "../lkControls/lkColour.h"

#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/radiobut.h>
#include <wx/sizer.h>
#include <wx/valtext.h>
#include <wx/valgen.h>

#include <wx/log.h>
#include <wx/msgdlg.h>
#include <wx/listimpl.cpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class lkFindParam
////
wxIMPLEMENT_CLASS(lkFindParam, wxObject)

lkFindParam::lkFindParam() : wxObject(), m_sField(), m_sReadable()
{
}
lkFindParam::lkFindParam(const wxString& field, const wxString& readable) : wxObject(), m_sField(field), m_sReadable(readable)
{
}
lkFindParam::~lkFindParam()
{
}
wxString lkFindParam::GetField() const
{
	return m_sField;
}
wxString lkFindParam::GetReadable() const
{
	return m_sReadable;
}

WX_DEFINE_LIST(lkFindList);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class DlgFind
////
wxIMPLEMENT_ABSTRACT_CLASS(DlgFind, wxDialog)

wxBEGIN_EVENT_TABLE(DlgFind, wxDialog)
	EVT_INIT_DIALOG(DlgFind::OnInitDialog)
	EVT_RIGHT_DOWN(DlgFind::OnRightClick)
	EVT_BUTTON(wxID_ANY, DlgFind::OnButton)
wxEND_EVENT_TABLE()

DlgFind::DlgFind(lkSQL3RecordSet* pRS, lkFindList* param) : wxDialog(), m_sSearch()
{
	m_pRS = pRS;
	m_pParams = param;

	m_pFieldCombo = NULL;
	m_pSearchCombo = NULL;
	m_pTextbox = NULL;

	m_nField = m_nSrcOn = 0;
	m_bForward = true;
	m_bBackward = false;
}
DlgFind::~DlgFind()
{
}

bool DlgFind::Create(wxWindow* parent, const wxString& szTitle, long style)
{
	wxString title = (szTitle.IsEmpty()) ? wxT("Find an Item") : szTitle;
	long st = style;
#ifdef __WXDEBUG__
	st |= wxRESIZE_BORDER;
#endif
	if ( !wxDialog::Create(parent, wxID_ANY, title, wxDefaultPosition, wxSize(341,226), st) )
		return false;
	SetMinClientSize(wxSize(325, 187));

	SetBackgroundColour(MAKE_RGB(0xa2, 0x00, 0x65)); // dark pink/purple
	SetForegroundColour(MAKE_RGB(0xff, 0xd5, 0x43)); // warm yellow

	CreateCanvas();
	return true;
}

void DlgFind::CreateCanvas()
{

	wxBoxSizer* szMain = new wxBoxSizer(wxVERTICAL);
	{
		wxBoxSizer* szTop = new wxBoxSizer(wxVERTICAL);

		{
			wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
			szHor->Add(new wxStaticText(this, wxID_ANY, wxT("Field :"), wxDefaultPosition, wxSize(50, -1), wxALIGN_RIGHT), 0, wxFIXED_MINSIZE | wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);
			szHor->Add(m_pFieldCombo = new lkImgComboBox(this, wxID_ANY, wxDefaultSize, wxCB_READONLY | wxCB_SORT, lkImgComboBoxValidator(&m_nField)), 1, wxEXPAND, 0);
			szTop->Add(szHor, 0, wxEXPAND | wxBOTTOM, 5);
		}

		{
			wxStaticBoxSizer* szBox = new wxStaticBoxSizer(wxVERTICAL, this, wxT(" Criteria "));
			{
				wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
				szHor->Add(new wxStaticText(this, wxID_ANY, wxT("Search String :"), wxDefaultPosition, wxDefaultSize), 0, wxFIXED_MINSIZE | wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);

				wxTextCtrl* tC;
				szHor->Add(tC = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_EMPTY, &m_sSearch)), 1, wxEXPAND, 0);
				if ( tC )
					tC->SetToolTip(wxT("% : The percent % wildcard matches any sequence of zero or more characters.\n" \
										"_ : The underscore _ wildcard matches any single character."));
				m_pTextbox = tC;
				szBox->Add(szHor, 0, wxEXPAND | wxALL, 5);
			}

			{
				wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
				{
					wxBoxSizer* szVer = new wxBoxSizer(wxVERTICAL);
					szVer->Add(new wxStaticText(this, wxID_ANY, wxT("Searching from...")), 0, wxBOTTOM, 3);
					szVer->Add(m_pSearchCombo = new lkImgComboBox(this, wxID_ANY, wxDefaultSize, wxCB_READONLY, lkImgComboBoxValidator(&m_nSrcOn)), 0, wxEXPAND, 0);
//					szVer->AddStretchSpacer(1);
					szHor->Add(szVer, 1, wxEXPAND | wxRIGHT, 5);
				}

				{
					wxStaticBoxSizer* szRBox = new wxStaticBoxSizer(wxHORIZONTAL, this, wxT(" Direction "));

					wxRadioButton* rB;
					szRBox->Add(rB = new wxRadioButton(this, wxID_ANY, wxT("Forward"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP, wxGenericValidator(&m_bForward)), 0, wxEXPAND | wxALL, 5);
					if (rB )
						rB->SetToolTip(wxT("Start search from FIRST record"));
					szRBox->Add(rB = new wxRadioButton(this, wxID_ANY, wxT("Backward"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_bBackward)), 0, wxEXPAND | wxTOP | wxBOTTOM | wxRIGHT, 5);
					if ( rB )
						rB->SetToolTip(wxT("Start search from LAST record"));

					szHor->Add(szRBox, 0, wxEXPAND, 0);
				}
				szBox->Add(szHor, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
			}
			szTop->Add(szBox, 0, wxEXPAND | wxBOTTOM, 5);
		}
		szTop->Add(CreateStdDialogButtonSizer(wxOK | wxCANCEL), 0, wxEXPAND);
		szMain->Add(szTop, 0, wxEXPAND | wxALL, 10);
	}

	SetSizer(szMain);
	Layout();
}

//virtual
void DlgFind::DoInit()
{
}

//virtual
void DlgFind::DoOK()
{
}

bool DlgFind::TestFind()
{
	wxString szFind = GetFindString();

	bool bRet = true;
	if ( !(bRet = m_pRS->TestFind(szFind, m_bForward)) )
		wxMessageBox(wxT("No records found."), wxT("Find"), wxOK | wxICON_INFORMATION, this);

	return bRet;
}

wxString DlgFind::GetFindString() const
{
	wxASSERT(m_pRS != NULL);
	wxString szFind;

	lkSQL3RecordSet::FIND_DIRECTION direction = (lkSQL3RecordSet::FIND_DIRECTION)m_nSrcOn;
	lkFindParam* p = (lkFindParam*)m_nField;

	m_pRS->CreateFindFilter(szFind, p->GetField(), m_sSearch, direction);

	return szFind;
}

bool DlgFind::IsForward() const
{
	return m_bForward;
}

// Event Handling
////////////////////////////////////////////////////////

void DlgFind::OnInitDialog(wxInitDialogEvent& event)
{
	if ( m_pSearchCombo )
	{
		struct _search_opt_
		{
			lkSQL3RecordSet::FIND_DIRECTION	nVal;
			const wxChar*					sName;
		} SearchOpt[] = {
			{ lkSQL3RecordSet::FIND_DIRECTION::fndStart, wxT("Start") },
			{ lkSQL3RecordSet::FIND_DIRECTION::fndEnd, wxT("End") },
			{ lkSQL3RecordSet::FIND_DIRECTION::fndAnywhere, wxT("Anywhere") },
			{ lkSQL3RecordSet::FIND_DIRECTION::fndEntire, wxT("Entire") },
			{ lkSQL3RecordSet::FIND_DIRECTION::fndINVALID }
		};

		lkBoxInitList initList;
		initList.DeleteContents(true);
		for ( int i = 0; SearchOpt[i].nVal != lkSQL3RecordSet::FIND_DIRECTION::fndINVALID; i++ )
			initList.Append(new lkBoxInit(SearchOpt[i].sName, (wxUint64)SearchOpt[i].nVal));

		m_nSrcOn = (wxUint64)lkSQL3RecordSet::FIND_DIRECTION::fndStart; // default

		if ( m_pSearchCombo->InitializeFromList(&initList, true) )
			m_pSearchCombo->Populate();
	}

	if ( m_pFieldCombo && m_pParams && !m_pParams->IsEmpty() )
	{
		lkBoxInitList initList;
		initList.DeleteContents(true);
		bool bFirst = true;
		for ( lkFindList::iterator iter = m_pParams->begin(); iter != m_pParams->end(); ++iter )
		{
			lkFindParam* current = *iter;
			initList.Append(new lkBoxInit(current->GetReadable(), (wxUint64)current));
			if ( bFirst )
			{
				// make first 1 default
				m_nField = (wxUint64)current;
				bFirst = false;
			}
		}
		if ( m_pFieldCombo->InitializeFromList(&initList, true) )
			m_pFieldCombo->Populate();
	}

	DoInit();
	m_bBackward = !m_bForward;

	Layout();
	wxDialog::OnInitDialog(event);

	if ( m_pTextbox )
		m_pTextbox->SetFocus();
}

void DlgFind::OnRightClick(wxMouseEvent& event)
{
	if (wxLog::GetLogLevel() == wxLOG_Debug)
	{
		wxSize sizeC = GetClientSize();
		wxSize sizeF = GetSize();
		wxString s; s.Printf(wxT("Frame Size\nwidth = %d | height = %d\n\nClient Size\nwidth = %d | height = %d"), sizeF.GetWidth(), sizeF.GetHeight(), sizeC.GetWidth(), sizeC.GetHeight());
		wxMessageBox(s, wxT("For your information"), wxOK | wxICON_INFORMATION, this);
	}
	else
		event.Skip();
}

void DlgFind::OnButton(wxCommandEvent& event)
{
	const int id = event.GetId();
	if ( id == GetAffirmativeId() )
	{
		if ( Validate() && TransferDataFromWindow() )
			if ( TestFind() )
			{
				DoOK();
				EndDialog(m_affirmativeId);
			}
	}
	else
		event.Skip();
}

