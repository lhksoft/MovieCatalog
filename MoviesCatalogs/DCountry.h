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
#ifndef __D_COUNTRY_H__
#define __D_COUNTRY_H__
#pragma once

#include "../lkControls/lkDialog.h"
#include <wx/arrstr.h>

#include "../lkControls/lkChkImgListbox.h"
#include "../lkSQLite3/lkSQL3Database.h"

class DCountry : public lkDialog
{
public:
	DCountry(const wxString& sCtr, lkSQL3Database* pDB = NULL);
	bool							Create(wxWindow* parent);

	wxString						GetCountryString(void) const;

// Implementation
protected:
	wxSortedArrayString				SplitCountries(const wxString&);

// Attributes
protected:
	lkSQL3Database*					m_pDB;
	lkChkImgListbox*				m_CListbox;
	wxButton*						m_srcBtn;

	wxWindowID						m_CountryListBox;
	wxWindowID						m_CountrySrearchBtn;

	wxString						m_sCountries;

// Event Handling
protected:
	void							OnInitDialog(wxInitDialogEvent& event);
	void							OnChekListbox(wxCommandEvent& event);
	void							OnSearchBtn(wxCommandEvent& event);

private:
	wxDECLARE_EVENT_TABLE();
	wxDECLARE_ABSTRACT_CLASS(DCountry);
	wxDECLARE_NO_COPY_CLASS(DCountry);
};

#endif // !__D_COUNTRY_H__
