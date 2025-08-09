/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // !_CRT_SECURE_NO_WARNINGS
#include "lkSQL3FindTable.h"
#include "lkSQL3Field.h"
#include "lkSQL3Exception.h"

 // name of Table
#define t3Find				wxT("tmpFind")
// Required Field-definitions
#define t3Find_OLD			wxT("origID")			// original ROWID of original TABLE

 // static definitions for setting the fields of new table
static sqlTblField_t t3FindFlds[] = {
	{ t3Find_OLD, sqlFieldType::sqlInt, true, false, 0 }, // Label (vereist)
	{ 0 }
};

// static definitions for creating a fresh new table
static sqlTblDef_t t3FindTbl = { t3Find, t3FindFlds, 0 };

wxIMPLEMENT_DYNAMIC_CLASS(lkSQL3FindTable, lkSQL3TableSet);


lkSQL3FindTable::lkSQL3FindTable() : lkSQL3TableSet()
{
	m_RowIdGrow = 10;
	m_pFldID = NULL;
}
lkSQL3FindTable::lkSQL3FindTable(lkSQL3Database* pDB) : lkSQL3TableSet(pDB)
{
	m_RowIdGrow = 10;
	m_pFldID = NULL;
}
lkSQL3FindTable::~lkSQL3FindTable()
{
}

//virtual
bool lkSQL3FindTable::Open()
{
	if ( IsOpen() ) return true;

	bool bRet = lkSQL3TableSet::Open();
	if ( bRet )
		bRet = m_DB->TableExists(t3Find);

	if ( bRet )
	{
		wxString s;
		lkSQL3TblInit init(t3FindTbl.sName);
		for ( int i = 0; t3FindTbl.pFields[i].sName && t3FindTbl.pFields[i].sName[0]; i++ )
		{
			if ( (s = t3FindTbl.pFields[i].sName).CmpNoCase(t3Find_OLD) == 0 )
				init.AddFldInit(t3FindTbl.pFields[i].sName, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldID);
		}

		if ( bRet = InitColumns(init, t3FindTbl) )
			bRet = (m_pFldID != NULL);
	}
	if ( !bRet )
		Throw_InvalidTable(t3Find);

	m_bIsOpen = bRet;
	return bRet;
}

//static
bool lkSQL3FindTable::CreateTable(lkSQL3Database* pDB)
{
	bool bRet = false;

	if ( !pDB->TableExists(t3Find) )
		bRet = lkSQL3TableSet::CreateTable(pDB, t3FindTbl);

	return bRet;
}

wxUint64 lkSQL3FindTable::GetOrigID() const
{
	wxASSERT(m_pFldID);
	return (wxUint64)(m_pFldID->GetValue2());
}
void lkSQL3FindTable::SetOrigID(wxUint64 _c)
{
	wxASSERT(m_pFldID);
	m_pFldID->SetValue2((wxInt64)_c);
}

wxUint64 lkSQL3FindTable::FindFirst(wxUint64& _Find)
{
	ResetFind();
	_Find = 0;

	//	if (!IsOpen()) return 0;

	wxString _qry;
	_qry.Printf(wxT("SELECT [ROWID], %s FROM [%s]"), m_Query, m_Table);
	_qry += wxT(" ORDER BY [ROWID]");
	_qry += wxT(" LIMIT 1;");

	lkSQL3Statement* stmt = new lkSQL3Statement(*m_DB, _qry);
	stmt->Step();
	_Find = stmt->GetField(0); // .GetI64();
	wxUint64 result = stmt->GetField(1); // .GetI64();
	delete stmt;

	return result;
}

wxUint64 lkSQL3FindTable::FindLast(wxUint64& _Find)
{
	ResetFind();
	_Find = 0;

	//	if (!IsOpen()) return 0;

	wxString _qry;
	_qry.Printf(wxT("SELECT [ROWID], %s FROM [%s]"), m_Query, m_Table);
	_qry += wxT(" ORDER BY [ROWID] DESC");
	_qry += wxT(" LIMIT 1;");

	lkSQL3Statement* stmt = new lkSQL3Statement(*m_DB, _qry);
	stmt->Step();
	_Find = stmt->GetField(0); // .GetI64();
	wxUint64 result = stmt->GetField(1); // .GetI64();
	delete stmt;

	return result;
}

wxUint64 lkSQL3FindTable::FindNext(wxUint64& _Find)
{
	//	if (!IsOpen()) return 0;

	wxString s, _qry;
	_qry.Printf(wxT("SELECT [ROWID], %s FROM [%s]"), m_Query, m_Table);
	s.Printf(wxT(" WHERE ([ROWID] > %I64u)"), _Find);
	_qry += s;
	_qry += wxT(" ORDER BY [ROWID]");
	_qry += wxT(" LIMIT 1;");

	lkSQL3Statement* stmt = new lkSQL3Statement(*m_DB, _qry);
	stmt->Step();
	_Find = stmt->GetField(0); // .GetI64();
	wxUint64 result = stmt->GetField(1); // .GetI64();
	delete stmt;

	return result;
}
wxUint64 lkSQL3FindTable::FindPrev(wxUint64& _Find)
{
	//	if (!IsOpen()) return 0;

	wxString s, _qry;
	_qry.Printf(wxT("SELECT [ROWID], %s FROM [%s]"), m_Query, m_Table);
	s.Printf(wxT(" WHERE ([ROWID] < %I64u)"), _Find);
	_qry += s;
	_qry += wxT(" ORDER BY [ROWID] DESC");
	_qry += wxT(" LIMIT 1;");

	lkSQL3Statement* stmt = new lkSQL3Statement(*m_DB, _qry);
	stmt->Step();
	_Find = stmt->GetField(0); // .GetI64();
	wxUint64 result = stmt->GetField(1); // .GetI64();
	delete stmt;

	return result;
}

bool lkSQL3FindTable::HasNext(wxUint64 _Find)
{
	wxString s, _qry;
	_qry.Printf(wxT("SELECT [ROWID] FROM [%s]"), m_Table);
	s.Printf(wxT(" WHERE ([ROWID] > %I64u)"), _Find);
	_qry += s;
	_qry += wxT(" ORDER BY [ROWID]");

	s.Printf(wxT("SELECT COUNT([ROWID]) FROM (%s);"), _qry);

	wxUint64 ret = m_DB->ExecuteSQLAndGet(s);

	return (ret > 0);
}

bool lkSQL3FindTable::HasPrev(wxUint64 _Find)
{
	wxString s, _qry;
	_qry.Printf(wxT("SELECT [ROWID] FROM [%s]"), m_Table);
	s.Printf(wxT(" WHERE ([ROWID] < %I64u)"), _Find);
	_qry += s;
	_qry += wxT(" ORDER BY [ROWID] DESC");

	s.Printf(wxT("SELECT COUNT([ROWID]) FROM (%s);"), _qry);

	wxUint64 ret = m_DB->ExecuteSQLAndGet(s);

	return (ret > 0);
}
