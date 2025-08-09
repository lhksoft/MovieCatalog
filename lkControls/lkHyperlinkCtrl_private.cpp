/* lkControls - custom controls using wxWidgets
 * Copyright (C) 2022 Laurens Koehoorn (lhksoft)
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include "lkHyperlinkCtrl_private.h"

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/valtext.h>
#ifdef __WXDEBUG__
# include <wx/msgdlg.h>
#endif // __WXDEBUG__
#include "lkColour.h"


///////////////////////////////////////////////////////////////////////////////
// lkHyperlinkDialog
///////////////////////////////////////////////////////////////////////////////
wxIMPLEMENT_DYNAMIC_CLASS(lkHyperlinkDialog, wxDialog)

wxBEGIN_EVENT_TABLE(lkHyperlinkDialog, wxDialog)
	EVT_RIGHT_DOWN(lkHyperlinkDialog::OnRightClick)
wxEND_EVENT_TABLE()

lkHyperlinkDialog::lkHyperlinkDialog() : wxDialog(), m_URL(), m_UriLabel()
{
}
lkHyperlinkDialog::~lkHyperlinkDialog()
{
}

bool lkHyperlinkDialog::Create(wxWindow* pParent)
{
	if ( !wxDialog::Create(pParent, wxID_ANY, wxT("URL Editor"), wxDefaultPosition, wxSize(535, 155), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER) )
		return false;
	SetMinSize(wxSize(535, 155));

	SetBackgroundColour(MAKE_RGB(255, 233, 129)); // light-orange
	SetForegroundColour(MAKE_RGB(182, 0, 111)); // dark-magenta

	wxBoxSizer* szMain = new wxBoxSizer(wxVERTICAL);
	{
		wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);

		wxTextCtrl* t;
		szHor->Add(new wxStaticText(this, wxID_ANY, wxT("Display :"), wxDefaultPosition, wxSize(65, -1), wxALIGN_RIGHT), 0, wxALIGN_CENTER_VERTICAL | wxFIXED_MINSIZE | wxRIGHT | wxBOTTOM, 5);
		szHor->Add(t = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_LEFT, wxTextValidator(wxFILTER_NONE, &m_UriLabel)), 1, wxEXPAND | wxBOTTOM, 5);
		t->SetForegroundColour(lkCol_DarkBlue);

		szMain->Add(szHor, 0, wxLEFT | wxTOP | wxRIGHT | wxEXPAND, 10);
	}

	{
		wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);

		wxTextCtrl* t;
		szHor->Add(new wxStaticText(this, wxID_ANY, wxT("HyperLink :"), wxDefaultPosition, wxSize(65, -1), wxALIGN_RIGHT), 0, wxALIGN_CENTER_VERTICAL | wxFIXED_MINSIZE | wxRIGHT, 5);
		szHor->Add(t = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_LEFT, wxTextValidator(wxFILTER_NONE, &m_URL)), 1, wxEXPAND);
		t->SetForegroundColour(lkCol_DarkBlue);

		szMain->Add(szHor, 0, wxLEFT | wxRIGHT | wxEXPAND, 10);
	}

	{
		wxSizer* stdBtnSz = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
		if ( stdBtnSz )
			szMain->Add(stdBtnSz, 0, wxALL | wxEXPAND, 10);
	}

	SetSizer(szMain);
	Layout();

	TransferDataToWindow();
	return true;
}

wxString lkHyperlinkDialog::GetURL() const
{
	return m_URL;
}
void lkHyperlinkDialog::SetURL(const wxString& u)
{
	m_URL = u;
	Refresh();
}

wxString lkHyperlinkDialog::GetUriLabel() const
{
	return m_UriLabel;
}
void lkHyperlinkDialog::SetUriLabel(const wxString& l)
{
	m_UriLabel = l;
	Refresh();
}

void lkHyperlinkDialog::OnRightClick(wxMouseEvent& event)
{
#ifdef __WXDEBUG__
	wxSize size = GetSize();
	wxString s = s.Format(wxT("width = %d\nheight = %d"), size.GetWidth(), size.GetHeight());
	wxMessageBox(s, wxT("For your information"), wxOK | wxICON_INFORMATION, this);
#endif // WXDEBUG
}


