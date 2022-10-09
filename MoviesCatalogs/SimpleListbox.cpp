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
#include "SimpleListbox.h"

#include "TCategory.h"
#include "TCountry.h"
#include "TGenre.h"
#include "TJudge.h"
#include "TLocation.h"
#include "TMedium.h"
#include "TOrigine.h"
#include "TQuality.h"
#include "TRating.h"
#include "TScreen.h"

#include "../lkControls/lkBoxInit.h"
#include "../lkSQLite3/lkSQL3Exception.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////
// class  PreviewListbox
////
wxIMPLEMENT_ABSTRACT_CLASS(PreviewListbox, lkChkImgListbox)

wxBEGIN_EVENT_TABLE(PreviewListbox, lkChkImgListbox)
	EVT_LEFT_DCLICK(PreviewListbox::OnDblClick)
wxEND_EVENT_TABLE()

PreviewListbox::PreviewListbox() : lkChkImgListbox()
{
	m_pDB = NULL;
	m_pDoc = NULL;
	// no checkboxes please
	ActAsCheckListbox(false);
}
PreviewListbox::PreviewListbox(lkSQL3Database* pDB, lkSQL3RecordDocument* pDoc) : lkChkImgListbox()
{
	m_pDB = pDB;
	m_pDoc = pDoc;
	// no checkboxes please
	ActAsCheckListbox(false);
}

void PreviewListbox::OnDblClick(wxMouseEvent& event)
{
	if ( IsEmpty() || !m_pDoc )
	{
		event.Skip();
		return;
	}

	wxPoint point = wxPoint(event.GetX(), event.GetY());
	// assume did not hit anything
	int nItem = -1;
	int cyItem = m_nBestHeight;
	int top = GetVisibleRowsBegin();
	nItem = top + point.y / cyItem;

	if ( nItem >= 0 )
	{
		lkSQL3MoveEvent* new_event = new lkSQL3MoveEvent(event.GetId());
		new_event->SetRow(GetLParam(nItem));
		wxQueueEvent(m_pDoc, new_event);
	}
	else
		event.Skip(); // SetSelection(nItem);
}

wxString PreviewListbox::GetOrder() const // will get it from the Document (if any was set)
{
	if ( m_pDoc )
		return m_pDoc->GetDefaultOrder();

	return wxString();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class  SubCatListbox
////
wxIMPLEMENT_CLASS(SubCatListbox, lkChkImgListbox)

SubCatListbox::SubCatListbox() : lkChkImgListbox()
{
	m_pDB = NULL;
	m_pCat = NULL;
	// no checkboxes please
	ActAsCheckListbox(false);
}
SubCatListbox::SubCatListbox(lkSQL3Database* pDB, wxUint32* nCat) : lkChkImgListbox()
{
	m_pDB = pDB;
	m_pCat = nCat;
	// no checkboxes please
	ActAsCheckListbox(false);
}
SubCatListbox::~SubCatListbox()
{
}

bool SubCatListbox::BuildList()
{
	wxASSERT(m_pDB != NULL);
	if ( (m_pDB == NULL) || !m_pDB->IsOpen() ) return false; // nothing to do
	wxASSERT(m_pCat != NULL);
	if ( !m_pCat ) return false;

	StartOver();

	TCategory RS;
	RS.SetDatabase(m_pDB);
	wxUint64 rowCnt = 0;

	try
	{
		wxString order, filter;
		order.Printf(wxT("[%s] COLLATE lkUTF8compare"), t3Category_NAME);
		filter.Printf(wxT("([%s] = %u) AND ([%s] != 0)"), t3Category_catID, *m_pCat, t3Category_subID);
		RS.SetOrder(order);
		RS.SetFilter(filter);
		RS.Open();
		rowCnt = RS.GetRowCount();
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return false;
	}

	if ( rowCnt == 0 )
	{
		Refresh();
		return true; // properly in ADD modus
	}

	lkBoxInitList* pInitList = NULL;
	if ( !RS.IsEmpty() )
	{
		wxImage			img;
		wxUint32		sub;
		wxString		name;
		wxUint64		lParam;

		pInitList = new lkBoxInitList();
		pInitList->DeleteContents(true);

		RS.MoveFirst();
		while ( !RS.IsEOF() )
		{
			name = RS.GetNameValue();
			img = RS.GetImage();
			sub = RS.GetSubValue();
			lParam = (wxUint64)sub;

			// fill initlist
			pInitList->Append(new lkBoxInit(name, img, lParam));
			RS.MoveNext();
		}
	}

	// populate listbox
	bool bRet = false;
	if ( pInitList != NULL )
		bRet = InitializeFromList(pInitList, true, lkExpandFlags::EXPAND_Center);

	// populate listbox
	if ( pInitList && !pInitList->IsEmpty() )
		Populate();

	delete pInitList;
	return bRet;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
// class  CountryListbox
////
wxIMPLEMENT_CLASS(CountryListbox, PreviewListbox)

CountryListbox::CountryListbox(lkSQL3Database* pDB, lkSQL3RecordDocument* pDoc) : PreviewListbox(pDB, pDoc)
{
	m_ExpandFlag = lkExpandFlags::EXPAND_Right;
}

bool CountryListbox::BuildList()
{
	wxASSERT(m_pDB != NULL);
	if ( (m_pDB == NULL) || !m_pDB->IsOpen() ) return false; // nothing to do

	StartOver();

	TCountry RS;
	RS.SetDatabase(m_pDB);
	wxUint64 rowCnt = 0;

	try
	{
//		wxString order;
//		order.Printf(wxT("ifnull([%s], [%s]) COLLATE lkUTF8compare"), t3Country_CUSTOM, t3Country_DEF);
		RS.SetOrder(GetOrder());
		RS.Open();
		rowCnt = RS.GetRowCount();
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return false;
	}

	if ( rowCnt == 0 )
		return true; // properly in ADD modus

	lkBoxInitList* pInitList = NULL;
	if ( !RS.IsEmpty() )
	{
		wxImage			flag;
		wxString		iso, country, s, nameFld;
		wxUint64		param;

		pInitList = new lkBoxInitList();
		pInitList->DeleteContents(true);

		RS.MoveFirst();
		while ( !RS.IsEOF() )
		{
			if ( !(s = RS.GetCustomIsoValue()).IsEmpty() )
				iso = s;
			else
				iso = RS.GetIsoValue();

			if ( !(s = RS.GetCustomCountryValue()).IsEmpty() )
				country = s;
			else
				country = RS.GetDefaultCountryValue();

			nameFld.Printf(wxT("%s (%s)"), country, iso);
			flag = RS.GetFlag();
			param = RS.GetCurRow();

			// fill initlist
			pInitList->Append(new lkBoxInit(nameFld, flag, param));
			RS.MoveNext();
		}
	}

	// populate listbox
	bool bRet = false;
	if ( pInitList != NULL )
		bRet = InitializeFromList(pInitList, true, m_ExpandFlag);

	// populate listbox
	if ( pInitList && bRet )
		Populate();

	delete pInitList;
	return bRet;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
// class  GenreListbox
////
wxIMPLEMENT_CLASS(GenreListbox, PreviewListbox)

GenreListbox::GenreListbox(lkSQL3Database* pDB, lkSQL3RecordDocument* pDoc) : PreviewListbox(pDB, pDoc)
{
}

bool GenreListbox::BuildList()
{
	wxASSERT(m_pDB != NULL);
	if ( (m_pDB == NULL) || !m_pDB->IsOpen() ) return false; // nothing to do

	StartOver();

	TGenre RS;
	RS.SetDatabase(m_pDB);
	wxUint64 rowCnt = 0;

	try
	{
//		wxString order;
//		order.Printf(wxT("[%s] COLLATE lkUTF8compare"), t3Genre_GENRE);
		RS.SetOrder(GetOrder());
		RS.Open();
		rowCnt = RS.GetRowCount();
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return false;
	}

	if ( rowCnt == 0 )
		return true; // properly in ADD modus

	lkBoxInitList* pInitList = NULL;
	if ( !RS.IsEmpty() )
	{
		wxImage			img;
		wxString		genre;
		wxUint64		param;

		pInitList = new lkBoxInitList();
		pInitList->DeleteContents(true);

		RS.MoveFirst();
		while ( !RS.IsEOF() )
		{
			if ( !(genre = RS.GetGenreValue()).IsEmpty() )
			{
				img = RS.GetImage();
				param = RS.GetCurRow();

				// fill initlist
				pInitList->Append(new lkBoxInit(genre, img, param));
			}

			RS.MoveNext();
		}
	}

	// populate listbox
	bool bRet = false;
	if ( pInitList != NULL )
		bRet = InitializeFromList(pInitList, true, lkExpandFlags::EXPAND_Center);

	// populate listbox
	if ( pInitList && bRet )
		Populate();

	delete pInitList;
	return bRet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class  JudgeListbox
////
wxIMPLEMENT_CLASS(JudgeListbox, PreviewListbox)

JudgeListbox::JudgeListbox(lkSQL3Database* pDB, lkSQL3RecordDocument* pDoc) : PreviewListbox(pDB, pDoc)
{
}

bool JudgeListbox::BuildList()
{
	wxASSERT(m_pDB != NULL);
	if ( (m_pDB == NULL) || !m_pDB->IsOpen() ) return false; // nothing to do

	StartOver();

	TJudge RS;
	RS.SetDatabase(m_pDB);
	wxUint64 rowCnt = 0;

	try
	{
		RS.SetOrder(GetOrder());
		RS.Open();
		rowCnt = RS.GetRowCount();
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return false;
	}

	if ( rowCnt == 0 )
		return true; // properly in ADD modus

	lkBoxInitList* pInitList = NULL;
	if ( !RS.IsEmpty() )
	{
		wxImage			img;
		wxString		judge;
		wxUint64		param;

		pInitList = new lkBoxInitList();
		pInitList->DeleteContents(true);

		RS.MoveFirst();
		while ( !RS.IsEOF() )
		{
			if ( !(judge = RS.GetJudgeValue()).IsEmpty() )
			{
				img = RS.GetImage();
				param = RS.GetCurRow();

				// fill initlist
				pInitList->Append(new lkBoxInit(judge, img, param));
			}

			RS.MoveNext();
		}
	}

	// populate listbox
	bool bRet = false;
	if ( pInitList != NULL )
		bRet = InitializeFromList(pInitList, true, lkExpandFlags::EXPAND_Right);

	// populate listbox
	if ( pInitList && bRet )
		Populate();

	delete pInitList;
	return bRet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class LocationListbox
////
wxIMPLEMENT_CLASS(LocationListbox, PreviewListbox)

LocationListbox::LocationListbox(lkSQL3Database* pDB, lkSQL3RecordDocument* pDoc) : PreviewListbox(pDB, pDoc)
{
}

bool LocationListbox::BuildList()
{
	wxASSERT(m_pDB != NULL);
	if ( (m_pDB == NULL) || !m_pDB->IsOpen() ) return false; // nothing to do

	StartOver();

	TLocation RS;
	RS.SetDatabase(m_pDB);
	wxUint64 rowCnt = 0;

	try
	{
		RS.SetOrder(GetOrder());
		RS.Open();
		rowCnt = RS.GetRowCount();
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return false;
	}

	if ( rowCnt == 0 )
		return true; // properly in ADD modus

	lkBoxInitList* pInitList = NULL;
	if ( !RS.IsEmpty() )
	{
		wxString		loc;
		wxUint64		param;

		pInitList = new lkBoxInitList();
		pInitList->DeleteContents(true);

		RS.MoveFirst();
		while ( !RS.IsEOF() )
		{
			if ( !(loc = RS.GetLocationValue()).IsEmpty() )
			{
				param = RS.GetCurRow();

				// fill initlist
				pInitList->Append(new lkBoxInit(loc, param));
			}

			RS.MoveNext();
		}
	}

	// populate listbox
	bool bRet = false;
	if ( pInitList != NULL )
		bRet = InitializeFromList(pInitList);

	// populate listbox
	if ( pInitList && bRet )
		Populate();

	delete pInitList;
	return bRet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class MediumListbox
////
wxIMPLEMENT_CLASS(MediumListbox, PreviewListbox)

MediumListbox::MediumListbox(lkSQL3Database* pDB, lkSQL3RecordDocument* pDoc) : PreviewListbox(pDB, pDoc)
{
}

bool MediumListbox::BuildList()
{
	wxASSERT(m_pDB != NULL);
	if ( (m_pDB == NULL) || !m_pDB->IsOpen() ) return false; // nothing to do

	StartOver();

	TMedium RS;
	RS.SetDatabase(m_pDB);
	wxUint64 rowCnt = 0;

	try
	{
		RS.SetOrder(GetOrder());
		RS.Open();
		rowCnt = RS.GetRowCount();
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return false;
	}

	if ( rowCnt == 0 )
		return true; // properly in ADD modus

	lkBoxInitList* pInitList = NULL;
	if ( !RS.IsEmpty() )
	{
		wxImage			img;
		wxString		medium;
		wxUint64		param;

		pInitList = new lkBoxInitList();
		pInitList->DeleteContents(true);

		RS.MoveFirst();
		while ( !RS.IsEOF() )
		{
			if ( (medium = RS.GetNameValue()).IsEmpty() )
				medium = RS.GetCodeValue();

			if ( !medium.IsEmpty() )
			{
				img = RS.GetImage();
				param = RS.GetCurRow();

				// fill initlist
				pInitList->Append(new lkBoxInit(medium, img, param));
			}

			RS.MoveNext();
		}
	}

	// populate listbox
	bool bRet = false;
	if ( pInitList != NULL )
		bRet = InitializeFromList(pInitList, true, lkExpandFlags::EXPAND_Right);

	// populate listbox
	if ( pInitList && bRet )
		Populate();

	delete pInitList;
	return bRet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class OrigineListbox
////
wxIMPLEMENT_CLASS(OrigineListbox, PreviewListbox)

OrigineListbox::OrigineListbox(lkSQL3Database* pDB, lkSQL3RecordDocument* pDoc) : PreviewListbox(pDB, pDoc)
{
}
bool OrigineListbox::BuildList()
{
	wxASSERT(m_pDB != NULL);
	if ( (m_pDB == NULL) || !m_pDB->IsOpen() ) return false; // nothing to do

	StartOver();

	TOrigine RS;
	RS.SetDatabase(m_pDB);
	wxUint64 rowCnt = 0;

	try
	{
		RS.SetOrder(GetOrder());
		RS.Open();
		rowCnt = RS.GetRowCount();
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return false;
	}

	if ( rowCnt == 0 )
		return true; // properly in ADD modus

	wxUint64 defRow = TOrigine::GetDefaultOrigine(m_pDB);

	lkBoxInitList* pInitList = NULL;
	if ( !RS.IsEmpty() )
	{
		wxString		orig, s;
		wxUint64		param;

		pInitList = new lkBoxInitList();
		pInitList->DeleteContents(true);

		RS.MoveFirst();
		while ( !RS.IsEOF() )
		{
			if ( !(orig = RS.GetOrigineValue()).IsEmpty() )
			{
				param = RS.GetCurRow();

				if ( defRow == RS.GetActualCurRow() )
					s.Printf(wxT("%s (*)"), orig);
				else
					s = orig;

				// fill initlist
				pInitList->Append(new lkBoxInit(s, param));
			}

			RS.MoveNext();
		}
	}

	// populate listbox
	bool bRet = false;
	if ( pInitList != NULL )
		bRet = InitializeFromList(pInitList);

	// populate listbox
	if ( pInitList && bRet )
		Populate();

	delete pInitList;
	return bRet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class QualityListbox
////
wxIMPLEMENT_CLASS(QualityListbox, PreviewListbox)

QualityListbox::QualityListbox(lkSQL3Database* pDB, lkSQL3RecordDocument* pDoc) : PreviewListbox(pDB, pDoc)
{
}
bool QualityListbox::BuildList()
{
	wxASSERT(m_pDB != NULL);
	if ( (m_pDB == NULL) || !m_pDB->IsOpen() ) return false; // nothing to do

	StartOver();

	TQuality RS;
	RS.SetDatabase(m_pDB);
	wxUint64 rowCnt = 0;

	try
	{
		RS.SetOrder(GetOrder());
		RS.Open();
		rowCnt = RS.GetRowCount();
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return false;
	}

	if ( rowCnt == 0 )
		return true; // properly in ADD modus

	wxUint64 defRow = TQuality::GetDefaultQuality(m_pDB);

	lkBoxInitList* pInitList = NULL;
	if ( !RS.IsEmpty() )
	{
		wxImage			img;
		wxString		qlty, s;
		wxUint64		param;

		pInitList = new lkBoxInitList();
		pInitList->DeleteContents(true);

		RS.MoveFirst();
		while ( !RS.IsEOF() )
		{
			if ( !(qlty = RS.GetQualityValue()).IsEmpty() )
			{
				img = RS.GetImage();
				param = RS.GetCurRow();

				if ( defRow == RS.GetActualCurRow() )
					s.Printf(wxT("%s (*)"), qlty);
				else
					s = qlty;

				// fill initlist
				pInitList->Append(new lkBoxInit(s, img, param));
			}

			RS.MoveNext();
		}
	}

	// populate listbox
	bool bRet = false;
	if ( pInitList != NULL )
		bRet = InitializeFromList(pInitList);

	// populate listbox
	if ( pInitList && bRet )
		Populate();

	delete pInitList;
	return bRet;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
// class RatingListbox
////
wxIMPLEMENT_CLASS(RatingListbox, PreviewListbox)

RatingListbox::RatingListbox(lkSQL3Database* pDB, lkSQL3RecordDocument* pDoc) : PreviewListbox(pDB, pDoc)
{
}
bool RatingListbox::BuildList()
{
	wxASSERT(m_pDB != NULL);
	if ( (m_pDB == NULL) || !m_pDB->IsOpen() ) return false; // nothing to do

	StartOver();

	TRating RS;
	RS.SetDatabase(m_pDB);
	wxUint64 rowCnt = 0;

	try
	{
//		wxString order; order.Printf(wxT("[%s]"), t3Rating_ID);
		RS.SetOrder(GetOrder());
		RS.Open();
		rowCnt = RS.GetRowCount();
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return false;
	}

	if ( rowCnt == 0 )
		return true; // properly in ADD modus

	lkBoxInitList* pInitList = NULL;
	if ( !RS.IsEmpty() )
	{
		wxImage			img;
		wxString		rtng;
		wxUint64		param;

		pInitList = new lkBoxInitList();
		pInitList->DeleteContents(true);

		RS.MoveFirst();
		while ( !RS.IsEOF() )
		{
			if ( !(rtng = RS.GetDescriptValue()).IsEmpty() )
			{
				img = RS.GetImage();
				param = RS.GetCurRow();

				// fill initlist
				pInitList->Append(new lkBoxInit(rtng, img, param));
			}

			RS.MoveNext();
		}
	}

	// populate listbox
	bool bRet = false;
	if ( pInitList != NULL )
		bRet = InitializeFromList(pInitList);

	// populate listbox
	if ( pInitList && bRet )
		Populate();

	delete pInitList;
	return bRet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class ScreenListbox
////
wxIMPLEMENT_CLASS(ScreenListbox, PreviewListbox)

ScreenListbox::ScreenListbox(lkSQL3Database* pDB, lkSQL3RecordDocument* pDoc) : PreviewListbox(pDB, pDoc)
{
}
bool ScreenListbox::BuildList()
{
	wxASSERT(m_pDB != NULL);
	if ( (m_pDB == NULL) || !m_pDB->IsOpen() ) return false; // nothing to do

	StartOver();

	TScreen RS;
	RS.SetDatabase(m_pDB);
	wxUint64 rowCnt = 0;

	try
	{
//		RS.SetOrder(wxT("[ROWID]"));
		RS.SetOrder(GetOrder());
		RS.Open();
		rowCnt = RS.GetRowCount();
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return false;
	}

	if ( rowCnt == 0 )
		return true; // properly in ADD modus

	wxUint64 defRow = TScreen::GetDefaultScreen(m_pDB);

	lkBoxInitList* pInitList = NULL;
	if ( !RS.IsEmpty() )
	{
		wxString		scr, s;
		wxUint64		param;

		pInitList = new lkBoxInitList();
		pInitList->DeleteContents(true);

		RS.MoveFirst();
		while ( !RS.IsEOF() )
		{
			if ( !(scr = RS.GetScreenValue()).IsEmpty() )
			{
				param = RS.GetCurRow();

				if ( defRow == RS.GetActualCurRow() )
					s.Printf(wxT("%s [*]"), scr);
				else
					s = scr;

				// fill initlist
				pInitList->Append(new lkBoxInit(s, param));
			}

			RS.MoveNext();
		}
	}

	// populate listbox
	bool bRet = false;
	if ( pInitList != NULL )
		bRet = InitializeFromList(pInitList);

	// populate listbox
	if ( pInitList && bRet )
		Populate();

	delete pInitList;
	return bRet;
}


