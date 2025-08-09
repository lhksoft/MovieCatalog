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
// This table is a helper table which will be created in a Memory database (tempory) and only be used when Compacting
#ifndef __T_TEMP_H__
#define __T_TEMP_H__
#pragma once

#include "../lkSQLite3/lkSQL3TableSet.h"

class TTemp : public lkSQL3TableSet
{
public:
	TTemp();
	TTemp(lkSQL3Database* pDB);

private:
	void								Init(void);

// Operations
public:
	bool								Open(const wxString& tmpTableName);

// Attributes -- needs to be public for our Validators
public:
	lkSQL3FieldData_Int*				m_pFldOldID;
	lkSQL3FieldData_Int*				m_pFldNewID;

	wxUint64							GetOld(void) const;
	void								SetOld(wxUint64);

	wxUint64							GetNew(void) const;
	void								SetNew(wxUint64);

// Implementation
public:
	static wxUint64						FindNewID(lkSQL3Database* _DB, const wxString& sTable, wxUint64 nOld);
	static wxUint64						FindNewGenre(lkSQL3Database* _DB, const wxString& sTable, wxUint64 nOld);

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(TTemp);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class TTempCat
// to be used for TCategory
////
class TTempCat : public lkSQL3TableSet
{
public:
	TTempCat();
	TTempCat(lkSQL3Database* pDB);

private:
	void								Init(void);


// Operations
public:
	virtual bool						Open(const wxString& tmpTableName);

// Attributes -- needs to be public for our Validators
public:
	lkSQL3FieldData_Int*				m_pFldOldCatID;
	lkSQL3FieldData_Int*				m_pFldOldSubID;
	lkSQL3FieldData_Int*				m_pFldNewCatID;
	lkSQL3FieldData_Int*				m_pFldNewSubID;

	wxUint32							GetOldCat(void) const;
	void								SetOldCat(wxUint32);

	wxUint32							GetNewCat(void) const;
	void								SetNewCat(wxUint32);

	wxUint32							GetOldSub(void) const;
	void								SetOldSub(wxUint32);

	wxUint32							GetNewSub(void) const;
	void								SetNewSub(wxUint32);

// Implementation
public:
	static wxUint32						FindNewID(lkSQL3Database* _DB, const wxString& sTable, wxUint32 nOldCat, wxUint32 nOldSub);

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(TTempCat);
};
#endif // !__T_TEMP_H__
