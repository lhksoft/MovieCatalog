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
#include "TScreen.h"
#include "TBase.h"
#include "TTemp.h"

#include "CompactThread.h"

#include "../lkControls/lkConfigTools.h"
#include "../lkSQLite3/lkSQL3Field.h"
#include "../lkSQLite3/lkSQL3Exception.h"

#define config_SCREEN_PATH			wxT("Screens")
#define config_SCREEN_DEFAULT		wxT("Default")

// static definitions for setting the fields of new table
static sqlTblField_t t3ScreenFlds[] = {
	{ t3Screen_Format, sqlFieldType::sqlText, true, false,   0 }, // Format - Required Field, cannot be empty nor NULL
	{ 0 }
};

// Fields of Index-definitions
static sqlFldIndex_t t3ScreenIDX_1[] = {
	{ t3Screen_Format, sqlSortOrder::sortAsc },
	{ 0 }
};

// static definitions for setting the index of new table
static sqlTblIndex_t t3ScreenIdx[] = {
	{ _T("idxScreen_Format"), t3ScreenIDX_1, true }, // UNIQUE
	{ 0 }
};

// static definitions for creating a fresh new table
static sqlTblDef_t t3ScreenTbl = { t3Screen, t3ScreenFlds, t3ScreenIdx };


wxIMPLEMENT_DYNAMIC_CLASS(TScreen, lkSQL3TableSet)

TScreen::TScreen() : lkSQL3TableSet()
{
	m_RowIdGrow = 5;
	Init();
}
TScreen::TScreen(lkSQL3Database* pDB) : lkSQL3TableSet(pDB)
{
	m_RowIdGrow = 5;
	Init();
}

void TScreen::Init()
{
	m_pFldName = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Operations

//virtual
bool TScreen::Open()
{
	lkInitTable msInit(&t3ScreenTbl);
	// fill Fields
	msInit.Append(new lkInitField(t3Screen_Format, wxCLASSINFO(lkSQL3FieldData_Text), (lkSQL3FieldData**)&m_pFldName));

	bool bRet = lkSQL3TableSet::Open(&msInit);

	if ( !bRet )
		Throw_InvalidTable(t3Screen);

	m_bIsOpen = bRet;
	return bRet;
}

//virtual
bool TScreen::CanDelete()
{
	bool bCan = (IsOpen() && lkSQL3TableSet::CanDelete());
	if (bCan )
		bCan = !(TBase::HasScreen(m_DB, GetActualCurRow()));
	return bCan;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Attributes

wxString TScreen::GetScreenValue() const
{
	wxASSERT(m_pFldName);
	return m_pFldName->GetValue2();
}
void TScreen::SetScreenValue(const wxString& _scr)
{
	wxASSERT(m_pFldName);
	m_pFldName->SetValue2(_scr);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation

//static
bool TScreen::VerifyTable(lkSQL3Database* pDB)
{
	return lkSQL3TableSet::VerifyTable(pDB, t3ScreenTbl);
}

//static
bool TScreen::CreateTable(lkSQL3Database* pDB)
{
	bool bRet = false;

	if ( !pDB->TableExists(t3Screen) )
		bRet = lkSQL3TableSet::CreateTable(pDB, t3ScreenTbl);

	return bRet;
}

//static
bool TScreen::IsUniqueScreen(lkSQL3Database* _DB, const wxString& orig, void*)
{
	bool ret = false;
	try
	{
		ret = lkSQL3TableSet::IsUniqueTextValue(_DB, t3Screen, t3Screen_Format, orig);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return false;
	}

	return ret;
}

//static
void TScreen::SetDefaultScreen(wxUint64 s)
{
	if ( s == 0 )
		DeleteKey(config_SCREEN_PATH, config_SCREEN_DEFAULT);
	else
		SetConfigInt(config_SCREEN_PATH, config_SCREEN_DEFAULT, s);

}
//static
wxUint64 TScreen::GetDefaultScreen(lkSQL3Database* pDB)
{
	wxUint64 defRow = GetConfigInt(config_SCREEN_PATH, config_SCREEN_DEFAULT);

	if ( (defRow > 0) && pDB && pDB->IsOpen() )
	{
		wxString qry;
		qry.Printf(wxT("SELECT COUNT([ROWID]) FROM [%s] WHERE ([ROWID] = %I64u);"), t3Screen, defRow);
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
void TScreen::CorrectConfig(lkSQL3Database* dbTmp)
{
	if ( !dbTmp || !dbTmp->IsOpen() )
		return;

	wxUint64 nNew = 0, nOld = GetConfigInt(config_SCREEN_PATH, config_SCREEN_DEFAULT);
	if ( nOld > 0 )
		nNew = TTemp::FindNewID(dbTmp, t3Screen, nOld);
	TScreen::SetDefaultScreen(nNew);
}

//static
bool TScreen::Compact(wxThread* thr, lkSQL3Database* dbSrc, lkSQL3Database* dbDest, lkSQL3Database* dbTmp) // wxThread required for sending messages to main-thread
{
	if ( !thr || !dbSrc || !dbDest || !dbTmp || !dbSrc->IsOpen() || !dbDest->IsOpen() || !dbTmp->IsOpen() )
		return false;

	TScreen rsSrc(dbSrc), rsDest(dbDest);
	TTemp rsTmp(dbTmp);
	rsSrc.SetOrder(wxT("[ROWID]"));
	try
	{
		rsSrc.Open();
		rsDest.Open();
		rsTmp.Open(t3Screen);

		(dynamic_cast<CompactThread*>(thr))->SendInfoRewriting(t3Screen);
		(dynamic_cast<CompactThread*>(thr))->InitProgressCtrl(0, rsSrc.GetRowCount());

		rsSrc.MoveFirst();

		while ( !thr->TestDestroy() && !rsSrc.IsEOF() )
		{
			rsDest.AddNew();

			// Format
			rsDest.SetScreenValue(rsSrc.GetScreenValue());

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
		sql.Printf(wxT("TScreen::Compact : %s"), ((lkSQL3Exception*)&e)->GetError());
		(dynamic_cast<CompactThread*>(thr))->SetErrString(sql);
		return false;
	}

	bool bOK = (!rsSrc.IsEmpty() && !rsDest.IsEmpty()) ? (rsSrc.GetRowCount() == rsDest.GetRowCount()) : (rsSrc.IsEmpty() && rsDest.IsEmpty());
	bOK = (bOK) ? !thr->TestDestroy() : bOK;
	return bOK;
}
