/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#include "lkTSqliteMaster.h"
#include "lkSQL3Statement.h"
#include "lkSQL3Field.h"
#include "lkSQL3Verify.h"
#include "lkSQL3Common.h"

#include "lkSQL3Exception.h"

// static definitions for setting the fields of new table
static sqlTblField_t t3SqliteMasterFlds[] = {
	{ t3SqliteMaster_TYPE,    sqlFieldType::sqlText, true, false, 0 },
	{ t3SqliteMaster_NAME,    sqlFieldType::sqlText, true, false, 0 },
	{ t3SqliteMaster_TBLNAME, sqlFieldType::sqlText, true, false, 0 },
	{ t3SqliteMaster_ROOT,    sqlFieldType::sqlInt,  true, false, 0 },
	{ t3SqliteMaster_SQL,     sqlFieldType::sqlText, true, false, 0 },
	{ 0 }
};

// static definitions for creating a fresh new table
static sqlTblDef_t t3SqliteMasterTbl = { t3SqliteMaster, t3SqliteMasterFlds, 0 };


wxIMPLEMENT_DYNAMIC_CLASS(lkTSqliteMaster, lkSQL3TableSet)

lkTSqliteMaster::lkTSqliteMaster() : lkSQL3TableSet()
{
	Init();
}
lkTSqliteMaster::lkTSqliteMaster(lkSQL3Database* pDB) : lkSQL3TableSet(pDB)
{
	Init();
}

void lkTSqliteMaster::Init()
{
	m_pFldType = NULL;
	m_pFldName = NULL;
	m_pFldTblName = NULL;
	m_pFldRootpage = NULL;
	m_pFldSql = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Operations

//virtual
bool lkTSqliteMaster::Open()
{
	if ( !lkSQL3TableSet::Open() ) return false;
	// table always exists -- master table of sqlite3

	for ( int i = 0; t3SqliteMasterTbl.pFields[i].sName && t3SqliteMasterTbl.pFields[i].sName[0]; i++ )
		m_Col.AddField(t3SqliteMasterTbl.pFields[i].sName, t3SqliteMasterTbl.pFields[i].fType);

	InitColumns(t3SqliteMaster);

	lkSQL3FieldData* o = m_Col.GetField(t3SqliteMaster_TYPE);
	bool bRet = (o && o->IsKindOf(wxCLASSINFO(lkSQL3FieldData_Text)));
	if ( bRet )
	{
		m_pFldType = (lkSQL3FieldData_Text*)o;
		o = m_Col.GetField(t3SqliteMaster_NAME);
		bRet = (o && o->IsKindOf(wxCLASSINFO(lkSQL3FieldData_Text)));
	}
	if ( bRet )
	{
		m_pFldName = (lkSQL3FieldData_Text*)o;
		o = m_Col.GetField(t3SqliteMaster_TBLNAME);
		bRet = (o && o->IsKindOf(wxCLASSINFO(lkSQL3FieldData_Text)));
	}
	if ( bRet )
	{
		m_pFldTblName = (lkSQL3FieldData_Text*)o;
		o = m_Col.GetField(t3SqliteMaster_ROOT);
		bRet = (o && o->IsKindOf(wxCLASSINFO(lkSQL3FieldData_Int)));
	}
	if ( bRet )
	{
		m_pFldRootpage = (lkSQL3FieldData_Int*)o;
		o = m_Col.GetField(t3SqliteMaster_SQL);
		bRet = (o && o->IsKindOf(wxCLASSINFO(lkSQL3FieldData_Text)));
	}
	if ( bRet )
		m_pFldSql = (lkSQL3FieldData_Text*)o;
	else
		Throw_InvalidTable(t3SqliteMaster);

	return bRet;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Members
#pragma warning(disable:6011)

wxString lkTSqliteMaster::GetFldType() const
{
	wxASSERT(m_pFldType != NULL);
	return m_pFldType->GetValue2();

}
wxString lkTSqliteMaster::GetFldName() const
{
	wxASSERT(m_pFldName != NULL);
	return m_pFldName->GetValue2();
}
wxString lkTSqliteMaster::GetFldTbleName() const
{
	wxASSERT(m_pFldTblName != NULL);
	return m_pFldTblName->GetValue2();
}
wxInt64 lkTSqliteMaster::GetFldRootpage() const
{
	wxASSERT(m_pFldRootpage != NULL);
	return m_pFldRootpage->GetValue2();
}
wxString lkTSqliteMaster::GetFldSql() const
{
	wxASSERT(m_pFldSql != NULL);
	return m_pFldSql->GetValue2();
}

#pragma warning(default:6011)

////////////////////////////////////////////////////////////////////////////////////////////
// Implementation

//virtual
void lkTSqliteMaster::InitColumns(const wxString& _tblName)
{
	wxASSERT(m_DB && m_DB->IsOpen() && !_tblName.IsEmpty());
	wxASSERT(!m_Col.IsEmpty());

	lkSQL3FieldData* _fldData;
	lkSQL3VerifyList* pList = NULL;
	{
		int cid;
		wxString name;
		wxString _type;
		bool _notnull;
		bool _useDefault;
		wxString _default;

		lkSQL3Verify* pOb;
		lkSQL3Statement* stmt = NULL;
		wxString _qry; _qry.Printf(_T("PRAGMA table_info (%s);"), _tblName);
		try
		{
			stmt = new lkSQL3Statement(*m_DB, _qry);
			while ( stmt->Step() )
			{
				cid = stmt->GetField(wxT("cid"));
				name = stmt->GetField(wxT("name")).GetText();
				_type = stmt->GetField(wxT("type")).GetText();
				_notnull = (stmt->GetField(wxT("notnull")).GetInt() != 0);
				_useDefault = !(stmt->GetField(wxT("dflt_value")).IsNull());
				//			_default = (_useDefault) ? SQLconvert_Reverse((LPCTSTR)stmt->GetField(_T("dflt_value"))) : _T("");
				_default = (_useDefault) ? stmt->GetField(wxT("dflt_value")).GetText() : wxT("");

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
			if ( stmt ) delete stmt;
			if ( pList )
			{
				delete pList;
				pList = NULL;
			}
			//		throw;
		}
		if ( stmt ) delete stmt;
	}

	if ( pList && !pList->IsEmpty() )
	{
		lkSQL3VerifyList::iterator Iter;
		for ( Iter = pList->begin(); Iter != pList->end(); Iter++ )
			if ( (_fldData = m_Col.GetField((*Iter)->m_sName)) != NULL )
			{
				_fldData->SetIndex((*Iter)->m_nIdx);
				_fldData->SetSqlType((*Iter)->m_nType);
				_fldData->SetNotNull((*Iter)->m_bCanbeNull);
				_fldData->SetDefault((*Iter)->m_bUsingDefault, (*Iter)->m_sDefault);
			}
	}
	if ( pList ) delete pList;

	lkSQL3RecordSet::InitColumns(_tblName);
}


//static
wxUint64 lkTSqliteMaster::GetTableCount(lkSQL3Database* pDB)
{
	if ( !pDB || !pDB->IsOpen() ) return 0;

	wxString sql;
	sql.Printf(wxT("SELECT COUNT(ROWID) FROM [%s] WHERE([type] = 'table');"), t3SqliteMaster);
	lkSQL3Statement stmt(*pDB, sql);
	stmt.Step();
	return (wxUint64)(stmt.GetField(0));
}
//static
wxUint64 lkTSqliteMaster::GetIndexCount(lkSQL3Database* pDB)
{
	if ( !pDB || !pDB->IsOpen() ) return 0;

	wxString sql;
	sql.Printf(wxT("SELECT COUNT(ROWID) FROM [%s] WHERE([type] = 'index');"), t3SqliteMaster);
	lkSQL3Statement stmt(*pDB, sql);
	stmt.Step();
	return (wxUint64)(stmt.GetField(0));
}
//static
wxUint64 lkTSqliteMaster::GetRowCount(lkSQL3Database* pDB, const wxString& table_name)
{
	if ( table_name.IsEmpty() ) return 0;
	if ( !pDB || !pDB->IsOpen() ) return 0;

	wxString sql;
	sql.Printf(wxT("SELECT COUNT(ROWID) FROM [%s];"), table_name);
	lkSQL3Statement stmt(*pDB, sql);
	stmt.Step();
	return (wxUint64)(stmt.GetField(0));
}
