/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#ifndef __LK_SQL3COMMON_H__
#define __LK_SQL3COMMON_H__
#pragma once

#include <wx/string.h>

typedef struct sqlite3			sqlite3;
typedef sqlite3*				HSQLITE3;

typedef struct sqlite3_stmt		sqlite3_stmt;
typedef sqlite3_stmt*			HSQLITE3STMT;

enum class sqlFieldType
{
	sqlNone = 0,
	sqlInt,			// INTEGER (bool / int / int64)
	sqlReal,		// REAL (single / double / date)
	sqlText,		// TEXT (char / string / memo)
	sqlBlob			// BLOB (binary)
};

enum class sqlSortOrder
{
	sortNone = 0,
	sortAsc,
	sortDesc
};

// //////////////////////////////////////////////////////////////////////////////////////////////

const sqlFieldType GetSQLiteType(const wxString& _sqltype);
wxString GetSQLiteTypeStr(sqlFieldType s);

// //////////////////////////////////////////////////////////////////////////////////////////////

// used as COLLATE sort function NO LOCALLE (or LOCALLE ALL) and CASE INSENSITIVE
int lkUTF8_sqlite3_SORT_nlcs(void* arg, int l1, const void* s1, int l2, const void* s2);

#endif // !__LK_SQL3COMMON_H__
