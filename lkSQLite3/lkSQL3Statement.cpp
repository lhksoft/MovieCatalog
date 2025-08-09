/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // !_CRT_SECURE_NO_WARNINGS
#include "lkSQL3Statement.h"
#include "lkSQL3Database.h"
#include "lkSQL3Field.h"
#include "lkSQL3Exception.h"

#ifdef __WXMSW__
#define SQLITE_API __declspec(dllimport)
#endif // __WXMSW__
#include <sqlite3.h>



static const char* _RESET_STMT = "Statement needs to be reseted.";
static const char* _UNK_FLD = "Unknown column name.";


lkSQL3Statement::lkSQL3Statement(lkSQL3Database& _db, const wxString& _qry) : m_Query(_qry), m_StmtPtr(_db.GetHandle(), _qry)
{
	m_bOK = false;
	m_bDone = false;

	m_nColCount = sqlite3_column_count(m_StmtPtr);
}

lkSQL3Statement::~lkSQL3Statement()
{
	// the finalization will be done by the destructor of the last shared pointer
}

// Members
wxString lkSQL3Statement::GetQuery() const
{
	return m_Query;
}
int lkSQL3Statement::GetColCount() const
{
	return m_nColCount;
}
bool lkSQL3Statement::IsOK() const
{
	return m_bOK;
}
bool lkSQL3Statement::IsDone() const
{
	return m_bDone;
}

int lkSQL3Statement::GetErrCode() const
{
	return sqlite3_errcode(m_StmtPtr);
}
int lkSQL3Statement::GetExtErrCode() const
{
	return sqlite3_extended_errcode(m_StmtPtr);
}
wxString lkSQL3Statement::GetErrMsg() const
{
	wxString errmsg = wxString::FromUTF8(sqlite3_errmsg(m_StmtPtr));
	return errmsg;
}


void lkSQL3Statement::Reset()
{
	m_bOK = false;
	m_bDone = false;
	const int ret = sqlite3_reset(m_StmtPtr);
	SQL3check(m_StmtPtr, ret);
}

void lkSQL3Statement::Clear()
{
	m_bOK = false;
	m_bDone = false;
	const int ret = sqlite3_clear_bindings(m_StmtPtr);
	SQL3check(m_StmtPtr, ret);
}

void lkSQL3Statement::Bind(const int _idx, const int& _i)
{
	const int ret = sqlite3_bind_int(m_StmtPtr, _idx, _i);
	SQL3check(m_StmtPtr, ret);
}
void lkSQL3Statement::Bind(const int _idx, const wxInt64& _i64)
{
	const int ret = sqlite3_bind_int64(m_StmtPtr, _idx, _i64);
	SQL3check(m_StmtPtr, ret);
}
void lkSQL3Statement::Bind(const int _idx, const double& _d)
{
	const int ret = sqlite3_bind_double(m_StmtPtr, _idx, _d);
	SQL3check(m_StmtPtr, ret);
}
void lkSQL3Statement::Bind(const int _idx, const wxString& _c)
{
	const int ret = sqlite3_bind_text(m_StmtPtr, _idx, _c.ToUTF8(), -1, SQLITE_TRANSIENT);
	SQL3check(m_StmtPtr, ret);
}
void lkSQL3Statement::Bind(const int _idx, const void* _v, size_t _t)
{
	const int ret = sqlite3_bind_blob(m_StmtPtr, _idx, _v, (int)_t, SQLITE_TRANSIENT);
	SQL3check(m_StmtPtr, ret);
}
void lkSQL3Statement::Bind(const int _idx) // sets the value to NULL
{
	const int ret = sqlite3_bind_null(m_StmtPtr, _idx);
	SQL3check(m_StmtPtr, ret);
}

void lkSQL3Statement::Bind(const wxString& _n, const int& _i)
{
	const int _idx = sqlite3_bind_parameter_index(m_StmtPtr, _n.ToUTF8());
	Bind(_idx, _i);
}
void lkSQL3Statement::Bind(const wxString& _n, const wxInt64& _i64)
{
	const int _idx = sqlite3_bind_parameter_index(m_StmtPtr, _n.ToUTF8());
	Bind(_idx, _i64);
}
void lkSQL3Statement::Bind(const wxString& _n, const double& _d)
{
	const int _idx = sqlite3_bind_parameter_index(m_StmtPtr, _n.ToUTF8());
	Bind(_idx, _d);
}
void lkSQL3Statement::Bind(const wxString& _n, const wxString& _s)
{
	const int _idx = sqlite3_bind_parameter_index(m_StmtPtr, _n.ToUTF8());
	Bind(_idx, _s);
}
void lkSQL3Statement::Bind(const wxString& _n, const void* _v, size_t _t)
{
	const int _idx = sqlite3_bind_parameter_index(m_StmtPtr, _n.ToUTF8());
	Bind(_idx, _v, _t);
}
void lkSQL3Statement::Bind(const wxString& _n) // sets the value to NULL
{
	const int _idx = sqlite3_bind_parameter_index(m_StmtPtr, _n.ToUTF8());
	Bind(_idx);
}


// Execute a step of the query to fetch one row of results
bool lkSQL3Statement::Step()
{
	if ( !m_bDone )
	{
		const int ret = sqlite3_step(m_StmtPtr);

		switch ( ret )
		{
			case SQLITE_ROW:
				m_bOK = true;
				break;
			case SQLITE_DONE:
				m_bOK = false;
				m_bDone = true;
				break;
			default:
				m_bOK = false;
				m_bDone = false;
				SQL3check(m_StmtPtr, ret);
				break;
		}
	}
	else
		SQL3throw(_RESET_STMT);

	return m_bOK; // true only if one row is accessible by getColumn(N)
}

wxUint64 lkSQL3Statement::GetLastInsertRowID()
{
	return (wxUint64)sqlite3_last_insert_rowid(m_StmtPtr);
}

// Execute a one-step query with no expected result
int lkSQL3Statement::Execute()
{
	if ( !m_bDone )
	{
		const int ret = sqlite3_step(m_StmtPtr);

		switch ( ret )
		{
			case SQLITE_DONE:
				m_bOK = false;
				m_bDone = true;
				break;
			case SQLITE_ROW:
				m_bOK = false;
				m_bDone = false;
				SQL3throw("lkSQL3Statement::Execute() does not expect results.\nUse lkSQL3Statement::Step() instead.");
				break;
			default:
				m_bOK = false;
				m_bDone = false;
				SQL3check(m_StmtPtr, ret);
				break;
		}
	}
	else
		SQL3throw(_RESET_STMT);

	// Return the number of rows modified by those SQL statements (INSERT, UPDATE or DELETE)
	return sqlite3_changes(m_StmtPtr);
}

// Return a copy of the column data specified by its index starting at 0
// (use the Column copy-constructor)
lkSQL3Field lkSQL3Statement::GetField(const int _idx)
{
	CheckRow();
	CheckIndex(_idx);

	// Share the Statement Object handle with the new Column created
	return lkSQL3Field(m_StmtPtr, _idx);
}

lkSQL3Field lkSQL3Statement::GetField(const wxString& _n)
{
	CheckRow();

	if ( m_ColNames.empty() )
	{
		for ( int i = 0; i < m_nColCount; ++i )
		{
			const char* pName = sqlite3_column_name(m_StmtPtr, i);
			m_ColNames[pName] = i;
		}
	}

	const char* _n_ = _n.ToUTF8();
	const TColumnNames::const_iterator iIndex = m_ColNames.find(_n_);

	if ( iIndex == m_ColNames.end() )
		SQL3throw(_UNK_FLD);

	// Share the Statement Object handle with the new Column created
	return lkSQL3Field(m_StmtPtr, (*iIndex).second);
}

// Test if the column value is NULL
bool lkSQL3Statement::IsFieldNull(const int _idx) const
{
	CheckRow();
	CheckIndex(_idx);
	return (sqlite3_column_type(m_StmtPtr, _idx) == SQLITE_NULL);
}
bool lkSQL3Statement::IsFieldNull(const wxString& _n)
{
	CheckRow();

	if ( m_ColNames.empty() )
	{
		for ( int i = 0; i < m_nColCount; ++i )
		{
			const char* pName = sqlite3_column_name(m_StmtPtr, i);
			m_ColNames[pName] = i;
		}
	}

	const char* _n_ = _n.ToUTF8();
	const TColumnNames::const_iterator iIndex = m_ColNames.find(_n_);

	if ( iIndex == m_ColNames.end() )
		SQL3throw(_UNK_FLD);

	return (sqlite3_column_type(m_StmtPtr, (*iIndex).second) == SQLITE_NULL);
}


// Return the named assigned to the specified result column (potentially aliased)
wxString lkSQL3Statement::GetFieldName(const int _idx) const
{
	CheckIndex(_idx);

	wxString s = wxString::FromUTF8(sqlite3_column_name(m_StmtPtr, _idx));

	return s;
}
// Return a pointer to the table column name that is the origin of the specified result column
wxString lkSQL3Statement::GetFieldOriginName(const int _idx) const
{
	CheckIndex(_idx);

	wxString s = wxString::FromUTF8(sqlite3_column_origin_name(m_StmtPtr, _idx));

	return s;
}



void lkSQL3Statement::CheckRow() const
{
	if ( !m_bOK )
		SQL3throw("No row to get a column from.\nlkSQL3Statement::Step() was not called, or returned false.");
}
void lkSQL3Statement::CheckIndex(const int _idx) const
{
	if ( (_idx < 0) || (_idx >= m_nColCount) )
		SQL3throw("Column index out of range.");
}

////////////////////////////////////////////////////////////////////////////////
// Internal class : shared pointer to the sqlite3_stmt SQLite Statement Object
////////////////////////////////////////////////////////////////////////////////

lkSQL3Statement::Ptr::Ptr()
{
	m_DB = NULL;
	m_Stmt = NULL;
	m_pRefCnt = NULL;

	wxASSERT_MSG(false, wxT("This constructor should NOT be used !!"));
}
/**
* Prepare the statement and initialize its reference counter
*
* param[in] apSQLite  The sqlite3 database connexion
* param[in] aQuery    The SQL query string to prepare
*/
lkSQL3Statement::Ptr::Ptr(HSQLITE3 _DB, const wxString& _qry)
{
	m_DB = _DB;
	m_Stmt = NULL;
	m_pRefCnt = NULL;

	const char* sA = _qry.ToUTF8();
	const int ret = sqlite3_prepare_v2(m_DB, sA, (int)wxStrlen(sA), &m_Stmt, NULL);

	SQL3check(m_DB, ret);

	// Initialize the reference counter of the sqlite3_stmt :
	// used to share the mStmtPtr between Statement and Column objects;
	// This is needed to enable Column objects to live longer than the Statement objet it refers to.
	m_pRefCnt = new wxUint32(1);  // NOLINT(readability/casting)
}

/**
* Copy constructor increments the ref counter
*
* param[in] aPtr Pointer to copy
*/
lkSQL3Statement::Ptr::Ptr(const lkSQL3Statement::Ptr& _Ptr)
{
	m_DB = _Ptr.m_DB;
	m_Stmt = _Ptr.m_Stmt;
	m_pRefCnt = _Ptr.m_pRefCnt;

	wxASSERT(m_pRefCnt != NULL);
	wxASSERT(*m_pRefCnt != 0);

	// Increment the reference counter of the sqlite3_stmt,
	// asking not to finalize the sqlite3_stmt during the lifetime of the new objet
	++(*m_pRefCnt);
}

/**
* Decrement the ref counter and finalize the sqlite3_stmt when it reaches 0
*/
lkSQL3Statement::Ptr::~Ptr() // nothrow
{
	wxASSERT(m_pRefCnt != NULL);
	wxASSERT(*m_pRefCnt != 0);

	// Decrement and check the reference counter of the sqlite3_stmt
	if ( --(*m_pRefCnt) == 0 )
	{
		// If count reaches zero, finalize the sqlite3_stmt, as no Statement nor Column objet use it anymore.
		// No need to check the return code, as it is the same as the last statement evaluation.
		sqlite3_finalize(m_Stmt);

		// and delete the reference counter
		delete m_pRefCnt;
		m_pRefCnt = NULL;
		m_Stmt = NULL;
	}
	// else, the finalization will be done later, by the last object
}


