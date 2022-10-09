/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************
 * This class based at lkSQL3Database and partialy at wxTempFile
 ***********************************************************/
#ifndef __LK_SQL3TEMPDB_H__
#define __LK_SQL3TEMPDB_H__
#pragma once

#include "lkSQL3Database.h"

class lkSQL3TempDB : public lkSQL3Database
{
public:
	lkSQL3TempDB();
	virtual ~lkSQL3TempDB();

	// open the temp file (dbPath is the name of file to be replaced) -- might throw
	bool							OpenAsTemp(const wxString& dbPath);

	// different ways to close the file
	 // accept changes and delete the old file of name m_strPath
	bool							Commit(void);
	 // discard changes
	void							Discard(void);

// Attributes
protected:
	wxString						m_strTemp, m_strPath;

private:
	wxDECLARE_NO_COPY_CLASS(lkSQL3TempDB);
};

#endif // !__LK_SQL3TEMPDB_H__

