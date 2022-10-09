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
#include "TCategory.h"
#include "TMovies.h"
#include "TTemp.h"

#include "CompactThread.h"

#include "../lkSQLite3/lkSQL3Field.h"
#include "../lkSQLite3/lkSQL3Exception.h"


// static definitions for setting the fields of new table
static sqlTblField_t t3CategoryFlds[] = {
	{ t3Category_catID,	 sqlFieldType::sqlInt,  true, false,   0 },		// (vereist)
	{ t3Category_subID,	 sqlFieldType::sqlInt,  true,  true, wxT("0") },// (vereist)
	{ t3Category_NAME,  sqlFieldType::sqlText,  true, false,   0 },		// (vereist)
	{ t3Category_IMG,   sqlFieldType::sqlBlob, false, false,   0 },		// containing (optional) image
//	{ t3Category_SORT,	 sqlFieldType::sqlInt,  true,  true, wxT("0") },// (vereist) -- sort order internally
//	{ t3Category_COUNT,	 sqlFieldType::sqlInt,  true,  true, wxT("0") },// (optional)
	{ 0 }
};

// Fields of Index-definitions
static sqlFldIndex_t t3CategoryIDX_1[] = { // Unique
	{ t3Category_catID, sqlSortOrder::sortAsc },
	{ t3Category_subID, sqlSortOrder::sortAsc },
	{ 0 }
};

static sqlFldIndex_t t3CategoryIDX_2[] = { // Unique
	{ t3Category_catID, sqlSortOrder::sortAsc },
	{ t3Category_NAME,  sqlSortOrder::sortAsc },
	{ 0 }
};

// static definitions for setting the index of new table
static sqlTblIndex_t t3CategoryIdx[] = {
	{ wxT("idxCategory_CatSub"),  t3CategoryIDX_1,  true }, // UNIQUE
	{ wxT("idxCategory_CatName"), t3CategoryIDX_2,  true }, // UNIQUE
	{ 0 }
};

// static definitions for creating a fresh new table
sqlTblDef_t TCategory::t3CategoryTbl = { t3Category, t3CategoryFlds, t3CategoryIdx };


wxIMPLEMENT_DYNAMIC_CLASS(TCategory, lkSQL3TableSet)

TCategory::TCategory() : lkSQL3TableSet(), m_sImageName()
{
	m_RowIdGrow = 25;
	Init();
}
TCategory::TCategory(lkSQL3Database* pDB) : lkSQL3TableSet(pDB)
{
	m_RowIdGrow = 25;
	Init();
}

void TCategory::Init()
{
	m_pFldCatID = NULL;
	m_pFldSubID = NULL;
	m_pFldName = NULL;
//	m_pFldSort = NULL;
//	m_pFldCount = NULL;
	m_pFldImage = NULL;

	m_sImageName.Empty();
}


// Operations

//virtual
bool TCategory::Open()
{
	lkInitTable msInit(&t3CategoryTbl);
	// fill Fields
	msInit.Append(new lkInitField(t3Category_catID, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldCatID));
	msInit.Append(new lkInitField(t3Category_subID, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldSubID));
	msInit.Append(new lkInitField(t3Category_NAME, wxCLASSINFO(lkSQL3FieldData_Text), (lkSQL3FieldData**)&m_pFldName));
	msInit.Append(new lkInitField(t3Category_IMG, wxCLASSINFO(lkSQL3FieldData_Image), (lkSQL3FieldData**)&m_pFldImage));
//	msInit.Append(new lkInitField(t3Category_SORT, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldSort));
//	msInit.Append(new lkInitField(t3Category_COUNT, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldCount));

	bool bRet = lkSQL3TableSet::Open(&msInit);

	if ( !bRet )
		Throw_InvalidTable(t3Category);

	m_bIsOpen = bRet;
	return bRet;
}


// Members
/////////////////////////////////////////

wxUint32 TCategory::GetCatValue() const
{
	wxASSERT(m_pFldCatID != NULL);
	wxUint32 val = (wxUint32)(m_pFldCatID->GetValue2());
	return val;
}
void TCategory::SetCatValue(wxUint32 _c)
{
	wxASSERT(m_pFldCatID != NULL);
	m_pFldCatID->SetValue2((wxInt64)_c);
}

wxUint32 TCategory::GetSubValue() const
{
	wxASSERT(m_pFldSubID != NULL);
	wxUint32 val = (wxUint32)(m_pFldSubID->GetValue2());
	return val;
}
void TCategory::SetSubValue(wxUint32 _s)
{
	wxASSERT(m_pFldSubID != NULL);
	m_pFldSubID->SetValue2((wxInt64)_s);
}

wxString TCategory::GetNameValue()
{
	wxASSERT(m_pFldName != NULL);
	return m_pFldName->GetValue2();
}
void TCategory::SetNameValue(const wxString& _n)
{
	wxASSERT(m_pFldName != NULL);
	m_pFldName->SetValue2(_n);
}
/*
bool TCategory::GetSortValue() const
{
	wxASSERT(m_pFldSort != NULL);
	bool val = m_pFldSort->Getbool();
	return val;
}
void TCategory::SetSortValue(bool _s)
{
	wxASSERT(m_pFldSort != NULL);
	m_pFldSort->Setbool(_s);
}
*/
wxImage TCategory::GetImage() const
{
	wxASSERT(m_pFldImage != NULL);
	wxString _name;
	wxImage img = m_pFldImage->GetAsImage(_name);

	*((wxString*)&m_sImageName) = (img.IsOk()) ? _name : wxT("");

	return img;
}
void TCategory::SetImage(const wxImage& _img, const wxString& _name)
{
	wxASSERT(m_pFldImage != NULL);
	m_sImageName = (m_pFldImage->SetFromImage(_img, _name)) ? _name : wxT("");
}

// returns the name (if any) retrieved from last GetImage() / SetImage(..)
wxString TCategory::GetImageName() const
{
	return m_sImageName;
}

/*
wxUint32 TCategory::GetCountValue() const
{
	wxASSERT(m_pFldCount != NULL);
	return (wxUint32)(m_pFldCount->GetValue2());
}
void TCategory::SetCountValue(wxUint32 _c)
{
	wxASSERT(m_pFldCount != NULL);
	m_pFldCount->SetValue2((wxInt64)_c);
}
*/

// Implementation

//virtual
bool TCategory::CanDelete()
{
	bool bRet = (IsOpen() && lkSQL3TableSet::CanDelete());
	if ( !bRet )
		return false;

	int nCat = GetCatValue();
	int nSub = GetSubValue();
	if ( nSub == 0 )
	{
		wxString qry;
		qry.Printf(wxT("SELECT COUNT(ROWID) FROM [%s] WHERE (([%s] = %d) AND ([%s] != 0));"), t3Category, t3Category_catID, nCat, t3Category_subID);
		try
		{
			bRet = (m_DB->ExecuteSQLAndGet(qry).GetInt() == 0);
		}
		catch ( const lkSQL3Exception& e )
		{
			((lkSQL3Exception*)&e)->ReportError();
			return false;
		}
	}

	if ( bRet )
		bRet = !(TMovies::HasCat(m_DB, nCat, nSub));

	return bRet;
}

//static
bool TCategory::VerifyTable(lkSQL3Database* pDB)
{
	return lkSQL3TableSet::VerifyTable(pDB, t3CategoryTbl);
}

//static
bool TCategory::CreateTable(lkSQL3Database* pDB)
{
	bool bRet = FALSE;

	if ( !pDB->TableExists(t3Category) )
		bRet = lkSQL3TableSet::CreateTable(pDB, t3CategoryTbl);

	return bRet;
}

//static
wxUint32 TCategory::GetFreeCatID(lkSQL3Database* pDB)
{ // when adding, get a free Cattegory ID
	if ( !pDB || !pDB->IsOpen() )
		return 0; // error

	wxString qry;
	qry = wxT("SELECT MAX([catID]) FROM tblCategory WHERE ([subID] = 0);");

	wxUint32 nMax = 0;
	try
	{
		nMax = (wxUint32)(pDB->ExecuteSQLAndGet(qry).GetInt());
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return 0;
	}

	return (nMax + 1);
}

//static
wxUint32 TCategory::GetFreeSubID(lkSQL3Database* pDB, wxUint32 nCat)
{ // when adding get a free Sub ID
	if ( !pDB || !pDB->IsOpen())
		return wxNOT_FOUND; // error

	wxString qry;
	qry.Printf(wxT("SELECT MAX([subID]) FROM tblCategory WHERE ([catID] = %u);"), nCat);

	wxUint32 nMax = 0;
	try
	{
		nMax = (wxUint32)(pDB->ExecuteSQLAndGet(qry).GetInt());
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return 0;
	}

	return (nMax + 1);
}

//static
bool TCategory::IsUniqueName(lkSQL3Database* _DB, const wxString& name, void* catID)
{
	if ( !_DB || !_DB->IsOpen())
		return false; // error

	wxUint32 cat = (catID) ? *((wxUint32*)catID) : 0;
	wxString qry, whr;
	if ( cat == 0 )
		whr.Printf(wxT("([%s] = 0)"), t3Category_subID); // only categories
	else
		whr.Printf(wxT("([%s] = %u) AND ([%s] > 0)"), t3Category_catID, cat, t3Category_subID); // by specific Category, only taking account for SubCats
	qry.Printf(wxT("SELECT COUNT(ROWID) FROM [%s] WHERE (%s AND ([%s] = '%s'));"), t3Category, whr, t3Category_NAME, name);

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
bool TCategory::HasImages(lkSQL3Database* _DB, wxUint32 catID, wxUint32 subID)
{
	if ( !_DB || !_DB->IsOpen() || (catID == 0) )
		return false; // error

	bool bRet = false;
	wxString qry, whr;
	if ( subID > 0 )
		whr.Printf(wxT("AND ([%s] = %u) "), t3Category_subID, subID);

	qry.Printf(wxT("SELECT COUNT([ROWID]) FROM [%s] WHERE (([%s] = %u) %sAND ([%s] NOT NULL));"), t3Category, t3Category_catID, catID, whr, t3Category_IMG);

	try
	{
		bRet = ((wxUint64)_DB->ExecuteSQLAndGet(qry) > 0);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return false;
	}

	return bRet;
}

/*
//static
void TCategory::CorrectRegistry(CSQL3Database* dbTmp)
{
	if ( !dbTmp || !dbTmp->IsOpen() ) return;

	INT32 nCat, oCat, nSub, oSub;
	if ( (oCat = regGet_Category()) != 0 )
	{
		nCat = CTCategoryTmp::FindNewCat(dbTmp, oCat, 0);
		if ( (nCat != 0) && (nCat != oCat) )
			regSet_Category(nCat);

		if ( (oSub = regGet_SubCat()) != 0 )
		{
			nSub = CTCategoryTmp::FindNewCat(dbTmp, oCat, oSub);
			if ( (nSub != 0) && (nSub != oSub) )
				regSet_SubCat(nSub);
		}
	}
}
*/

//static
bool TCategory::Compact(wxThread* thr, lkSQL3Database* dbSrc, lkSQL3Database* dbDest, lkSQL3Database* dbTmp) // wxThread required for sending messages to main-thread
{
	if ( !thr || !dbSrc || !dbDest || !dbTmp || !dbSrc->IsOpen() || !dbDest->IsOpen() || !dbTmp->IsOpen() )
		return false;

	wxString s;
	TCategory rsSrcCat(dbSrc), rsSrcSub(dbSrc), rsDest(dbDest);
	TTempCat rsTmp(dbTmp);

	wxUint64 cnt = 0;
	try
	{
		s.Printf(wxT("[%s] COLLATE lkUTF8compare"), t3Category_NAME);
		rsSrcCat.SetOrder(s);
		rsSrcCat.Open();
		cnt = rsSrcCat.GetRowCount();
		// From the sources, only select the Categories
		s.Printf(wxT("([%s] = 0)"), t3Category_subID);
		rsSrcCat.SetFilter(s);
		rsSrcCat.Requery();

		rsSrcSub.Open();

		rsDest.Open();
		rsTmp.Open(t3Category);

		(dynamic_cast<CompactThread*>(thr))->SendInfoRewriting(t3Category);
		(dynamic_cast<CompactThread*>(thr))->InitProgressCtrl(0, cnt);

		rsSrcCat.MoveFirst();

		wxString name;
		wxImage img;
		wxUint32 nNewCat, nOldCat, nNewSub, nOldSub;

		while ( !thr->TestDestroy() && !rsSrcCat.IsEOF() )
		{
			nOldCat = rsSrcCat.GetCatValue();
			nNewCat = TCategory::GetFreeCatID(dbDest);

			// Add this Category
			rsDest.AddNew();
			rsDest.SetCatValue(nNewCat);
			rsDest.SetSubValue(0);
			rsDest.SetNameValue(rsSrcCat.GetNameValue());
			if ( (img = rsSrcCat.GetImage()).IsOk() )
			{
				name = rsSrcCat.GetImageName();
				rsDest.SetImage(img, name);
			}
			rsDest.Update();

			// set in Tmp too
			rsTmp.AddNew();
			rsTmp.SetOldCat(nOldCat);
			rsTmp.SetNewCat(nNewCat);
			rsTmp.SetOldSub(0);
			rsTmp.SetNewSub(0);

			rsTmp.Update();
			(dynamic_cast<CompactThread*>(thr))->StepProgressCtrl(0);

			// any SubCategory's too
			s.Printf(wxT("([%s] = %d) AND ([%s] != 0)"), t3Category_catID, nOldCat, t3Category_subID);
			rsSrcSub.SetFilter(s);
			s.Printf(wxT("[%s] COLLATE lkUTF8compare"), t3Category_NAME);
			rsSrcSub.SetOrder(s);
			rsSrcSub.Requery();

			if ( !rsSrcSub.IsEmpty() )
			{
				rsSrcSub.MoveFirst();
				while ( !thr->TestDestroy() && !rsSrcSub.IsEOF() )
				{
					nOldSub = rsSrcSub.GetSubValue();
					nNewSub = TCategory::GetFreeSubID(dbDest, nNewCat);

					// Add this SubCategory
					rsDest.AddNew();
					rsDest.SetCatValue(nNewCat);
					rsDest.SetSubValue(nNewSub);
					rsDest.SetNameValue(rsSrcSub.GetNameValue());
					if ( (img = rsSrcSub.GetImage()).IsOk() )
					{
						name = rsSrcSub.GetImageName();
						rsDest.SetImage(img, name);
					}
					rsDest.Update();

					// set in Tmp too
					rsTmp.AddNew();
					rsTmp.SetOldCat(nOldCat);
					rsTmp.SetNewCat(nNewCat);
					rsTmp.SetOldSub(nOldSub);
					rsTmp.SetNewSub(nNewSub);

					rsTmp.Update();
					(dynamic_cast<CompactThread*>(thr))->StepProgressCtrl(0);

					rsSrcSub.MoveNext();
				}
			}

			rsSrcCat.MoveNext();
		}
	}
	catch ( const lkSQL3Exception& e )
	{
		wxString sql;
		sql.Printf(wxT("TCategory::Compact : %s"), ((lkSQL3Exception*)&e)->GetError());
		(dynamic_cast<CompactThread*>(thr))->SetErrString(sql);
		return false;
	}

	bool bOK = (!rsSrcCat.IsEmpty() && !rsDest.IsEmpty()) ? (rsDest.GetRowCount() == cnt) : (rsSrcCat.IsEmpty() && rsDest.IsEmpty());
	bOK = (bOK) ? !thr->TestDestroy() : bOK;
	return bOK;
}
