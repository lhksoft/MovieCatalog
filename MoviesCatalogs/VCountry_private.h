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
#ifndef __V_COUNTRY_PRIVATE_H__
#define __V_COUNTRY_PRIVATE_H__
#pragma once

#include "DlgFind.h"
#include "../lkSQLite3/lkSQL3RecordSet.h"

#define conf_COUNTRY_PATH			wxT("Countries")

#define rowset_COUNTRY_DEFORDER		wxT("ifnull([%s], [%s]) COLLATE lkUTF8compare")

//////////////////////////////////////////////////////////////////////////////////////////////////////
// class DlgCountryFind
////
class DlgCountryFind : public DlgFind
{
public:
	DlgCountryFind(lkSQL3RecordSet*, lkFindList*);
	bool					Create(wxWindow*, long style = wxDEFAULT_DIALOG_STYLE);

	// returns first actual_rowid if successful, otherwise returns 0
	static wxUint64			FindCountry(wxWindow* parent, lkSQL3RecordSet* pRS);

protected:
	virtual void			DoInit(void) wxOVERRIDE;
	virtual void			DoOK(void) wxOVERRIDE;

private:
	wxDECLARE_ABSTRACT_CLASS(DlgCountryFind);
	wxDECLARE_NO_COPY_CLASS(DlgCountryFind);
};


#endif // !__V_COUNTRY_PRIVATE_H__
