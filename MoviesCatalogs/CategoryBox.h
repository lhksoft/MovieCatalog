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
#ifndef __CATEGORYBOX_H__
#define __CATEGORYBOX_H__
#pragma once

#include "../lkControls/lkImgComboBox.h"
#include "../lkSQLite3/lkSQL3Database.h"

////////////////////////////////////////////////////////////////////////////
// class CategoryBox, used in CMovies + DlgMoviesFilter
////
class CategoryBox : public lkImgComboBox
{
public:
	CategoryBox();
	CategoryBox(wxWindow* parent, wxWindowID id = wxID_ANY, const wxSize& size = wxDefaultSize, long style = 0,
				const wxValidator& validator = wxDefaultValidator, const wxPoint& pos = wxDefaultPosition);
	virtual ~CategoryBox();

	void				SetSubCatlist(CategoryBox*); // only making sence in main category-list
	void				SetDatabase(lkSQL3Database*);

	virtual void		SetSelectedLParam(wxUint64) wxOVERRIDE;
//	virtual wxUint64	GetSelectedLParam(void) const wxOVERRIDE;

	bool				BuildCatList(bool bKeepSel = false);
	bool				BuildSubList(wxUint64 nCat, bool bKeepSel = false);

protected:
	bool				BuildList(const wxString& filter, const wxString& order, bool bIsCat, bool bKeepSel = false);

	void				OnCatSel(wxCommandEvent& event);

private:
	CategoryBox*		m_SubCatBox;
	int					m_nCurSel;
	lkSQL3Database*		m_pDB;

	wxDECLARE_EVENT_TABLE();
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(CategoryBox);
};


#endif // !__CATEGORYBOX_H__
