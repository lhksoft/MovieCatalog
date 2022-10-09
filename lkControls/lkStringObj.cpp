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
#include "lkStringObj.h"
#include <wx/listimpl.cpp>

wxIMPLEMENT_ABSTRACT_CLASS(lkStringObj, wxObject)

lkStringObj::lkStringObj() : wxObject(), m_String1(), m_String2(), m_String3()
{
	m_lParam = 0;
}
lkStringObj::lkStringObj(const wxString& o1, const wxString& o2, const wxString& o3) : wxObject(), m_String1(o1), m_String2(o2), m_String3(o3)
{
	m_lParam = 0;
}
lkStringObj::lkStringObj(wxUint64 lParam, const wxString& o1, const wxString& o2, const wxString& o3) : wxObject(), m_String1(o1), m_String2(o2), m_String3(o3)
{
	m_lParam = lParam;
}
lkStringObj::lkStringObj(const lkStringObj& other) : wxObject(), m_String1(), m_String2(), m_String3()
{
	m_String1 = other.m_String1;
	m_String2 = other.m_String2;
	m_String3 = other.m_String3;
	m_lParam = other.m_lParam;
}
lkStringObj::~lkStringObj()
{
}

lkStringObj& lkStringObj::operator=(const lkStringObj& other)
{
	m_String1 = other.m_String1;
	m_String2 = other.m_String2;
	m_String3 = other.m_String3;
	m_lParam = other.m_lParam;

	return *this;
}

WX_DEFINE_LIST(lkStringObjList);
