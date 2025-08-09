/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#ifndef __LK_SQL3STATEMENT_H__
#define __LK_SQL3STATEMENT_H__
#pragma once

#include "lkSQL3Common.h"

#include <string>
#include <map>

class lkSQL3Database;
class lkSQL3Field;
class lkSQL3Statement
{
	friend class lkSQL3Field;
public:
	lkSQL3Statement(lkSQL3Database&, const wxString& _qry);
	virtual ~lkSQL3Statement();

	// Members
public:
	wxString			GetQuery(void) const;
	int					GetColCount(void) const;
	bool				IsOK(void) const;
	bool				IsDone(void) const;

	int					GetErrCode(void) const;
	int					GetExtErrCode(void) const;
	wxString			GetErrMsg(void) const;

public:
	void				Reset(void); // Reset the statement to make it ready for a new execution.
	void				Clear(void); // Use this routine to reset all parameters to NULL.

	////////////////////////////////////////////////////////////////////////////
	// Bind a value to a parameter of the SQL statement,
	// in the form "?" (unnamed), "?NNN", ":VVV", "@VVV" or "$VVV".
	//
	// Can use the parameter index, starting from "1", to the higher NNN value,
	// or the complete parameter name "?NNN", ":VVV", "@VVV" or "$VVV"
	// (prefixed with the corresponding sign "?", ":", "@" or "$")
	//
	// Note that for text and blob values, the SQLITE_TRANSIENT flag is used,
	// which tell the sqlite library to make its own copy of the data before the bind() call returns.
	// This choice is done to prevent any common misuses, like passing a pointer to a
	// dynamic allocated and temporary variable (a std::string for instance).
	// This is under-optimized for static data (a static text define in code)
	// as well as for dynamic allocated buffer which could be transfer to sqlite
	// instead of being copied.

	// Bind values according their 1-based index
	void				Bind(const int, const int&);
	void				Bind(const int, const wxInt64&);
	void				Bind(const int, const double&);
	void				Bind(const int, const wxString&);
	void				Bind(const int, const void*, size_t);
	void				Bind(const int); // sets the value to NULL

	// Bind values according named parameters in the SQL
	void				Bind(const wxString&, const int&);
	void				Bind(const wxString&, const wxInt64&);
	void				Bind(const wxString&, const double&);
	void				Bind(const wxString&, const wxString&);
	void				Bind(const wxString&, const void*, size_t);
	void				Bind(const wxString&); // sets the value to NULL

	// Operations
public:
	/**
	* Execute a step of the prepared query to fetch one row of results.
	*
	*  While true is returned, a row of results is available, and can be accessed thru the getColumn() method
	*
	* return - true  (SQLITE_ROW)  if there is another row ready : you can call getColumn(N) to get it
	*                              then you have to call executeStep() again to fetch more rows until the query is finished
	*        - false (SQLITE_DONE) if the query has finished executing : there is no (more) row of result
	*                              (case of a query with no result, or after N rows fetched successfully)
	*/
	bool				Step(void);
	wxUint64			GetLastInsertRowID(void);

	/**
	* Execute a one-step query with no expected result.
	*
	*  This method is useful for any kind of statements other than the Data Query Language (DQL) "SELECT" :
	*  - Data Definition Language (DDL) statements "CREATE", "ALTER" and "DROP"
	*  - Data Manipulation Language (DML) statements "INSERT", "UPDATE" and "DELETE"
	*  - Data Control Language (DCL) statements "GRANT", "REVOKE", "COMMIT" and "ROLLBACK"
	*
	* It is similar to CSQL3Database::ExecuteSQL(), but using a precompiled statement, it adds :
	* - the ability to bind() arguments to it (best way to insert data),
	* - reusing it allows for better performances (efficient for multiple insertion).
	*
	* return number of row modified by this SQL statement (INSERT, UPDATE or DELETE)
	*/
	int					Execute(void);

	// Return a copy of the column data specified by its index
	lkSQL3Field			GetField(const int);
	// Return a copy of the column data specified by its (aliased) column name
	lkSQL3Field			GetField(const wxString&);
	// Test if the column value is NULL
	bool				IsFieldNull(const int) const;
	bool				IsFieldNull(const wxString&);

	// Return a pointer to the named assigned to the specified result column (potentially aliased)
	wxString			GetFieldName(const int) const;
	// Return a pointer to the table column name that is the origin of the specified result column
	wxString			GetFieldOriginName(const int) const;

private:
	void				CheckRow(void) const;
	void				CheckIndex(const int) const;

	class Ptr
	{
	public:
		Ptr(); // not allowed, but error C2512 will be raised in CSQL3Field if no default contructor is available
		// Prepare the statement and initialize its reference counter
		Ptr(HSQLITE3, const wxString& _qry);
		// Copy constructor increments the ref counter
		Ptr(const Ptr&);
		// Decrement the ref counter and finalize the sqlite3_stmt when it reaches 0
		~Ptr(); // nothrow (no virtual destructor needed here)

	// Data Members
	private:
		HSQLITE3		m_DB;		// Pointer to SQLite Database Connection Handle
		HSQLITE3STMT	m_Stmt;		// Pointer to SQLite Statement Object
		wxUint32*		m_pRefCnt;	// Pointer to the heap allocated reference counter of the sqlite3_stmt

	public:
		inline operator HSQLITE3() const	// Inline cast operator returning the pointer to SQLite Database Connection Handle
		{
			return m_DB;
		}
		inline operator HSQLITE3STMT() const // Inline cast operator returning the pointer to SQLite Statement Object
		{
			return m_Stmt;
		}
	};

	// Data Members
private:
	typedef std::map<std::string, int> TColumnNames;

	TColumnNames		m_ColNames;
	wxString			m_Query;
	Ptr					m_StmtPtr;
	int					m_nColCount;
	bool				m_bOK; // true when a row has been fetched with executeStep()
	bool				m_bDone; // true when the last executeStep() had no more row to fetch
};

#endif // !__LK_SQL3STATEMENT_H__
