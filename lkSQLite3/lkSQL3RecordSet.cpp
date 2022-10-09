/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // !_CRT_SECURE_NO_WARNINGS
#include "lkSQL3RecordSet.h"
#include "lkSQL3Field.h"
#include "lkSQL3FindTable.h"
#include "lkSQL3Exception.h"
#include <sqlite3.h>

#include "internal_tools.h"

wxIMPLEMENT_DYNAMIC_CLASS(lkSQL3RecordSet, wxObject);

lkSQL3RecordSet::lkSQL3RecordSet() : wxObject(),
	m_Col(), m_Query(), m_Filter(), m_Sort(), m_Table(), m_View(), m_Rowid(),
	m_sFltFilter(), m_sFltOrder()
{
	m_RowIdGrow = 100;
	m_Rowid = wxT("[ROWID] AS ID");

	m_ColCount = 0;
	m_RowCount = 0;
	m_CurRow = 0;
	m_pRowIDs = NULL;
	m_pRowIDsSize = 0;
	m_bIsOpen = false;

	m_FindRowid = 0;
	m_GotFindNext = m_GotFindPrev = false;
	m_FindDB = NULL;
	m_DB = NULL;

	m_bFilterActive = false;
}
lkSQL3RecordSet::lkSQL3RecordSet(lkSQL3Database* pDB) : wxObject(),
	m_Col(), m_Query(), m_Filter(), m_Sort(), m_Table(), m_View(), m_Rowid(),
	m_sFltFilter(), m_sFltOrder()
{
	m_RowIdGrow = 100;
	m_Rowid = wxT("[ROWID] AS ID");

	m_ColCount = 0;
	m_RowCount = 0;
	m_CurRow = 0;
	m_pRowIDs = NULL;
	m_pRowIDsSize = 0;
	m_bIsOpen = false;

	m_FindRowid = 0;
	m_GotFindNext = m_GotFindPrev = false;
	m_FindDB = NULL;

	m_bFilterActive = false;

	SetDatabase(pDB);
}
lkSQL3RecordSet::~lkSQL3RecordSet()
{
	if ( m_pRowIDs )
		delete[] m_pRowIDs;

	if ( m_FindDB )
		delete m_FindDB;
}

//static
lkSQL3RecordSet* lkSQL3RecordSet::CreateDynamic(wxClassInfo* rc, lkSQL3Database* db)
{
	wxASSERT(rc != NULL);
	if ( !rc ) return NULL;
	wxASSERT(rc->IsDynamic());
	if ( !rc->IsDynamic() )
		return NULL; // proberly not done a DECLARE_DYNAMIC

	if ( !rc->IsKindOf(wxCLASSINFO(lkSQL3RecordSet)) )
		return NULL;

	// create one
	wxObject* o;
	if ( !(o = rc->CreateObject()) )
	{
		wxASSERT(false);
		return NULL; // something wend wrong?
	}
	wxASSERT(o->IsKindOf(rc) && o->IsKindOf(wxCLASSINFO(lkSQL3RecordSet)));

	lkSQL3RecordSet* pSet = (lkSQL3RecordSet*)o;
	o = NULL;

	pSet->SetDatabase(db);
	pSet->PostCreateDynamic();

	return pSet;
}

//virtual
void lkSQL3RecordSet::PostCreateDynamic() // will be called after previous function, default doesn't do anything
{
	// in here derived classes can set things that should be set in newly dynamicly created recordsets
}

// Members
void lkSQL3RecordSet::SetDatabase(lkSQL3Database* pDB)
{
	m_DB = pDB;
}

bool lkSQL3RecordSet::IsOpen(void) const
{
	return m_bIsOpen;
}

int lkSQL3RecordSet::GetColCount() const
{
	return m_ColCount;
}
wxUint64 lkSQL3RecordSet::GetRowCount() const
{
	return m_RowCount;
}
wxUint64 lkSQL3RecordSet::GetCurRow() const
{
	return m_CurRow;
}
wxUint64 lkSQL3RecordSet::GetActualCurRow() const // returns m_pRowIDs[m_CurRow]
{
	return (m_pRowIDs && (m_CurRow > 0) && (m_CurRow <= m_RowCount)) ? m_pRowIDs[m_CurRow - 1] : 0;
}
wxUint64 lkSQL3RecordSet::FindActualCurRow(wxUint64 curRow)
{
	wxUint64 pos = 0;
	if ( (m_RowCount > 0) && (curRow > 0) && (m_pRowIDs != NULL) )
	{
		wxUint64 start;
		if ( curRow == m_RowCount )
			return (m_RowCount);
		else if ( curRow > (m_RowCount / 2) )
			start = m_RowCount / 2;
		else
			start = 0;

		for ( pos = start; pos < m_RowCount; pos++ )
			if ( m_pRowIDs[pos] == curRow )
			{
				pos++;
				break;
			}
	}
	return pos;
}

wxString lkSQL3RecordSet::GetViewName() const
{
	return m_Table;
}

wxUint64 lkSQL3RecordSet::GetMaxActualRow()
{
	if ( !m_DB || !m_DB->IsOpen() ) return 0;
	wxASSERT(!m_Table.IsEmpty() || !m_View.IsEmpty());
	wxUint64 ret = 0;

	if ( !IsEmpty() )
	{
		wxString qry;
		if ( !m_Table.IsEmpty() )
			qry.Printf(wxT("SELECT MAX([ROWID]) FROM [%s];"), m_Table);
		else
			qry.Printf(wxT("SELECT MAX([ROWID]) FROM (%s);"), m_View);
		try
		{
			ret = m_DB->ExecuteSQLAndGet(qry); // .GetInt());
		}
		catch ( const lkSQL3Exception& e )
		{
			((lkSQL3Exception*)&e)->ReportError();
			ret = 0;
		}
	}
	return ret;
}

void lkSQL3RecordSet::SetFilter(const wxString& _F) // should NOT start with 'WHERE '
{
	if ( _F.IsEmpty() )
		m_Filter.Empty();
	else
	{
		// **DEBUG ME** not tested yet !!
		static const wxChar* _where = wxT("WHERE");

		wxString s1, s2;
		s2 = _F;
		s1 = lkTrimLeft(s2, wxT(" ")); // remove any leading space
		s2 = s1;
		if ( !(s1 = s2.Left(wxStrlen(_where))).IsEmpty() && (s1.CmpNoCase(_where) == 0) )
			s1 = s2.Right(s2.Len() - wxStrlen(_where));
		else
			s1 = s2;
		s2 = lkTrimLeft(s1, wxT(" ")); // remove any leading space

		m_Filter = s2;
	}
}
wxString lkSQL3RecordSet::GetFilter() const
{
	return m_Filter;
}

void lkSQL3RecordSet::SetOrder(const wxString& _S) // should NOT start with 'ORDER BY '
{
	if ( _S.IsEmpty() )
		m_Sort.Empty();
	else
	{
		// **DEBUG ME** not tested yet !!
		static const wxChar* _order = wxT("ORDER BY");

		wxString s1, s2;
		s2 = _S;
		s1 = lkTrimLeft(s2, wxT(" ")); // remove any leading space
		s2 = s1;
		if ( !(s1 = s2.Left(wxStrlen(_order))).IsEmpty() && (s1.CmpNoCase(_order) == 0) )
			s1 = s2.Right(s2.Len() - wxStrlen(_order));
		else
			s1 = s2;
		s2 = lkTrimLeft(s1, wxT(" ")); // remove any leading space

		m_Sort = s2;
	}
}
wxString lkSQL3RecordSet::GetOrder() const
{
	return m_Sort;
}

void lkSQL3RecordSet::SetFltFilter(const wxString& f)
{
	m_sFltFilter = f;
}
void lkSQL3RecordSet::SetFltOrder(const wxString& o)
{
	m_sFltOrder = o;
}

void lkSQL3RecordSet::SetFilterActive(bool b)
{
	m_bFilterActive = b;
}


lkSQL3FieldData* lkSQL3RecordSet::GetField(const wxString& fldName)
{ // NULL if not found
	if ( fldName.IsEmpty() ) return NULL;
	if ( !m_DB || !m_DB->IsOpen() ) return NULL;
	if ( m_Col.IsEmpty() ) return NULL;

	return m_Col.GetField(fldName);
}

// Operations /////////////////////////////////////////////////////////////////////////////////////

//virtual
void lkSQL3RecordSet::InitColumns(const wxString& _View)
{
	m_Table = _View;
	m_Query.Empty();

	if ( !m_Col.IsEmpty() )
	{ // Initialize Query
		lkSQL3FieldSet::iterator Iter;
		wxString _qry, s;

		for ( Iter = m_Col.begin(); Iter != m_Col.end(); Iter++ )
		{
			if ( !_qry.IsEmpty() )
				_qry += wxT(", ");

			s.Printf(wxT("[%s]"), (*Iter)->GetName());
			_qry += s;
		}
			

		m_Query = _qry;
	}

	if ( m_Query.IsEmpty() )
		m_Query = wxT("*");

	Query();
}

//virtual
bool lkSQL3RecordSet::Open()
{
	return (m_DB && m_DB->IsOpen());
}

//virtual
wxString lkSQL3RecordSet::GetQuery(bool _rowid/* = false*/, bool _filter/* = true*/, bool _order/* = true*/)
{
	wxString _qry, s;
	if ( !m_Table.IsEmpty() )
		_qry.Printf(wxT("SELECT %s FROM [%s]"), (_rowid) ? m_Rowid : m_Query, m_Table);
	else
	{
		if ( !m_View.IsEmpty() )
			_qry.Printf(wxT("SELECT %s FROM (%s)"), (_rowid) ? m_Rowid : m_Query, m_View);
	}


	if ( _filter && !m_Filter.IsEmpty() )
	{
		s.Printf(wxT(" WHERE %s"), m_Filter);
		_qry += s;
	}

	if ( _order && !m_Sort.IsEmpty() )
	{
		s.Printf(wxT(" ORDER BY %s"), m_Sort);
		_qry += s;
	}

	return _qry;
}

void lkSQL3RecordSet::Query()
{
	ResetFind();

	lkSQL3Statement* stmt = NULL;
	wxString _qry;
	wxUint64* pIDs;

	try
	{
		_qry.Printf(wxT("SELECT COUNT([ID]) FROM (%s);"), GetQuery(true, true, false));

		m_RowCount = m_DB->ExecuteSQLAndGet(_qry); // .GetInt();

		_qry.Printf(wxT("%s;"), GetQuery(false, false, false));
		stmt = new lkSQL3Statement(*m_DB, _qry);
		m_ColCount = stmt->GetColCount();
		delete stmt; stmt = NULL;

		if ( m_pRowIDs )
		{
			delete[] m_pRowIDs;
			m_pRowIDs = NULL;
		}
		m_pRowIDsSize = 0;
		m_CurRow = 0;

		_qry.Printf(wxT("%s;"), GetQuery(true));
		stmt = new lkSQL3Statement(*m_DB, _qry);

		for ( wxUint64 id = 0; id < m_RowCount; id++ )
		{
			if ( id == (wxUint64)m_pRowIDsSize )
			{
				pIDs = new wxUint64[(m_pRowIDsSize + m_RowIdGrow)];
				memset(pIDs, 0, sizeof(wxUint64) * (m_pRowIDsSize + m_RowIdGrow));
				if ( m_pRowIDs )
				{
					memcpy(pIDs, m_pRowIDs, sizeof(wxUint64) * m_pRowIDsSize);
					delete[] m_pRowIDs;
				}
				m_pRowIDs = pIDs;
				m_pRowIDsSize += m_RowIdGrow;
			}
			stmt->Step();
			m_pRowIDs[id] = stmt->GetField(0); // .GetInt();
		}

		delete stmt; stmt = NULL;

		if ( m_RowCount > 0 )
			MoveFirst();
	}
	catch ( const lkSQL3Exception&/* e*/ )
	{
		//		((CSQL3Exception*)&e)->ReportError();
		if ( stmt ) delete stmt;
		throw;
	}
}

void lkSQL3RecordSet::Requery(wxUint64 repos)
{
	ResetFind();

	lkSQL3Statement* stmt = NULL;
	wxString _qry;
	wxUint64* pIDs;
	wxUint64 actCurRow = (repos > 0) ? repos : GetActualCurRow();
	wxUint64 newCurRow = 0;

	try
	{
		_qry.Printf(wxT("SELECT COUNT([ID]) FROM (%s);"), GetQuery(true, true, false));

		m_RowCount = m_DB->ExecuteSQLAndGet(_qry); // .GetInt();

		_qry.Printf(wxT("%s;"), GetQuery(false, false, false));
		stmt = new lkSQL3Statement(*m_DB, _qry);
		m_ColCount = stmt->GetColCount();
		delete stmt; stmt = NULL;

		if ( m_pRowIDs )
		{
			delete[] m_pRowIDs;
			m_pRowIDs = NULL;
		}
		m_pRowIDsSize = 0;
		m_CurRow = 0;

		_qry.Printf(_T("%s;"), GetQuery(true));
		stmt = new lkSQL3Statement(*m_DB, _qry);

		for ( wxUint64 id = 0; id < m_RowCount; id++ )
		{
			if ( id == m_pRowIDsSize )
			{
				pIDs = new wxUint64[(m_pRowIDsSize + m_RowIdGrow)];
				memset(pIDs, 0, sizeof(wxUint64) * (m_pRowIDsSize + m_RowIdGrow));
				if ( m_pRowIDs )
				{
					memcpy(pIDs, m_pRowIDs, sizeof(wxUint64) * m_pRowIDsSize);
					delete[] m_pRowIDs;
				}
				m_pRowIDs = pIDs;
				m_pRowIDsSize += m_RowIdGrow;
			}
			stmt->Step();
			m_pRowIDs[id] = stmt->GetField(0); // .GetInt();

			if ( m_pRowIDs[id] == actCurRow )
				newCurRow = id + 1; // 1-based, while id is 0-based
		}

		delete stmt; stmt = NULL;

		if ( m_RowCount > 0 )
		{
			if ( newCurRow > 0 )
				Move(newCurRow);
			else
				MoveFirst();
		}
	}
	catch ( const lkSQL3Exception&/* e*/ )
	{
		//		((CSQL3Exception*)&e)->ReportError();
		if ( stmt ) delete stmt;
		throw;
	}
}

bool lkSQL3RecordSet::LoadData() // reads current row into m_Col
{
	if ( !m_DB || !m_DB->IsOpen() ) return false;
	if ( (m_CurRow <= 0) || (m_CurRow > m_RowCount) ) return false;

	// Requery and fill the internal data
	wxString _qry;
	_qry.Printf(wxT("%s WHERE ([ROWID] = %I64u);"), GetQuery(false, false, false), m_pRowIDs[m_CurRow - 1]);

	lkSQL3Statement* stmt = NULL;
	int _i;
	bool _null;
	try
	{
		stmt = new lkSQL3Statement(*m_DB, _qry);
		if ( m_ColCount == 0 )
			m_ColCount = stmt->GetColCount();
		stmt->Step();

		// load columns
		for ( lkSQL3FieldSet::iterator Iter = m_Col.begin(); Iter != m_Col.end(); Iter++ )
		{
			_i = (*Iter)->GetIndex();

			if ( (_i >= 0) && (_i < m_ColCount) )
			{
				_null = stmt->GetField(_i).IsNull();
				(*Iter)->SetNull(_null);
				(*Iter)->SetDirty(false);

				if ( (*Iter)->IsKindOf(wxCLASSINFO(lkSQL3FieldData_Int)) )
					(dynamic_cast<lkSQL3FieldData_Int*>(*Iter))->SetValue((_null) ? (dynamic_cast<lkSQL3FieldData_Int*>(*Iter))->Geti64Default() : stmt->GetField(_i)); // .GetI64()
				else if ( (*Iter)->IsKindOf(wxCLASSINFO(lkSQL3FieldData_Real)) )
					(dynamic_cast<lkSQL3FieldData_Real*>(*Iter))->SetValue((_null) ? (dynamic_cast<lkSQL3FieldData_Real*>(*Iter))->GetdDefault() : stmt->GetField(_i)); // .GetReal()
				else if ( (*Iter)->IsKindOf(wxCLASSINFO(lkSQL3FieldData_Text)) )
					(dynamic_cast<lkSQL3FieldData_Text*>(*Iter))->SetValue(stmt->GetField(_i).GetText((dynamic_cast<lkSQL3FieldData_Text*>(*Iter))->GetsDefault()));
				else if ( (*Iter)->IsKindOf(wxCLASSINFO(lkSQL3FieldData_Blob)) )
				{
					if ( _null )
						(dynamic_cast<lkSQL3FieldData_Blob*>(*Iter))->SetValue(NULL, 0);
					else
						(dynamic_cast<lkSQL3FieldData_Blob*>(*Iter))->SetValue(stmt->GetField(_i).GetBlob(), stmt->GetField(_i).GetSize());
				}
			}
		}
		delete stmt; stmt = NULL;
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		if ( stmt ) delete stmt;
		throw;
		return false;
	}
	return true;
}

//virtual
bool lkSQL3RecordSet::CanAdd()
{
	return false;
}
//virtual
bool lkSQL3RecordSet::AddNew()
{
	wxASSERT(0);
	return false;
}
//virtual
void lkSQL3RecordSet::CancelAddNew()
{
	wxASSERT(0);
}
//virtual
bool lkSQL3RecordSet::IsAdding() const
{
	return false;
}
//virtual
bool lkSQL3RecordSet::CanUpdate()
{
	return false;
}
// virtual
bool lkSQL3RecordSet::Update()
{
	wxASSERT(0);
	return false;
}
//virtual
void lkSQL3RecordSet::CancelUpdate()
{
	wxASSERT(0);
}
// either Adds a new row, or udates current row
//virtual
bool lkSQL3RecordSet::CanDelete()
{
	return false;
}
//virtual
bool lkSQL3RecordSet::Delete()
{
	wxASSERT(0);
	return false;
}
//virtual
bool lkSQL3RecordSet::DeleteRow(wxUint64 nRow)
{
	wxASSERT(0);
	return false;
}

bool lkSQL3RecordSet::IsBOF()
{
	return ((m_RowCount == 0) || (m_CurRow < 1));
}
bool lkSQL3RecordSet::IsEOF()
{
	return ((m_RowCount == 0) || (m_CurRow > m_RowCount));
}
bool lkSQL3RecordSet::IsEmpty()
{
	return (m_RowCount == 0);
}

bool lkSQL3RecordSet::Move(wxUint64 idx) // move to a specific index.  One-based, because of I was making it like CRecordSet
{
	if ( !m_DB || !m_DB->IsOpen() ) return false;
	if ( m_RowCount == 0 ) return true; // nothing to do, query is empty
//	if ( idx < 0 ) return false;

	m_CurRow = (idx > m_RowCount) ? m_RowCount + 1 : (idx == 0) ? m_CurRow : idx;
	if ( m_CurRow <= m_RowCount )
	{
		wxASSERT(m_pRowIDs[m_CurRow - 1] != 0);
		wxASSERT(!m_Col.IsEmpty());

		return LoadData();
	}
	else
		return false; // EOF
}

bool lkSQL3RecordSet::MoveFirst()
{
	return Move(1);
}
bool lkSQL3RecordSet::MoveLast()
{
	return Move(m_RowCount);
}
bool lkSQL3RecordSet::MoveNext()
{
	return Move(m_CurRow + 1);
}
bool lkSQL3RecordSet::MovePrev()
{
	return Move(m_CurRow - 1);
}

bool lkSQL3RecordSet::CanMoveFirst()
{
	return (!IsBOF() && (m_CurRow > 1));
}
bool lkSQL3RecordSet::CanMoveLast()
{
	return (!IsEOF() && (m_CurRow < m_RowCount));
}
bool lkSQL3RecordSet::IsOnFirstRecord()
{
	return (m_CurRow <= 1); // when Empty, m_CurRow=0 !
}
bool lkSQL3RecordSet::IsOnLastRecord()
{
	return (m_CurRow >= m_RowCount);
}

wxUint64 lkSQL3RecordSet::GetMaxID() // returns MAX of ROWID
{
	if ( !m_DB || !m_DB->IsOpen() || IsEmpty() ) return 0;

	wxUint64 ret = 0;
	wxString _qry;
	_qry.Printf(wxT("SELECT MAX([ID]) AS MAXID FROM (%s);"), GetQuery(true));
	try
	{
		ret = m_DB->ExecuteSQLAndGet(_qry);
	}
	catch ( const lkSQL3Exception&/* e*/ )
	{
		//		((CSQL3Exception*)&e)->ReportError();
		throw;
	}

	return ret;
}

bool lkSQL3RecordSet::IsDirty(lkSQL3FieldData* pFLD) // checks if given Field is marked Dirty, if NULL checks all fields until 1 is marked as Dirty
{
	if ( m_Col.IsEmpty() ) return false;

	if ( pFLD )
		return pFLD->IsDirty();
	// else...
	for ( lkSQL3FieldSet::iterator Iter = m_Col.begin(); Iter != m_Col.end(); Iter++ )
		if ( (*Iter)->IsDirty() )
			return true;

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void lkSQL3RecordSet::ResetFind()
{ // resets settings and kils tempory findDatabase
	m_GotFindNext = m_GotFindPrev = false;
	m_FindRowid = 0;
	if ( m_FindDB ) delete m_FindDB;
	m_FindDB = NULL;
}

wxUint64 lkSQL3RecordSet::FindInit(wxClassInfo* rc, const wxString& search_on, bool bForward)
{
	ResetFind();

	if ( IsEmpty() ) return 0;
	wxASSERT(!search_on.IsEmpty());

	wxUint64 ret = 0;
	wxString s, _qry;
	_qry = GetQuery(true, 0, 0);

	if ( !m_Filter.IsEmpty() )
		s.Printf(wxT(" WHERE ((%s) AND (%s))"), m_Filter, search_on);
	else
		s.Printf(wxT(" WHERE (%s)"), search_on);
	_qry += s;
/*
	if ( !m_Sort.IsEmpty() )
		s.Printf(wxT(" ORDER BY (%s)"), m_Sort);
	else
		s = wxT(" ORDER BY [ROWID]");
	_qry += s;
*/
	s.Printf(wxT("SELECT COUNT([ID]) FROM (%s);"), _qry);

	lkSQL3RecordSet* rsOrig = NULL;
	lkSQL3FindTable* rsFind = NULL;
	try
	{
		if ( ((int)m_DB->ExecuteSQLAndGet(s)) > 0 )
		{
			m_FindDB = new lkSQL3Database();
			m_FindDB->Open(wxT(":memory:"), lkSQL3Database::enOpenFlags::ofCreate | lkSQL3Database::enOpenFlags::ofMemory);
			lkSQL3FindTable::CreateTable(m_FindDB);

			rsFind = new lkSQL3FindTable(m_FindDB);
			rsFind->Open();

			rsOrig = CreateDynamic(rc, m_DB);
			if ( !m_Filter.IsEmpty() )
				s.Printf(wxT("(%s) AND (%s)"), m_Filter, search_on);
			else
				s.Printf(wxT("(%s)"), search_on);
			rsOrig->SetFilter(s);
			if ( !m_Sort.IsEmpty() )
				s.Printf(wxT("%s"), m_Sort);
			else
				s = wxT("[ROWID]");
			rsOrig->SetOrder(s);
			rsOrig->Open();
			rsOrig->MoveFirst();
			while ( !rsOrig->IsEOF() )
			{
				rsFind->AddNew();
				rsFind->SetOrigID(rsOrig->GetActualCurRow());
				rsFind->Update();
				rsOrig->MoveNext();
			}
			delete rsOrig;
			rsOrig = NULL;

			ret = (bForward) ? rsFind->FindFirst(m_FindRowid) : rsFind->FindLast(m_FindRowid);
		}
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		if ( rsOrig ) delete rsOrig;
		if ( rsFind ) delete rsFind;
		if ( m_FindDB ) delete m_FindDB; m_FindDB = NULL;
		return 0;
	}

	if ( rsOrig ) delete rsOrig;
	if ( rsFind ) delete rsFind;

	if ( ret > 0 ) // find index of this row
	{
		SetFindBools();

		if ( bForward )
		{
			for ( wxUint64 i = 0; i < m_RowCount; i++ )
				if ( m_pRowIDs[i] == ret )
					return ++i;
		}
		else
		{
			for ( wxUint64 i = (m_RowCount - 1); i >= 0; i-- )
				if ( m_pRowIDs[i] == ret )
					return ++i;
		}
	}

	return 0;
}
wxUint64 lkSQL3RecordSet::FindNext()
{
	m_GotFindNext = m_GotFindPrev = false;
	if ( IsEmpty() ) return 0;
	wxASSERT(m_FindDB && m_FindDB->IsOpen());
	if ( m_FindRowid <= 0 ) return 0;

	wxUint64 ret = 0;
	try
	{
		lkSQL3FindTable rsFind(m_FindDB);
		rsFind.Open();
		ret = rsFind.FindNext(m_FindRowid);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		ret = 0;
		m_GotFindNext = m_GotFindPrev = false;
	}

	if ( ret > 0 ) // find index of this row
	{
		SetFindBools();

		for ( int i = 0; i < m_RowCount; i++ )
			if ( m_pRowIDs[i] == ret )
				return ++i;
	}

	return 0;
}

wxUint64 lkSQL3RecordSet::FindPrev()
{
	m_GotFindNext = m_GotFindPrev = false;
	if ( IsEmpty() ) return 0;
	wxASSERT(m_FindDB && m_FindDB->IsOpen());
	if ( m_FindRowid <= 0 ) return 0;

	wxUint64 ret = 0;
	try
	{
		lkSQL3FindTable rsFind(m_FindDB);
		rsFind.Open();
		ret = rsFind.FindPrev(m_FindRowid);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		ret = 0;
		m_GotFindNext = m_GotFindPrev = false;
	}

	if ( ret > 0 ) // find index of this row
	{
		SetFindBools();

		for ( int i = (m_RowCount - 1); i >= 0; i-- )
			if ( m_pRowIDs[i] == ret )
				return ++i;
	}

	return 0; // not found ?
}

void lkSQL3RecordSet::SetFindBools()
{ // used internally
	m_GotFindNext = m_GotFindPrev = false;
	if ( IsEmpty() ) return;
	if ( m_FindRowid <= 0 ) return;

	lkSQL3FindTable rsFind(m_FindDB);
	rsFind.Open();

	try
	{
		m_GotFindNext = rsFind.HasNext(m_FindRowid);
		m_GotFindPrev = rsFind.HasPrev(m_FindRowid);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		m_GotFindNext = m_GotFindPrev = false;
	}
}

//virtual
bool lkSQL3RecordSet::TestFind(const wxString& search_on, bool forward)
{
	if ( IsEmpty() ) return false;
	if ( search_on.IsEmpty() ) return false;

	bool bOK = false;

	wxString s, _qry;

	if ( !m_Table.IsEmpty() )
		_qry.Printf(wxT("SELECT %s, %s FROM [%s]"), m_Rowid, m_Query, m_Table);
	else
	{
		if ( !m_View.IsEmpty() )
			_qry.Printf(wxT("SELECT %s, %s FROM (%s)"), m_Rowid, m_Query, m_View);
	}

	if ( !m_Filter.IsEmpty() )
		s.Printf(wxT(" WHERE ((%s) AND (%s))"), m_Filter, search_on);
	else
		s.Printf(wxT(" WHERE (%s)"), search_on);

	if ( !_qry.IsEmpty() )
		_qry += s;

	/* -- Testing, no need for Sort
	if ( !m_Sort.IsEmpty() )
	{
		wxString _sort = m_Sort;
		_sort.Replace(wxT("ROWID"), wxT("ID"), true);
		sOrder.Printf(wxT(" ORDER BY %s%s"), _sort, (!forward) ? _T(" DESC") : _T(""));
	}
	else
		sOrder.Printf(wxT(" ORDER BY [ID]%s"), (!forward) ? _T(" DESC") : _T(""));
	*/
	if ( !_qry.IsEmpty() )
	{
		s = _qry;
		_qry.Printf(wxT("SELECT * FROM (%s)"), s);

		try
		{
			s.Printf(wxT("SELECT COUNT([ID]) FROM (%s);"), _qry);
			bOK = ((int)(m_DB->ExecuteSQLAndGet(s)) > 0);
		}
		catch ( const lkSQL3Exception& e )
		{
			((lkSQL3Exception*)&e)->ReportError();
			bOK = false;
		}
	}
	else
		bOK = false;

	return bOK;
}

//virtual
void lkSQL3RecordSet::CreateFindFilter(wxString& Filter, const wxString& Field, const wxString& Search, FIND_DIRECTION findDirection)
{
	Filter = lkSQL3RecordSet::CreateFindFilter(Field, Search, findDirection);
}

//static
wxString lkSQL3RecordSet::CreateFindFilter(const wxString& Field, const wxString& Search, FIND_DIRECTION findDirection, const wxString& Table)
{
	wxString Filter, s, t = wxT("");
	if ( Field.IsEmpty() || Search.IsEmpty() )
		return Filter;

	if ( !Table.IsEmpty() )
		t.Printf(wxT("[%s]."), Table);

	switch ( findDirection )
	{
		case FIND_DIRECTION::fndStart:
			s = wxT("(%s[%s] LIKE '%s%%')");
			break;
		case FIND_DIRECTION::fndAnywhere:
			s = wxT("(%s[%s] LIKE '%%%s%%')");
			break;
		case FIND_DIRECTION::fndEnd:
			s = wxT("(%s[%s] LIKE '%%%s')");
			break;
		case FIND_DIRECTION::fndEntire:
		default: // exact
			s = wxT("(LOWER(%s[%s]) = LOWER('%s'))");
			break;
	}

	wxString corrSrch = Search;
	corrSrch.Replace(wxT("'"), wxT("''"), true);
	Filter.Printf(s, t, Field, corrSrch);

	return Filter;
}

