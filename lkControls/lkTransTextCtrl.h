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
#ifndef __LK_TRANS_TEXTCTRL_H__
#define __LK_TRANS_TEXTCTRL_H__
#pragma once

#include <wx/textctrl.h>

///////////////////////////////////////////////////////////////////////////////////
// class lkTransTextCtrl
// --
// a class that by default supports transparant background
////
class lkTransTextCtrl : public wxTextCtrl
{
public:
	lkTransTextCtrl();
	lkTransTextCtrl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& value = wxEmptyString,
					const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxValidator& validator = wxDefaultValidator);

	virtual bool		HasTransparentBackground() wxOVERRIDE;
#if defined(__WXMSW__)
	virtual WXHBRUSH	MSWControlColor(WXHDC hDC, WXHWND hWnd) wxOVERRIDE;
#endif // __WXMSW__


private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(lkTransTextCtrl);
};

#endif // !__LK_TRANS_TEXTCTRL_H__
