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
#ifndef __GENRESBOX_H__
#define __GENRESBOX_H__
#pragma once

#include <wx/validate.h>
#include "../lkControls/lkChkImgListbox.h"
#include "../lkSQLite3/lkSQL3Database.h"
#include "../lkSQLite3/lkSQL3FieldData.h"

////////////////////////////////////////////////////////////////////////////
// class GenresBox, used in CMovies + DlgMoviesFilter
////
class GenresBox : public lkChkImgListbox
{
public:
	GenresBox(lkSQL3Database*);
	GenresBox(lkSQL3Database*, wxWindow* pParent, wxWindowID nId = wxID_ANY, const wxPoint& nPos = wxDefaultPosition, const wxSize& nSize = wxDefaultSize,
			  long nStyle = 0, const wxValidator& validator = wxDefaultValidator);
	virtual ~GenresBox();

	bool				BuildList(void);

	void				SetCheck(wxUint64); // checks all in given value
	wxUint64			GetChecked(void) const; // all unique lparam of each checked value will be OR'd into 1 uint64 value

protected:
	lkSQL3Database*		m_pDB;
private:
	wxDECLARE_CLASS(GenresBox);
	wxDECLARE_NO_COPY_CLASS(GenresBox);
};

////////////////////////////////////////////////////////////////////////////
// class GenresValidator
////
class GenresValidator : public wxValidator
{
public:
	GenresValidator();
	GenresValidator(lkSQL3FieldData_Int*);
	GenresValidator(wxUint64*);
	GenresValidator(const GenresValidator&);

	virtual wxObject*				Clone() const wxOVERRIDE;

	virtual bool					Validate(wxWindow* parent) wxOVERRIDE;
	virtual bool					TransferToWindow(void) wxOVERRIDE;
	virtual bool					TransferFromWindow(void) wxOVERRIDE;

protected:
	lkSQL3FieldData_Int*			m_pField;
	wxUint64*						m_pValue;

	GenresBox*						GetControl(void);

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(GenresValidator);
};

#endif // !__GENRESBOX_H__
