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
#include "lkDialog.h"
#include <wx/log.h>
#include <wx/msgdlg.h>

#if defined(__UNIX__)
#include <wx/font.h>
#endif


wxIMPLEMENT_CLASS(lkDialog, wxCustomBackgroundWindow<wxDialog>)

wxBEGIN_EVENT_TABLE(lkDialog, wxCustomBackgroundWindow<wxDialog>)
	EVT_RIGHT_DOWN(lkDialog::OnRightClick)
wxEND_EVENT_TABLE()


lkDialog::lkDialog() : wxCustomBackgroundWindow<wxDialog>()
{
#if defined(__UNIX__)
	wxFont defaultFont(*wxSMALL_FONT);
	SetFont(defaultFont);
#endif
}
lkDialog::~lkDialog()
{
}

bool lkDialog::Create(wxWindow* parent, const wxBitmap& background, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
{
	if ( !wxCustomBackgroundWindow<wxDialog>::Create(parent, id, title, pos, size, style) )
		return false;

	if ( background.IsOk() )
		SetBackgroundBitmap(background);

	return true;
}
bool lkDialog::Create(wxWindow* parent, const wxImage& background, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
{
	wxBitmap bmp;
	if ( background.IsOk() )
		bmp = wxBitmap(background);

	return lkDialog::Create(parent, bmp, id, title, pos, size, style);
}

void lkDialog::OnRightClick(wxMouseEvent& event)
{
	if (wxLog::GetLogLevel() == wxLOG_Debug)
	{
		wxSize sizeC = GetClientSize();
		wxSize sizeF = GetSize();
		wxString s = s.Format(wxT("Frame Size\nwidth = %d | height = %d\n\nClient Size\nwidth = %d | height = %d"), sizeF.GetWidth(), sizeF.GetHeight(), sizeC.GetWidth(), sizeC.GetHeight());
		wxMessageBox(s, wxT("For your information"), wxOK | wxICON_INFORMATION, this);
	}
	else
		event.Skip();
}

