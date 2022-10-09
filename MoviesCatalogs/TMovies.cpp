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
#include "TMovies.h"
#include "TBase.h"
#include "TCategory.h"	// required in Compact, needs table-name
#include "TGenre.h"		// required in Compact, needs table-name
#include "TJudge.h"		// required in Compact, needs table-name
#include "TTemp.h"

#include "CompactThread.h"

#include "../lkSQLite3/lkSQL3Field.h"
#include "../lkSQLite3/lkSQL3Exception.h"

// static definitions for setting the fields of new table
static sqlTblField_t t3MoviesFlds[] = {
	{ t3Movies_TITLE,    sqlFieldType::sqlText,  true, false,  0 },
	{ t3Movies_SUBTITLE, sqlFieldType::sqlText, false, false,  0 }, // not required, if empty should be NULL
	{ t3Movies_AKATITLE, sqlFieldType::sqlText, false, false,  0 }, // idem
	{ t3Movies_EPISODE,   sqlFieldType::sqlInt,  true,  true, wxT("0") },
	{ t3Movies_CAT,       sqlFieldType::sqlInt,  true,  true, wxT("0") }, // required, links with TCategory::catID
	{ t3Movies_SUBCAT,    sqlFieldType::sqlInt,  true,  true, wxT("0") }, // required, links with TCategory::subID
	{ t3Movies_GENRE,     sqlFieldType::sqlInt,  true,  true, wxT("0") }, // UINT64 -- required, links with TGenre::ID
	{ t3Movies_JUDGE,     sqlFieldType::sqlInt,  true,  true, wxT("0") }, // required, links with TJudge::ID
	{ t3Movies_RATE,      sqlFieldType::sqlInt,  true,  true, wxT("0") }, // required, links with TRating::ID
	{ t3Movies_COUNTRY,  sqlFieldType::sqlText, false, false,  0 }, // combination of TCountry::Code/ISO, sep by ';'
	{ t3Movies_DATE,     sqlFieldType::sqlReal, false, false,  0 }, // date of Issue
	{ t3Movies_YEAR,      sqlFieldType::sqlInt,  true,  true, wxT("1") }, // true if date is YEAR only
	{ t3Movies_SERIE,     sqlFieldType::sqlInt,  true,  true, wxT("0") }, // true if is a serie, false otherwise
	{ t3Movies_URL,      sqlFieldType::sqlText, false, false,  0 }, // URL to online information (eg. IMdb)
	{ t3Movies_COVER,    sqlFieldType::sqlText, false, false,  0 }, // Full path to cover
	{ t3Movies_INFO,     sqlFieldType::sqlText, false, false,  0 }, // Description
	{ 0 }
};

// Fields of Index-definitions
static sqlFldIndex_t t3MoviesIDX_1[] = {
	{ t3Movies_TITLE,   sqlSortOrder::sortAsc },
	{ 0 }
};
static sqlFldIndex_t t3MoviesIDX_2[] = {
	{ t3Movies_EPISODE, sqlSortOrder::sortAsc },
	{ 0 }
};
static sqlFldIndex_t t3MoviesIDX_3[] = {
	{ t3Movies_CAT,     sqlSortOrder::sortAsc },
	{ t3Movies_SUBCAT,  sqlSortOrder::sortAsc },
	{ 0 }
};
static sqlFldIndex_t t3MoviesIDX_4[] = {
	{ t3Movies_SERIE,   sqlSortOrder::sortAsc },
	{ 0 }
};

// static definitions for setting the index of new table
static sqlTblIndex_t t3MoviesIdx[] = {
	{ wxT("idxMovies_Title"),    t3MoviesIDX_1, false }, // not UNIQUE
	{ wxT("idxMovies_Episode"),  t3MoviesIDX_2, false }, // not UNIQUE
	{ wxT("idxMovies_Category"), t3MoviesIDX_3, false }, // not UNIQUE
	{ wxT("idxMovies_Serie"),    t3MoviesIDX_4, false }, // not UNIQUE
	{ 0 }
};

// static definitions for creating a fresh new table
static sqlTblDef_t t3MoviesTbl = { t3Movies, t3MoviesFlds, t3MoviesIdx };


///////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_DYNAMIC_CLASS(TMovies, lkSQL3TableSet)

TMovies::TMovies() : lkSQL3TableSet(),
	m_sFltFilterPrev(), m_sFltOrderPrev()
{
	Init();
}
TMovies::TMovies(lkSQL3Database* pDB) : lkSQL3TableSet(pDB),
	m_sFltFilterPrev(), m_sFltOrderPrev()
{
	Init();
}

void TMovies::Init()
{
	m_pFldTitle = NULL;
	m_pFldSubTtl = NULL;
	m_pFldAlias = NULL;
	m_pFldEp = NULL;
	m_pFldCat = NULL;
	m_pFldSub = NULL;
	m_pFldGenre = NULL;
	m_pFldJudge = NULL;
	m_pFldRating = NULL;
	m_pFldCountry = NULL;
	m_pFldDate = NULL;
	m_pFldYear = NULL;
	m_pFldSerie = NULL;
	m_pFldURL = NULL;
	m_pFldCover = NULL;
	m_pFldInfo = NULL;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
// Operations
////

//virtual
bool TMovies::Open()
{
	lkInitTable msInit(&t3MoviesTbl);
	// fill Fields
	msInit.Append(new lkInitField(t3Movies_TITLE, wxCLASSINFO(lkSQL3FieldData_Text), (lkSQL3FieldData**)&m_pFldTitle));
	msInit.Append(new lkInitField(t3Movies_SUBTITLE, wxCLASSINFO(lkSQL3FieldData_Text), (lkSQL3FieldData**)&m_pFldSubTtl));
	msInit.Append(new lkInitField(t3Movies_AKATITLE, wxCLASSINFO(lkSQL3FieldData_Text), (lkSQL3FieldData**)&m_pFldAlias));
	msInit.Append(new lkInitField(t3Movies_EPISODE, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldEp));
	msInit.Append(new lkInitField(t3Movies_CAT, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldCat));
	msInit.Append(new lkInitField(t3Movies_SUBCAT, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldSub));
	msInit.Append(new lkInitField(t3Movies_GENRE, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldGenre));
	msInit.Append(new lkInitField(t3Movies_JUDGE, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldJudge));
	msInit.Append(new lkInitField(t3Movies_RATE, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldRating));
	msInit.Append(new lkInitField(t3Movies_COUNTRY, wxCLASSINFO(lkSQL3FieldData_Text), (lkSQL3FieldData**)&m_pFldCountry));
	msInit.Append(new lkInitField(t3Movies_DATE, wxCLASSINFO(lkSQL3FieldData_Real), (lkSQL3FieldData**)&m_pFldDate));
	msInit.Append(new lkInitField(t3Movies_YEAR, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldYear));
	msInit.Append(new lkInitField(t3Movies_SERIE, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldSerie));
	msInit.Append(new lkInitField(t3Movies_URL, wxCLASSINFO(lkSQL3FieldData_Text), (lkSQL3FieldData**)&m_pFldURL));
	msInit.Append(new lkInitField(t3Movies_COVER, wxCLASSINFO(lkSQL3FieldData_Text), (lkSQL3FieldData**)&m_pFldCover));
	msInit.Append(new lkInitField(t3Movies_INFO, wxCLASSINFO(lkSQL3FieldData_Text), (lkSQL3FieldData**)&m_pFldInfo));

	bool bRet = lkSQL3TableSet::Open(&msInit);

	if ( !bRet )
		Throw_InvalidTable(t3Movies);

	m_bIsOpen = bRet;
	return bRet;
}

//virtual
bool TMovies::CanDelete()
{
	bool bCan = lkSQL3TableSet::CanDelete();
	if ( bCan )
		bCan = !(TBase::HasMovie(m_DB, GetActualCurRow()));
	//	return (__super::CanDelete() && !(CTBase::HasMovie(m_DB, GetActualCurRow())) && !(CTMatroska::HasMovie(m_DB, GetActualCurRow())));
	return bCan;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
// Members
////

wxString TMovies::GetTitleValue() const
{
	wxASSERT(m_pFldTitle);
	return m_pFldTitle->GetValue2();
}
void TMovies::SetTitleValue(const wxString& _t) // cannot be NULL !
{
	wxASSERT(m_pFldTitle);
	m_pFldTitle->SetValue2(_t);
}

wxString TMovies::GetSubTtlValue() const
{
	wxASSERT(m_pFldSubTtl);
	return m_pFldSubTtl->GetValue2();
}
void TMovies::SetSubTtlValue(const wxString& _s)
{
	wxASSERT(m_pFldSubTtl);
	m_pFldSubTtl->SetValue2(_s);
}

wxString TMovies::GetAliasValue() const
{
	wxASSERT(m_pFldAlias);
	return m_pFldAlias->GetValue2();
}
void TMovies::SetAliasValue(const wxString& _a)
{
	wxASSERT(m_pFldAlias);
	m_pFldAlias->SetValue2(_a);
}

wxUint32 TMovies::GetEpisodeValue() const
{
	wxASSERT(m_pFldEp);
	return static_cast<wxUint32>(m_pFldEp->GetValue2());
}
void TMovies::SetEpisodeValue(wxUint32 _e)
{
	wxASSERT(m_pFldEp);
	m_pFldEp->SetValue2(static_cast<wxInt64>(_e));
}

wxUint32 TMovies::GetCatValue() const
{
	wxASSERT(m_pFldCat);
	return static_cast<wxUint32>(m_pFldCat->GetValue2());
}
void TMovies::SetCatValue(wxUint32 _c)
{
	wxASSERT(m_pFldCat);
	m_pFldCat->SetValue2(static_cast<wxInt64>(_c));
}

wxUint32 TMovies::GetSubValue() const
{
	wxASSERT(m_pFldSub);
	return static_cast<wxUint32>(m_pFldSub->GetValue2());
}
void TMovies::SetSubValue(wxUint32 _s)
{
	wxASSERT(m_pFldSub);
	m_pFldSub->SetValue2(static_cast<wxInt64>(_s));
}

wxUint64 TMovies::GetGenreValue() const
{
	wxASSERT(m_pFldGenre);
	return static_cast<wxUint64>(m_pFldGenre->GetValue2());
}
void TMovies::SetGenreValue(wxUint64 _g)
{
	wxASSERT(m_pFldGenre);
	m_pFldGenre->SetValue2(static_cast<wxInt64>(_g));
}

wxUint32 TMovies::GetJudgeValue() const
{
	wxASSERT(m_pFldJudge);
	return static_cast<wxUint32>(m_pFldJudge->GetValue2());
}
void TMovies::SetJudgeValue(wxUint32 _j)
{
	wxASSERT(m_pFldJudge);
	m_pFldJudge->SetValue2(static_cast<wxInt64>(_j));
}

wxUint8 TMovies::GetRateValue() const
{
	wxASSERT(m_pFldRating);
	return static_cast<wxUint8>(m_pFldRating->GetValue2());
}
void TMovies::SetRateValue(wxUint8 _r)
{
	wxASSERT(m_pFldRating);
	m_pFldRating->SetValue2(static_cast<wxInt64>(_r));
}

wxString TMovies::GetCountryValue() const // empty if NULL
{
	wxASSERT(m_pFldCountry);
	return m_pFldCountry->GetValue2();
}
void TMovies::SetCountryValue(const wxString& _c)
{
	wxASSERT(m_pFldCountry);
	m_pFldCountry->SetValue2(_c);
}

lkDateTime TMovies::GetDateValue() const
{
	wxASSERT(m_pFldDate);
	lkDateTime dt;
	if ( m_pFldDate->IsNull() )
		dt.SetInvalid();
	else
		dt = m_pFldDate->GetValue2();
	return dt;
}
void TMovies::SetDateValue(const lkDateTime& _d) // it actually is a double .. if 'lkInvalidDateTime' sets the field to NULL
{
	wxASSERT(m_pFldDate);
	if ( !_d.IsValid() )
		m_pFldDate->SetValueNull();
	else
		m_pFldDate->SetValue2(_d);
}

bool TMovies::GetYearValue() const
{
	wxASSERT(m_pFldYear);
	return m_pFldYear->Getbool();
}
void TMovies::SetYearValue(bool _y)
{
	wxASSERT(m_pFldYear);
	m_pFldYear->Setbool(_y);
}

bool TMovies::GetSerieValue() const
{
	wxASSERT(m_pFldSerie);
	return m_pFldSerie->Getbool();
}
void TMovies::SetSerieValue(bool _s)
{
	wxASSERT(m_pFldSerie);
	m_pFldSerie->Setbool(_s);
}

wxString TMovies::GetUrlValue() const // empty if NULL
{
	wxASSERT(m_pFldURL);
	return m_pFldURL->GetValue2();
}
void TMovies::SetUrlValue(const wxString& _u)
{
	wxASSERT(m_pFldURL);
	m_pFldURL->SetValue2(_u);
}

wxString TMovies::GetCoverValue() const // empty if NULL
{
	wxASSERT(m_pFldCover);
	return m_pFldCover->GetValue2();
}
void TMovies::SetCoverValue(const wxString& _c)
{
	wxASSERT(m_pFldCover);
	m_pFldCover->SetValue2(_c);
}

wxString TMovies::GetInfoValue() const // empty if NULL
{
	wxASSERT(m_pFldInfo);
	return m_pFldInfo->GetValue2();
}
void TMovies::SetInfoValue(const wxString& _i)
{
	wxASSERT(m_pFldInfo);
	m_pFldInfo->SetValue2(_i);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation
////

//virtual
void TMovies::SetFilterActive(bool a)
{
	wxASSERT(!m_sFltFilter.IsEmpty());
	if ( m_sFltFilter.IsEmpty() )
		return;

	bool b = m_bFilterActive;
	m_bFilterActive = a;
	if ( !a && b )
	{
		m_Filter = m_sFltFilterPrev;
		m_Sort = m_sFltOrderPrev;

		m_sFltFilterPrev.Empty();
		m_sFltOrderPrev.Empty();
	}
	else if ( a && !b )
	{
		m_sFltFilterPrev = m_Filter;
		m_sFltOrderPrev = m_Sort;

		m_Filter = m_sFltFilter;
		m_Sort = m_sFltOrder;
	}
}

//virtual
void TMovies::CreateFindFilter(wxString& Filter, const wxString& WXUNUSED(Field), const wxString& Search, lkSQL3RecordSet::FIND_DIRECTION findDirection)
{
	Filter = TMovies::CreateMoviesFindFilter(Search, findDirection);
}

//static
wxString TMovies::CreateMoviesFindFilter(const wxString& Search, FIND_DIRECTION findDirection)
{
	wxString Filter, sT, sS, sA;

	sT = lkSQL3RecordSet::CreateFindFilter(t3Movies_TITLE, Search, findDirection, t3Movies);
	wxASSERT(!sT.IsEmpty());
	sS = lkSQL3RecordSet::CreateFindFilter(t3Movies_SUBTITLE, Search, findDirection, t3Movies);
	wxASSERT(!sS.IsEmpty());
	sA = lkSQL3RecordSet::CreateFindFilter(t3Movies_AKATITLE, Search, findDirection, t3Movies);
	wxASSERT(!sA.IsEmpty());

	Filter.Printf(wxT("(%s OR %s OR %s)"), sT, sS, sA);

	return Filter;
}

//static
bool TMovies::VerifyTable(lkSQL3Database* pDB)
{
	return lkSQL3TableSet::VerifyTable(pDB, t3MoviesTbl);
}

//static
bool TMovies::CreateTable(lkSQL3Database* pDB)
{
	bool bRet = false;

	if ( !pDB->TableExists(t3Movies) )
		bRet = lkSQL3TableSet::CreateTable(pDB, t3MoviesTbl);

	return bRet;
}

//static
bool TMovies::TestFilter(lkSQL3Database* pDB, const wxString& filter, const wxString& order)
{
	wxASSERT(!filter.IsEmpty());
	if ( filter.IsEmpty() || !pDB || !pDB->IsOpen() ) return true;
	bool bRet = true;

	wxString qry, s;
	if ( !order.IsEmpty() )
		s.Printf(wxT(" ORDER BY %s"), order);
	qry.Printf(wxT("SELECT COUNT([ROWID]) FROM [%s] WHERE %s%s;"), t3Movies, filter, s);
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
bool TMovies::HasCountry(lkSQL3Database* pDB, const wxString& iso, const wxString& code)
{
	bool bRet = true;
	if ( !pDB || !pDB->IsOpen() || iso.IsEmpty() )
		return bRet;

	wxString sql, qry, s;

	s = wxT("([%s] = '%s') OR ([%s] LIKE '%s %%') OR ([%s] LIKE '%% %s') OR ([%s] LIKE '%% %s %%')");
	qry.Printf(s, t3Movies_COUNTRY, iso, t3Movies_COUNTRY, iso, t3Movies_COUNTRY, iso, t3Movies_COUNTRY, iso);

	if ( !code.IsEmpty() )
	{
		wxString qryCode;
		qryCode.Printf(s, t3Movies_COUNTRY, code, t3Movies_COUNTRY, code, t3Movies_COUNTRY, code, t3Movies_COUNTRY, code);
		s.Printf(wxT("%s OR %s"), qry, qryCode);
		qry = qryCode;
	}

	sql.Printf(wxT("SELECT COUNT([ROWID]) FROM [%s] WHERE (%s);"), t3Movies, qry);
	try
	{
		bRet = ((wxUint64)pDB->ExecuteSQLAndGet(sql) > 0);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		bRet = true;
	}

	return bRet;
}

//static
bool TMovies::HasGenre(lkSQL3Database* pDB, wxUint64 genre)
{  // true if at least 1 genre match has been found
	if ( !pDB || !pDB->IsOpen() ) return true;
	wxString sql, whr;
	if ( genre == 0 )
		whr = wxT(" = 0");
	else
		whr.Printf(wxT(" & %llu"), genre);
	sql.Printf(wxT("SELECT COUNT([ROWID]) FROM [%s] WHERE ([%s]%s);"), t3Movies, t3Movies_GENRE, whr);
	wxUint64 uRet = 0;
	try
	{
		uRet = (wxUint64)pDB->ExecuteSQLAndGet(sql);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return true;
	}
	return (uRet > 0);
}

//static
bool TMovies::HasCat(lkSQL3Database* pDB, wxUint32 nCat, wxUint32 nSub)
{ // true if at least 1 cat/sub has been found
	if ( !pDB || !pDB->IsOpen() ) return true;
	bool bRet = true;

	wxString qry;
	qry.Printf(wxT("SELECT COUNT([ROWID]) FROM [%s] WHERE (([%s] = %u) AND ([%s] = %u));"), t3Movies, t3Movies_CAT, nCat, t3Movies_SUBCAT, nSub);
	wxUint64 nRet = 0;
	try
	{
		nRet = (wxUint64)pDB->ExecuteSQLAndGet(qry);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return true;
	}

	return (nRet > 0);
}

//static
bool TMovies::HasRating(lkSQL3Database* pDB, wxUint8 nRate)
{
	if ( !pDB || !pDB->IsOpen() ) return true;
	bool bRet = true;

	wxString qry;
	qry.Printf(wxT("SELECT COUNT([ROWID]) FROM [%s] WHERE ([%s] = %u);"), t3Movies, t3Movies_RATE, nRate);
	try
	{
		bRet = ((int)pDB->ExecuteSQLAndGet(qry) > 0);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		bRet = true;
	}

	return bRet;

}

//static
bool TMovies::HasJudge(lkSQL3Database* pDB, wxUint32 judge)
{
	if ( !pDB || !pDB->IsOpen() ) return true;
	bool bRet = true;

	wxString qry;
	qry.Printf(wxT("SELECT COUNT([ROWID]) FROM [%s] WHERE ([%s] = %u);"), t3Movies, t3Movies_JUDGE, judge);
	try
	{
		bRet = ((int)pDB->ExecuteSQLAndGet(qry) > 0);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		bRet = true;
	}

	return bRet;
}

//static
bool TMovies::Compact(wxThread* thr, lkSQL3Database* dbSrc, lkSQL3Database* dbDest, lkSQL3Database* dbTmp) // wxThread required for sending messages to main-thread
{
	if ( !thr || !dbSrc || !dbDest || !dbTmp || !dbSrc->IsOpen() || !dbDest->IsOpen() || !dbTmp->IsOpen() )
		return false;

	wxString s;
	TMovies rsSrc(dbSrc), rsDest(dbDest);
	TTemp rsTmp(dbTmp);
	rsSrc.SetOrder(wxT("[ROWID]"));
	try
	{
		rsSrc.Open();
		rsDest.Open();
		rsTmp.Open(t3Movies);

		(dynamic_cast<CompactThread*>(thr))->SendInfoRewriting(t3Movies);
		(dynamic_cast<CompactThread*>(thr))->InitProgressCtrl(0, rsSrc.GetRowCount());

		rsSrc.MoveFirst();

		wxUint32 nCat, oCat, oSub;
		wxUint64 nGenre, oGenre;
		wxUint64 nJudge, oJudge;
		wxUint32 nEpi;
		wxUint8 nRate;
		lkDateTime nDate;
		while ( !thr->TestDestroy() && !rsSrc.IsEOF() )
		{
			rsDest.AddNew();

			// Title
			rsDest.SetTitleValue(rsSrc.GetTitleValue());
			// Sub-Title
			if ( !(s = rsSrc.GetSubTtlValue()).IsEmpty() )
				rsDest.SetSubTtlValue(s);
			// Alias
			if ( !(s = rsSrc.GetAliasValue()).IsEmpty() )
				rsDest.SetAliasValue(s);
			// Episode
			if ( (nEpi = rsSrc.GetEpisodeValue()) > 0 )
				rsDest.SetEpisodeValue(nEpi);
			// Category
			if ( (oCat = rsSrc.GetCatValue()) != 0 )
			{
				nCat = TTempCat::FindNewID(dbTmp, t3Category, oCat, 0);
				rsDest.SetCatValue(nCat);
				if ( (oSub = rsSrc.GetSubValue()) != 0 )
				{
					nCat = TTempCat::FindNewID(dbTmp, t3Category, oCat, oSub);
					rsDest.SetSubValue(nCat);
				}
			}
			// Genre
			if ( (oGenre = rsSrc.GetGenreValue()) != 0 )
			{
				nGenre = TTemp::FindNewGenre(dbTmp, t3Genre, oGenre);
				rsDest.SetGenreValue(nGenre);
			}
			// Judgement
			if ( (oJudge = rsSrc.GetJudgeValue()) != 0 )
			{
				nJudge = TTemp::FindNewID(dbTmp, t3Judge, oJudge);
				rsDest.SetJudgeValue(nJudge);
			}
			// Rating -- as is --
			if ( (nRate = rsSrc.GetRateValue()) != 0 )
				rsDest.SetRateValue(nRate);
			// Country
			if ( !(s = rsSrc.GetCountryValue()).IsEmpty() )
				rsDest.SetCountryValue(s);
			// Date
			if ( (nDate = rsSrc.GetDateValue()).IsValid() )
				rsDest.SetDateValue(nDate);
			// Year - boolean
			rsDest.SetYearValue(rsSrc.GetYearValue());
			// Serie - boolean
			rsDest.SetSerieValue(rsSrc.GetSerieValue());
			// URL
			if ( !(s = rsSrc.GetUrlValue()).IsEmpty() )
				rsDest.SetUrlValue(s);
			// Cover - as string
			if ( !(s = rsSrc.GetCoverValue()).IsEmpty() )
			{
				// replace any '\\' to '/'
				s.Replace(wxT("\\"), wxT("/"), true);
				rsDest.SetCoverValue(s);
			}
			// Info
			if ( !(s = rsSrc.GetInfoValue()).IsEmpty() )
				rsDest.SetInfoValue(s);

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
		sql.Printf(wxT("TMovies::Compact : %s"), ((lkSQL3Exception*)&e)->GetError());
		(dynamic_cast<CompactThread*>(thr))->SetErrString(sql);
		return false;
	}

	bool bOK = (!rsSrc.IsEmpty() && !rsDest.IsEmpty()) ? (rsSrc.GetRowCount() == rsDest.GetRowCount()) : (rsSrc.IsEmpty() && rsDest.IsEmpty());
	bOK = (bOK) ? !thr->TestDestroy() : bOK;
	return bOK;
}
