/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // !_CRT_SECURE_NO_WARNINGS
#include "lkSQL3Field.h"
#include "lkSQL3Database.h"
#include "lkSQL3Statement.h"
#include "lkSQL3Exception.h"

#ifdef __WXMSW__
#define SQLITE_API __declspec(dllimport)
#endif // __WXMSW__
#include <sqlite3.h>

#include <wx/filename.h>

static int ToSQL3OpenFlags(wxUint32 flags)
{
	int _flags = 0;

	if ( flags & lkSQL3Database::enOpenFlags::ofCreate )			_flags = SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE;
	else if ( flags & lkSQL3Database::enOpenFlags::ofReadWrite )	_flags = SQLITE_OPEN_READWRITE;
	else if ( flags & lkSQL3Database::enOpenFlags::ofReadOnly )		_flags = SQLITE_OPEN_READONLY;

	if ( flags & lkSQL3Database::enOpenFlags::ofDelOnClose )		_flags |= SQLITE_OPEN_DELETEONCLOSE;
	if ( flags & lkSQL3Database::enOpenFlags::ofExclusive )			_flags |= SQLITE_OPEN_EXCLUSIVE;
	if ( flags & lkSQL3Database::enOpenFlags::ofMemory )			_flags |= SQLITE_OPEN_MEMORY;

	return _flags;
}

// lkSQL3Database

lkSQL3Database::lkSQL3Database() : m_DB(NULL), m_Path()
{
	m_bIsOpen = false;
	m_bCanUpdate = false;
}
lkSQL3Database::~lkSQL3Database()
{
	Close();
}

// lkSQL3Database member functions

void lkSQL3Database::Open(const wxString& _dbPath, wxUint32/*lkSQL3Database::enOpenFlags*/ _flags, const int aBusyTimeoutMs/* = 0*/)
{
	Close(); // if open

	int ret = sqlite3_open_v2(_dbPath.ToUTF8(), &m_DB, ToSQL3OpenFlags(_flags), 0);

	if ( ret != SQLITE_OK )
	{
		wxString strerr = wxString::FromUTF8(sqlite3_errmsg(m_DB));
		sqlite3_close(m_DB); // close is required even in case of error on opening
		SQL3throw(strerr);
	}

	// Registring custom sort-routine for legacy-UTF8 string, to be used only in 'ORDER BY' clausule
	ret = sqlite3_create_collation(m_DB, "lkUTF8compare", SQLITE_UTF8, NULL, &lkUTF8_sqlite3_SORT_nlcs);
	SQL3check(m_DB, ret);

	if ( aBusyTimeoutMs > 0 )
		SetBusyTimeout(aBusyTimeoutMs);
	m_Path = _dbPath;
	m_bIsOpen = true;
	m_bCanUpdate = !(_flags & lkSQL3Database::enOpenFlags::ofReadOnly);
}

bool lkSQL3Database::Close()
{
	if ( m_DB == NULL )
		return true;

	const int ret = sqlite3_close(m_DB);

	// Only case of error is SQLITE_BUSY: "database is locked" (some statements are not finalized)
	// Never throw an exception in a destructor :
	wxASSERT(SQLITE_OK == ret); // database is locked

	if ( SQLITE_OK == ret )
	{
		m_Path.Empty();
		m_DB = NULL;
		m_bIsOpen = false;
		m_bCanUpdate = false;
		return true;
	}
	// else
	return false;
}

bool lkSQL3Database::IsOpen() const
{
	return m_bIsOpen;
}
bool lkSQL3Database::CanUpdate() const
{
	return m_bCanUpdate;
}

void lkSQL3Database::SetBusyTimeout(const int aBusyTimeoutMs)
{
	const int ret = sqlite3_busy_timeout(m_DB, aBusyTimeoutMs);
	SQL3check(m_DB, ret);
}

// Shortcut to execute one or multiple SQL statements without results (UPDATE, INSERT, ALTER, COMMIT, CREATE...).
void lkSQL3Database::ExecuteSQL(const wxString& _sql)
{
	wxASSERT(!_sql.IsEmpty());
	const int ret = sqlite3_exec(m_DB, _sql.ToUTF8(), NULL, NULL, NULL);

	SQL3check(m_DB, ret);
}
// Shortcut to execute a one step query and fetch the first column of the result.
lkSQL3Field lkSQL3Database::ExecuteSQLAndGet(const wxString& _sql)
{
	lkSQL3Statement stmt(*this, _sql);
	stmt.Step();
	return stmt.GetField(0);
}

bool lkSQL3Database::TableExists(const wxString& _table)
{
	wxASSERT(!_table.IsEmpty());

	lkSQL3Statement stmt(*this, wxT("SELECT count(*) FROM sqlite_master WHERE type='table' AND name=?"));
	stmt.Bind(1, _table);
	stmt.Step();
	const int b = stmt.GetField(0);
	return (b == 1);
}

wxUint64 lkSQL3Database::GetLastInsertRowid() const
{
	return (wxUint64)sqlite3_last_insert_rowid(m_DB);
}

int lkSQL3Database::GetTotalChanges() const
{
	return sqlite3_total_changes(m_DB);
}

wxString lkSQL3Database::GetPath() const
{
	return m_Path;
}

wxString lkSQL3Database::GetName() const
{
	wxFileName fn = wxFileName(m_Path);
	wxString fName = fn.GetName();

	return fName;
}

HSQLITE3 lkSQL3Database::GetHandle()
{
	return m_DB;
}

int lkSQL3Database::GetErrorCode() const
{
	return sqlite3_errcode(m_DB);
}

int lkSQL3Database::GetExtendedErrorCode() const
{
	return sqlite3_extended_errcode(m_DB);
}

wxString lkSQL3Database::GetErrorMsg() const
{
	wxString errmsg = wxString::FromUTF8(sqlite3_errmsg(m_DB));
	return errmsg;
}

void lkSQL3Database::Compact() // be careful using this, as it will take time, plus if you use the auto-vacuum, you don't need it
{
	ExecuteSQL(wxT("VACUUM"));
}

