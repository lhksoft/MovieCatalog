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
#include "CanvasCtrl.h"
#include <wx/bitmap.h>
#include <wx/msgdlg.h>
//#include <wx/dataobj.h>
#include <wx/clipbrd.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class CanvasCtrl
////
wxIMPLEMENT_ABSTRACT_CLASS(CanvasCtrl, wxCustomBackgroundWindow<wxPanel>)

wxBEGIN_EVENT_TABLE(CanvasCtrl, wxCustomBackgroundWindow<wxPanel>)
	EVT_RIGHT_DOWN(CanvasCtrl::OnRightClick)
wxEND_EVENT_TABLE()

CanvasCtrl::CanvasCtrl() : wxCustomBackgroundWindow<wxPanel>()
{
	m_pTitleCtrl = NULL;
	m_nBtnCopyTitle = wxWindow::NewControlId(1);
	m_bValid = false;
	m_nInternalID = 0;
}
CanvasCtrl::~CanvasCtrl()
{
}

bool CanvasCtrl::Create(wxWindow* parent, const wxSize& size, long style, const wxValidator& validator)
{
	if ( !(wxCustomBackgroundWindow<wxPanel>::Create(parent, wxID_ANY, wxDefaultPosition, size, style)) )
		return false;

	SetValidator(validator);

	return true;
}
bool CanvasCtrl::Create(wxWindow* parent, const wxImage& backGround, const wxSize& size, long style, const wxValidator& validator)
{
	if ( !CanvasCtrl::Create(parent, size, style, validator) )
		return false;

	if ( backGround.IsOk() )
	{
		wxBitmap bmp(backGround);
		SetBackgroundBitmap(bmp);
	}

	return true;
}

wxUint64 CanvasCtrl::GetInternalID() const
{
	return m_nInternalID;
}
//virtual
void CanvasCtrl::SetInternalID(wxUint64 nID)
{
	m_bValid = ((m_nInternalID = nID) != 0);
}

//virtual
bool CanvasCtrl::AcceptsFocus() const
{
	return false;
}

void CanvasCtrl::OnRightClick(wxMouseEvent& event)
{
#ifdef __WXDEBUG__
	wxWindowDisabler wd(true);

	wxSize sizeC = GetClientSize();
	wxSize sizeF = GetSize();
	wxString s; s.Printf(wxT("Size\nwidth = %d | height = %d\n\nClient Size\nwidth = %d | height = %d"), sizeF.GetWidth(), sizeF.GetHeight(), sizeC.GetWidth(), sizeC.GetHeight());
	wxMessageBox(s, wxT("For your information"), wxOK | wxICON_INFORMATION, this);
#else
	event.Skip();
#endif // WXDEBUG
}

void CanvasCtrl::OnBtnCopyTitle(wxCommandEvent& event)
{
	if ( m_bValid && m_pTitleCtrl )
	{
		if ( wxTheClipboard->Open() )
		{
			// This data objects are held by the clipboard,
			// so do not delete them in the app.
			wxTheClipboard->SetData(new wxTextDataObject(m_pTitleCtrl->GetLabel()));
			wxTheClipboard->Close();
		}
	}
	else
		event.Skip();
}


////////////////////////////////////////////////////////////////////////////////////////////////
// class lkStaticPanelValidator -- for BaseMoviesCanvas-Control, 
////
wxIMPLEMENT_DYNAMIC_CLASS(lkStaticPanelValidator, wxValidator)

lkStaticPanelValidator::lkStaticPanelValidator() : wxValidator()
{
	m_pField = NULL;
}
lkStaticPanelValidator::lkStaticPanelValidator(lkSQL3FieldData_Int* pField) : wxValidator()
{
	m_pField = pField;
}
lkStaticPanelValidator::lkStaticPanelValidator(const lkStaticPanelValidator& other) : wxValidator(other)
{
	m_pField = other.m_pField;
}
lkStaticPanelValidator::~lkStaticPanelValidator()
{
}

//virtual
wxObject* lkStaticPanelValidator::Clone() const
{
	return new lkStaticPanelValidator(*this);
}

bool lkStaticPanelValidator::Validate(wxWindow* parent)
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pField != NULL);
	if ( !m_validatorWindow || !m_pField )
		return false;

	CanvasCtrl* canvas = GetControl();
	wxUint64 val = canvas->GetInternalID();

	wxString msg;
	if ( val == 0 )
	{
		if ( m_pField->IsNotNull() && !m_pField->UseDefault() ) // cannot be null & not using a default value
			msg = wxT("This field is required and cannot be empty.");
	}

	if ( !msg.IsEmpty() )
	{
		m_validatorWindow->SetFocus();
		wxMessageBox(msg, wxT("CanvasCtrl : Validation conflict"), wxOK | wxICON_EXCLAMATION, parent);

		return false;
	}

	return true;
}

// Called to transfer data TO the window
bool lkStaticPanelValidator::TransferToWindow()
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pField != NULL);
	if ( !m_validatorWindow || !m_pField )
		return false;

	CanvasCtrl* canvas = GetControl();

	if ( canvas )
	{
		canvas->SetInternalID(m_pField->GetValue2()); // if pField=Invalid (adding modus?) it will be '0'
		return true;
	}

	return false;
}

// Called to transfer data FROM the window
bool lkStaticPanelValidator::TransferFromWindow()
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pField != NULL);
	if ( !m_validatorWindow || !m_pField )
		return false;

	CanvasCtrl* canvas = GetControl();
	if ( !canvas ) return false;

	wxUint64 origID = m_pField->GetValue2();
	wxUint64 newID = canvas->GetInternalID();
	if ( newID != origID )
	{
		if ( newID == 0 )
			m_pField->SetValueNull(); // let the db decide what to do
		else
			m_pField->SetValue2(newID);
	}

	return true;
}

CanvasCtrl* lkStaticPanelValidator::GetControl()
{
	if ( wxDynamicCast( m_validatorWindow, CanvasCtrl) )
		return dynamic_cast<CanvasCtrl*>(m_validatorWindow);

	wxFAIL_MSG("lkStaticPanelValidator can only be used with CanvasCtrl");

	return NULL;
}


