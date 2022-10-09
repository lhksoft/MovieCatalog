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
#include "TGroup.h"

#include "TBase.h"		// required in Compact, needs table-name
#include "TStorage.h"	// required in Compact, needs table-name
#include "TTemp.h"

#include "CompactThread.h"

#include "../lkSQLite3/lkSQL3Field.h"
#include "../lkSQLite3/lkSQL3Exception.h"


// static definitions for setting the fields of new table
static sqlTblField_t t3GroupFlds[] = {
	{ t3Group_BASE,    sqlFieldType::sqlInt, true,  false, 0 },
	{ t3Group_STORAGE, sqlFieldType::sqlInt, true,  false, 0 },
	{ t3Group_PART,   sqlFieldType::sqlText, false, false, 0, 1, 1 }, // optional (can be NULL) if entered, max. 1 char Uppercase,A..Z,0..9
	{ t3Group_TIME,   sqlFieldType::sqlReal, false, false, 0 },
	{ 0 }
};

// static definitions for creating a fresh new table
static sqlTblDef_t t3GroupTbl = { t3Group, t3GroupFlds, NULL };

///////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_DYNAMIC_CLASS(TGroup, lkSQL3TableSet)

TGroup::TGroup() : lkSQL3TableSet()
{
	Init();
}
TGroup::TGroup(lkSQL3Database* pDB) : lkSQL3TableSet(pDB)
{
	Init();
}

void TGroup::Init()
{
	m_pFldBase = NULL;
	m_pFldStorage = NULL;
	m_pFldPart = NULL;
	m_pFldTime = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Operations
////

//virtual
bool TGroup::Open()
{
	lkInitTable msInit(&t3GroupTbl);
	// fill Fields
	msInit.Append(new lkInitField(t3Group_BASE, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldBase));
	msInit.Append(new lkInitField(t3Group_STORAGE, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldStorage));
	msInit.Append(new lkInitField(t3Group_PART, wxCLASSINFO(lkSQL3FieldData_Text), (lkSQL3FieldData**)&m_pFldPart));
	msInit.Append(new lkInitField(t3Group_TIME, wxCLASSINFO(lkSQL3FieldData_Real), (lkSQL3FieldData**)&m_pFldTime));

	bool bRet = lkSQL3TableSet::Open(&msInit);

	if ( !bRet )
		Throw_InvalidTable(t3Group);

	m_bIsOpen = bRet;
	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Attributes
////

wxUint64 TGroup::GetBaseID() const
{
	wxASSERT(m_pFldBase);
	return m_pFldBase->GetValue2();
}
void TGroup::SetBaseID(wxUint64 _b)
{
	wxASSERT(m_pFldBase);
	m_pFldBase->SetValue2(_b);
}

wxUint64 TGroup::GetStorageID() const
{
	wxASSERT(m_pFldStorage);
	return m_pFldStorage->GetValue2();
}
void TGroup::SetStorageID(wxUint64 _s)
{
	wxASSERT(m_pFldStorage);
	m_pFldStorage->SetValue2(_s);
}

wxString TGroup::GetPartValue() const
{
	wxASSERT(m_pFldPart);
	return m_pFldPart->GetValue2();
}
void TGroup::SetPartValue(const wxString& _p)
{
	wxASSERT(m_pFldPart);
	if ( _p.IsEmpty() )
		m_pFldPart->SetValueNull();
	else
		m_pFldPart->SetValue2(_p);
}

double TGroup::GetTimeValue() const
{
	wxASSERT(m_pFldTime);
	return m_pFldTime->GetValue2();
}
void TGroup::SetTimeValue(double _t)
{
	wxASSERT(m_pFldTime);
	if ( _t == 0.0 )
		m_pFldTime->SetValueNull();
	else
		m_pFldTime->SetValue2(_t);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation
////

//static
bool TGroup::VerifyTable(lkSQL3Database* pDB)
{
	return lkSQL3TableSet::VerifyTable(pDB, t3GroupTbl);
}

//static
bool TGroup::CreateTable(lkSQL3Database* pDB)
{
	bool bRet = false;

	if ( !pDB->TableExists(t3Group) )
		bRet = lkSQL3TableSet::CreateTable(pDB, t3GroupTbl);

	return bRet;
}

//static
bool TGroup::HasStorage(lkSQL3Database* pDB, wxUint64 storageID)
{ // TRUE if at least 1 storage match has been found
	bool bRet = true;
	if ( !pDB || !pDB->IsOpen() ) return bRet;

	wxString qry;
	qry.Printf(wxT("SELECT COUNT([ROWID]) FROM [%s] WHERE ([%s] = %I64u);"), t3Group, t3Group_STORAGE, storageID);
	try
	{
		bRet = (pDB->ExecuteSQLAndGet(qry).GetInt() > 0);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		bRet = true;
	}

	return bRet;
}

//static
bool TGroup::HasBase(lkSQL3Database* pDB, wxUint64 baseID)
{ // TRUE if at least 1 Base match has been found
	bool bRet = true;
	if ( !pDB || !pDB->IsOpen() ) return bRet;

	wxString qry;
	qry.Printf(wxT("SELECT COUNT([ROWID]) FROM [%s] WHERE ([%s] = %I64u);"), t3Group, t3Group_BASE, baseID);
	try
	{
		bRet = (pDB->ExecuteSQLAndGet(qry).GetInt() > 0);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		bRet = true;
	}

	return bRet;
}

//static
bool TGroup::Compact(wxThread* thr, lkSQL3Database* dbSrc, lkSQL3Database* dbDest, lkSQL3Database* dbTmp) // wxThread required for sending messages to main-thread
{
	if ( !thr || !dbSrc || !dbDest || !dbTmp || !dbSrc->IsOpen() || !dbDest->IsOpen() || !dbTmp->IsOpen() )
		return false;

	wxUint64 curBase, newBase, curStorage, newStorage;
	double dbl;
	wxString s;
	TGroup rsSrc(dbSrc), rsDest(dbDest);
//	s.Printf(wxT("(([%s] != 0) AND ([%s] != 0))"), t3Group_BASE, t3Group_STORAGE);
//	rsSrc.SetFilter(s);
	rsSrc.SetOrder(wxT("[ROWID]"));
	try
	{
		rsSrc.Open();
		rsDest.Open();

		(dynamic_cast<CompactThread*>(thr))->SendInfoRewriting(t3Group);
		(dynamic_cast<CompactThread*>(thr))->InitProgressCtrl(0, rsSrc.GetRowCount());

		rsSrc.MoveFirst();

		while ( !thr->TestDestroy() && !rsSrc.IsEOF() )
		{
			if ( ((curBase=rsSrc.GetBaseID()) != 0) && ((curStorage=rsSrc.GetStorageID()) != 0) &&
				((newBase = TTemp::FindNewID(dbTmp, t3Base, curBase)) != 0) && ((newStorage = TTemp::FindNewID(dbTmp, t3Storage, curStorage)) != 0) )
			{
				rsDest.AddNew();
				// Base ID
				rsDest.SetBaseID(newBase);
				// Storage ID
				rsDest.SetStorageID(newStorage);
				// Part
				if ( !(s = rsSrc.GetPartValue()).IsEmpty() )
					rsDest.SetPartValue(s);
				// Time-Span
				if ( (dbl = rsSrc.GetTimeValue()) != 0.0 )
					rsDest.SetTimeValue(dbl);

				rsDest.Update();

				TStorage::SetLinked(dbDest, newStorage);
			}

			rsSrc.MoveNext();
			(dynamic_cast<CompactThread*>(thr))->StepProgressCtrl(0);
		}
	}
	catch ( const lkSQL3Exception& e )
	{
		wxString sql;
		sql.Printf(wxT("TGroup::Compact : %s"), ((lkSQL3Exception*)&e)->GetError());
		(dynamic_cast<CompactThread*>(thr))->SetErrString(sql);
		return false;
	}

	//BOOL bOK = (!rsSrc.IsEmpty() && !rsDest.IsEmpty()) ? (rsSrc.GetRowCount() == rsDest.GetRowCount()) : (rsSrc.IsEmpty() && rsDest.IsEmpty());
	// 29-08-2022 : mismatch, rsDest could be empty if no base or storage was linked in rsSrc
	return !(thr->TestDestroy());
}