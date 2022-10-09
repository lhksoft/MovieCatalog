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
#include "TCountry.h"
#include "TMovies.h"
#include "TBase.h"

#include "CompactThread.h"

#include "../lkSQLite3/lkSQL3Field.h"
#include "../lkSQLite3/lkSQL3Exception.h"
//#include <lkSQL3Common.h>

// static definitions for setting the fields of new table
static sqlTblField_t t3CountryFlds[] = {
	{ t3Country_ISO,      sqlFieldType::sqlText,  true, false,  0, 2, 2 }, // ISO-Label (vereist) min/max 2/2 chars ! - UNIQUE
//	{ t3Country_ISO636,   sqlFieldType::sqlText, false, false,  0, 2, 3 }, // ISO636-Label (optioneel) min/max 2/3 chars ! - UNIQUE
	{ t3Country_CODE,     sqlFieldType::sqlText, false, false,  0, 1, 5 }, // Eigen Code (optioneel) min/max 1/5 chars ! - can be NULL, UNIQUE if entered
	{ t3Country_DEF,      sqlFieldType::sqlText,  true, false,  0 }, // Land Naam (vereist) - UNIQUE
	{ t3Country_CUSTOM,   sqlFieldType::sqlText, false, false,  0 }, // Eigen Land Naam (optioneel) - can be NULL, UNIQUE if entered
	{ t3Country_AUDIOSUBS, sqlFieldType::sqlInt,  true,  true, wxT("0") },// wordt gebruikt als Audio(1), Subs(2), of beide(1|2), of niet(0)
	{ t3Country_SPOKEN,   sqlFieldType::sqlText, false, false,  0 }, // Naam van de Gesproken Taal (optioneel) - can be NULL
	{ t3Country_FLAG,     sqlFieldType::sqlBlob, false, false,  0 }, // Vlag (optioneel)
	{ t3Country_SFLAG,    sqlFieldType::sqlBlob, false, false,  0 }, // Vlag for spoken language (optioneel)
	{ 0 }
};

// Fields of Index-definitions
static sqlFldIndex_t t3CountryIDX_1[] = {
	{ t3Country_ISO, sqlSortOrder::sortAsc },
	{ 0 }
};
static sqlFldIndex_t t3CountryIDX_2[] = {
	{ t3Country_DEF, sqlSortOrder::sortAsc },
	{ 0 }
};

// static definitions for setting the index of new table
static sqlTblIndex_t t3CountryIdx[] = {
	{ wxT("idxCountry_ISO"),  t3CountryIDX_1, true }, // UNIQUE
	{ wxT("idxCountry_Name"), t3CountryIDX_2, true }, // UNIQUE
	{ 0 }
};

// static definitions for creating a fresh new table
//static 
sqlTblDef_t TCountry::t3CountryTbl = { t3Country, t3CountryFlds, t3CountryIdx };

wxIMPLEMENT_DYNAMIC_CLASS(TCountry, lkSQL3TableSet)

TCountry::TCountry() : lkSQL3TableSet(), m_sFlagName(), m_sSpokenFlagName()
{
	Init();
}
TCountry::TCountry(lkSQL3Database* pDB) : lkSQL3TableSet(pDB), m_sFlagName(), m_sSpokenFlagName()
{
	Init();
}

void TCountry::Init()
{
	m_sFlagName.Empty();
	m_sSpokenFlagName.Empty();

	m_pFldIsoDefault = NULL;
//	m_pFldIso636 = NULL;
	m_pFldIsoCustom = NULL;
	m_pFldCountryDefault = NULL;
	m_pFldCountryCustom = NULL;
	m_pFldAudioSubs = NULL;
	m_pFldSpoken = NULL;
	m_pFldFlag = NULL;
	m_pFldSpokenFlag = NULL;
}

// Operations

//virtual
bool TCountry::Open()
{
	lkInitTable msInit(&t3CountryTbl);
	// fill Fields
	msInit.Append(new lkInitField(t3Country_ISO, wxCLASSINFO(lkSQL3FieldData_Text), (lkSQL3FieldData**)&m_pFldIsoDefault));
//	msInit.Append(new lkInitField(t3Country_ISO636, wxCLASSINFO(lkSQL3FieldData_Text), (lkSQL3FieldData**)&m_pFldIso636));
	msInit.Append(new lkInitField(t3Country_CODE, wxCLASSINFO(lkSQL3FieldData_Text), (lkSQL3FieldData**)&m_pFldIsoCustom));
	msInit.Append(new lkInitField(t3Country_DEF, wxCLASSINFO(lkSQL3FieldData_Text), (lkSQL3FieldData**)&m_pFldCountryDefault));
	msInit.Append(new lkInitField(t3Country_CUSTOM, wxCLASSINFO(lkSQL3FieldData_Text), (lkSQL3FieldData**)&m_pFldCountryCustom));
	msInit.Append(new lkInitField(t3Country_AUDIOSUBS, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldAudioSubs));
	msInit.Append(new lkInitField(t3Country_SPOKEN, wxCLASSINFO(lkSQL3FieldData_Text), (lkSQL3FieldData**)&m_pFldSpoken));
	msInit.Append(new lkInitField(t3Country_FLAG, wxCLASSINFO(lkSQL3FieldData_Image), (lkSQL3FieldData**)&m_pFldFlag));
	msInit.Append(new lkInitField(t3Country_SFLAG, wxCLASSINFO(lkSQL3FieldData_Image), (lkSQL3FieldData**)&m_pFldSpokenFlag));

	bool bRet = lkSQL3TableSet::Open(&msInit);

	if ( !bRet )
		Throw_InvalidTable(t3Country);

	m_bIsOpen = bRet;
	return bRet;
}

//virtual
bool TCountry::CanDelete()
{
	bool bCan = (IsOpen() && lkSQL3TableSet::CanDelete());
	wxString iso = GetIsoValue();
	wxString cus = GetCustomIsoValue();
	if ( bCan )
		bCan = !(TMovies::HasCountry(m_DB, iso, cus));
	if ( bCan )
		bCan = !(TBase::HasAudio(m_DB, iso));
	if ( bCan )
		bCan = !(TBase::HasSubtitle(m_DB, iso));

	return bCan;
}


// --- Members ----------------------------------------------------------------------------------------------------- //

wxString TCountry::GetIsoValue() const
{
	wxASSERT(m_pFldIsoDefault);
	return m_pFldIsoDefault->GetValue2();
}
void TCountry::SetIsoValue(const wxString& _i) // NULL not allowed
{
	wxASSERT(m_pFldIsoDefault);
	m_pFldIsoDefault->SetValue2(_i);
}
/*
wxString TCountry::GetIso636Value() const
{
	wxASSERT(m_pFldIso636);
	return m_pFldIso636->GetValue2();
}
void TCountry::SetIso636Value(const wxString& _i) // NULL not allowed
{
	wxASSERT(m_pFldIso636);
	if ( _i.IsEmpty() )
		m_pFldIso636->SetValueNull();
	else
		m_pFldIso636->SetValue2(_i);
}
*/
wxString TCountry::GetCustomIsoValue() const
{
	wxASSERT(m_pFldIsoCustom);
	return m_pFldIsoCustom->GetValue2();
}
void TCountry::SetCustomIsoValue(const wxString& _c)
{
	wxASSERT(m_pFldIsoCustom);
	if ( _c.IsEmpty() )
		m_pFldIsoCustom->SetValueNull();
	else
		m_pFldIsoCustom->SetValue2(_c);
}

wxString TCountry::GetDefaultCountryValue() const
{
	wxASSERT(m_pFldCountryDefault);
	return m_pFldCountryDefault->GetValue2();
}
void TCountry::SetDefaultCountryValue(const wxString& _c) // NULL not allowed
{
	wxASSERT(m_pFldCountryDefault);
	m_pFldCountryDefault->SetValue2(_c);
}

wxString TCountry::GetCustomCountryValue() const
{
	wxASSERT(m_pFldCountryCustom);
	return m_pFldCountryCustom->GetValue2();
}
void TCountry::SetCustomCountryValue(const wxString& _c)
{
	wxASSERT(m_pFldCountryCustom);
	if ( _c.IsEmpty() )
		m_pFldCountryCustom->SetValueNull();
	else
		m_pFldCountryCustom->SetValue2(_c);
}

wxUint8 TCountry::GetAudioSubsValue() const
{
	wxASSERT(m_pFldAudioSubs);
	return (wxUint8)(m_pFldAudioSubs->GetValue2());
}
void TCountry::SetAudioSubsValue(wxUint8 _b)
{
	wxASSERT(m_pFldAudioSubs);
	m_pFldAudioSubs->SetValue2((wxInt64)_b);
}

wxString TCountry::GetSpokenValue() const
{
	wxASSERT(m_pFldSpoken);
	return m_pFldSpoken->GetValue2();
}
void TCountry::SetSpokenValue(const wxString& s) // if entering NULL, sets te value to IsNull
{
	wxASSERT(m_pFldSpoken);
	if ( s.IsEmpty() )
		m_pFldSpoken->SetValueNull();
	else
		m_pFldSpoken->SetValue2(s);
}

wxImage TCountry::GetFlag() const
{
	wxASSERT(m_pFldFlag != NULL);
	wxString _name;
	wxImage img = m_pFldFlag->GetAsImage(_name);

	*((wxString*)&m_sFlagName) = (img.IsOk()) ? _name : wxT("");

	return img;
}
// if invalid image, sets the image to NULL
void TCountry::SetFlag(const wxImage& _img, const wxString& _name)
{
	wxASSERT(m_pFldFlag != NULL);
	m_sFlagName = (m_pFldFlag->SetFromImage(_img, _name)) ? _name : wxT("");
}
// returns the name (if any) retrieved from last GetImage() / SetImage(..)
wxString TCountry::GetFlagName() const
{
	return m_sFlagName;
}

wxImage TCountry::GetSpokenFlag(void) const
{
	wxASSERT(m_pFldSpokenFlag != NULL);
	wxString _name;
	wxImage img = m_pFldSpokenFlag->GetAsImage(_name);

	*((wxString*)&m_sSpokenFlagName) = (img.IsOk()) ? _name : wxT("");

	return img;
}
// if invalid image, sets the image to NULL
void TCountry::SetSpokenFlag(const wxImage& _img, const wxString& _name)
{
	wxASSERT(m_pFldSpokenFlag != NULL);
	m_sSpokenFlagName = (m_pFldSpokenFlag->SetFromImage(_img, _name)) ? _name : wxT("");
}
// returns the name (if any) retrieved from last GetImage() / SetImage(..)
wxString TCountry::GetSpokenFlagName() const
{
	return m_sSpokenFlagName;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation
////

//virtual
void TCountry::CreateFindFilter(wxString& Filter, const wxString& Field, const wxString& Search, lkSQL3RecordSet::FIND_DIRECTION findDirection)
{
	Filter.Empty();

	wxString s;
	switch ( findDirection )
	{
		case FIND_DIRECTION::fndStart:
			s = wxT("(([%s] LIKE '%s%%') OR ([%s] LIKE '%s%%'))");
			break;
		case FIND_DIRECTION::fndAnywhere:
			s = wxT("(([%s] LIKE '%%%s%%') OR ([%s] LIKE '%%%s%%'))");
			break;
		case FIND_DIRECTION::fndEnd:
			s = wxT("(([%s] LIKE '%%%s') OR ([%s] LIKE '%%%s'))");
			break;
		case FIND_DIRECTION::fndEntire:
		default: // exact
			s = wxT("((LOWER([%s]) = LOWER('%s')) OR (LOWER([%s]) = LOWER('%s')))");
			break;
	}
	if ( Field.IsSameAs(t3Country_ISO) || Field.IsSameAs(t3Country_CODE) )
		Filter.Printf(s, t3Country_CODE, Search, t3Country_ISO, Search);
	else if ( Field.IsSameAs(t3Country_DEF) || Field.IsSameAs(t3Country_CUSTOM) )
		Filter.Printf(s, t3Country_CUSTOM, Search, t3Country_DEF, Search);
}


//static
bool TCountry::VerifyTable(lkSQL3Database* pDB)
{
	return lkSQL3TableSet::VerifyTable(pDB, t3CountryTbl);
}

//static
bool TCountry::CreateTable(lkSQL3Database* pDB)
{
	bool bRet = false;

	if ( !pDB->TableExists(t3Country) )
		bRet = lkSQL3TableSet::CreateTable(pDB, t3CountryTbl);

	return bRet;
}

//static
bool TCountry::IsUniqueIso(lkSQL3Database* _DB, const wxString& iso, void*)
{
	bool ret = false;
	try
	{
		ret = lkSQL3TableSet::IsUniqueTextValue(_DB, t3Country, t3Country_ISO, iso);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		ret = false;
	}

	return ret;
}
/*
//static
bool TCountry::IsUniqueIso636(lkSQL3Database* _DB, const wxString& iso636, void*) // TRUE if given iso doesn't exist in table
{
	bool ret = false;
	try
	{
		ret = lkSQL3TableSet::IsUniqueTextValue(_DB, t3Country, t3Country_ISO636, iso636);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		ret = false;
	}

	return ret;
}
*/
//static
bool TCountry::IsUniqueCode(lkSQL3Database* _DB, const wxString& code, void*)
{
	bool ret = false;
	try
	{
		ret = lkSQL3TableSet::IsUniqueTextValue(_DB, t3Country, t3Country_CODE, code);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		ret = false;
	}

	return ret;
}
//static
bool TCountry::IsUniqueCountry(lkSQL3Database* _DB, const wxString& country, void*)
{
	bool ret = false;
	try
	{
		ret = lkSQL3TableSet::IsUniqueTextValue(_DB, t3Country, t3Country_DEF, country);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		ret = false;
	}

	return ret;
}
//static
bool TCountry::IsUniqueCustom(lkSQL3Database* _DB, const wxString& custom, void*)
{
	bool ret = false;
	try
	{
		ret = lkSQL3TableSet::IsUniqueTextValue(_DB, t3Country, t3Country_CUSTOM, custom);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		ret = false;
	}

	return ret;
}

//static
bool TCountry::Compact(wxThread* t, lkSQL3Database* dbSrc, lkSQL3Database* dbDest)
{
	if ( !t || !dbSrc || !dbDest || !dbSrc->IsOpen() || !dbDest->IsOpen() )
		return false;

	wxString s;
	TCountry rsSrc(dbSrc), rsDest(dbDest);
	s.Printf(wxT("[%s]"), t3Country_ISO);
	rsSrc.SetOrder(s);
	try
	{
		rsSrc.Open();
		TCountry::CreateTable(dbDest);
		rsDest.Open();

		(dynamic_cast<CompactThread*>(t))->SendInfoMain(wxEmptyString);
		(dynamic_cast<CompactThread*>(t))->SendInfoRewriting(t3Country);
		(dynamic_cast<CompactThread*>(t))->InitProgressCtrl(0, rsSrc.GetRowCount());

		rsSrc.MoveFirst();

		wxUint8 _as;
		wxString name;
		wxImage img;
		while ( !t->TestDestroy() && !rsSrc.IsEOF() )
		{
			rsDest.AddNew();

			// ISO
			rsDest.SetIsoValue(rsSrc.GetIsoValue());
			// Custom ISO
			s = rsSrc.GetCustomIsoValue();
			if ( !s.IsEmpty() )
				rsDest.SetCustomIsoValue(s);
			// Default Name
			rsDest.SetDefaultCountryValue(rsSrc.GetDefaultCountryValue());
			// Custom Name
			s = rsSrc.GetCustomCountryValue();
			if ( !s.IsEmpty() )
				rsDest.SetCustomCountryValue(s);
			// AudioSubs
			_as = rsSrc.GetAudioSubsValue();
			if ( _as > 0 )
				rsDest.SetAudioSubsValue(_as);
			// Speaking
			s = rsSrc.GetSpokenValue();
			if ( !s.IsEmpty() )
				rsDest.SetSpokenValue(s);
			// Image
			if ( (img = rsSrc.GetFlag()).IsOk() )
			{
				name = rsSrc.GetFlagName();
				rsDest.SetFlag(img, name);
			}
			// Image2
			if ( (img = rsSrc.GetSpokenFlag()).IsOk() )
			{
				name = rsSrc.GetSpokenFlagName();
				rsDest.SetSpokenFlag(img, name);
			}

			rsDest.Update();
			rsSrc.MoveNext();

			(dynamic_cast<CompactThread*>(t))->StepProgressCtrl(0);
		}
	}
	catch ( const lkSQL3Exception& e )
	{
		wxString sql;
		sql.Printf(wxT("TCountry::Compact : %s"), ((lkSQL3Exception*)&e)->GetError());
		(dynamic_cast<CompactThread*>(t))->SetErrString(sql);
		return false;
	}

	bool bOK = (!rsSrc.IsEmpty() && !rsDest.IsEmpty()) ? (rsSrc.GetRowCount() == rsDest.GetRowCount()) : (rsSrc.IsEmpty() && rsDest.IsEmpty());
	bOK = (bOK) ? !t->TestDestroy() : bOK;
	return bOK;
}
