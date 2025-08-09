/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#ifndef __LK_SQL3RECORDSET_H__
#define __LK_SQL3RECORDSET_H__
#pragma once

#include "lkSQL3Statement.h"
#include "lkSQL3Database.h"
#include "lkSQL3FieldData.h"

#include <wx/object.h>

 // Based at CDaoRecordset


class lkSQL3RecordSet : public wxObject
{
public:
	lkSQL3RecordSet();
	lkSQL3RecordSet(lkSQL3Database* pDB);
	virtual ~lkSQL3RecordSet();

	static lkSQL3RecordSet*			CreateDynamic(wxClassInfo*, lkSQL3Database*);
	virtual void					PostCreateDynamic(void); // will be called after previous function, default doesn't do anything

// Data Members
protected:
	lkSQL3Database*					m_DB;
	lkSQL3FieldSet					m_Col;

	lkSQL3Database*					m_FindDB; // tempory database, used for the Find-routines

	int								m_ColCount;	// number of actual columns in table
	wxUint64						m_RowCount;
	wxUint64						m_CurRow;	// current index in m_pRowIDs (1-based)
	wxUint64*						m_pRowIDs;
	size_t							m_pRowIDsSize; // bigger to allow us to add new records

	wxUint32						m_RowIdGrow; // initialy set to 1000 - set to smaller value in case of small tables

	wxString						m_Table; // Table
	wxString						m_View; // in case m_Table=='', m_View could contain an entire sql-query to use instead
	wxString						m_Rowid; // default=[ROWID] ALIAS ID
	wxString						m_Query;
	wxString						m_Filter;
	wxString						m_Sort;

	bool							m_bIsOpen; // true if all columns initialized, done in derived class

	wxUint64						m_FindRowid; // actual row-id of current Find

// Filter Attributes
	wxString						m_sFltFilter, m_sFltOrder;
	bool							m_bFilterActive;

public:
	bool							m_GotFindNext, m_GotFindPrev; // set during 'FindFirst', 'FindLast', 'FindNext' and 'FindLast'

	void							SetDatabase(lkSQL3Database* pDB);
	bool							IsOpen(void) const;

	int								GetColCount(void) const;
	wxUint64						GetRowCount(void) const;
	wxUint64						GetCurRow(void) const;
	wxUint64						GetActualCurRow(void) const; // returns m_pRowIDs[m_CurRow]
	wxString						GetViewName(void) const; // returns m_Table (NOT m_View)
	wxUint64						FindActualCurRow(wxUint64); // returns 0 if not found, if found value is > 0

	wxUint64						GetMaxActualRow(void); // returns highest number of ROWID

	void							SetFilter(const wxString&); // should NOT start with 'WHERE '
	wxString						GetFilter(void) const;

	void							SetOrder(const wxString&); // should NOT start with 'ORDER BY '
	wxString						GetOrder(void) const;

	virtual lkSQL3FieldData*		GetField(const wxString& fldName); // NULL if not found

	void							ResetFind(void); // resets settings and kils tempory findDatabase

	// each of following returns index to the first result -- will not change current location, rather returns the location to which one can go using Move
	virtual wxUint64				FindInit(wxClassInfo* rc, const wxString& search_on, bool bForward);
	wxUint64						FindNext(void);
	wxUint64						FindPrev(void);
	virtual bool					TestFind(const wxString& search_on, bool forward);

	void							SetFltFilter(const wxString&);
	void							SetFltOrder(const wxString&);
	virtual void					SetFilterActive(bool);

	enum class FIND_DIRECTION
	{
		fndINVALID = 0,
		fndStart = 1, // default
		fndAnywhere,
		fndEntire,
		fndEnd,
		fndLAST = fndEnd
	};
	virtual void					CreateFindFilter(wxString& Filter, const wxString& Field, const wxString& Search, FIND_DIRECTION findDirection); // if required, can be overwritten by derived class
	static wxString					CreateFindFilter(const wxString& Field, const wxString& Search, FIND_DIRECTION findDirection, const wxString& Table = wxEmptyString);

	// Operations
protected:
	virtual void					InitColumns(const wxString&); // param = name of the View or Table
	virtual wxString				GetQuery(bool _rowid = false, bool _filter = true, bool _order = true);
	virtual bool					LoadData(void); // reads current row into m_Col

	void							SetFindBools(void); // used internally

public:
	virtual bool					Open(void); // needs to be overwritten by derived class
	void							Query(void); // analizes how much data is in the query
	virtual void					Requery(wxUint64 repos = 0); // re-analizes how much data is in the query

	// Each of following 8 returns by default false, it should be overwritten in derived classes
	virtual bool					CanAdd(void);
	virtual bool					AddNew(void);
	virtual void					CancelAddNew(void);
	virtual bool					IsAdding(void) const;

	virtual bool					CanUpdate(void);
	virtual bool					Update(void);
	virtual void					CancelUpdate(void);

	virtual bool					CanDelete(void);
	virtual bool					Delete(void); // deletes current row and requery, moves current row to first row after deleted row and if atEOF then 1 row before current row
	virtual bool					DeleteRow(wxUint64 nRow); // deletes given row, does not do a requery

	bool							IsBOF(void);
	bool							IsEOF(void);
	bool							IsEmpty(void);

	bool							Move(wxUint64 idx); // move to a specific index.  1-based, because of I was making it like CRecordSet
	bool							MoveFirst(void);
	bool							MoveLast(void);
	bool							MoveNext(void);
	bool							MovePrev(void);

	bool							CanMoveFirst(void);
	bool							CanMoveLast(void);
	bool							IsOnFirstRecord(void);
	bool							IsOnLastRecord(void);

	wxUint64						GetMaxID(void); // returns MAX of ROWID

	bool							IsDirty(lkSQL3FieldData*); // checks if given Field is marked Dirty, if NULL checks all fields until 1 is marked as Dirty

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(lkSQL3RecordSet);
};

#endif // !__LK_SQL3RECORDSET_H__
