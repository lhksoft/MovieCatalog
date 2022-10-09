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
#include "TJudge.h"
#include "TMovies.h"
#include "TTemp.h"

#include "CompactThread.h"

#include "../lkSQLite3/lkSQL3Field.h"
#include "../lkSQLite3/lkSQL3Exception.h"

// static definitions for setting the fields of new table
static sqlTblField_t t3JudgeFlds[] = {
	{ t3Judge_NAME, sqlFieldType::sqlText,  true, false, 0 }, // Label (vereist)
	{ t3Judge_IMG,  sqlFieldType::sqlBlob, false, false, 0 }, // containing image of judgement
	{ 0 }
};

// static definitions for creating a fresh new table
static sqlTblDef_t t3JudgeTbl = { t3Judge, t3JudgeFlds, 0 };

wxIMPLEMENT_DYNAMIC_CLASS(TJudge, lkSQL3TableSet)

TJudge::TJudge() : lkSQL3TableSet(), m_sImageName()
{
	m_RowIdGrow = 5;
	Init();
}
TJudge::TJudge(lkSQL3Database* pDB) : lkSQL3TableSet(pDB), m_sImageName()
{
	m_RowIdGrow = 5;
	Init();
}

void TJudge::Init()
{
	m_pFldJudge = NULL;
	m_pFldImage = NULL;
}

//virtual
bool TJudge::Open()
{
	lkInitTable msInit(&t3JudgeTbl);
	// fill Fields
	msInit.Append(new lkInitField(t3Judge_NAME, wxCLASSINFO(lkSQL3FieldData_Text), (lkSQL3FieldData**)&m_pFldJudge));
	msInit.Append(new lkInitField(t3Judge_IMG, wxCLASSINFO(lkSQL3FieldData_Image), (lkSQL3FieldData**)&m_pFldImage));

	bool bRet = lkSQL3TableSet::Open(&msInit);

	if ( !bRet )
		Throw_InvalidTable(t3Judge);

	m_bIsOpen = bRet;
	return bRet;
}

//virtual
bool TJudge::CanDelete()
{
	bool bCan = (IsOpen() && lkSQL3TableSet::CanDelete());
	return (bCan && !(TMovies::HasJudge(m_DB, GetActualCurRow())));
}

wxString TJudge::GetJudgeValue() const
{
	wxASSERT(m_pFldJudge);
	return m_pFldJudge->GetValue2();
}
void TJudge::SetJudgeValue(const wxString& _j) // NULL not allowed
{
	wxASSERT(m_pFldJudge);
	m_pFldJudge->SetValue2(_j);
}

wxImage TJudge::GetImage() const
{
	wxASSERT(m_pFldImage != NULL);
	wxString _name;
	wxImage img = m_pFldImage->GetAsImage(_name);

	*((wxString*)&m_sImageName) = (img.IsOk()) ? _name : wxT("");

	return img;
}
void TJudge::SetImage(const wxImage& _img, const wxString& _name)
{
	wxASSERT(m_pFldImage != NULL);
	m_sImageName = (m_pFldImage->SetFromImage(_img, _name)) ? _name : wxT("");
}
wxString TJudge::GetImageName() const
{
	return m_sImageName;
}

//static
bool TJudge::VerifyTable(lkSQL3Database* pDB)
{
	return lkSQL3TableSet::VerifyTable(pDB, t3JudgeTbl);
}

//static
bool TJudge::CreateTable(lkSQL3Database* pDB)
{
	bool bRet = false;

	if ( !pDB->TableExists(t3Judge) )
		bRet = lkSQL3TableSet::CreateTable(pDB, t3JudgeTbl);

	return bRet;
}

//static
bool TJudge::IsUniqueJudge(lkSQL3Database* _DB, const wxString& judge, void*)
{
	bool ret = false;
	try
	{
		ret = lkSQL3TableSet::IsUniqueTextValue(_DB, t3Judge, t3Judge_NAME, judge);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return false;
	}

	return ret;
}

//static
bool TJudge::Compact(wxThread* thr, lkSQL3Database* dbSrc, lkSQL3Database* dbDest, lkSQL3Database* dbTmp) // wxThread required for sending messages to main-thread
{
	if ( !thr || !dbSrc || !dbDest || !dbTmp || !dbSrc->IsOpen() || !dbDest->IsOpen() || !dbTmp->IsOpen() )
		return false;

	wxString s;
	TJudge rsSrc(dbSrc), rsDest(dbDest);
	TTemp rsTmp(dbTmp);
	rsSrc.SetOrder(wxT("[ROWID]"));
	try
	{
		rsSrc.Open();
		rsDest.Open();
		rsTmp.Open(t3Judge);

		(dynamic_cast<CompactThread*>(thr))->SendInfoRewriting(t3Judge);
		(dynamic_cast<CompactThread*>(thr))->InitProgressCtrl(0, rsSrc.GetRowCount());

		rsSrc.MoveFirst();

		wxString name;
		wxImage img;

		while ( !thr->TestDestroy() && !rsSrc.IsEOF() )
		{
			rsDest.AddNew();

			// Judge
			rsDest.SetJudgeValue(rsSrc.GetJudgeValue());
			// Image -- return buffer is CONST, don't release it yourself !
			if ( (img = rsSrc.GetImage()).IsOk() )
			{
				name = rsSrc.GetImageName();
				rsDest.SetImage(img, name);
			}

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
		sql.Printf(wxT("TJudge::Compact : %s"), ((lkSQL3Exception*)&e)->GetError());
		(dynamic_cast<CompactThread*>(thr))->SetErrString(sql);
		return false;
	}

	bool bOK = (!rsSrc.IsEmpty() && !rsDest.IsEmpty()) ? (rsSrc.GetRowCount() == rsDest.GetRowCount()) : (rsSrc.IsEmpty() && rsDest.IsEmpty());
	bOK = (bOK) ? !thr->TestDestroy() : bOK;
	return bOK;
}
