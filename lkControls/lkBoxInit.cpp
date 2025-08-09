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
#include "lkBoxInit.h"
#include <wx/listimpl.cpp>

wxIMPLEMENT_ABSTRACT_CLASS(lkBoxInit, wxObject);

lkBoxInit::lkBoxInit() : wxObject(), m_szAddString()
{
	wxFAIL_MSG(wxT("Using the default constructor is not allowed"));
	m_pImage = NULL;
	m_lParam = 0;
}
lkBoxInit::lkBoxInit(const wxString& szAddString, wxUint64 lParam) : wxObject(), m_szAddString(szAddString)
{
	wxASSERT_MSG(!szAddString.IsEmpty(), wxT("'szAddString' should NOT be empty !!"));
	m_pImage = NULL;
	m_lParam = lParam;
}
lkBoxInit::lkBoxInit(const wxString& szAddString, const wxImage& image, wxUint64 lParam) : wxObject(), m_szAddString(szAddString)
{
	wxASSERT_MSG(!szAddString.IsEmpty(), wxT("'szAddString' should NOT be empty !!"));
	m_pImage = (image.IsOk()) ? (new wxImage(image)) : NULL;
	m_lParam = lParam;
}
lkBoxInit::lkBoxInit(const lkBoxInit& boxInit) : wxObject(), m_szAddString(boxInit.m_szAddString)
{
	m_pImage = ((boxInit.m_pImage != NULL) && boxInit.m_pImage->IsOk()) ? (new wxImage(*(boxInit.m_pImage))) : NULL;
	m_lParam = boxInit.m_lParam;
}
lkBoxInit::~lkBoxInit()
{
	if (m_pImage)
		delete m_pImage;
}

const lkBoxInit& lkBoxInit::operator = (const lkBoxInit& i)
{
	m_szAddString = i.m_szAddString;
	m_lParam = i.m_lParam;
	if (m_pImage != NULL) delete m_pImage;
	m_pImage = ((i.m_pImage != NULL) && i.m_pImage->IsOk()) ? (new wxImage(*(i.m_pImage))) : NULL;

	return *this;
}

const wxString& lkBoxInit::GetString()
{
	return m_szAddString;
}
wxUint64 lkBoxInit::GetParam() const
{
	return m_lParam;
}
wxImage* lkBoxInit::GetImage()
{
	return m_pImage;
}

void lkBoxInit::SetParam(wxUint64 u)
{
	m_lParam = u;
}


// ///////////////////////////////////////////////////////////////////////////////////

WX_DEFINE_LIST(lkBoxInitList);

