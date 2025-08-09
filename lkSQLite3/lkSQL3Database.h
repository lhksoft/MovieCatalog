/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#ifndef __LK_SQL3DATABASE_H__
#define __LK_SQL3DATABASE_H__
#pragma once

#include "lkSQL3Common.h"

// CSQL3Database
class lkSQL3Field;
class lkSQL3Database
{
public:
	enum enOpenFlags
	{
		ofReadOnly = 0x01,
		ofReadWrite = 0x02,
		ofCreate = 0x04,
		ofDelOnClose = 0x08,
		ofExclusive = 0x10,
		ofMemory = 0x20,
		ofStandard = (ofReadWrite | ofExclusive)
	};

	lkSQL3Database();
	virtual ~lkSQL3Database();

	void				Open(const wxString& _dbPath, wxUint32/*lkSQL3Database::enOpenFlags*/ _flags = lkSQL3Database::enOpenFlags::ofStandard, const int aBusyTimeoutMs = 0);
	bool				Close(void);

	bool				IsOpen(void) const;
	bool				CanUpdate(void) const;

	void				SetBusyTimeout(const int aBusyTimeoutMs);

	/*  This is useful for any kind of statements other than the Data Query Language (DQL) "SELECT" :
	 *  - Data Manipulation Language (DML) statements "INSERT", "UPDATE" and "DELETE"
	 *  - Data Definition Language (DDL) statements "CREATE", "ALTER" and "DROP"
	 *  - Data Control Language (DCL) statements "GRANT", "REVOKE", "COMMIT" and "ROLLBACK"
	 */
	void				ExecuteSQL(const wxString& _sql);

	// Shortcut to execute a one step query and fetch the first column of the result.
	lkSQL3Field			ExecuteSQLAndGet(const wxString& _sql);

	/*  Shortcut to test if a table exists.
	 *  Table names are case sensitive.
	 */
	bool				TableExists(const wxString& _table);

	// Get the rowid of the most recent successful INSERT into the database from the current connection.
	wxUint64			GetLastInsertRowid(void) const;

	// Get total number of rows modified by all INSERT, UPDATE or DELETE statement since connection.
	int					GetTotalChanges(void) const;

	// Return the numeric result code for the most recent failed API call (if any).
	int					GetErrorCode(void) const;

	// Return the extended numeric result code for the most recent failed API call (if any).
	int					GetExtendedErrorCode(void) const;

	// Return English language explanation of the most recent failed API call (if any).
	wxString			GetErrorMsg(void) const;

	void				Compact(void); // be careful using this, as it will take time, plus if you use the auto-vacuum, you don't need it

//	const CSQL3Database& operator=(const CSQL3Database&);
	// Data Members
protected:
	HSQLITE3			m_DB;
	wxString			m_Path;
	bool				m_bIsOpen;
	bool				m_bCanUpdate;

public:
	wxString			GetPath(void) const;
	wxString			GetName(void) const;
	HSQLITE3			GetHandle(void);
};


#endif // !__LK_SQL3DATABASE_H__
