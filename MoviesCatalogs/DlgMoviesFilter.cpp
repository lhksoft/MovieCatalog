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
#include "DlgMoviesFilter.h"
#include "CMovies.h"

#include <wx/sizer.h>
#include <wx/valgen.h>
#include <wx/valtext.h>
#include <wx/stattext.h>
#include <wx/radiobut.h>
#include <wx/button.h>
#include <wx/statbox.h>
#include "../lkControls/lkTransTextCtrl.h"

#include "CategoryBox.h"
#include "FilterCtrls.h"
#include "GenresBox.h"

#include "../lkControls/lkColour.h"
#include "../lkControls/lkConfigTools.h"

#include "../lkSQLite3/lkSQL3Field.h"
#include "TMovies.h"

#include "../lkSQLite3/lkSQL3Exception.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class MoviesFilterDlg
////
wxIMPLEMENT_CLASS(MoviesFilterDlg, DlgFilter)

MoviesFilterDlg::MoviesFilterDlg(lkSQL3Database* DB) : DlgFilter(DB)
{
}

bool MoviesFilterDlg::Create(wxWindow* parent)
{
	wxSize size =
#ifdef __WXMSW__
		wxSize(520, 337)
#else
		wxSize(504, 332)
#endif
		;
	bool b = DlgFilter::Create(parent, wxT("Movies Filter"), size);

	size =
#ifdef __WXMSW__
		wxSize(504, 298)
#else
		wxSize(504, 308)
#endif
		;
	SetMinClientSize(size);
	return b;
}

//virtual
wxString MoviesFilterDlg::GetConfigSection() const
{
	return CMovies::GetConfigKeyString();
}

//virtual
void MoviesFilterDlg::AddCanvas()
{
	m_nID_MovieTitle = wxID_HIGHEST + wxWindowID(IDS::MovieTitle);
	m_nID_Category = wxID_HIGHEST + wxWindowID(IDS::Category);
	m_nID_Genres = wxID_HIGHEST + wxWindowID(IDS::Genres);

	wxBoxSizer* szMain = new wxBoxSizer(wxVERTICAL);
	wxSize rdSize(95, -1);

	/* ***********************************************************************************************
	 * First Group
	 * *********************************************************************************************** */
	{
		wxBoxSizer* szTop = new wxBoxSizer(wxHORIZONTAL);
		/* *******************************************************************************************
		 * Sorting
		 * ******************************************************************************************* */
		{
			wxBoxSizer* szLeft = new wxBoxSizer(wxVERTICAL);
			szLeft->AddStretchSpacer(1);
			szLeft->Add(new wxStaticText(this, wxID_ANY, wxT("Sorting"), wxDefaultPosition, wxDefaultSize, 0), 0, wxBOTTOM | wxEXPAND, 3);
			szLeft->Add(m_pSorting = new SortComboBox(this, lkImgComboBoxValidator(&m_nSort)), 0, wxRIGHT | wxEXPAND, 5);
			szLeft->AddStretchSpacer(1);
			szTop->Add(szLeft, 1, wxRIGHT | wxEXPAND, 5);
		}
		/* *******************************************************************************************
		 * Showing
		 * ******************************************************************************************* */
		{
			wxStaticBoxSizer* szMid = new wxStaticBoxSizer(wxVERTICAL, this, wxT(" Show "));
			wxWindow* box = szMid->GetStaticBox();
			szMid->Add(new wxRadioButton(box, wxID_ANY, wxT("All"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP, wxGenericValidator(&m_bShowAll)), 0, wxALL | wxEXPAND, 5);
			szMid->Add(new wxRadioButton(box, wxID_ANY, wxT("Movies only"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_bShowMovies)), 0, wxLEFT | wxRIGHT | wxEXPAND, 5);
			szMid->Add(new wxRadioButton(box, wxID_ANY, wxT("Series only"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_bShowSeries)), 0, wxALL | wxEXPAND, 5);
			szTop->Add(szMid, 0, wxRIGHT | wxEXPAND, 5);
		}
		/* *******************************************************************************************
		 * OK / Cancel
		 * ******************************************************************************************* */
		{
			wxBoxSizer* szRight = new wxBoxSizer(wxVERTICAL);
			szRight->AddStretchSpacer(1);
			wxButton* ok;
			szRight->Add(ok = new wxButton(this, wxID_OK), 0, wxBOTTOM, 5);
			ok->SetDefault();
			szRight->Add(new wxButton(this, wxID_CANCEL), 0);
			szRight->AddStretchSpacer(1);
			szTop->Add(szRight, 0, wxLEFT | wxEXPAND, 5);
		}
		szMain->Add(szTop, 0, wxLEFT | wxTOP | wxRIGHT | wxEXPAND, 10);
	}
	szMain->Add(0, 0, 0, wxBOTTOM | wxEXPAND, 5);
	/* ***********************************************************************************************
	 * Criteria Group
	 * *********************************************************************************************** */
	wxStaticBoxSizer* szBox = new wxStaticBoxSizer(wxVERTICAL, this, wxT(" Criteria "));

	/* ***********************************************************************************************
	 * Movies Group
	 * *********************************************************************************************** */
	wxWindow* parent = szBox->GetStaticBox();
	/* *******************************************************************************************
	 * by MovieTitle
	 * ******************************************************************************************* */
	{
		wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
		/* ***************************************************************************************
		 * radio-button
		 * *************************************************************************************** */
		{
			wxBoxSizer* szVer = new wxBoxSizer(wxVERTICAL);
			szVer->AddStretchSpacer(1);
			szVer->Add(m_pRd_MovieTitle = new wxRadioButton(parent, m_nID_MovieTitle, wxT("Movie Title"), wxDefaultPosition, rdSize,
															wxALIGN_RIGHT | wxRB_GROUP, wxGenericValidator(&m_bMovieTitle)), 0, wxFIXED_MINSIZE);
			szHor->Add(szVer, 0, wxRIGHT | wxEXPAND, 10);
		}
		/* ***************************************************************************************
		 * string to filter
		 * *************************************************************************************** */
		{
			wxBoxSizer* szVer = new wxBoxSizer(wxVERTICAL);
			szVer->Add(m_pFilterStrStat = new wxStaticText(parent, wxID_ANY, wxT("Filter-String :")), 0, wxBOTTOM | wxFIXED_MINSIZE, 3);
			szVer->Add(m_pFilterStr = new lkTransTextCtrl(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200, -1), 0, wxTextValidator(wxFILTER_EMPTY, &m_sFilter)), 0, wxEXPAND);
			m_pFilterStr->SetForegroundColour(lkCol_Yellow);
			szHor->Add(szVer, 1, wxRIGHT | wxEXPAND, 5);
		}
		/* ***************************************************************************************
		 * occurance
		 * *************************************************************************************** */
		{
			wxBoxSizer* szVer = new wxBoxSizer(wxVERTICAL);
			szVer->Add(m_pOccStat = new wxStaticText(parent, wxID_ANY, wxT("Occurance :")), 0, wxBOTTOM | wxFIXED_MINSIZE, 3);
			szVer->Add(m_pOccCombo = new lkImgComboBox(parent, wxID_ANY, wxDefaultSize, wxCB_READONLY, lkImgComboBoxValidator(&m_nOcc)), 0, wxEXPAND);
			szHor->Add(szVer, 0, wxEXPAND);
		}

		szBox->Add(szHor, 0, wxALL | wxEXPAND, 5);
	}
	/* *******************************************************************************************
	 * by Category
	 * ******************************************************************************************* */
	{
		wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
		/* ***************************************************************************************
		 * radio-button
		 * *************************************************************************************** */
		szHor->Add(m_pRd_Category = new wxRadioButton(parent, m_nID_Category, wxT("Category"), wxDefaultPosition, rdSize, wxALIGN_RIGHT,
													  wxGenericValidator(&m_bCategory)), 0, wxFIXED_MINSIZE | wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);
		/* ***************************************************************************************
		 * Category
		 * *************************************************************************************** */
		szHor->Add(m_pCat = new CategoryBox(parent, wxID_ANY, wxDefaultSize, wxCB_READONLY, lkImgComboBoxValidator(&m_nCat)), 1, wxRIGHT | wxEXPAND, 5);
		(dynamic_cast<CategoryBox*>(m_pCat))->SetDatabase(m_pDB);
		/* ***************************************************************************************
		 * SubCategory
		 * *************************************************************************************** */
		szHor->Add(m_pSubCat = new CategoryBox(parent, wxID_ANY, wxDefaultSize, wxCB_READONLY, lkImgComboBoxValidator(&m_nSubCat)), 1, wxEXPAND);
		(dynamic_cast<CategoryBox*>(m_pSubCat))->SetDatabase(m_pDB);
		(dynamic_cast<CategoryBox*>(m_pCat))->SetSubCatlist(static_cast<CategoryBox*>(m_pSubCat));

		szBox->Add(szHor, 0, wxLEFT | wxBOTTOM | wxRIGHT | wxEXPAND, 5);
	}
	/* *******************************************************************************************
	 * by Genres
	 * ******************************************************************************************* */
	{
		wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
		/* ***************************************************************************************
		 * radio-button
		 * *************************************************************************************** */
		{
			wxBoxSizer* szVer = new wxBoxSizer(wxVERTICAL);
			szVer->Add(m_pRd_Genres = new wxRadioButton(parent, m_nID_Genres, wxT("Genres"), wxDefaultPosition, rdSize, wxALIGN_RIGHT,
														wxGenericValidator(&m_bGenres)), 0, wxTOP | wxFIXED_MINSIZE, 3);
			szVer->AddStretchSpacer(1);
			szHor->Add(szVer, 0, wxRIGHT, 10);
		}
		/* ***************************************************************************************
		 * Genres Box
		 * *************************************************************************************** */
		wxSize size =
#ifdef __WXGTK__
			wxSize(-1, 72)
#else
			wxDefaultSize
#endif
			;
		szHor->Add(m_pGenres = new GenresBox(m_pDB, parent, wxID_ANY, wxDefaultPosition, size, 0, GenresValidator(&m_nGenres)), 1, wxEXPAND);

		szBox->Add(szHor, 0, wxLEFT | wxBOTTOM | wxRIGHT | wxEXPAND, 5);
	}

	szMain->Add(szBox, 1, wxLEFT | wxBOTTOM | wxRIGHT | wxEXPAND, 10);
	SetSizer(szMain);
}

//virtual
bool MoviesFilterDlg::TestFilter()
{
	wxASSERT(m_pDB && m_pDB->IsOpen());
	bool bRet = false;

	wxString qry;
	qry.Printf(wxT("SELECT COUNT([ROWID]) FROM [%s] WHERE %s;"), t3Movies, GetFilter());
	try
	{
		bRet = (((wxUint64)m_pDB->ExecuteSQLAndGet(qry)) > 0);
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		bRet = false;
	}

	return bRet;
}

//static
void MoviesFilterDlg::CorrectConfig(lkSQL3Database* dbTmp)
{
	if ( !dbTmp || !dbTmp->IsOpen() )
		return;

	wxString section = CMovies::GetConfigKeyString();
	DlgFilter::DoCorrectCatConfig(section, dbTmp);
	DlgFilter::DoCorrectGenConfig(section, dbTmp);
}
