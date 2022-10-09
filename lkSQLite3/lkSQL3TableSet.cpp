/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // !_CRT_SECURE_NO_WARNINGS
#include "lkSQL3TableSet.h"
#include "lkSQL3Field.h"
#include "lkSQL3Exception.h"

#include <wx/debug.h>
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(lkInitFieldListBase);

// //////////////////////////////////////////////////////////////////////////////////////////////

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper for 'Open'

wxIMPLEMENT_DYNAMIC_CLASS(lkInitField, wxObject);

lkInitField::lkInitField() : wxObject(), m_szField()
{
	m_rcField = NULL;
	m_ppField = NULL;
}
lkInitField::lkInitField(const wxString& szFld, const wxClassInfo* rcFld, lkSQL3FieldData** ppFld) : wxObject(), m_szField(szFld)
{
	m_rcField = rcFld;
	m_ppField = ppFld;
}

// ///////////////////////////////////////

lkInitTable::lkInitTable() : lkInitFieldListBase()
{
	DeleteContents(true);
	m_pTblDef = NULL;
}
lkInitTable::lkInitTable(sqlTblDef_p pTbl) : lkInitFieldListBase()
{
	DeleteContents(true);
	m_pTblDef = pTbl;
}
lkInitTable::~lkInitTable()
{
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_DYNAMIC_CLASS(lkSQL3TableSet, lkSQL3RecordSet);

lkSQL3TableSet::lkSQL3TableSet() : lkSQL3RecordSet()
{
	m_bAdd = false;
}
lkSQL3TableSet::lkSQL3TableSet(lkSQL3Database* pDB) : lkSQL3RecordSet(pDB)
{
	m_bAdd = false;
}
lkSQL3TableSet::~lkSQL3TableSet()
{
}

//static
bool lkSQL3TableSet::CreateTable(lkSQL3Database* pDB, const lkSQL3TableDef& def, bool _Temp/*=false*/)
{
	if ( !pDB || !pDB->IsOpen() || !pDB->CanUpdate() ) return false;
	bool bRet = false;
	try
	{
		bRet = ((lkSQL3TableDef*)&def)->CreateTable(*pDB, _Temp);
	}
	catch ( const lkSQL3Exception& )
	{
		bRet = false;
		//		throw;
	}

	return bRet;
}

//static
bool lkSQL3TableSet::CreateTable(lkSQL3Database* pDB, const sqlTblDef_t& _def, bool _Temp/* = false*/)
{
	lkSQL3TableDef _tDef(_def);

	return CreateTable(pDB, _tDef, _Temp);
}

void lkSQL3TableSet::InitColumns(const wxString& _tblName)
{
	wxASSERT(m_DB && m_DB->IsOpen() && !_tblName.IsEmpty());
	wxASSERT(!m_Col.IsEmpty());
	if ( !m_DB->TableExists(_tblName) )
		return; // nothing to do

	lkSQL3VerifyList* pList = lkSQL3TableSet::GetColumns(m_DB, _tblName);
	if ( pList && !pList->IsEmpty() )
	{
		int _idx = 0;
		lkSQL3FieldData* _fldData;
		lkSQL3VerifyList::iterator Iter;
		for ( Iter = pList->begin(); Iter != pList->end(); Iter++ )
			if ( (_fldData = m_Col.GetField((*Iter)->m_sName)) != NULL )
			{
				_fldData->SetIndex(_idx++);
//				_fldData->SetIndex((*Iter)->m_nIdx);
				_fldData->SetSqlType((*Iter)->m_nType);
				_fldData->SetNotNull(!(*Iter)->m_bCanbeNull);
				_fldData->SetDefault((*Iter)->m_bUsingDefault, (*Iter)->m_sDefault);
			}
	}
	if ( pList ) delete pList;

	lkSQL3RecordSet::InitColumns(_tblName);
}

bool lkSQL3TableSet::InitColumns(lkSQL3TblInit& init, const sqlTblDef_t& def, const wxString& tName)
{
	wxASSERT(!init.IsEmpty());
	wxASSERT(!init.m_TableName.IsEmpty());
	lkSQL3TableDef _tDef(def);
	wxASSERT(!_tDef.GetName().IsEmpty() || !tName.IsEmpty());
	wxASSERT_MSG(m_Col.IsEmpty(), wxT("lkSQL3TableSet::InitColumns should be called only once")); // do this only once

	wxString _tblName = (!_tDef.GetName().IsEmpty()) ? _tDef.GetName() : tName;
	const lkSQL3FieldDefList* pFldsDef = _tDef.GetFields();
	wxASSERT(!pFldsDef->IsEmpty());
	if ( pFldsDef->IsEmpty() )
		return false; // ERR


	bool bRet = true;

	lkSQL3FieldDefList::const_iterator Iter;
	for ( Iter = pFldsDef->begin(); Iter != pFldsDef->end(); Iter++ )
		m_Col.AddField((*Iter)->GetName(), (*Iter)->GetType());

//	InitColumns(_tblName);

	lkSQL3FieldData* fld;
	lkSQL3TblFieldInit* finit;
	for ( Iter = pFldsDef->begin(); Iter != pFldsDef->end(); Iter++ )
	{
		if ( bRet = (((fld = m_Col.GetField((*Iter)->GetName())) != NULL) &&
			((finit = init.FindByName((*Iter)->GetName())) != NULL)) )
		{
			fld->SetDDVmin((*Iter)->GetDDVmin());
			fld->SetDDVmax((*Iter)->GetDDVmax());

			if ( bRet = finit->m_pClass->IsKindOf(fld->GetClassInfo()) ) // fld->IsKindOf(finit->m_pClass)
				*(finit->m_ppField) = fld;
		}
		if ( !bRet )
			break;
	}

	InitColumns(_tblName);

	return bRet;
}

//virtual
bool lkSQL3TableSet::Open(lkInitTable* pTblInit) // nothrow
{
	if ( IsOpen() ) return true;
	if ( pTblInit == NULL ) return false;
	if ( (pTblInit->m_pTblDef == NULL) || pTblInit->IsEmpty() ) return false;

	bool bRet = lkSQL3TableSet::Open();
	if ( bRet )
		bRet = m_DB->TableExists(pTblInit->m_pTblDef->sName);

	if ( bRet )
	{
		lkSQL3TblInit init(pTblInit->m_pTblDef->sName);

		lkInitTable::iterator Iter;
		for ( int i = 0; pTblInit->m_pTblDef->pFields[i].sName && pTblInit->m_pTblDef->pFields[i].sName[0]; i++ )
			for ( Iter = pTblInit->begin(); Iter != pTblInit->end(); Iter++ )
			{
				if ( (*Iter)->m_szField.CmpNoCase(pTblInit->m_pTblDef->pFields[i].sName) == 0 )
				{
					init.AddFldInit(pTblInit->m_pTblDef->pFields[i].sName, (*Iter)->m_rcField, (*Iter)->m_ppField);
					break;
				}
			}

		if ( bRet = InitColumns(init, *(pTblInit->m_pTblDef)) )
			for ( Iter = pTblInit->begin(); Iter != pTblInit->end(); Iter++ )
			{
				if ( bRet )
					bRet = (*((*Iter)->m_ppField) != NULL);
				else
					break;
			}
	}

	return bRet;
}

//virtual
bool lkSQL3TableSet::Open()
{
	return lkSQL3RecordSet::Open();
}

//virtual
bool lkSQL3TableSet::AddData()
{
	wxASSERT(!m_Col.IsEmpty());
	if ( !m_bAdd ) return false; // nothing to do

	lkSQL3FieldSet::iterator Iter;
	lkSQL3Statement* stmt = NULL;
	wxString _qry, _val, _param, s;
	int i;
	wxUint64 _row = 0;

	i = 0;
	for ( Iter = m_Col.begin(); Iter != m_Col.end(); Iter++ )
		if ( (*Iter)->IsDirty() )
		{
			if ( !_val.IsEmpty() )
				_val += wxT(", ");
			s.Printf(wxT("[%s]"), (*Iter)->GetName());
			_val += s;

			if ( !_param.IsEmpty() )
				_param += wxT(", ");
			s.Printf(wxT(":p%d"), ++i);
			_param += s;
		}

	if ( _val.IsEmpty() )
		return false; // nothing to do ..

	_qry.Printf(wxT("INSERT INTO [%s] (%s) VALUES (%s);"), m_Table, _val, _param);

	try
	{
		stmt = new lkSQL3Statement(*m_DB, _qry);
		// bind columns
		i = 0;
		for ( Iter = m_Col.begin(); Iter != m_Col.end(); Iter++ )
			if ( (*Iter)->IsDirty() )
			{
					s.Printf(wxT(":p%d"), ++i);
					if ( (*Iter)->IsNull() )
						stmt->Bind(s);
					else if ( (*Iter)->IsKindOf(wxCLASSINFO(lkSQL3FieldData_Int)) )
						stmt->Bind(s, (dynamic_cast<lkSQL3FieldData_Int*>(*Iter))->GetValue());
					else if ( (*Iter)->IsKindOf(wxCLASSINFO(lkSQL3FieldData_Real)) )
						stmt->Bind(s, (dynamic_cast<lkSQL3FieldData_Real*>(*Iter))->GetValue());
					else if ( (*Iter)->IsKindOf(wxCLASSINFO(lkSQL3FieldData_Text)) )
						stmt->Bind(s, (dynamic_cast<lkSQL3FieldData_Text*>(*Iter))->GetValue());
					else if ( (*Iter)->IsKindOf(wxCLASSINFO(lkSQL3FieldData_Blob)) )
						stmt->Bind(s, (dynamic_cast<lkSQL3FieldData_Blob*>(*Iter))->GetValue(), (dynamic_cast<lkSQL3FieldData_Blob*>(*Iter))->GetSize());
					else // undefind
					{
						wxASSERT(0);
						stmt->Bind(s); // setting it to Null
					}
				}

		stmt->Step();
		_row = stmt->GetLastInsertRowID();
		delete stmt; stmt = NULL;
	}
	catch ( const lkSQL3Exception&/* e*/ )
	{
		//	((lkSQL3Exception*)&e)->ReportError();
		if ( stmt ) delete stmt;
		i = 0;
		throw;
		return false;
	}
	// we came here, so it was successfull
	if ( _row > 0 )
	{
		Requery(_row);
	}
	m_bAdd = false;

	return true;
}

bool lkSQL3TableSet::UpdateData()
{
	wxASSERT(!m_Col.IsEmpty());
	if ( m_bAdd ) return false; // nothing to do
	if ( (!m_pRowIDs) || (m_CurRow < 1) || (m_CurRow > m_RowCount) ) return false;

	if ( CanUpdate() )
	{
		lkSQL3FieldSet::iterator Iter;
		lkSQL3Statement* stmt = NULL;
		wxString _qry, _val, _param, s;
		int i = 0;

		for ( Iter = m_Col.begin(); Iter != m_Col.end(); Iter++ )
			if ( (*Iter)->IsDirty() )
			{
				if ( !_val.IsEmpty() )
					_val += wxT(", ");
				s.Printf(wxT("[%s]=:p%d"), (*Iter)->GetName(), ++i);
				_val += s;
			}

		if ( _val.IsEmpty() )
			return false; // nothing to do ..

		_qry.Printf(wxT("UPDATE [%s] SET %s WHERE ([ROWID] = %llu);"), m_Table, _val, m_pRowIDs[m_CurRow - 1]);

		try
		{
			stmt = new lkSQL3Statement(*m_DB, _qry);
			// bind columns
			i = 0;
			for ( Iter = m_Col.begin(); Iter != m_Col.end(); Iter++ )
				if ( (*Iter)->IsDirty() )
				{
					s.Printf(wxT(":p%d"), ++i);
					if ( (*Iter)->IsNull() )
						stmt->Bind(s);
					else if ( (*Iter)->IsKindOf(wxCLASSINFO(lkSQL3FieldData_Int)) )
						stmt->Bind(s, (dynamic_cast<lkSQL3FieldData_Int*>(*Iter))->GetValue());
					else if ( (*Iter)->IsKindOf(wxCLASSINFO(lkSQL3FieldData_Real)) )
						stmt->Bind(s, (dynamic_cast<lkSQL3FieldData_Real*>(*Iter))->GetValue());
					else if ( (*Iter)->IsKindOf(wxCLASSINFO(lkSQL3FieldData_Text)) )
						stmt->Bind(s, (dynamic_cast<lkSQL3FieldData_Text*>(*Iter))->GetValue());
					else if ( (*Iter)->IsKindOf(wxCLASSINFO(lkSQL3FieldData_Blob)) )
						stmt->Bind(s, (dynamic_cast<lkSQL3FieldData_Blob*>(*Iter))->GetValue(), (dynamic_cast<lkSQL3FieldData_Blob*>(*Iter))->GetSize());
					else // undefind
					{
						wxASSERT(0);
						stmt->Bind(s); // setting it to Null
					}
				}

			stmt->Step();
			delete stmt; stmt = NULL;
		}
		catch ( const lkSQL3Exception&/* e*/ )
		{
			//	((lkSQL3Exception*)&e)->ReportError();
			if ( stmt ) delete stmt;
			throw;
			return false;
		}
	}
	LoadData();
	ResetFind();
	return true;
}
bool lkSQL3TableSet::Update() // either Adds a new row, or udates current row
{
	if ( !m_DB || !m_DB->IsOpen() || !m_DB->CanUpdate() ) return false;

	bool bRet = false;
	if ( m_bAdd )
		bRet = AddData();
	else
		bRet = UpdateData();

	return bRet;
}
void lkSQL3TableSet::CancelUpdate()
{
	LoadData();
}
void lkSQL3TableSet::AddRowID(wxUint64 _newid)
{
	wxASSERT(_newid > 0);
	if ( (m_RowCount == m_pRowIDsSize) || (m_RowCount == 0) )
	{
		wxUint64* pIDs = new wxUint64[(m_pRowIDsSize + m_RowIdGrow)];
		memset(pIDs, 0, sizeof(wxUint64) * (m_pRowIDsSize + m_RowIdGrow));
		if ( m_pRowIDs )
		{
			memcpy(pIDs, m_pRowIDs, sizeof(wxUint64) * m_pRowIDsSize);
			delete[] m_pRowIDs;
		}
		m_pRowIDs = pIDs;
		m_pRowIDsSize += m_RowIdGrow;
	}

	m_CurRow = ++m_RowCount;
	m_pRowIDs[m_CurRow - 1] = _newid;

	LoadData();
}

//virtual
bool lkSQL3TableSet::CanAdd()
{
	return (!m_bFilterActive && CanUpdate() && !m_bAdd);
}
//virtual
bool lkSQL3TableSet::AddNew()
{
	if ( CanAdd() )
	{
		wxASSERT(!m_Col.IsEmpty());
		m_Col.Reset();
		m_bAdd = true;
		return true;
	}
	return false;
}
//virtual
bool lkSQL3TableSet::IsAdding() const
{
	return m_bAdd;
}

void lkSQL3TableSet::CancelAddNew() // resets adding and updates current values in m_Col to current row
{
	if ( !m_bAdd ) return; // nothing to do
	m_bAdd = false;
	LoadData();
}

//virtual
bool lkSQL3TableSet::CanUpdate()
{
	return (m_DB && m_DB->IsOpen() && m_DB->CanUpdate());
}

//virtual
bool lkSQL3TableSet::CanDelete()
{
	return (!m_bFilterActive && CanUpdate() && !IsBOF() && !IsEOF() && !m_bAdd && m_pRowIDs && (m_CurRow >= 1) && (m_CurRow <= m_RowCount) && (m_RowCount > 0));
}

bool lkSQL3TableSet::DeleteRow(wxUint64 nRow) // deletes given row, does not do a requery
{
	if ( !CanDelete() ) return false;

	wxString _qry;

	try
	{
		_qry.Printf(_T("DELETE FROM [%s] WHERE (ROWID = %I64u);"), m_Table, nRow);
		m_DB->ExecuteSQL(_qry);
	}
	catch ( const lkSQL3Exception& )
	{
		throw;
		return false;
	}
	ResetFind();
	return true;
}

bool lkSQL3TableSet::Delete() // deletes current row and requery, moves current row to first row after deleted row and if atEOF then 1 row before current row
{
	if ( !CanDelete() ) return false;
	if ( m_pRowIDs == NULL ) return false;

	lkSQL3Statement* stmt = NULL;
	wxString _qry;
	wxUint64* pIDs;

	wxUint64 newPos, curPos;
	curPos = (m_CurRow >= m_RowCount) ? m_RowCount : m_CurRow;

	if ( curPos == m_RowCount )
		newPos = curPos - 1;
	else
		newPos = curPos;

	try
	{
		DeleteRow(m_pRowIDs[curPos - 1]);

		// Requery
		_qry.Printf(wxT("SELECT COUNT([ID]) FROM (%s);"), GetQuery(true));

		m_RowCount = m_DB->ExecuteSQLAndGet(_qry);

		_qry.Printf(wxT("%s;"), GetQuery());
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

		for ( int id = 0; id < m_RowCount; id++ )
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
			m_pRowIDs[id] = stmt->GetField(0);
		}

		delete stmt; stmt = NULL;

		if ( m_RowCount > 0 )
		{
			m_CurRow = newPos;
			wxASSERT(m_CurRow > 0);
			LoadData();
		}
		else
			m_Col.Reset();
	}
	catch ( const lkSQL3Exception&/* e*/ )
	{
		//		((CSQL3Exception*)&e)->ReportError();
		if ( stmt ) delete stmt;
		throw;
		return false;
	}
	ResetFind();
	return true;
}

//static
lkSQL3VerifyList* lkSQL3TableSet::GetColumns(lkSQL3Database* pDB, const wxString& tblName)
{ // returns list of Fiels from 'PRAGMA table_info ...'
	if ( !pDB || !pDB->IsOpen() ) return NULL;
	if ( tblName.IsEmpty() ) return NULL;
	if ( !pDB->TableExists(tblName) ) return NULL;

	int cid;
	wxString name;
	wxString _type;
	bool _notnull;
	bool _useDefault;
	wxString _default;

	lkSQL3VerifyList* pList = NULL;
	lkSQL3Verify* pOb;
	lkSQL3Statement* stmt = NULL;
	wxString _qry;
	_qry.Printf(wxT("PRAGMA table_info (%s);"), tblName);
	try
	{
		stmt = new lkSQL3Statement(*pDB, _qry);
		while ( stmt->Step() )
		{
			cid = stmt->GetField(wxT("cid"));
			name = (const wxString&)stmt->GetField(wxT("name"));
			_type = (const wxString&)stmt->GetField(wxT("type"));
			_notnull = (stmt->GetField(wxT("notnull")).GetInt() != 0);
			_useDefault = !(stmt->GetField(wxT("dflt_value")).IsNull());
			//			_default = (_useDefault) ? SQLconvert_Reverse((LPCTSTR)stmt->GetField(_T("dflt_value"))) : _T("");
			_default = (_useDefault) ? (const wxString&)stmt->GetField(wxT("dflt_value")) : wxT("");

			if ( !pList )
				pList = new lkSQL3VerifyList();

			pOb = new lkSQL3Verify();
			pOb->m_nType = GetSQLiteType(_type);
			pOb->m_nIdx = cid;
			pOb->m_sName = name;
			pOb->m_bCanbeNull = !_notnull;
			pOb->m_bUsingDefault = _useDefault;
			pOb->m_sDefault = _default;
			pList->Append(pOb);
		}
		delete stmt; stmt = NULL;
	}
	catch ( const lkSQL3Exception/*& e*/ )
	{
		//		((CSQL3Exception*)&e)->ReportError();
		if ( stmt )
		{
			delete stmt;
			stmt = NULL;
		}
		if ( pList )
		{
			delete pList;
			pList = NULL;
		}
		//		throw;
	}
	if ( stmt ) delete stmt;
	return pList;
}

//static
bool lkSQL3TableSet::VerifyTable(lkSQL3Database* pDB, const sqlTblDef_t& tDef) // verifys that the table exists, and the fields are present with given type
{
	lkSQL3VerifyList* pList = lkSQL3TableSet::GetColumns(pDB, tDef.sName);
	if ( !pList ) return false;

	bool bRet = !pList->IsEmpty();
	if ( !bRet )
	{
		delete pList;
		return	false;
	}

	lkSQL3VerifyList::iterator Iter;
	// initialy set all indexes to 0
	for ( Iter = pList->begin(); Iter != pList->end(); Iter++ )
		(*Iter)->m_nIdx = 0;

	wxUint32 ctr1 = 0;
	lkSQL3Verify* pOb;
	for ( int i = 0; tDef.pFields[i].sName && tDef.pFields[i].sName[0]; i++ )
	{
		bRet = ((pOb = pList->GetField(tDef.pFields[i].sName)) != NULL);
		if ( bRet )
			bRet = ((pOb->m_sName.CmpNoCase(tDef.pFields[i].sName) == 0) &&
					(pOb->m_nType == tDef.pFields[i].fType) &&
					(pOb->m_bCanbeNull == !(tDef.pFields[i].bNotNull)) &&
					(pOb->m_bUsingDefault == tDef.pFields[i].bDefault));

		if ( bRet )
		{
			pOb->m_nIdx = 1;
			ctr1++;
		}
		else
			break;
	}

	wxUint32 ctr2 = 0;
	if ( bRet )
	{
		for ( Iter = pList->begin(); Iter != pList->end(); Iter++ )
			if ( (*Iter)->m_nIdx != 0 )
				ctr2++;

		// we could have more fields than actual required
		bRet = (ctr1 == ctr2);
	}

	delete pList;
	return bRet;
}
void lkSQL3TableSet::Throw_InvalidTable(const wxString& _tblName) // throws an exception that given table is incorrect
{
	wxString s;
	s.Printf(wxT("Invalid Table Definition.\n[%s]."), _tblName);
	SQL3throw(s);
}

//static
bool lkSQL3TableSet::IsUniqueTextValue(lkSQL3Database* pDB, const wxString& tblName, const wxString& fldName, const wxString& fldData) // true if given fldData doesn't exist in table
{
	wxASSERT(pDB && pDB->IsOpen());
	wxASSERT(!tblName.IsEmpty());
	wxASSERT(!fldName.IsEmpty());
	if ( !pDB || !pDB->IsOpen() ) return false;

	wxString qry;
	qry.Printf(wxT("SELECT COUNT(ROWID) FROM [%s] WHERE ([%s] = '%s');"), tblName, fldName, fldData);

	wxUint64 ret = 0;
	try
	{
		ret = pDB->ExecuteSQLAndGet(qry);
	}
	catch ( const lkSQL3Exception&/* e*/ )
	{
		//		((CSQL3Exception*)&e)->ReportError();
		throw;
		return false;
	}

	return (ret == 0);
}
