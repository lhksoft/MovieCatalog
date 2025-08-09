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
#include "TLocation.h"
#include "TStorage.h"
#include "TTemp.h"

#include "CompactThread.h"

#include "../lkSQLite3/lkSQL3Field.h"
#include "../lkSQLite3/lkSQL3Exception.h"

// static definitions for setting the fields of new table
static sqlTblField_t t3LocationFlds[] = {
	{ t3Location_NAME, sqlFieldType::sqlText,  true, false, 0 }, // Label (vereist)
	{ 0 }
};

// static definitions for creating a fresh new table
static sqlTblDef_t t3LocationTbl = { t3Location, t3LocationFlds, 0 };

wxIMPLEMENT_DYNAMIC_CLASS(TLocation, lkSQL3TableSet)

TLocation::TLocation() : lkSQL3TableSet()
{
	m_RowIdGrow = 25;
	Init();
}
TLocation::TLocation(lkSQL3Database* pDB) : lkSQL3TableSet(pDB)
{
	m_RowIdGrow = 25;
	Init();
}

void TLocation::Init()
{
	m_pFldLocation = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Operations

//virtual
bool TLocation::Open()
{
	lkInitTable msInit(&t3LocationTbl);
	// fill Fields
	msInit.Append(new lkInitField(t3Location_NAME, wxCLASSINFO(lkSQL3FieldData_Text), (lkSQL3FieldData**)&m_pFldLocation));

	bool bRet = lkSQL3TableSet::Open(&msInit);

	if ( !bRet )
		Throw_InvalidTable(t3Location);

	m_bIsOpen = bRet;
	return bRet;
}

//virtual
bool TLocation::CanDelete()
{
	return (IsOpen() && lkSQL3TableSet::CanDelete() && !TStorage::HasLocation(m_DB, static_cast<wxUint32>(GetActualCurRow())));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Attributes

wxString TLocation::GetLocationValue() const
{
	wxASSERT(m_pFldLocation);
	return m_pFldLocation->GetValue2();
}
void TLocation::SetLocationValue(const wxString& _loc)
{
	wxASSERT(m_pFldLocation);
	m_pFldLocation->SetValue2(_loc);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation

//static
bool TLocation::VerifyTable(lkSQL3Database* pDB)
{
	return lkSQL3TableSet::VerifyTable(pDB, t3LocationTbl);
}

//static
bool TLocation::CreateTable(lkSQL3Database* pDB)
{
	bool bRet = false;

	if ( !pDB->TableExists(t3Location) )
		bRet = lkSQL3TableSet::CreateTable(pDB, t3LocationTbl);

	return bRet;
}

//static
bool TLocation::IsUniqueLocation(lkSQL3Database* _DB, const wxString& location, void*)
{
	bool ret = false;
	try
	{
		ret = lkSQL3TableSet::IsUniqueTextValue(_DB, t3Location, t3Location_NAME, location);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return false;
	}

	return ret;
}

//static
bool TLocation::Compact(wxThread* thr, lkSQL3Database* dbSrc, lkSQL3Database* dbDest, lkSQL3Database* dbTmp) // wxThread required for sending messages to main-thread
{
	if ( !thr || !dbSrc || !dbDest || !dbTmp || !dbSrc->IsOpen() || !dbDest->IsOpen() || !dbTmp->IsOpen() )
		return false;

	wxString s;
	TLocation rsSrc(dbSrc), rsDest(dbDest);
	TTemp rsTmp(dbTmp);
	s.Printf(wxT("[%s] COLLATE lkUTF8compare"), t3Location_NAME);
	rsSrc.SetOrder(s);
	wxUint64 cnt = 0;

	try
	{
		rsSrc.Open();
		rsDest.Open();
		rsTmp.Open(t3Location);
		cnt = rsSrc.GetRowCount();

		(dynamic_cast<CompactThread*>(thr))->SendInfoRewriting(t3Location);
		(dynamic_cast<CompactThread*>(thr))->InitProgressCtrl(0, rsSrc.GetRowCount());

		rsSrc.MoveFirst();

		// Find our "Kast" first
		s.Printf(wxT("([%s] = 'Kast')"), t3Location_NAME);
		rsSrc.SetFilter(s);
		rsSrc.Requery();
		if ( !rsSrc.IsEmpty() )
		{
			rsDest.AddNew();
			rsDest.SetLocationValue(rsSrc.GetLocationValue());
			rsDest.Update();

			rsTmp.AddNew();
			rsTmp.SetOld(rsSrc.GetActualCurRow());
			rsTmp.SetNew(rsDest.GetActualCurRow());
			rsTmp.Update();

			(dynamic_cast<CompactThread*>(thr))->StepProgressCtrl(0);

			s.Printf(wxT("([ROWID] != %I64u)"), rsSrc.GetActualCurRow());
			rsSrc.SetFilter(s);
		}
		else
			rsSrc.SetFilter(wxEmptyString);
		rsSrc.Requery();

		// Now add all the rest
		while ( !thr->TestDestroy() && !rsSrc.IsEOF() )
		{
			rsDest.AddNew();

			// Location
			rsDest.SetLocationValue(rsSrc.GetLocationValue());

			rsDest.Update();

			rsTmp.AddNew();
			rsTmp.SetOld(rsSrc.GetActualCurRow());
			rsTmp.SetNew(rsDest.GetActualCurRow());
			rsTmp.Update();

			rsSrc.MoveNext();
			(dynamic_cast<CompactThread*>(thr))->StepProgressCtrl(0);
		}
	}
	catch ( const lkSQL3Exception& e )
	{
		wxString sql;
		sql.Printf(wxT("TLocation::Compact : %s"), ((lkSQL3Exception*)&e)->GetError());
		(dynamic_cast<CompactThread*>(thr))->SetErrString(sql);
		return false;
	}

	bool bOK = (!rsSrc.IsEmpty() && !rsDest.IsEmpty()) ? (cnt == rsDest.GetRowCount()) : (rsSrc.IsEmpty() && rsDest.IsEmpty());
	bOK = (bOK) ? !thr->TestDestroy() : bOK;
	return bOK;
}
