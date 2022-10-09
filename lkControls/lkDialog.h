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
#ifndef __LK_DIALOG_H__
#define __LK_DIALOG_H__
#pragma once

#include <wx/dialog.h>
#include <wx/custombgwin.h>
#include <wx/bitmap.h>
#include <wx/image.h>

class lkDialog : public wxCustomBackgroundWindow<wxDialog>
{
public:
	lkDialog();
	virtual ~lkDialog();

	bool			Create(wxWindow* parent, const wxBitmap& background, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE);
	bool			Create(wxWindow* parent, const wxImage& background, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE);

protected:
	void			OnRightClick(wxMouseEvent& event);

private:
	wxDECLARE_EVENT_TABLE();
	wxDECLARE_CLASS(lkDialog);
	wxDECLARE_NO_COPY_CLASS(lkDialog);
};

#endif // !__LK_DIALOG_H__
