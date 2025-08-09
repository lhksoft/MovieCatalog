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
#ifndef __LKBOXINIT_H__
#define __LKBOXINIT_H__
#pragma once

#include <wx/object.h>
#include <wx/list.h>
#include <wx/string.h>
#include <wx/image.h>

/* *****************************************************************************
 * class lkBoxInit : used to auto-initialize/populate a lkImageBox-control
 * ***************************************************************************** */

class lkBoxInit : public wxObject
{
// C'tor / D'tor
protected:
	lkBoxInit(); // this c'tor shouldn't be used
public:
	lkBoxInit(const wxString& szAddString, wxUint64 lParam = 0); // adding a String, and optionally extra data as lParam
	lkBoxInit(const wxString& szAddString, const wxImage& image, wxUint64 lParam = 0); // adding a String, an Image and optionally extra data as lParam
	lkBoxInit(const lkBoxInit&); // copy constructor
	virtual ~lkBoxInit();

// Member Data
protected:
	wxString				m_szAddString;
	wxImage*				m_pImage;
	wxUint64				m_lParam; // in wxWidgets a 'lParam' is signed, though in msw it is unsigned -- for x86 compilation this should be wxUint32

// Get / Set
public:
	const wxString&			GetString(void);
	wxUint64				GetParam(void) const;
	wxImage*				GetImage(void);

	void					SetParam(wxUint64);

// Operator for easyness
public:
	const lkBoxInit&		operator = (const lkBoxInit&);

private:
	wxDECLARE_ABSTRACT_CLASS(lkBoxInit);
};



// this macro declares and partly implements MyList class
WX_DECLARE_LIST(lkBoxInit, lkBoxInitList);

#endif // !__LKBOXINIT_H__
