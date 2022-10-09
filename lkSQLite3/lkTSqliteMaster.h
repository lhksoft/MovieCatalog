/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#ifndef __LK_TSQLITEMASTER_H__
#define __LK_TSQLITEMASTER_H__
#pragma once

#include "lkSQL3TableSet.h"
#include "lkSQL3FieldData.h"

// name of Table
#define t3SqliteMaster				wxT("sqlite_master")
// Required Field-definitions
#define t3SqliteMaster_TYPE   		wxT("type")
#define t3SqliteMaster_NAME			wxT("name")
#define t3SqliteMaster_TBLNAME		wxT("tbl_name")
#define t3SqliteMaster_ROOT			wxT("rootpage")
#define t3SqliteMaster_SQL			wxT("sql")

class lkTSqliteMaster : public lkSQL3TableSet
{
public:
	lkTSqliteMaster();
	lkTSqliteMaster(lkSQL3Database* pDB);

// Operations
public:
	virtual bool				Open(void) wxOVERRIDE; // needs to be overwritten by derived class -- should return FALSE if incorrect table (aka, doesn't exist or 1 or more fields are missing)

// Members
protected:
	lkSQL3FieldData_Text*		m_pFldType;
	lkSQL3FieldData_Text*		m_pFldName;
	lkSQL3FieldData_Text*		m_pFldTblName;
	lkSQL3FieldData_Int*		m_pFldRootpage; // INT64
	lkSQL3FieldData_Text*		m_pFldSql; // memo

private:
	void						Init(void);

public:
	wxString					GetFldType(void) const;
	wxString					GetFldName(void) const;
	wxString					GetFldTbleName(void) const;
	wxInt64						GetFldRootpage(void) const; // cast __int64 to INT64
	wxString					GetFldSql(void) const;

// Implementation
protected:
	virtual void				InitColumns(const wxString&) wxOVERRIDE; // param = tablename

public:
	static wxUint64				GetTableCount(lkSQL3Database* pDB);
	static wxUint64				GetIndexCount(lkSQL3Database* pDB);
	static wxUint64				GetRowCount(lkSQL3Database* pDB, const wxString& table_name);

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(lkTSqliteMaster);
};

#endif // !__LK_TSQLITEMASTER_H__
