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
#include "SimpleBox.h"
#include "TJudge.h"
#include "TRating.h"
#include "TMedium.h"
#include "TLocation.h"
#include "TQuality.h"
#include "TOrigine.h"
#include "TScreen.h"

#include "../lkSQLite3/lkSQL3Exception.h"

#define F_UNSET_STR		wxT("<unset>") // having value '0'

wxIMPLEMENT_ABSTRACT_CLASS(SimpleBox, lkImgComboBox)

SimpleBox::SimpleBox() : lkImgComboBox()
{
	m_pDB = NULL;
}

SimpleBox::SimpleBox(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator) :
	lkImgComboBox(parent, id, size, style, validator, pos)
{
	m_pDB = NULL;
}

void SimpleBox::SetDatabase(lkSQL3Database* pDB)
{
	m_pDB = pDB;
}

////////////////////////////////////////////////////////////////////////////
// JudgeBox
////
wxIMPLEMENT_DYNAMIC_CLASS(JudgeBox, SimpleBox)

JudgeBox::JudgeBox() : SimpleBox()
{
}
JudgeBox::JudgeBox(wxWindow* parent, lkSQL3Database* pDB, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator)
	: SimpleBox(parent, wxID_ANY, pos, size, style, validator)
{
	SetDatabase(pDB);
}

bool JudgeBox::BuildList(bool bKeepSel)
{
	wxASSERT(m_pDB != NULL);
	if ( m_pDB == NULL ) return false; // nothing to do

	TJudge	RS(m_pDB);
	wxUint64 curSel = 0; // not using if ==0

	try
	{
		wxString order;
		order = wxT("[ROWID]");
		RS.SetOrder(order);
		RS.Open();
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return false;
	}

	if ( bKeepSel && (GetCount() > 0) )
		curSel = GetSelectedLParam();

	lkBoxInitList initList;
	initList.DeleteContents(true);

	if ( RS.IsEmpty() )
	{ // cannot be empty, enter at least something
		wxString s = F_UNSET_STR;
		initList.Append(new lkBoxInit(s, 0));
		curSel = 0;
	}
	else
	{
		wxString sJudge;
		wxImage img;
		wxUint64 lParam;

		RS.MoveFirst();
		while ( !RS.IsEOF() )
		{
			sJudge = RS.GetJudgeValue();
			img = RS.GetImage();
			lParam = RS.GetActualCurRow();

			// fill initlist with some data
			initList.Append(new lkBoxInit(sJudge, img, lParam));

			RS.MoveNext();
		}
	}

	bool bRet = false;
	if ( InitializeFromList(&initList, true, lkExpandFlags::EXPAND_Right) )
		if ( bRet = Populate() )
			if ( !bKeepSel )
				curSel = 3; // our default

	if ( bRet )
		SetSelectedLParam(curSel);
	else
		SetSelection(0);

	return bRet;
}

////////////////////////////////////////////////////////////////////////////
// RatingBox
////
wxIMPLEMENT_DYNAMIC_CLASS(RatingBox, SimpleBox)

RatingBox::RatingBox() : SimpleBox()
{
}
RatingBox::RatingBox(wxWindow* parent, lkSQL3Database* pDB, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator)
	: SimpleBox(parent, wxID_ANY, pos, size, style, validator)
{
	SetDatabase(pDB);
}

bool RatingBox::BuildList(bool bKeepSel)
{
	wxASSERT(m_pDB != NULL);
	if ( m_pDB == NULL ) return false; // nothing to do

	TRating	RS(m_pDB);
	wxUint64 curSel = 0; // not using if ==0

	try
	{
		wxString order;
		order.Printf(wxT("[%s]"), t3Rating_ID);
		RS.SetOrder(order);
		RS.Open();
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return false;
	}

	if ( bKeepSel && (GetCount() > 0) )
		curSel = GetSelectedLParam();

	lkBoxInitList initList;
	initList.DeleteContents(true);

	if ( RS.IsEmpty() )
	{ // cannot be empty, enter at least something
		wxString s = F_UNSET_STR;
		initList.Append(new lkBoxInit(s, 0));
		curSel = 0;
	}
	else
	{
		wxString szRating;
		wxImage img;
		wxUint8 rate;

		RS.MoveFirst();
		while ( !RS.IsEOF() )
		{
			szRating = RS.GetDescriptValue();
			if ( szRating.IsEmpty() )
				szRating = RS.GetRatingValue();

			img = RS.GetImage();
			rate = RS.GetFlagValue();

			// fill initlist 
			initList.Append(new lkBoxInit(szRating, img, (wxUint64)rate));

			RS.MoveNext();
		}
	}

	bool bRet = false;
	if ( InitializeFromList(&initList, true, lkExpandFlags::EXPAND_Center) )
		if ( bRet = Populate() )
			if ( !bKeepSel )
				curSel = 1; // our default

	if ( bRet )
		SetSelectedLParam(curSel);
	else
		SetSelection(0);

	return bRet;
}


////////////////////////////////////////////////////////////////////////////
// MediumBox
////
wxIMPLEMENT_DYNAMIC_CLASS(MediumBox, SimpleBox)

MediumBox::MediumBox() : SimpleBox()
{
}
MediumBox::MediumBox(wxWindow* parent, const wxSize& size, const wxValidator& validator) : SimpleBox(parent, wxID_ANY, wxDefaultPosition, size, 0L, validator)
{
}

bool MediumBox::BuildList(bool bKeepSel)
{
	wxASSERT(m_pDB != NULL);
	if ( m_pDB == NULL ) return false; // nothing to do

	TMedium	RS(m_pDB);
	wxUint64 curSel = 0; // not using if ==0

	try
	{
		RS.SetOrder(wxT("[ROWID]"));
		RS.Open();
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return false;
	}

	if ( bKeepSel && (GetCount() > 0) )
		curSel = GetSelectedLParam();

	StartOver();

	lkBoxInitList initList;
	initList.DeleteContents(true);

	if ( RS.IsEmpty() )
	{ // cannot be empty, enter at least something
		wxString s = F_UNSET_STR;
		initList.Append(new lkBoxInit(s, 0));
		curSel = 0;
	}
	else
	{
		wxString sMedium;
		wxImage img;
		wxUint64 lParam;

		RS.MoveFirst();
		while ( !RS.IsEOF() )
		{
			if ( (sMedium = RS.GetNameValue()).IsEmpty() )
				sMedium = RS.GetCodeValue();
			img = RS.GetImage();
			lParam = RS.GetActualCurRow();

			// fill initlist with some data
			initList.Append(new lkBoxInit(sMedium, img, lParam));

			RS.MoveNext();
		}
	}

	bool bRet = false;
	if ( InitializeFromList(&initList, true, lkExpandFlags::EXPAND_Right) )
		bRet = Populate();

	if ( bRet && (curSel > 0) )
		SetSelectedLParam(curSel);
	else
		SetSelection(0);

	return bRet;
}


////////////////////////////////////////////////////////////////////////////
// LocationBox
////
wxIMPLEMENT_DYNAMIC_CLASS(LocationBox, SimpleBox)

LocationBox::LocationBox() : SimpleBox()
{
}
LocationBox::LocationBox(wxWindow* parent, const wxSize& size, const wxValidator& validator) : SimpleBox(parent, wxID_ANY, wxDefaultPosition, size, 0L, validator)
{
}

bool LocationBox::BuildList(bool bKeepSel)
{
	wxASSERT(m_pDB != NULL);
	if ( m_pDB == NULL ) return false; // nothing to do

	TLocation RS(m_pDB);
	wxUint64 curSel = 0; // not using if ==0

	try
	{
		RS.SetOrder(wxT("[ROWID]"));
		RS.Open();
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return false;
	}

	if ( bKeepSel && (GetCount() > 0) )
		curSel = GetSelectedLParam();

	StartOver();

	lkBoxInitList initList;
	initList.DeleteContents(true);

	if ( RS.IsEmpty() )
	{ // cannot be empty, enter at least something
		wxString s = F_UNSET_STR;
		initList.Append(new lkBoxInit(s, 0));
		curSel = 0;
	}
	else
	{
		wxString sLoc;
		wxUint64 lParam;

		RS.MoveFirst();
		while ( !RS.IsEOF() )
		{
			if ( !(sLoc = RS.GetLocationValue()).IsEmpty() )
			{
				lParam = RS.GetActualCurRow();
				// fill initlist
				initList.Append(new lkBoxInit(sLoc, lParam));
			}
			RS.MoveNext();
		}
	}

	bool bRet = false;
	if ( InitializeFromList(&initList, true, lkExpandFlags::EXPAND_Right) )
		bRet = Populate();

	if ( bRet && (curSel > 0) )
		SetSelectedLParam(curSel);
	else
		SetSelection(0);

	return bRet;
}


////////////////////////////////////////////////////////////////////////////
// QualityBox
////
wxIMPLEMENT_DYNAMIC_CLASS(QualityBox, SimpleBox)

QualityBox::QualityBox() : SimpleBox()
{
}
QualityBox::QualityBox(wxWindow* parent, const wxSize& size, const wxValidator& validator) : SimpleBox(parent, wxID_ANY, wxDefaultPosition, size, 0L, validator)
{
}

bool QualityBox::BuildList(bool bKeepSel)
{
	wxASSERT(m_pDB != NULL);
	if ( m_pDB == NULL ) return false; // nothing to do

	TQuality RS(m_pDB);
	wxUint64 curSel = 0; // not using if ==0

	try
	{
		RS.SetOrder(wxT("[ROWID]"));
		RS.Open();
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return false;
	}

	if ( bKeepSel && (GetCount() > 0) )
		curSel = GetSelectedLParam();

	StartOver();

	lkBoxInitList initList;
	initList.DeleteContents(true);

	if ( !RS.IsEmpty() )
	{
		wxString sQty;
		wxUint64 lParam;
		wxImage img;

		RS.MoveFirst();
		while ( !RS.IsEOF() )
		{
			if ( !(sQty = RS.GetQualityValue()).IsEmpty() )
			{
				lParam = RS.GetActualCurRow();
				img = RS.GetImage();
				// fill initlist
				initList.Append(new lkBoxInit(sQty, img, lParam));
			}
			RS.MoveNext();
		}
	}

	bool bRet = false;
	if ( InitializeFromList(&initList, true, lkExpandFlags::EXPAND_Center) )
		bRet = Populate();

	if ( bRet && (curSel > 0) )
		SetSelectedLParam(curSel);
	else
		SetSelection(0);

	return bRet;
}


////////////////////////////////////////////////////////////////////////////
// OrigineBox
////
wxIMPLEMENT_DYNAMIC_CLASS(OrigineBox, SimpleBox)

OrigineBox::OrigineBox() : SimpleBox()
{
}
OrigineBox::OrigineBox(wxWindow* parent, const wxSize& size, const wxValidator& validator) : SimpleBox(parent, wxID_ANY, wxDefaultPosition, size, 0L, validator)
{
}

bool OrigineBox::BuildList(bool bKeepSel)
{
	wxASSERT(m_pDB != NULL);
	if ( m_pDB == NULL ) return false; // nothing to do

	TOrigine RS(m_pDB);
	wxUint64 curSel = 0; // not using if ==0

	try
	{
		RS.SetOrder(wxT("[ROWID]"));
		RS.Open();
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return false;
	}

	if ( bKeepSel && (GetCount() > 0) )
		curSel = GetSelectedLParam();

	StartOver();

	lkBoxInitList initList;
	initList.DeleteContents(true);

	if ( !RS.IsEmpty() )
	{
		wxString sQty;
		wxUint64 lParam;

		RS.MoveFirst();
		while ( !RS.IsEOF() )
		{
			if ( !(sQty = RS.GetOrigineValue()).IsEmpty() )
			{
				lParam = RS.GetActualCurRow();
				// fill initlist
				initList.Append(new lkBoxInit(sQty, lParam));
			}
			RS.MoveNext();
		}
	}

	bool bRet = false;
	if ( InitializeFromList(&initList, true, lkExpandFlags::EXPAND_Center) )
		bRet = Populate();

	if ( bRet && (curSel > 0) )
		SetSelectedLParam(curSel);
	else
		SetSelection(0);

	return bRet;
}


////////////////////////////////////////////////////////////////////////////
// ScreenBox
////
wxIMPLEMENT_DYNAMIC_CLASS(ScreenBox, SimpleBox)

ScreenBox::ScreenBox() : SimpleBox()
{
}
ScreenBox::ScreenBox(wxWindow* parent, const wxSize& size, const wxValidator& validator) : SimpleBox(parent, wxID_ANY, wxDefaultPosition, size, 0L, validator)
{
}

bool ScreenBox::BuildList(bool bKeepSel)
{
	wxASSERT(m_pDB != NULL);
	if ( m_pDB == NULL ) return false; // nothing to do

	TScreen RS(m_pDB);
	wxUint64 curSel = 0; // not using if ==0

	try
	{
		RS.SetOrder(wxT("[ROWID]"));
		RS.Open();
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return false;
	}

	if ( bKeepSel && (GetCount() > 0) )
		curSel = GetSelectedLParam();

	StartOver();

	lkBoxInitList initList;
	initList.DeleteContents(true);

	if ( !RS.IsEmpty() )
	{
		wxString sQty;
		wxUint64 lParam;

		RS.MoveFirst();
		while ( !RS.IsEOF() )
		{
			if ( !(sQty = RS.GetScreenValue()).IsEmpty() )
			{
				lParam = RS.GetActualCurRow();
				// fill initlist
				initList.Append(new lkBoxInit(sQty, lParam));
			}
			RS.MoveNext();
		}
	}

	bool bRet = false;
	if ( InitializeFromList(&initList, true, lkExpandFlags::EXPAND_Center) )
		bRet = Populate();

	if ( bRet && (curSel > 0) )
		SetSelectedLParam(curSel);
	else
		SetSelection(0);

	return bRet;
}
