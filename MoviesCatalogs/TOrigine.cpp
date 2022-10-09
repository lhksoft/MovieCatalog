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
#include "TOrigine.h"
#include "TBase.h"
#include "TTemp.h"

#include "CompactThread.h"

#include "../lkControls/lkConfigTools.h"
#include "../lkSQLite3/lkSQL3Field.h"
#include "../lkSQLite3/lkSQL3Exception.h"

#define config_ORIGINE_PATH			wxT("Origines")
#define config_ORIGINE_DEFAULT		wxT("Default")

// static definitions for setting the fields of new table
static sqlTblField_t t3OrigineFlds[] = {
	{ t3Orig_NAME, sqlFieldType::sqlText, true, false, 0 }, // (vereist)
	{ 0 }
};

// static definitions for creating a fresh new table
static sqlTblDef_t t3OrigineTbl = { t3Orig, t3OrigineFlds, 0 };


wxIMPLEMENT_DYNAMIC_CLASS(TOrigine, lkSQL3TableSet)

TOrigine::TOrigine() : lkSQL3TableSet()
{
	m_RowIdGrow = 5;
	Init();
}
TOrigine::TOrigine(lkSQL3Database* pDB) : lkSQL3TableSet(pDB)
{
	m_RowIdGrow = 5;
	Init();
}

void TOrigine::Init()
{
	m_pFldName = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Operations

//virtual
bool TOrigine::Open()
{
	lkInitTable msInit(&t3OrigineTbl);
	// fill Fields
	msInit.Append(new lkInitField(t3Orig_NAME, wxCLASSINFO(lkSQL3FieldData_Text), (lkSQL3FieldData**)&m_pFldName));

	bool bRet = lkSQL3TableSet::Open(&msInit);

	if ( !bRet )
		Throw_InvalidTable(t3Orig);

	m_bIsOpen = bRet;
	return bRet;
}

//virtual
bool TOrigine::CanDelete()
{
	bool bCan = (IsOpen() && lkSQL3TableSet::CanDelete());
	if (bCan )
		bCan = !(TBase::HasOrigine(m_DB, GetActualCurRow()));
	return bCan;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Attributes

wxString TOrigine::GetOrigineValue() const
{
	wxASSERT(m_pFldName);
	return m_pFldName->GetValue2();
}
void TOrigine::SetOrigineValue(const wxString& _orig)
{
	wxASSERT(m_pFldName);
	m_pFldName->SetValue2(_orig);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation

//static
bool TOrigine::VerifyTable(lkSQL3Database* pDB)
{
	return lkSQL3TableSet::VerifyTable(pDB, t3OrigineTbl);
}

//static
bool TOrigine::CreateTable(lkSQL3Database* pDB)
{
	bool bRet = false;

	if ( !pDB->TableExists(t3Orig) )
		bRet = lkSQL3TableSet::CreateTable(pDB, t3OrigineTbl);

	return bRet;
}

//static
bool TOrigine::IsUniqueOrigine(lkSQL3Database* _DB, const wxString& orig, void*)
{
	bool ret = false;
	try
	{
		ret = lkSQL3TableSet::IsUniqueTextValue(_DB, t3Orig, t3Orig_NAME, orig);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return false;
	}

	return ret;
}

//static
void TOrigine::SetDefaultOrigine(wxUint64 o)
{
	if ( o == 0 )
		DeleteKey(config_ORIGINE_PATH, config_ORIGINE_DEFAULT);
	else
		SetConfigInt(config_ORIGINE_PATH, config_ORIGINE_DEFAULT, o);
}
//static
wxUint64 TOrigine::GetDefaultOrigine(lkSQL3Database* pDB)
{
	wxUint64 defRow = GetConfigInt(config_ORIGINE_PATH, config_ORIGINE_DEFAULT);

	if ( (defRow > 0) && pDB && pDB->IsOpen() )
	{
		wxString qry;
		qry.Printf(wxT("SELECT COUNT([ROWID]) FROM [%s] WHERE ([ROWID] = %I64u);"), t3Orig, defRow);
		bool bRet = false;

		try
		{
			bRet = ((wxUint64)pDB->ExecuteSQLAndGet(qry) > 0);
		}
		catch ( const lkSQL3Exception&/* e*/ )
		{
			bRet = false;
		}

		if ( !bRet )
			defRow = 0;
	}

	return defRow;
}

//static
void TOrigine::CorrectConfig(lkSQL3Database* dbTmp)
{
	if ( !dbTmp || !dbTmp->IsOpen() )
		return;

	wxUint64 nNew = 0, nOld = GetConfigInt(config_ORIGINE_PATH, config_ORIGINE_DEFAULT);
	if ( nOld > 0 )
		nNew = TTemp::FindNewID(dbTmp, t3Orig, nOld);
	TOrigine::SetDefaultOrigine(nNew);
}

//static
bool TOrigine::Compact(wxThread* thr, lkSQL3Database* dbSrc, lkSQL3Database* dbDest, lkSQL3Database* dbTmp) // wxThread required for sending messages to main-thread
{
	if ( !thr || !dbSrc || !dbDest || !dbTmp || !dbSrc->IsOpen() || !dbDest->IsOpen() || !dbTmp->IsOpen() )
		return false;

	wxString s;
	TOrigine rsSrc(dbSrc), rsDest(dbDest);
	TTemp rsTmp(dbTmp);
	rsSrc.SetOrder(wxT("[ROWID]"));
	try
	{
		rsSrc.Open();
		rsDest.Open();
		rsTmp.Open(t3Orig);

		(dynamic_cast<CompactThread*>(thr))->SendInfoRewriting(t3Orig);
		(dynamic_cast<CompactThread*>(thr))->InitProgressCtrl(0, rsSrc.GetRowCount());

		rsSrc.MoveFirst();

		while ( !thr->TestDestroy() && !rsSrc.IsEOF() )
		{
			rsDest.AddNew();
			// Origine
			rsDest.SetOrigineValue(rsSrc.GetOrigineValue());
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
		sql.Printf(wxT("TOrigine::Compact : %s"), ((lkSQL3Exception*)&e)->GetError());
		(dynamic_cast<CompactThread*>(thr))->SetErrString(sql);
		return false;
	}

	bool bOK = (!rsSrc.IsEmpty() && !rsDest.IsEmpty()) ? (rsSrc.GetRowCount() == rsDest.GetRowCount()) : (rsSrc.IsEmpty() && rsDest.IsEmpty());
	bOK = (bOK) ? !thr->TestDestroy() : bOK;
	return bOK;
}
