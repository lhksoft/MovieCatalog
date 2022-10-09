/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#ifndef __LK_SQL3FINDTABLE_H__
#define __LK_SQL3FINDTABLE_H__
#pragma once

#include "lkSQL3TableSet.h"

class lkSQL3FindTable : public lkSQL3TableSet
{
public:
	lkSQL3FindTable();
	lkSQL3FindTable(lkSQL3Database* pDB);
	virtual ~lkSQL3FindTable();

	// Operations
public:
	virtual bool				Open(void);

	// Members
protected:
	lkSQL3FieldData_Int*		m_pFldID; // UINT

public:
	wxUint64					GetOrigID(void) const;
	void						SetOrigID(wxUint64);

	// Operations
public:
	wxUint64					FindFirst(wxUint64&);
	wxUint64					FindLast(wxUint64&);
	wxUint64					FindNext(wxUint64&);
	wxUint64					FindPrev(wxUint64&);

	bool						HasNext(wxUint64);
	bool						HasPrev(wxUint64);

	// Implementation
public:
	static bool					CreateTable(lkSQL3Database*); // false on error or already exists

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(lkSQL3FindTable);
};

#endif // !__LK_SQL3FINDTABLE_H__
