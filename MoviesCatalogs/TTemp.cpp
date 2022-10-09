/* ********************************************************************************
 * MoviesCatalog - a Programm to catalogue a private collection of Movies using SQLite3
 * Copyright (C) 2022 by Laurens Koehoorn (lhksoft)
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * ********************************************************************************/
#include "TTemp.h"

#include "../lkSQLite3/lkSQL3Field.h"
#include "../lkSQLite3/lkSQL3Exception.h"

// Required Field-definitions for TTemp
#define t3Temp_OLD			wxT("oldID")
#define t3Temp_NEW			wxT("newID")

// static definitions for setting the fields of new table
static sqlTblField_t t3TempFlds[] = {
	{ t3Temp_OLD, sqlFieldType::sqlInt, true, false, 0 },
	{ t3Temp_NEW, sqlFieldType::sqlInt, true, false, 0 },
	{ 0 }
};

// Required Field-definitions for TTempCat
#define t3Temp_OLDCAT		wxT("oldCatID")
#define t3Temp_OLDSUB		wxT("oldSubID")
#define t3Temp_NEWCAT		wxT("newCatID")
#define t3Temp_NEWSUB		wxT("newSubID")

// static definitions for setting the fields of new table
static sqlTblField_t t3TempCatFlds[] = {
	{ t3Temp_OLDCAT, sqlFieldType::sqlInt, true, false, 0 },
	{ t3Temp_OLDSUB, sqlFieldType::sqlInt, true, true, wxT("0") },
	{ t3Temp_NEWCAT, sqlFieldType::sqlInt, true, false, 0 },
	{ t3Temp_NEWSUB, sqlFieldType::sqlInt, true, true, wxT("0") },
	{ 0 }
};

wxIMPLEMENT_DYNAMIC_CLASS(TTemp, lkSQL3TableSet)

TTemp::TTemp() : lkSQL3TableSet()
{
	Init();
}
TTemp::TTemp(lkSQL3Database* pDB) : lkSQL3TableSet(pDB)
{
	Init();
}

void TTemp::Init()
{
	m_pFldOldID = NULL;
	m_pFldNewID = NULL;
}

wxUint64 TTemp::GetOld() const
{
	wxASSERT(m_pFldOldID);
	return m_pFldOldID->GetValue2();
}
void TTemp::SetOld(wxUint64 o)
{
	wxASSERT(m_pFldOldID);
	m_pFldOldID->SetValue2(o);
}

wxUint64 TTemp::GetNew() const
{
	wxASSERT(m_pFldNewID);
	return m_pFldNewID->GetValue2();
}
void TTemp::SetNew(wxUint64 n)
{
	wxASSERT(m_pFldNewID);
	m_pFldNewID->SetValue2(n);
}

bool TTemp::Open(const wxString& tmpTableName)
{
	if ( tmpTableName.IsEmpty() )
		return false;

	if ( IsOpen() )
		return true;

	// static definitions for creating a fresh new table
	sqlTblDef_t TempTbl = { tmpTableName, t3TempFlds, NULL };

	bool bRet = lkSQL3TableSet::Open();
	if ( bRet && !m_DB->TableExists(tmpTableName) )
	{
		try
		{
			bRet = lkSQL3TableSet::CreateTable(m_DB, TempTbl);
		}
		catch ( const lkSQL3Exception& )
		{
			bRet = false;
			throw;
		}
	}

	if ( bRet )
	{
		lkInitTable msInit(&TempTbl);
		// fill Fields
		msInit.Append(new lkInitField(t3Temp_OLD, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldOldID));
		msInit.Append(new lkInitField(t3Temp_NEW, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldNewID));

		bRet = lkSQL3TableSet::Open(&msInit);

		if ( !bRet )
			Throw_InvalidTable(tmpTableName);
	}
	m_bIsOpen = bRet;
	return bRet;
}

//static
wxUint64 TTemp::FindNewID(lkSQL3Database* _DB, const wxString& sTable, wxUint64 nOld)
{
	wxASSERT(!sTable.IsEmpty());
	wxASSERT(nOld > 0);
	if ( (_DB == NULL) || !_DB->IsOpen() || sTable.IsEmpty() || !_DB->TableExists(sTable) ) return 0;

	wxString sql, qry;
	qry.Printf(wxT("SELECT COUNT([ROWID]) FROM [%s] WHERE ([%s] = %I64u);"), sTable, t3Temp_OLD, nOld);
	sql.Printf(wxT("SELECT [%s] FROM [%s] WHERE ([%s] = %I64u);"), t3Temp_NEW, sTable, t3Temp_OLD, nOld);

	wxUint64 nRet = 0;
	try
	{
		if ( (int)_DB->ExecuteSQLAndGet(qry) > 0 )
			nRet = (wxUint64)_DB->ExecuteSQLAndGet(sql).GetInt();
	}
	catch ( const lkSQL3Exception& e )
	{
		nRet = 0;
		throw;
	}
	return nRet;
}

//static
wxUint64 TTemp::FindNewGenre(lkSQL3Database* _DB, const wxString& sTable, wxUint64 nOld)
{
	wxASSERT(!sTable.IsEmpty());
	wxASSERT(nOld > 0);

	if ( (_DB == NULL) || !_DB->IsOpen() || sTable.IsEmpty() || !_DB->TableExists(sTable) ) return 0;

	wxUint64 g = 0;
	TTemp RS(_DB);
	wxString flt;
	flt.Printf(wxT("[%s]"), t3Temp_OLD);
	RS.SetOrder(flt);
	RS.Open(sTable);
	RS.MoveFirst();

	wxUint64 o;
	while ( !RS.IsEOF() )
	{
		o = RS.GetOld();
		if ( o > nOld )
			break;

		if ( nOld & o )
			g |= RS.GetNew();

		RS.MoveNext();
	}

	return g;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class TTempCat
////
wxIMPLEMENT_DYNAMIC_CLASS(TTempCat, lkSQL3TableSet)

TTempCat::TTempCat() : lkSQL3TableSet()
{
	Init();
}
TTempCat::TTempCat(lkSQL3Database* pDB) : lkSQL3TableSet(pDB)
{
	Init();
}

void TTempCat::Init()
{
	m_pFldOldCatID = NULL;
	m_pFldOldSubID = NULL;
	m_pFldNewCatID = NULL;
	m_pFldNewSubID = NULL;
}

wxUint32 TTempCat::GetOldCat() const
{
	wxASSERT(m_pFldOldCatID);
	return (wxUint32)m_pFldOldCatID->GetValue2();
}
void TTempCat::SetOldCat(wxUint32 o)
{
	wxASSERT(m_pFldOldCatID);
	m_pFldOldCatID->SetValue2(o);
}

wxUint32 TTempCat::GetNewCat() const
{
	wxASSERT(m_pFldNewCatID);
	return (wxUint32)m_pFldNewCatID->GetValue2();
}
void TTempCat::SetNewCat(wxUint32 n)
{
	wxASSERT(m_pFldNewCatID);
	m_pFldNewCatID->SetValue2(n);
}

wxUint32 TTempCat::GetOldSub() const
{
	wxASSERT(m_pFldOldSubID);
	return (wxUint32)m_pFldOldSubID->GetValue2();
}
void TTempCat::SetOldSub(wxUint32 o)
{
	wxASSERT(m_pFldOldSubID);
	m_pFldOldSubID->SetValue2(o);
}

wxUint32 TTempCat::GetNewSub() const
{
	wxASSERT(m_pFldNewSubID);
	return (wxUint32)m_pFldNewSubID->GetValue2();
}
void TTempCat::SetNewSub(wxUint32 n)
{
	wxASSERT(m_pFldNewSubID);
	m_pFldNewSubID->SetValue2(n);
}

bool TTempCat::Open(const wxString& tmpTableName)
{
	if ( tmpTableName.IsEmpty() )
		return false;

	if ( IsOpen() )
		return true;

	// static definitions for creating a fresh new table
	sqlTblDef_t TempTbl = { tmpTableName, t3TempCatFlds, NULL };

	bool bRet = lkSQL3TableSet::Open();
	if ( bRet && !m_DB->TableExists(tmpTableName) )
	{
		try
		{
			bRet = lkSQL3TableSet::CreateTable(m_DB, TempTbl);
		}
		catch ( const lkSQL3Exception& )
		{
			bRet = false;
			throw;
		}
	}

	if ( bRet )
	{
		lkInitTable msInit(&TempTbl);
		// fill Fields
		msInit.Append(new lkInitField(t3Temp_OLDCAT, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldOldCatID));
		msInit.Append(new lkInitField(t3Temp_OLDSUB, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldOldSubID));
		msInit.Append(new lkInitField(t3Temp_NEWCAT, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldNewCatID));
		msInit.Append(new lkInitField(t3Temp_NEWSUB, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldNewSubID));

		bRet = lkSQL3TableSet::Open(&msInit);

		if ( !bRet )
			Throw_InvalidTable(tmpTableName);
	}
	m_bIsOpen = bRet;
	return bRet;
}

//static
wxUint32 TTempCat::FindNewID(lkSQL3Database* _DB, const wxString& sTable, wxUint32 nOldCat, wxUint32 nOldSub)
{
	wxASSERT(!sTable.IsEmpty());

	if ( (_DB == NULL) || !_DB->IsOpen() || sTable.IsEmpty() || !_DB->TableExists(sTable) ) return 0;

	wxString whr;
	whr.Printf(wxT("([%s] = %u) AND ([%s] = %u)"), t3Temp_OLDCAT, nOldCat, t3Temp_OLDSUB, nOldSub);

	TTempCat rs(_DB);
	rs.Open(sTable);
	rs.SetFilter(whr);
	rs.Requery();

	wxUint32 n = (!rs.IsEmpty()) ? ((nOldSub == 0) ? rs.GetNewCat() : rs.GetNewSub()) : 0;

	return n;
}

