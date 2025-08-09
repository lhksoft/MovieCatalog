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
#ifndef __T_ORIGINE_H__
#define __T_ORIGINE_H__
#pragma once

#include "../lkSQLite3/lkSQL3TableSet.h"
#include <wx/thread.h>

// name of Table
#define t3Orig			wxT("tblOrigine")
// Required Field-definitions
#define t3Orig_NAME		wxT("Origine")

class TOrigine : public lkSQL3TableSet
{
public:
	TOrigine();
	TOrigine(lkSQL3Database* pDB);


private:
	void						Init(void);

	// Operations
public:
	virtual bool				Open(void) wxOVERRIDE; // should return false if incorrect table (aka, doesn't exist or 1 or more fields are missing)
	virtual bool				CanDelete(void) wxOVERRIDE;

	// Attributes -- needs to be public for our Validators
public:
	lkSQL3FieldData_Text*		m_pFldName;

public:
	wxString					GetOrigineValue(void) const;
	void						SetOrigineValue(const wxString&); // NULL not allowed

// Implementation
public:
	static bool					VerifyTable(lkSQL3Database*); // verifys that the table exists, and the fields are present with given type
	static bool					CreateTable(lkSQL3Database*); // FALSE on error or already exists

	static bool					IsUniqueOrigine(lkSQL3Database* _DB, const wxString& orig, void*); // TRUE if given location-name doesn't exist in table

	static void					SetDefaultOrigine(wxUint64);
	static wxUint64				GetDefaultOrigine(lkSQL3Database*); // if valid pDB, a check will be done whether the default exists as a valid rowid

	static void					CorrectConfig(lkSQL3Database* dbTmp);
	static bool					Compact(wxThread*, lkSQL3Database* dbSrc, lkSQL3Database* dbDest, lkSQL3Database* dbTmp); // wxThread required for sending messages to main-thread

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(TOrigine);
};

#endif // !__T_ORIGINE_H__
