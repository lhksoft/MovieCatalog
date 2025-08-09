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
#include "TRating.h"
#include "TMovies.h"

#include "CompactThread.h"

#include "../lkSQLite3/lkSQL3Field.h"
#include "../lkSQLite3/lkSQL3Exception.h"

// static definitions for setting the fields of new table
static sqlTblField_t t3RatingFlds[] = {
	{ t3Rating_ID,     sqlFieldType::sqlInt,  true, false,   0, 1, 255 }, // Label (vereist)
	{ t3Rating_NAME,  sqlFieldType::sqlText,  true, false,   0 }, // Korte Naam (vereist)
	{ t3Rating_DESCR, sqlFieldType::sqlText,  true, false,   0 }, // Langere Naam (vereist)
	{ t3Rating_IMAGE, sqlFieldType::sqlBlob, false, false,   0 }, // containing image of rating
	{ 0 }
};

// Fields of Index-definitions
static sqlFldIndex_t t3RatingIDX_1[] = {
	{ t3Rating_ID, sqlSortOrder::sortAsc },
	{ 0 }
};

// static definitions for setting the index of new table
static sqlTblIndex_t t3RatingIdx[] = {
	{ wxT("idxRating_Flag"), t3RatingIDX_1, true }, // UNIQUE
	{ 0 }
};

// static definitions for creating a fresh new table
static sqlTblDef_t t3RatingTbl = { t3Rating, t3RatingFlds, t3RatingIdx };


wxIMPLEMENT_DYNAMIC_CLASS(TRating, lkSQL3TableSet)

TRating::TRating() : lkSQL3TableSet(), m_sImageName()
{
	m_RowIdGrow = 5;
	Init();
}
TRating::TRating(lkSQL3Database* pDB) : lkSQL3TableSet(pDB), m_sImageName()
{
	m_RowIdGrow = 5;
	Init();
}

void TRating::Init()
{
	m_pFldFlag = NULL;
	m_pFldRating = NULL;
	m_pFldDescription = NULL;
	m_pFldImage = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Operations

//virtual
bool TRating::Open()
{
	lkInitTable msInit(&t3RatingTbl);
	// fill Fields
	msInit.Append(new lkInitField(t3Rating_ID, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldFlag));
	msInit.Append(new lkInitField(t3Rating_NAME, wxCLASSINFO(lkSQL3FieldData_Text), (lkSQL3FieldData**)&m_pFldRating));
	msInit.Append(new lkInitField(t3Rating_DESCR, wxCLASSINFO(lkSQL3FieldData_Text), (lkSQL3FieldData**)&m_pFldDescription));
	msInit.Append(new lkInitField(t3Rating_IMAGE, wxCLASSINFO(lkSQL3FieldData_Image), (lkSQL3FieldData**)&m_pFldImage));

	bool bRet = lkSQL3TableSet::Open(&msInit);

	if ( !bRet )
		Throw_InvalidTable(t3Rating);

	m_bIsOpen = bRet;
	return bRet;
}

//virtual
bool TRating::CanDelete()
{
	bool bCan = (IsOpen() && lkSQL3TableSet::CanDelete());
	return (bCan && !(TMovies::HasRating(m_DB, GetFlagValue())));
}

//virtual
bool TRating::CanAdd()
{
	wxASSERT(m_DB && m_DB->IsOpen());
	bool bCan = (IsOpen()) ? lkSQL3TableSet::CanAdd() : false;
	if ( bCan )
	{
		wxString qry;
		qry.Printf(wxT("SELECT COUNT(ROWID) FROM [%s];"), t3Rating);
		try
		{
			bCan = (m_DB->ExecuteSQLAndGet(qry).GetInt() < 255);
		}
		catch ( const lkSQL3Exception& e )
		{
			((lkSQL3Exception*)&e)->ReportError();
			return false;
		}
	}
	return bCan;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
// Attributes

wxUint8 TRating::GetFlagValue() const
{
	wxASSERT(m_pFldFlag);
	return (wxUint8)m_pFldFlag->GetValue2();
}
void TRating::SetFlagValue(wxUint8 _b)
{ // null would be allowed, in which case default will be set (0)
	wxASSERT(m_pFldFlag);
	m_pFldFlag->SetValue2((wxInt64)_b);
}

wxString TRating::GetRatingValue() const
{
	wxASSERT(m_pFldRating);
	return m_pFldRating->GetValue2();
}
void TRating::SetRatingValue(const wxString& _r)
{
	wxASSERT(m_pFldRating);
	m_pFldRating->SetValue2(_r);
}

wxString TRating::GetDescriptValue() const
{
	wxASSERT(m_pFldDescription);
	return m_pFldDescription->GetValue2();
}
void TRating::SetDescriptValue(const wxString& _d)
{
	wxASSERT(m_pFldDescription);
	m_pFldDescription->SetValue2(_d);
}

wxImage TRating::GetImage() const
{
	wxASSERT(m_pFldImage != NULL);
	wxString _name;
	wxImage img = m_pFldImage->GetAsImage(_name);

	*((wxString*)&m_sImageName) = (img.IsOk()) ? _name : wxT("");

	return img;
}
void TRating::SetImage(const wxImage& _img, const wxString& _name)
{
	wxASSERT(m_pFldImage != NULL);
	m_sImageName = (m_pFldImage->SetFromImage(_img, _name)) ? _name : wxT("");
}
wxString TRating::GetImageName() const
{
	return m_sImageName;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation

//static
bool TRating::VerifyTable(lkSQL3Database* pDB)
{
	return lkSQL3TableSet::VerifyTable(pDB, t3RatingTbl);
}

//static
bool TRating::CreateTable(lkSQL3Database* pDB)
{
	bool bRet = false;

	if ( !pDB->TableExists(t3Rating) )
		bRet = lkSQL3TableSet::CreateTable(pDB, t3RatingTbl);

	return bRet;
}

//static
bool TRating::IsUniqueFlag(lkSQL3Database* _DB, const wxString& _val, void*)
{
	if ( !_DB || !_DB->IsOpen() ) return false;
	wxUint64 val;
	if ( _val.IsEmpty() || !_val.ToULongLong(&val) )
		val = 0;
	if ( val == 0 )
		return false;

	wxString qry;
	qry.Printf(wxT("SELECT COUNT(ROWID) FROM [%s] WHERE ([%s] = %llu);"), t3Rating, t3Rating_ID, val);

	int ret = 0;
	try
	{
		ret = _DB->ExecuteSQLAndGet(qry);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return false;
	}

	return (ret == 0);
}

//static
bool TRating::IsUniqueRating(lkSQL3Database* _DB, const wxString& name, void*)
{
	if ( !_DB || !_DB->IsOpen() ) return false;

	wxString qry;
	qry.Printf(wxT("SELECT COUNT(ROWID) FROM [%s] WHERE ([%s] = '%s');"), t3Rating, t3Rating_NAME, name);

	int ret = 0;
	try
	{
		ret = _DB->ExecuteSQLAndGet(qry);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return false;
	}

	return (ret == 0);
}

//static
bool TRating::Compact(wxThread* thr, lkSQL3Database* dbSrc, lkSQL3Database* dbDest) // wxThread required for sending messages to main-thread
{
	if ( !thr || !dbSrc || !dbDest || !dbSrc->IsOpen() || !dbDest->IsOpen() )
		return false;

	wxString s;
	TRating rsSrc(dbSrc), rsDest(dbDest);
	s.Printf(wxT("[%s]"), t3Rating_ID);
	rsSrc.SetOrder(s);
	try
	{
		rsSrc.Open();
		rsDest.Open();

		(dynamic_cast<CompactThread*>(thr))->SendInfoRewriting(t3Rating);
		(dynamic_cast<CompactThread*>(thr))->InitProgressCtrl(0, rsSrc.GetRowCount());

		rsSrc.MoveFirst();

		wxString name;
		wxImage img;
		while ( !thr->TestDestroy() && !rsSrc.IsEOF() )
		{
			rsDest.AddNew();
			// ID
			rsDest.SetFlagValue(rsSrc.GetFlagValue());
			// Rating
			rsDest.SetRatingValue(rsSrc.GetRatingValue());
			// Description
			rsDest.SetDescriptValue(rsSrc.GetDescriptValue());
			// Image -- return buffer is CONST, don't release it yourself !
			if ( (img = rsSrc.GetImage()).IsOk() )
			{
				name = rsSrc.GetImageName();
				rsDest.SetImage(img, name);
			}
			rsDest.Update();

			rsSrc.MoveNext();
			(dynamic_cast<CompactThread*>(thr))->StepProgressCtrl(0);
		}
	}
	catch ( const lkSQL3Exception& e )
	{
		wxString sql;
		sql.Printf(wxT("TRating::Compact : %s"), ((lkSQL3Exception*)&e)->GetError());
		(dynamic_cast<CompactThread*>(thr))->SetErrString(sql);
		return false;
	}

	bool bOK = (!rsSrc.IsEmpty() && !rsDest.IsEmpty()) ? (rsSrc.GetRowCount() == rsDest.GetRowCount()) : (rsSrc.IsEmpty() && rsDest.IsEmpty());
	bOK = (bOK) ? !thr->TestDestroy() : bOK;
	return bOK;
}
