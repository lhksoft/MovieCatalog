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
#include "TGenre.h"
#include "TMovies.h"
#include "TTemp.h"

#include "CompactThread.h"

#include "../lkSQLite3/lkSQL3Field.h"
#include "../lkSQLite3/lkSQL3Exception.h"

// static definitions for setting the fields of new table
static sqlTblField_t t3GenreFlds[] = {
	{ t3Genre_FLAG,	   sqlFieldType::sqlInt,  true, false,   0 },	// UINT64 (vereist)
	{ t3Genre_GENRE,  sqlFieldType::sqlText,  true, false,   0 },	// Genre (vereist)
	{ t3Genre_IMG,	  sqlFieldType::sqlBlob, false, false,   0 },	// containing image of genre
	{ 0 }
};

// Fields of Index-definitions
static sqlFldIndex_t t3GenreIDX_1[] = {
	{ t3Genre_GENRE, sqlSortOrder::sortAsc },
	{ 0 }
};

// static definitions for setting the index of new table
static sqlTblIndex_t t3GenreIdx[] = {
	{ wxT("idxGenre_Genre"),   t3GenreIDX_1, true }, // UNIQUE
	{ 0 }
};

// static definitions for creating a fresh new table
static sqlTblDef_t t3GenreTbl = { t3Genre, t3GenreFlds, t3GenreIdx };

wxIMPLEMENT_DYNAMIC_CLASS(TGenre, lkSQL3TableSet)

TGenre::TGenre() : lkSQL3TableSet(), m_sImageName()
{
	m_RowIdGrow = 32;
	Init();
}
TGenre::TGenre(lkSQL3Database* pDB) : lkSQL3TableSet(pDB), m_sImageName()
{
	m_RowIdGrow = 32;
	Init();
}

void TGenre::Init()
{
	m_pFldFlag = NULL;
	m_pFldGenre = NULL;
	m_pFldImage = NULL;
}

//virtual
bool TGenre::Open()
{
	lkInitTable msInit(&t3GenreTbl);
	// fill Fields
	msInit.Append(new lkInitField(t3Genre_FLAG, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldFlag));
	msInit.Append(new lkInitField(t3Genre_GENRE, wxCLASSINFO(lkSQL3FieldData_Text), (lkSQL3FieldData**)&m_pFldGenre));
	msInit.Append(new lkInitField(t3Genre_IMG, wxCLASSINFO(lkSQL3FieldData_Image), (lkSQL3FieldData**)&m_pFldImage));

	bool bRet = lkSQL3TableSet::Open(&msInit);

	if ( !bRet )
		Throw_InvalidTable(t3Genre);

	m_bIsOpen = bRet;
	return bRet;
}

//virtual
bool TGenre::CanDelete()
{
	bool bCan = (IsOpen() && lkSQL3TableSet::CanDelete());
	return (bCan && !(TMovies::HasGenre(m_DB, GetFlagValue())));
}

//virtual
bool TGenre::CanAdd()
{
	return (lkSQL3TableSet::CanAdd() && (m_RowCount < 63));
}

//virtual
bool TGenre::AddData()
{
	wxASSERT(m_RowCount < 63);
	if ( !m_bAdd ) return false; // nothing to do
	if ( m_RowCount >= 63 )
		return false;

	// find first available free genre=flag
	wxUint64 u = TGenre::FindFreeID64(m_DB);
	if ( u == 0 )
		return false; // no good

	SetFlagValue(u);
	// continue ADDING
	return lkSQL3TableSet::AddData();
}


// Members

wxUint64 TGenre::GetFlagValue() const
{
	wxASSERT(m_pFldFlag);
	wxUint64 val = (wxUint64)(m_pFldFlag->GetValue2());
	return val;
}
void TGenre::SetFlagValue(wxUint64 _f) // NULL not allowed
{
	wxASSERT(m_pFldFlag);
	m_pFldFlag->SetValue2((wxInt64)_f);
}

wxString TGenre::GetGenreValue() const
{
	wxASSERT(m_pFldGenre);
	return m_pFldGenre->GetValue2();
}
void TGenre::SetGenreValue(const wxString& _g) // NULL not allowed
{
	wxASSERT(m_pFldGenre);
	m_pFldGenre->SetValue2(_g);
}

wxImage TGenre::GetImage() const
{
	wxASSERT(m_pFldImage != NULL);
	wxString _name;
	wxImage img = m_pFldImage->GetAsImage(_name);

	*((wxString*)&m_sImageName) = (img.IsOk()) ? _name : wxT("");

	return img;
}
void TGenre::SetImage(const wxImage& _img, const wxString& _name)
{
	wxASSERT(m_pFldImage != NULL);
	m_sImageName = (m_pFldImage->SetFromImage(_img, _name)) ? _name : wxT("");
}
wxString TGenre::GetImageName() const
{
	return m_sImageName;
}


//static
bool TGenre::VerifyTable(lkSQL3Database* pDB)
{
	return lkSQL3TableSet::VerifyTable(pDB, t3GenreTbl);
}

//static
bool TGenre::CreateTable(lkSQL3Database* pDB)
{
	bool bRet = false;

	if ( !pDB->TableExists(t3Genre) )
		bRet = lkSQL3TableSet::CreateTable(pDB, t3GenreTbl);

	return bRet;
}

//static 
wxUint64 TGenre::FindFreeID64(lkSQL3Database* _DB)
{
	if ( !_DB || !_DB->IsOpen() )
		return 0;
	// find first available free genre=flag
	wxString sql;
	wxUint64 u = 1;
	int i, c;
	bool bRet = true;
	for ( i = 1; i < 63; i++ )
	{
		sql.Printf(wxT("SELECT COUNT([ROWID]) FROM [%s] WHERE (([%s] & %I64u) = %I64u);"), t3Genre, t3Genre_FLAG, u, u);
		try
		{
			c = _DB->ExecuteSQLAndGet(sql).GetInt();
		}
		catch ( const lkSQL3Exception& e )
		{
			((lkSQL3Exception*)&e)->ReportError();
			return false;
		}
		if ( c == 0 )
			break; // got a free ID
				   //else
		u <<= 1;
	}
	if ( i >= 63 )
		u = 0; // no good

	return u;
}

//static
bool TGenre::IsUniqueGenre(lkSQL3Database* _DB, const wxString& genre, void*)
{
	bool ret = false;
	try
	{
		ret = lkSQL3TableSet::IsUniqueTextValue(_DB, t3Genre, t3Genre_GENRE, genre);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return false;
	}

	return ret;
}

//static
bool TGenre::Compact(wxThread* thr, lkSQL3Database* dbSrc, lkSQL3Database* dbDest, lkSQL3Database* dbTmp) // wxThread required for sending messages to main-thread
{
	if ( !thr || !dbSrc || !dbDest || !dbTmp || !dbSrc->IsOpen() || !dbDest->IsOpen() || !dbTmp->IsOpen() )
		return false;

	wxString s;
	TGenre rsSrc(dbSrc), rsDest(dbDest);
	TTemp rsTmp(dbTmp);
	s.Printf(wxT("[%s] COLLATE lkUTF8compare"), t3Genre_GENRE);
	rsSrc.SetOrder(s);
	try
	{
		rsSrc.Open();
		rsDest.Open();
		rsTmp.Open(t3Genre);

		(dynamic_cast<CompactThread*>(thr))->SendInfoRewriting(t3Genre);
		(dynamic_cast<CompactThread*>(thr))->InitProgressCtrl(0, rsSrc.GetRowCount());

		rsSrc.MoveFirst();

		wxString name;
		wxImage img;

		wxUint64 nOld, nNew;
		while ( !thr->TestDestroy() && !rsSrc.IsEOF() )
		{
			rsDest.AddNew();
			// Flag
			nNew = TGenre::FindFreeID64(dbDest);
			if ( nNew == 0 ) break;
			nOld = rsSrc.GetFlagValue();
			rsDest.SetFlagValue(nNew);
			// Genre
			rsDest.SetGenreValue(rsSrc.GetGenreValue());
			// Image -- return buffer is CONST, don't release it yourself !
			if ( (img = rsSrc.GetImage()).IsOk() )
			{
				name = rsSrc.GetImageName();
				rsDest.SetImage(img, name);
			}
			rsDest.Update();

			rsTmp.AddNew();
			rsTmp.SetOld(nOld);
			rsTmp.SetNew(nNew);
			rsTmp.Update();

			rsSrc.MoveNext();
			(dynamic_cast<CompactThread*>(thr))->StepProgressCtrl(0);
		}
	}
	catch ( const lkSQL3Exception& e )
	{
		wxString sql;
		sql.Printf(wxT("TGenre::Compact : %s"), ((lkSQL3Exception*)&e)->GetError());
		(dynamic_cast<CompactThread*>(thr))->SetErrString(sql);
		return false;
	}

	bool bOK = (!rsSrc.IsEmpty() && !rsDest.IsEmpty()) ? (rsSrc.GetRowCount() == rsDest.GetRowCount()) : (rsSrc.IsEmpty() && rsDest.IsEmpty());
	bOK = (bOK) ? !thr->TestDestroy() : bOK;
	return bOK;
}
