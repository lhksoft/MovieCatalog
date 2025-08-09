/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // !_CRT_SECURE_NO_WARNINGS

#ifdef __WXMSW__
#define SQLITE_API __declspec(dllimport)
#endif // __WXMSW__
#include <sqlite3.h>

#include "lkSQL3Common.h"
#include "internal_tools.h"

#include <wx/arrstr.h>

typedef struct FldTypes
{
	sqlFieldType	sqlType;
	const char*		sqlStr;
} FldTypes_t, * FldTypes_p;

static FldTypes_t sqlFieldTypes[] = {
	{ sqlFieldType::sqlInt,		"INTEGER" },
	{ sqlFieldType::sqlReal,	"REAL" },
	{ sqlFieldType::sqlText,	"TEXT" },
	{ sqlFieldType::sqlBlob,	"BLOB" },
	{ sqlFieldType::sqlNone,	0 }
};

const sqlFieldType GetSQLiteType(const wxString& _sqltype)
{
	if ( _sqltype.IsEmpty() )
		return sqlFieldType::sqlNone;

	for ( int i = 0; sqlFieldTypes[i].sqlType != sqlFieldType::sqlNone; i++ )
		if ( _sqltype.CmpNoCase(sqlFieldTypes[i].sqlStr) == 0 )
			return sqlFieldTypes[i].sqlType;

	return sqlFieldType::sqlNone;
}

wxString GetSQLiteTypeStr(sqlFieldType s)
{
	wxString t;

	for ( int i = 0; sqlFieldTypes[i].sqlType != sqlFieldType::sqlNone; i++ )
		if ( sqlFieldTypes[i].sqlType == s )
		{
			t = sqlFieldTypes[i].sqlStr;
			break;
		}
	return t;
}


static bool VoidIsEmpty(int l, const char* s)
{
	return ((l == 0) || (s == NULL) || !s[0]);
}
int lkUTF8_sqlite3_SORT_nlcs(void* arg, int l1, const void* s1, int l2, const void* s2)
{
	bool e1 = VoidIsEmpty(l1, (const char*)s1),
		 e2 = VoidIsEmpty(l2, (const char*)s2);

	int n = 0;
	if ( e1 && !e2 ) n = -1;
	else if ( !e1 && e2 ) n = 1;
	else if ( e1 && e2 ) n = 0;
	else
	{
		wxString ss1, ss2;
		ss1 = lkMakeAnsi(wxString::FromUTF8((const char*)s1, (size_t)l1));
		ss2 = lkMakeAnsi(wxString::FromUTF8((const char*)s2, (size_t)l2));

//		n = ss1.CmpNoCase(ss2);
		n = wxCmpNaturalGeneric(ss1, ss2);
	}

	return n;
}