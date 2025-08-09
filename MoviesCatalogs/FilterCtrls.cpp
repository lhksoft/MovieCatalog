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
#include "FilterCtrls.h"
#include "XPMs.h"

/////////////////////////////////////////////////////////////////////////////////////////
// class SortComboBox
////
wxIMPLEMENT_CLASS(SortComboBox, lkImgComboBox)

SortComboBox::SortComboBox(wxWindow* parent, const wxValidator& validator) : lkImgComboBox(parent, wxID_ANY, wxDefaultSize, 0, validator, wxDefaultPosition)
{
}
bool SortComboBox::BuildList()
{
	lkBoxInitList initList;
	initList.DeleteContents(true);

	initList.Append(new lkBoxInit(wxT("No Sort"), Get_NoSort_Xpm(), SortComboBox::NoSort));
	initList.Append(new lkBoxInit(wxT("Ascending"), Get_SortAsc_Xpm(), SortComboBox::Ascending));
	initList.Append(new lkBoxInit(wxT("Descending"), Get_SortDesc_Xpm(), SortComboBox::Descending));

	bool bRet = false;
	if ( InitializeFromList(&initList, true, lkExpandFlags::EXPAND_Center) )
		bRet = Populate();

	return bRet;
}
