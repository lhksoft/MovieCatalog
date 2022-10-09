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
#ifndef __LK_STRING_OBJ_H__
#define __LK_STRING_OBJ_H__
#pragma once

#include <wx/string.h>
#include <wx/list.h>

//////////////////////////////////////////////////////////////////////////////////////////
// class lkStringObj
// a base class for storing strings as parameter in a listbox/combobox where the lParam field should be a string
////
class lkStringObj : public wxObject
{
public:
	lkStringObj();
	lkStringObj(const wxString& o1, const wxString& o2 = wxEmptyString, const wxString& o3 = wxEmptyString);
	lkStringObj(wxUint64 lParam, const wxString& o1, const wxString& o2 = wxEmptyString, const wxString& o3 = wxEmptyString);
	lkStringObj(const lkStringObj&);
	virtual ~lkStringObj();

public:
	lkStringObj&					operator=(const lkStringObj&);

public:
	wxString						m_String1, m_String2, m_String3;
	wxUint64						m_lParam;

private:
	wxDECLARE_ABSTRACT_CLASS(lkStringObj);
};

WX_DECLARE_LIST(lkStringObj, lkStringObjList);

#endif // !__LK_STRING_OBJ_H__
