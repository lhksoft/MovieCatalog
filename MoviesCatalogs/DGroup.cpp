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
#include "DGroup.h"
#include "CanvasCtrl.h"
#include "DStorage.h"

#include "QStorage.h"
#include "TStorage.h"

#include "XPMs.h"
#include "Backgrounds.h"

#include "DGroup_validators.h"
#include "../lkSQLite3/lkSQL3Validators.h"

#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/font.h>
#include <wx/bmpbuttn.h>
#include <wx/textctrl.h>

#include "../lkControls/lkStaticImage.h"
#include "../lkControls/lkColour.h"
#include "../lkSQLite3/lkDateTime.h"

#include "../lkSQLite3/lkSQL3Exception.h"

//////////////////////////////////////////////////////////////////////////////////////
// class GroupStorageCanvas : a wxPanel acting as a Control inside DGroup
class GroupStorageCanvas : public CanvasCtrl, public QStorage
{
public:
	GroupStorageCanvas(wxWindow*, lkSQL3Database*, const wxValidator& validator = wxDefaultValidator);

	bool					Create(wxWindow* pParent, const wxValidator& validator = wxDefaultValidator);

	virtual void			SetInternalID(wxUint64) wxOVERRIDE;

// Attributes
private:
	wxWindowID				m_nBtnCopyTitle;

	wxDECLARE_CLASS(GroupStorageCanvas);
	wxDECLARE_NO_COPY_CLASS(GroupStorageCanvas);
};

wxIMPLEMENT_CLASS2(GroupStorageCanvas, CanvasCtrl, QStorage)


GroupStorageCanvas::GroupStorageCanvas(wxWindow* parent, lkSQL3Database* pDB, const wxValidator& validator) : CanvasCtrl(), QStorage(pDB)
{
	m_nBtnCopyTitle = wxWindow::NewControlId(1);

	(void)GroupStorageCanvas::Create(parent, validator);
}

//virtual
void GroupStorageCanvas::SetInternalID(wxUint64 nID)
{
	CanvasCtrl::SetInternalID(nID);

	QStorage* pSet = dynamic_cast<QStorage*>(this);
	if ( !pSet->IsOpen() ) return;

	if ( m_bValid )
	{
		SetStorageFilter(m_nInternalID);
		try
		{
			pSet->Requery();
		}
		catch ( const lkSQL3Exception& e )
		{
			((lkSQL3Exception*)&e)->ReportError();
			return;
		}
	}
}

bool GroupStorageCanvas::Create(wxWindow* pParent, const wxValidator& validator)
{
	if ( !(CanvasCtrl::Create(pParent, GetImage_FBG_STORAGES(), wxDefaultSize, wxBORDER_SUNKEN, validator)) )
		return false;

	QStorage* pSet = dynamic_cast<QStorage*>(this);
	// Open and Initialize TMovies first
	if ( !pSet->IsOpen() )
	{
		pSet->SetOrder(wxT("[ROWID]"));
		try
		{
			pSet->Open();
		}
		catch ( const lkSQL3Exception& e )
		{
			((lkSQL3Exception*)&e)->ReportError();
			return false;
		}
	}

	SetForegroundColour(lkCol_White);
	lkColour color(MAKE_RGB(0xc0, 0xff, 0xc0)); // light green

	lkStaticText* sT;
	wxBoxSizer* szMain = new wxBoxSizer(wxVERTICAL);
	// 1st Line
	{
		wxBoxSizer* szTop = new wxBoxSizer(wxHORIZONTAL);
		/* *******************************************************************************
		 * Poster (StaticImage)
		 * ******************************************************************************* */
		szTop->Add(new lkStaticImage(this, wxSize(75, -1), wxID_ANY, DGroupStaticImageValidator(m_pFldImage, &m_bValid)), 0, wxRIGHT | wxBOTTOM | wxFIXED_MINSIZE | wxEXPAND, 5);
		/* *******************************************************************************
		 * Storage Label
		 * ******************************************************************************* */
		szTop->Add(m_pTitleCtrl = sT = new lkStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 
														wxBORDER_SUNKEN | wxST_NO_AUTORESIZE, lkSQL3StaticTxtValidator(m_pFldStorage, &m_bValid)), 1, wxBOTTOM | wxEXPAND, 5);
		sT->SetFont(wxFont(
#ifdef __WXMSW__
			wxSize(0, 23)
#else
			wxSize(0, 32)
#endif // __WXMSW__
			, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Comic Sans MS")));
		sT->SetForegroundColour(color);
		/* *******************************************************************************
		 * Copy button
		 * ******************************************************************************* */
		wxBitmapButton* but;
		szTop->Add(but = new wxBitmapButton(this, m_nBtnCopyTitle, Get_Copy24_Xpm()), 0, wxLEFT | wxBOTTOM | wxALIGN_CENTER_VERTICAL, 5);
		but->SetToolTip(wxT("Copy Storage label to Clipboard"));

		szMain->Add(szTop, 0, wxLEFT | wxTOP | wxRIGHT | wxEXPAND, 10);
	}
	// 2nd Line
	{
		wxBoxSizer* szTop = new wxBoxSizer(wxHORIZONTAL);
		/* *******************************************************************************
		 * Location
		 * ******************************************************************************* */
		{
			wxBoxSizer* szVer = new wxBoxSizer(wxVERTICAL);
			szVer->Add(new wxStaticText(this, wxID_ANY, wxT("Location :"), wxDefaultPosition, wxDefaultSize), 0, wxBOTTOM | wxALIGN_CENTER, 2);
			szVer->Add(sT = new lkStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 
											 wxALIGN_CENTRE_HORIZONTAL | wxBORDER_SUNKEN | wxST_NO_AUTORESIZE, lkSQL3StaticTxtValidator(m_pFldLocation, &m_bValid)), 1, wxEXPAND);
			sT->SetForegroundColour(color);
			szTop->Add(szVer, 1, wxRIGHT | wxEXPAND, 5);
		}
		/* *******************************************************************************
		 * Info
		 * ******************************************************************************* */
		{
			wxBoxSizer* szVer = new wxBoxSizer(wxVERTICAL);
			szVer->Add(new wxStaticText(this, wxID_ANY, wxT("Info :"), wxDefaultPosition, wxDefaultSize), 0, wxBOTTOM | wxALIGN_CENTER, 2);
			szVer->Add(sT = new lkStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(50, -1), 
											 wxALIGN_CENTRE_HORIZONTAL | wxBORDER_SUNKEN | wxST_NO_AUTORESIZE, lkSQL3StaticTxtValidator(m_pFldInfo, &m_bValid)), 0, wxEXPAND);
			sT->SetForegroundColour(color);
			szTop->Add(szVer, 0, wxEXPAND);
		}
		/* *******************************************************************************
		 * Creation date
		 * ******************************************************************************* */
		{
			wxBoxSizer* szVer = new wxBoxSizer(wxVERTICAL);
			szVer->Add(new wxStaticText(this, wxID_ANY, wxT("Creation Date :"), wxDefaultPosition, wxDefaultSize), 0, wxBOTTOM | wxALIGN_CENTER, 2);
			szVer->Add(sT = new lkStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 
											 wxALIGN_CENTRE_HORIZONTAL | wxBORDER_SUNKEN | wxST_NO_AUTORESIZE, DGroupStaticDateValidator(m_pFldCreation, &m_bValid, true, true)), 1, wxEXPAND);
			sT->SetForegroundColour(color);
			szTop->Add(szVer, 1, wxLEFT | wxEXPAND, 5);
		}
		szMain->Add(szTop, 0, wxLEFT | wxBOTTOM | wxRIGHT | wxEXPAND, 10);
	}

	SetSizer(szMain);
	Layout();

	Bind(wxEVT_BUTTON, &GroupStorageCanvas::OnBtnCopyTitle, this, m_nBtnCopyTitle);

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////
wxIMPLEMENT_ABSTRACT_CLASS2(DGroup, lkSQL3RecordDialog, TGroup)

DGroup::DGroup(lkSQL3Database* pDB, wxUint64 nBaseID) : lkSQL3RecordDialog(pDB), TGroup(pDB), m_sHours(), m_sMinutes(), m_sSeconds()
{
	SetCurRecord(nBaseID);

	m_pStorageCtrl = NULL;
	m_nIDStorage = wxWindow::NewControlId(1);
}
bool DGroup::Create(wxWindow* parent)
{
	wxASSERT(GetCurRecord() > 0); // must be set prior of calling Create
	if ( !lkSQL3RecordDialog::Create(parent, GetImage_FBG_GROUP(), wxT("Grouping Dialog"), wxDefaultPosition, wxSize(566, 340), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER) )
		return false;
	SetMinClientSize(wxSize(550, 252));

	// this forces to open the recordset and initialize its internal fields
	{
		wxString whr;
		whr.Printf(wxT("([%s] = %I64u)"), t3Group_BASE, GetCurRecord());
		SetFilter(whr);
		(void)GetRecordset();
	}

	wxBoxSizer* szMain = new wxBoxSizer(wxVERTICAL);

	/* *******************************************************************************
	 * Movies-Preview in a Panel acting as a Control
	 * ******************************************************************************* */
	{
		szMain->Add(m_pStorageCtrl = new GroupStorageCanvas(this, GetDB(), lkStaticPanelValidator(m_pFldStorage)), 1, wxALL | wxEXPAND, 10);
	}
	/* *******************************************************************************
	 * Controls for Base
	 * ******************************************************************************* */
	{
		wxBoxSizer* szTop = new wxBoxSizer(wxHORIZONTAL);
		szTop->AddStretchSpacer(1);
		/* *******************************************************************************
		 * Part
		 * ******************************************************************************* */
		{
			wxBoxSizer* szLeft = new wxBoxSizer(wxVERTICAL);
			szLeft->AddStretchSpacer(2);
			szLeft->Add(new wxStaticText(this, wxID_ANY, wxT("Part :"), wxDefaultPosition, wxSize(50, -1), wxALIGN_CENTRE_HORIZONTAL), 0, wxBOTTOM | wxFIXED_MINSIZE | wxALIGN_CENTRE_HORIZONTAL, 3);

			wxTextCtrl* t;
			szLeft->Add(t = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_CENTRE,
										   lkSQL3TextValidator(m_pFldPart, wxFILTER_ALPHA | wxFILTER_DIGITS)), 0, wxFIXED_MINSIZE);
			if ( m_pFldPart->GetDDVmax() > 0 )
				t->SetMaxLength(static_cast<unsigned long>(m_pFldPart->GetDDVmax()));
			t->ForceUpper();

			szLeft->AddStretchSpacer(1);
			szTop->Add(szLeft, 0, wxRIGHT | wxEXPAND, 5);
		}
		szTop->AddStretchSpacer(1);
		/* *******************************************************************************
		 * Duration
		 * ******************************************************************************* */
		{
			wxStaticBoxSizer* szRight = new wxStaticBoxSizer(wxHORIZONTAL, this, wxT(" Duration "));
			// Hours
			{
				wxBoxSizer* szHour = new wxBoxSizer(wxVERTICAL);
				szHour->Add(new wxStaticText(this, wxID_ANY, wxT("Hours"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL), 0, wxBOTTOM | wxFIXED_MINSIZE | wxALIGN_CENTER, 3);
//				wxTextValidator textVal(wxFILTER_DIGITS, &m_sHours);
				szHour->Add(new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_RIGHT,
										   wxTextValidator(wxFILTER_DIGITS, &m_sHours)), 0, wxFIXED_MINSIZE);
				szRight->Add(szHour, 0, wxALL, 5);
			}
			// Minutes
			{
				wxBoxSizer* szMin = new wxBoxSizer(wxVERTICAL);
				szMin->Add(new wxStaticText(this, wxID_ANY, wxT("Minutes"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL), 0, wxBOTTOM | wxFIXED_MINSIZE | wxALIGN_CENTER, 3);
				//				wxTextValidator textVal(wxFILTER_DIGITS, &m_sHours);
				szMin->Add(new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_RIGHT,
										  wxTextValidator(wxFILTER_DIGITS, &m_sMinutes)), 0, wxFIXED_MINSIZE);
				szRight->Add(szMin, 0, wxALL, 5);
			}
			// Seconds
			{
				wxBoxSizer* szSec = new wxBoxSizer(wxVERTICAL);
				szSec->Add(new wxStaticText(this, wxID_ANY, wxT("Seconds"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL), 0, wxBOTTOM | wxFIXED_MINSIZE | wxALIGN_CENTER, 3);
				//				wxTextValidator textVal(wxFILTER_DIGITS, &m_sHours);
				szSec->Add(new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_RIGHT,
										  wxTextValidator(wxFILTER_DIGITS, &m_sSeconds)), 0, wxFIXED_MINSIZE);
				szRight->Add(szSec, 0, wxALL, 5);
			}
			szTop->Add(szRight, 0);
		}
		szTop->AddStretchSpacer(1);
		szMain->Add(szTop, 1, wxLEFT | wxBOTTOM | wxRIGHT | wxEXPAND, 10);
	}

	szMain->Add(CreateSeparatedButtonSizer(wxOK), 1, wxLEFT | wxBOTTOM | wxRIGHT | wxEXPAND, 10);

	SetSizer(szMain);
	Layout();

	return true;
}

void DGroup::OnDlgStorages(wxCommandEvent& WXUNUSED(event))
{
	DStorage dlg(GetDB());
	if ( dlg.Create(this) )
	{
		wxUint64 n = TGroup::GetStorageID();
		dlg.SetCurRecord(n);
		if ( dlg.ShowModal() == wxID_OK )
		{
			wxUint64 m = dlg.GetCurRecord();
			if ( m != n )
			{
				TGroup::SetStorageID(m);
				m_pStorageCtrl->TransferDataToWindow();
			}
		}
	}
}

// Overrides of base-class lkSQL3RecordDialog
////////////////////////////////////////////////////////

wxToolBar* DGroup::OnCreateToolBar(long style, wxWindowID id, const wxString& name)
{
	wxToolBar* tb = lkSQL3RecordDialog::OnCreateToolBar(style, id, name);
	if ( tb )
	{
		tb->AddSeparator();
		tb->AddTool(m_nIDStorage, wxT(""), Get_Storage_Xpm(), wxT("Set/Change Storage"));
	}
	return tb;
}

//virtual
void DGroup::BindButtons()
{
	lkSQL3RecordDialog::BindButtons();

	Bind(wxEVT_TOOL, &DGroup::OnDlgStorages, this, m_nIDStorage);
}

//virtual
bool DGroup::TransferDataToWindow()
{
	double dSpan = 0.0;
	long hrs = 0, min = 0, sec = 0;
	if ( m_pFldTime && ((dSpan = m_pFldTime->GetValue2()) != 0.0) )
	{
		lkDateTimeSpan dt = dSpan;
		if ( dt.IsValid() )
		{
			hrs = long(dt.GetTotalHours());
			min = dt.GetMinutes();
			sec = dt.GetSeconds();
		}
	}
	m_sHours.Printf(wxT("%lu"), hrs);
	m_sMinutes.Printf(wxT("%lu"), min);
	m_sSeconds.Printf(wxT("%lu"), sec);

	return lkSQL3RecordDialog::TransferDataToWindow();
}

//virtual
bool DGroup::TransferDataFromWindow()
{
	bool bRet = lkSQL3RecordDialog::TransferDataFromWindow();

	if ( bRet )
	{
		long hrs = 0, min = 0, sec = 0;
		m_sHours.ToLong(&hrs);
		m_sMinutes.ToLong(&min);
		m_sSeconds.ToLong(&sec);

		lkDateTimeSpan dt;
		if ( (hrs == 0) && (min == 0) && (sec == 0) )
			dt.SetInvalid();
		else
			dt.SetDateTimeSpan(0, hrs, min, sec);

		if ( !dt.IsValid() )
		{
			if ( !m_pFldTime->IsNull() )
				m_pFldTime->SetValueNull();
		}
		else
		{
			double dSpan = dt;
			if ( !m_pFldTime->IsNull() )
			{
				if ( m_pFldTime->GetValue2() != dSpan )
					m_pFldTime->SetValue2(dSpan);
			}
			else
				m_pFldTime->SetValue2(dSpan);
		}
	}

	return bRet;
}

//virtual
wxString DGroup::GetDefaultOrder() const
{
	wxString order;
	order.Printf(wxT("[%s]"), t3Group_STORAGE);
	return order;
}

//virtual
lkSQL3RecordSet* DGroup::GetBaseSet()
{
	return static_cast<lkSQL3RecordSet*>(this);
}

//virtual
void DGroup::DoThingsBeforeAdd()
{
	if ( m_pFldBase )
		m_pFldBase->SetValue2(GetCurRecord());
}

