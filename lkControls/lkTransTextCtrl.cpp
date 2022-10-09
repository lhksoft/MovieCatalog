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
#include "lkTransTextCtrl.h"

wxIMPLEMENT_DYNAMIC_CLASS(lkTransTextCtrl, wxTextCtrl)

lkTransTextCtrl::lkTransTextCtrl() : wxTextCtrl()
{
}

lkTransTextCtrl::lkTransTextCtrl(wxWindow* parent, wxWindowID id, const wxString& value,
								 const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator) :
	wxTextCtrl(parent, id, value, pos, size, style, validator)
{
}

//virtual
bool lkTransTextCtrl::HasTransparentBackground()
{
	return true;
}

#if defined(__WXMSW__)
//virtual
WXHBRUSH lkTransTextCtrl::MSWControlColor(WXHDC hDC, WXHWND hWnd)
{
	//    vvvvvvvvvvvvvvvvvvvvvvvv : don't draw solid background if we supposed to be transparant with a brush, even if we're disabled !
	if ( !HasTransparentBackground() && !IsThisEnabled() && !HasFlag(wxTE_MULTILINE) )
		return MSWControlColorDisabled(hDC);

	return wxTextCtrlBase::MSWControlColor(hDC, hWnd);
}

#endif // __WXMSW__
