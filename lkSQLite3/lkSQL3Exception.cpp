/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // !_CRT_SECURE_NO_WARNINGS
#include "lkSQL3Exception.h"
#include <wx/msgdlg.h>

#ifdef __WXMSW__
#define SQLITE_API __declspec(dllimport)
#endif // __WXMSW__
#include <sqlite3.h>

static const char* _msgTitle = "SQLite3 - Exception";


lkSQL3Exception::lkSQL3Exception() : std::runtime_error("")
{
}

lkSQL3Exception::lkSQL3Exception(const lkSQL3Exception& e) : std::runtime_error(e.what())
{
}
lkSQL3Exception::lkSQL3Exception(const char* _szErr) : std::runtime_error(_szErr)
{
}
lkSQL3Exception::lkSQL3Exception(const char* _szErr, int i) : std::runtime_error(_szErr)
{
}
lkSQL3Exception& lkSQL3Exception::operator=(const lkSQL3Exception& e)
{
	*((std::runtime_error*)this) = *((std::runtime_error*)&e); // copy the private DATA member
	return *this;
}

lkSQL3Exception::~lkSQL3Exception()
{
}

int lkSQL3Exception::ReportError()
{
	wxString szErr = GetError();
	return wxMessageBox(szErr, _msgTitle, wxOK | wxICON_WARNING | wxCENTER);
}

wxString lkSQL3Exception::GetError() const
{
	wxString szErr = wxString::FromUTF8(what());

	if ( szErr.IsEmpty() )
		szErr = wxT("Unknown SQLite3 error.");

	return szErr;
}

// /////////////////////////////////////////////////////////////////////////////////////////////////

void SQL3throw(const char* _sErr)
{
	throw lkSQL3Exception(_sErr);
}

void SQL3check(HSQLITE3 _db, const int aRet)
{
	if ( SQLITE_OK != aRet )
	{
		SQL3throw(sqlite3_errmsg(_db));
	}
}
