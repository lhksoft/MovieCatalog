/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#ifndef __LK_SQL3TABLESET_H__
#define __LK_SQL3TABLESET_H__
#pragma once

#include "lkSQL3RecordSet.h"
#include "lkSQL3TableDef.h"
#include "lkSQL3Verify.h"
#include "lkSQL3TblFieldInit.h"

#include <wx/object.h>
#include <wx/list.h>

class lkInitTable;
class lkSQL3TableSet : public lkSQL3RecordSet
{
public:
	lkSQL3TableSet();
	lkSQL3TableSet(lkSQL3Database* pDB);
	virtual ~lkSQL3TableSet();

	// Data Members
protected:
	bool						m_bAdd; // set to true if in the process of adding a new row

// Operations
public:
	static bool					CreateTable(lkSQL3Database* pDB, const sqlTblDef_t&, bool _Temp = false);
	static bool					CreateTable(lkSQL3Database* pDB, const lkSQL3TableDef&, bool _Temp = false);

protected:
	virtual void				InitColumns(const wxString&) wxOVERRIDE; // param = tablename
	virtual bool				InitColumns(lkSQL3TblInit&, const sqlTblDef_t&, const wxString& tName = wxEmptyString);

	virtual bool				AddData(void);
	void						AddRowID(wxUint64 _newid);
	bool						UpdateData(void);

	// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Helper for 'Open'
protected:
	virtual bool				Open(lkInitTable*); // nothrow

public:
	virtual bool				Open(void) wxOVERRIDE; // needs to be overwritten by derived class -- should return false if incorrect table (aka, doesn't exist or 1 or more fields are missing)

	virtual bool				CanAdd(void) wxOVERRIDE;
	virtual bool				AddNew(void) wxOVERRIDE; // returns false if already in process of adding a new row
	virtual void				CancelAddNew(void) wxOVERRIDE; // resets adding and updates current values in m_Col to current row
	virtual bool				IsAdding(void) const wxOVERRIDE;

	virtual bool				CanUpdate(void) wxOVERRIDE;
	virtual bool				Update(void) wxOVERRIDE; // either Adds a new row, or udates current row
	virtual void				CancelUpdate(void) wxOVERRIDE;

	virtual bool				CanDelete(void) wxOVERRIDE;
	virtual bool				Delete(void) wxOVERRIDE; // deletes current row and requery, moves current row to first row after deleted row and if atEOF then 1 row before current row
	virtual bool				DeleteRow(wxUint64 nRow) wxOVERRIDE; // deletes given row, does not do a requery

	static lkSQL3VerifyList*	GetColumns(lkSQL3Database* pDB, const wxString& tblName); // returns list of Fiels from 'PRAGMA table_info ...'
	static bool					VerifyTable(lkSQL3Database*, const sqlTblDef_t&); // verifys that the table exists, and the fields are present with given type
	static bool					IsUniqueTextValue(lkSQL3Database* pDB, const wxString& tblName, const wxString& fldName, const wxString& fldData); // true if given fldData doesn't exist in table

	void						Throw_InvalidTable(const wxString& _tblName); // throws an exception that given table is incorrect

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(lkSQL3TableSet);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper Classes for lkSQL3TableSet::Open(..)

class lkInitField : public wxObject
{
public:
	lkInitField();
	lkInitField(const wxString&, const wxClassInfo*, lkSQL3FieldData**);

	wxString					m_szField;
	const wxClassInfo*			m_rcField;
	lkSQL3FieldData**			m_ppField;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(lkInitField);
};

// ///////////////////////////////////////////////////////////////////////////////

// This list contains multiple lkInitField -objects
WX_DECLARE_LIST(lkInitField, lkInitFieldListBase);

class lkInitTable : public lkInitFieldListBase
{
public:
	lkInitTable();
	lkInitTable(sqlTblDef_p);
	virtual ~lkInitTable();

	sqlTblDef_p				m_pTblDef;
};

#endif // !__LK_SQL3TABLESET_H__
