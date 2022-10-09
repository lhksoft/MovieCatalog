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
#ifndef __T_GROUP_H__
#define __T_GROUP_H__
#pragma once

#include "../lkSQLite3/lkSQL3TableSet.h"
#include <wx/thread.h>

// name of Table
#define t3Group					_T("tblGroup")
// Required Field-definitions
#define t3Group_BASE			_T("BaseID")
#define t3Group_STORAGE			_T("StorageID")
#define t3Group_PART			_T("Part")			// (optional) 1-char (uppercase) AlfaNumeric
#define t3Group_TIME			_T("PlayTime")		// (optional) lkDateTimeSpan

class TGroup : public lkSQL3TableSet
{
public:
	TGroup();
	TGroup(lkSQL3Database* pDB);

private:
	void								Init(void);
//	virtual bool						CanDelete(void) wxOVERRIDE;

// Operations
public:
	virtual bool						Open(void) wxOVERRIDE; // should return false if incorrect table (aka, doesn't exist or 1 or more fields are missing)

// Attributes -- needs to be public for our Validators
public:
	lkSQL3FieldData_Int*				m_pFldBase;
	lkSQL3FieldData_Int*				m_pFldStorage;
	lkSQL3FieldData_Text*				m_pFldPart;
	lkSQL3FieldData_Real*				m_pFldTime;

public:
	wxUint64					GetBaseID(void) const;
	void						SetBaseID(wxUint64);

	wxUint64					GetStorageID(void) const;
	void						SetStorageID(wxUint64);

	wxString					GetPartValue(void) const;
	void						SetPartValue(const wxString&);

	double						GetTimeValue(void) const;
	void						SetTimeValue(double);

// Implementation
public:
	static bool					VerifyTable(lkSQL3Database*); // verifys that the table exists, and the fields are present with given type
	static bool					CreateTable(lkSQL3Database*); // FALSE on error or already exists

	static bool					HasStorage(lkSQL3Database*, wxUint64); // TRUE if at least 1 Storage match has been found
	static bool					HasBase(lkSQL3Database*, wxUint64); // TRUE if at least 1 Base match has been found

	static bool					Compact(wxThread*, lkSQL3Database* dbSrc, lkSQL3Database* dbDest, lkSQL3Database* dbTmp); // wxThread required for sending messages to main-thread

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(TGroup);
};

#endif // !__T_GROUP_H__
