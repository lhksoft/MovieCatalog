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
#include "TQuality.h"
#include "TBase.h"
#include "TTemp.h"

#include "CompactThread.h"

#include "../lkControls/lkConfigTools.h"
#include "../lkSQLite3/lkSQL3Field.h"
#include "../lkSQLite3/lkSQL3Exception.h"

// static definitions for setting the fields of new table
static sqlTblField_t t3QualityFlds[] = {
	{ t3Quality_NAME, sqlFieldType::sqlText,  true, false, 0 }, // (vereist)
	{ t3Quality_IMG,  sqlFieldType::sqlBlob, false, false, 0 }, // containing image of rating
	{ 0 }
};

// static definitions for creating a fresh new table
static sqlTblDef_t t3QualityTbl = { t3Quality, t3QualityFlds, NULL };

// This is the default setting when adding a new record, value is ActualCurRow (wxUint64)
#define config_QUALITY_PATH			wxT("Qualities")
#define config_QUALITY_DEFAULT		wxT("Default")

wxIMPLEMENT_DYNAMIC_CLASS(TQuality, lkSQL3TableSet)

TQuality::TQuality() : lkSQL3TableSet()
{
	m_RowIdGrow = 5;
	Init();
}
TQuality::TQuality(lkSQL3Database* pDB) : lkSQL3TableSet(pDB)
{
	m_RowIdGrow = 5;
	Init();
}

void TQuality::Init()
{
	m_pFldQuality = NULL;
	m_pFldImage = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Operations

//virtual
bool TQuality::Open()
{
	lkInitTable msInit(&t3QualityTbl);
	// fill Fields
	msInit.Append(new lkInitField(t3Quality_NAME, wxCLASSINFO(lkSQL3FieldData_Text), (lkSQL3FieldData**)&m_pFldQuality));
	msInit.Append(new lkInitField(t3Quality_IMG, wxCLASSINFO(lkSQL3FieldData_Image), (lkSQL3FieldData**)&m_pFldImage));

	bool bRet = lkSQL3TableSet::Open(&msInit);

	if ( !bRet )
		Throw_InvalidTable(t3Quality);

	m_bIsOpen = bRet;
	return bRet;
}

//virtual
bool TQuality::CanDelete()
{
	bool bCan = (IsOpen() && lkSQL3TableSet::CanDelete());
	if (bCan )
		bCan = !(TBase::HasQuality(m_DB, GetActualCurRow()));
	return bCan;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Attributes

wxString TQuality::GetQualityValue() const
{
	wxASSERT(m_pFldQuality);
	return m_pFldQuality->GetValue2();
}
void TQuality::SetQualityValue(const wxString& _orig)
{
	wxASSERT(m_pFldQuality);
	m_pFldQuality->SetValue2(_orig);
}

wxImage TQuality::GetImage() const
{
	wxASSERT(m_pFldImage != NULL);
	wxString _name;
	wxImage img = m_pFldImage->GetAsImage(_name);

	*((wxString*)&m_sImageName) = (img.IsOk()) ? _name : wxT("");

	return img;
}
void TQuality::SetImage(const wxImage& _img, const wxString& _name)
{
	wxASSERT(m_pFldImage != NULL);
	m_sImageName = (m_pFldImage->SetFromImage(_img, _name)) ? _name : wxT("");
}
wxString TQuality::GetImageName() const
{
	return m_sImageName;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation

//static
bool TQuality::VerifyTable(lkSQL3Database* pDB)
{
	return lkSQL3TableSet::VerifyTable(pDB, t3QualityTbl);
}

//static
bool TQuality::CreateTable(lkSQL3Database* pDB)
{
	bool bRet = false;

	if ( !pDB->TableExists(t3Quality) )
		bRet = lkSQL3TableSet::CreateTable(pDB, t3QualityTbl);

	return bRet;
}

//static
bool TQuality::IsUniqueQuality(lkSQL3Database* _DB, const wxString& orig, void*)
{
	bool ret = false;
	try
	{
		ret = lkSQL3TableSet::IsUniqueTextValue(_DB, t3Quality, t3Quality_NAME, orig);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return false;
	}

	return ret;
}

//static
void TQuality::SetDefaultQuality(wxUint64 q)
{
	if ( q == 0 )
		DeleteKey(config_QUALITY_PATH, config_QUALITY_DEFAULT);
	else
		SetConfigInt(config_QUALITY_PATH, config_QUALITY_DEFAULT, q);

}
//static
wxUint64 TQuality::GetDefaultQuality(lkSQL3Database* pDB)
{
	wxUint64 defRow = GetConfigInt(config_QUALITY_PATH, config_QUALITY_DEFAULT);

	if ( (defRow > 0) && pDB && pDB->IsOpen() )
	{
		wxString qry;
		qry.Printf(wxT("SELECT COUNT([ROWID]) FROM [%s] WHERE ([ROWID] = %I64u);"), t3Quality, defRow);
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
void TQuality::CorrectConfig(lkSQL3Database* dbTmp)
{
	if ( !dbTmp || !dbTmp->IsOpen() )
		return;

	wxUint64 nNew = 0, nOld = GetConfigInt(config_QUALITY_PATH, config_QUALITY_DEFAULT);
	if ( nOld > 0 )
		nNew = TTemp::FindNewID(dbTmp, t3Quality, nOld);
	TQuality::SetDefaultQuality(nNew);
}

//static
bool TQuality::Compact(wxThread* thr, lkSQL3Database* dbSrc, lkSQL3Database* dbDest, lkSQL3Database* dbTmp) // wxThread required for sending messages to main-thread
{
	if ( !thr || !dbSrc || !dbDest || !dbTmp || !dbSrc->IsOpen() || !dbDest->IsOpen() || !dbTmp->IsOpen() )
		return false;

	wxString name;
	wxImage img;
	TQuality rsSrc(dbSrc), rsDest(dbDest);
	TTemp rsTmp(dbTmp);
	rsSrc.SetOrder(wxT("[ROWID]"));
	try
	{
		rsSrc.Open();
		rsDest.Open();
		rsTmp.Open(t3Quality);

		(dynamic_cast<CompactThread*>(thr))->SendInfoRewriting(t3Quality);
		(dynamic_cast<CompactThread*>(thr))->InitProgressCtrl(0, rsSrc.GetRowCount());

		rsSrc.MoveFirst();

		while ( !thr->TestDestroy() && !rsSrc.IsEOF() )
		{
			rsDest.AddNew();

			// Label
			rsDest.SetQualityValue(rsSrc.GetQualityValue());
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
		sql.Printf(wxT("TQuality::Compact : %s"), ((lkSQL3Exception*)&e)->GetError());
		(dynamic_cast<CompactThread*>(thr))->SetErrString(sql);
		return false;
	}

	bool bOK = (!rsSrc.IsEmpty() && !rsDest.IsEmpty()) ? (rsSrc.GetRowCount() == rsDest.GetRowCount()) : (rsSrc.IsEmpty() && rsDest.IsEmpty());
	bOK = (bOK) ? !thr->TestDestroy() : bOK;
	return bOK;
}
