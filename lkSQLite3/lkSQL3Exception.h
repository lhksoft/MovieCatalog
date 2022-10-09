/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#ifndef __LK_SQL3EXCEPTION_H__
#define __LK_SQL3EXCEPTION_H__
#pragma once

#include "lkSQL3Common.h"
//#include <exception>
#include <stdexcept>
#include <wx/string.h>

class lkSQL3Exception : public std::runtime_error
{
public:
	lkSQL3Exception();
	lkSQL3Exception(const lkSQL3Exception&);
	lkSQL3Exception(const char* _szErr);
	lkSQL3Exception(const char* _szErr, int);
	lkSQL3Exception& operator=(const lkSQL3Exception&);
	virtual ~lkSQL3Exception();

	virtual int	ReportError(void);
	wxString	GetError(void) const;
};

/**
* Check if aRet equal SQLITE_OK, else throw a CSQL3Exception with the SQLite error message
*/
void SQL3throw(const char*);
void SQL3check(HSQLITE3, const int aRet);

#endif // !__LK_SQL3EXCEPTION_H__
