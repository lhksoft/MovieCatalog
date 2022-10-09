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
#include "TMedium.h"
#include "TStorage.h"
#include "TTemp.h"

#include "CompactThread.h"

#include "../lkSQLite3/lkSQL3Field.h"
#include "../lkSQLite3/lkSQL3Exception.h"

// static definitions for setting the fields of new table
static sqlTblField_t t3MediumFlds[] = {
	{ t3Medium_CODE,     sqlFieldType::sqlText,  true, false, 0 }, // Label (vereist-unique)
	{ t3Medium_NAME,     sqlFieldType::sqlText, false, false, 0 }, // 
	{ t3Medium_IMG,      sqlFieldType::sqlBlob, false, false, 0 }, // containing image of Mediumment
	{ t3Medium_LIKES,    sqlFieldType::sqlText, false, false, 0 }, // used in StorageForm = Leading character
	{ t3Medium_NOTLIKES, sqlFieldType::sqlText, false, false, 0 }, // used in StorageForm = Leading character(s) to exclude
	{ 0 }
};

// static definitions for creating a fresh new table
static sqlTblDef_t t3MediumTbl = { t3Medium, t3MediumFlds, 0 };

wxIMPLEMENT_DYNAMIC_CLASS(TMedium, lkSQL3TableSet)

TMedium::TMedium() : lkSQL3TableSet(), m_sImageName()
{
	m_RowIdGrow = 5;
	Init();
}
TMedium::TMedium(lkSQL3Database* pDB) : lkSQL3TableSet(pDB), m_sImageName()
{
	m_RowIdGrow = 5;
	Init();
}

void TMedium::Init()
{
	m_pFldCode = NULL;
	m_pFldName = NULL;
	m_pFldImage = NULL;
	m_pFldLikes = NULL;
	m_pFldNotLikes = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Operations

//virtual
bool TMedium::Open()
{
	lkInitTable msInit(&t3MediumTbl);
	// fill Fields
	msInit.Append(new lkInitField(t3Medium_CODE, wxCLASSINFO(lkSQL3FieldData_Text), (lkSQL3FieldData**)&m_pFldCode));
	msInit.Append(new lkInitField(t3Medium_NAME, wxCLASSINFO(lkSQL3FieldData_Text), (lkSQL3FieldData**)&m_pFldName));
	msInit.Append(new lkInitField(t3Medium_IMG, wxCLASSINFO(lkSQL3FieldData_Image), (lkSQL3FieldData**)&m_pFldImage));
	msInit.Append(new lkInitField(t3Medium_LIKES, wxCLASSINFO(lkSQL3FieldData_Text), (lkSQL3FieldData**)&m_pFldLikes));
	msInit.Append(new lkInitField(t3Medium_NOTLIKES, wxCLASSINFO(lkSQL3FieldData_Text), (lkSQL3FieldData**)&m_pFldNotLikes));

	bool bRet = lkSQL3TableSet::Open(&msInit);

	if ( !bRet )
		Throw_InvalidTable(t3Medium);

	m_bIsOpen = bRet;
	return bRet;
}

//virtual
bool TMedium::CanDelete()
{
	return (IsOpen() && lkSQL3TableSet::CanDelete() && !TStorage::HasMedium(m_DB, static_cast<wxUint32>(GetActualCurRow())));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Attributes

wxString TMedium::GetCodeValue() const
{
	wxASSERT(m_pFldCode);
	return m_pFldCode->GetValue2();
}
void TMedium::SetCodeValue(const wxString& _c)
{
	wxASSERT(m_pFldCode);
	m_pFldCode->SetValue2(_c);
}

wxString TMedium::GetNameValue() const
{
	wxASSERT(m_pFldName);
	return m_pFldName->GetValue2();
}
void TMedium::SetNameValue(const wxString& _n)
{
	wxASSERT(m_pFldName);
	m_pFldName->SetValue2(_n);
}

wxImage TMedium::GetImage() const
{
	wxASSERT(m_pFldImage != NULL);
	wxString _name;
	wxImage img = m_pFldImage->GetAsImage(_name);

	*((wxString*)&m_sImageName) = (img.IsOk()) ? _name : wxT("");

	return img;
}
void TMedium::SetImage(const wxImage& _img, const wxString& _name)
{
	wxASSERT(m_pFldImage != NULL);
	m_sImageName = (m_pFldImage->SetFromImage(_img, _name)) ? _name : wxT("");
}
wxString TMedium::GetImageName() const
{
	return m_sImageName;
}

wxString TMedium::GetStorageLikes() const
{
	wxASSERT(m_pFldLikes);
	return m_pFldLikes->GetValue2();
}
void TMedium::SetStorageLikes(const wxString& l)
{
	wxASSERT(m_pFldLikes);
	m_pFldLikes->SetValue2(l);
}

wxString TMedium::GetStorageNotLikes() const
{
	wxASSERT(m_pFldNotLikes);
	return m_pFldNotLikes->GetValue2();
}
void TMedium::SetStorageNotLikes(const wxString& nl)
{
	wxASSERT(m_pFldNotLikes);
	m_pFldNotLikes->SetValue2(nl);
}

//static
bool TMedium::VerifyTable(lkSQL3Database* pDB)
{
	return lkSQL3TableSet::VerifyTable(pDB, t3MediumTbl);
}

//static
bool TMedium::CreateTable(lkSQL3Database* pDB)
{
	bool bRet = false;

	if ( !pDB->TableExists(t3Medium) )
		bRet = lkSQL3TableSet::CreateTable(pDB, t3MediumTbl);

	return bRet;
}

//static
bool TMedium::IsUniqueCode(lkSQL3Database* _DB, const wxString& code, void*)
{
	bool ret = false;
	try
	{
		ret = lkSQL3TableSet::IsUniqueTextValue(_DB, t3Medium, t3Medium_CODE, code);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return false;
	}

	return ret;
}

//static
bool TMedium::Compact(wxThread* thr, lkSQL3Database* dbSrc, lkSQL3Database* dbDest, lkSQL3Database* dbTmp) // wxThread required for sending messages to main-thread
{
	if ( !thr || !dbSrc || !dbDest || !dbTmp || !dbSrc->IsOpen() || !dbDest->IsOpen() || !dbTmp->IsOpen() )
		return false;

	TMedium rsSrc(dbSrc), rsDest(dbDest);
	TTemp rsTmp(dbTmp);
	rsSrc.SetOrder(wxT("[ROWID]"));
	wxString name, s;
	wxImage img;
	try
	{
		rsSrc.Open();
		rsDest.Open();
		rsTmp.Open(t3Medium);

		(dynamic_cast<CompactThread*>(thr))->SendInfoRewriting(t3Medium);
		(dynamic_cast<CompactThread*>(thr))->InitProgressCtrl(0, rsSrc.GetRowCount());

		rsSrc.MoveFirst();

		while ( !thr->TestDestroy() && !rsSrc.IsEOF() )
		{
			rsDest.AddNew();

			// Code
			rsDest.SetCodeValue(rsSrc.GetCodeValue());
			// Name (optional)
			if ( !(s = rsSrc.GetNameValue()).IsEmpty() )
				rsDest.SetNameValue(s);
			// Image -- return buffer is CONST, don't release it yourself !
			if ( (img = rsSrc.GetImage()).IsOk() )
			{
				name = rsSrc.GetImageName();
				rsDest.SetImage(img, name);
			}
			// Likes
			if ( !(s = rsSrc.GetStorageLikes()).IsEmpty() )
				rsDest.SetStorageLikes(s);
			// Not Likes
			if ( !(s = rsSrc.GetStorageNotLikes()).IsEmpty() )
				rsDest.SetStorageNotLikes(s);

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
		sql.Printf(wxT("TMedium::Compact : %s"), ((lkSQL3Exception*)&e)->GetError());
		(dynamic_cast<CompactThread*>(thr))->SetErrString(sql);
		return false;
	}

	bool bOK = (!rsSrc.IsEmpty() && !rsDest.IsEmpty()) ? (rsSrc.GetRowCount() == rsDest.GetRowCount()) : (rsSrc.IsEmpty() && rsDest.IsEmpty());
	bOK = (bOK) ? !thr->TestDestroy() : bOK;
	return bOK;
}
