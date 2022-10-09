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
#include "DlgBaseFilter.h"
#include "VBase_private.h"

#include <wx/sizer.h>
#include <wx/valgen.h>
#include <wx/valtext.h>
#include <wx/stattext.h>
#include <wx/radiobut.h>
#include <wx/button.h>
#include <wx/statbox.h>
#include "../lkControls/lkTransTextCtrl.h"

#include "SimpleBox.h"
#include "CategoryBox.h"
#include "FilterCtrls.h"
#include "GenresBox.h"

#include "VBase_controls.h" // need 'BaseCountryBox' for Audio/Sub controls
#include "VBase_validators.h" // need 'lkAudioSubsValidator' for Audio/Sub controls

#include "../lkControls/lkColour.h"
#include "../lkControls/lkConfigTools.h"

#include "../lkSQLite3/lkSQL3Field.h"

#include "TBase.h"
#include "TGroup.h"
#include "TMovies.h"
#include "TStorage.h"

#include "../lkSQLite3/lkSQL3Exception.h"

#define config_FILTER_SortBase		wxT("FltSortBase")

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class BaseFilterDlg
////
wxIMPLEMENT_CLASS(BaseFilterDlg, DlgFilter)

wxBEGIN_EVENT_TABLE(BaseFilterDlg, DlgFilter)
	EVT_INIT_DIALOG(BaseFilterDlg::OnInitDialog)
wxEND_EVENT_TABLE()

BaseFilterDlg::BaseFilterDlg(lkSQL3Database* DB) : DlgFilter(DB)
{
	m_bSortMovies = m_bSortStorages = false;
}

bool BaseFilterDlg::Create(wxWindow* parent)
{
	wxSize size =
#ifdef __WXMSW__
		wxSize(520, 520)
#else
		wxSize(504, 538)
#endif
		;
	bool b = DlgFilter::Create(parent, wxT("Base Filter"), size);

	size =
#ifdef __WXMSW__
		wxSize(504, 481)
#else
		wxSize(504, 514)
#endif
		;
	SetMinClientSize(size);
	return b;
}

//virtual
bool BaseFilterDlg::TransferDataFromWindow()
{
	bool bRet = DlgFilter::TransferDataFromWindow();

	if ( bRet )
	{
		if ( m_bSortMovies )
			m_eSortBy = Sorting::Movies;
		else
			m_eSortBy = Sorting::Storage;
	}

	return bRet;
}

wxString BaseFilterDlg::GetQueryFrom() const
{
	wxString s;
	s.Printf(wxT("(([%s] LEFT JOIN [%s] ON [%s].[%s] = [%s].[ROWID]) LEFT JOIN [%s] ON [%s].[ROWID] = [%s].[%s]) LEFT JOIN [%s] ON [%s].[%s] = [%s].[ROWID] GROUP BY [%s].[ROWID]"),
			 t3Base, t3Movies, t3Base, t3Base_MOVIE, t3Movies, t3Group, t3Base, t3Group, t3Group_BASE, t3Storage, t3Group, t3Group_STORAGE, t3Storage, t3Base);
	return s;
}

wxString BaseFilterDlg::GetConfigSection() const
{
	return GetBaseSection();
}

//virtual
void BaseFilterDlg::AddCanvas()
{
	m_nID_MovieTitle = wxID_HIGHEST + wxWindowID(IDS::MovieTitle);
	m_nID_Storage = wxID_HIGHEST + wxWindowID(IDS::Storage);
	m_nID_Category = wxID_HIGHEST + wxWindowID(IDS::Category);
	m_nID_Location = wxID_HIGHEST + wxWindowID(IDS::Location);
	m_nID_Medium = wxID_HIGHEST + wxWindowID(IDS::Medium);
	m_nID_Genres = wxID_HIGHEST + wxWindowID(IDS::Genres);
	m_nID_Origine = wxID_HIGHEST + wxWindowID(IDS::Origine);
	m_nID_Audio = wxID_HIGHEST + wxWindowID(IDS::Audio);
	m_nID_Subs = wxID_HIGHEST + wxWindowID(IDS::Subs);
	m_nID_AandS = wxID_HIGHEST + wxWindowID(IDS::A_and_S);
	m_nID_AorS = wxID_HIGHEST + wxWindowID(IDS::A_or_S);

	wxBoxSizer* szMain = new wxBoxSizer(wxVERTICAL);

	wxSize rdSize(105, -1);

	/* ***********************************************************************************************
	 * First Group
	 * *********************************************************************************************** */
	{
		wxBoxSizer* szTop = new wxBoxSizer(wxHORIZONTAL);
		/* *******************************************************************************************
		 * Sorting
		 * ******************************************************************************************* */
		{
			wxStaticBoxSizer* szLeft = new wxStaticBoxSizer(wxVERTICAL, this, wxT(" Sorting "));
			wxWindow* box = szLeft->GetStaticBox();
			wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
			wxRadioButton* r;
			szHor->Add(r = new wxRadioButton(box, wxID_ANY, wxT("Movies"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP | wxALIGN_RIGHT, wxGenericValidator(&m_bSortMovies)), 1, wxRIGHT | wxEXPAND, 5);
			r->SetForegroundColour(MAKE_RGB(0x80,0xff,0xff));
			szHor->Add(r = new wxRadioButton(box, wxID_ANY, wxT("Storages"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_bSortStorages)), 1, wxEXPAND);
			r->SetForegroundColour(MAKE_RGB(0x80, 0xff, 0xff));
			szLeft->Add(szHor, 0, wxLEFT | wxBOTTOM | wxRIGHT | wxEXPAND, 5);
			szLeft->Add(m_pSorting = new SortComboBox(box, lkImgComboBoxValidator(&m_nSort)), 0, wxLEFT | wxBOTTOM | wxRIGHT | wxEXPAND, 5);

			wxBoxSizer* szVer = new wxBoxSizer(wxVERTICAL);
			szVer->AddStretchSpacer(1);
			szVer->Add(szLeft, 0, wxEXPAND);
			szVer->AddStretchSpacer(1);
			szTop->Add(szVer, 1, wxRIGHT | wxEXPAND, 5);
		}
		/* *******************************************************************************************
		 * Showing
		 * ******************************************************************************************* */
		{
			wxStaticBoxSizer* szMid = new wxStaticBoxSizer(wxVERTICAL, this, wxT(" Show "));
			wxWindow* box = szMid->GetStaticBox();
			wxRadioButton* r;
			szMid->Add(r = new wxRadioButton(box, wxID_ANY, wxT("All"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP, wxGenericValidator(&m_bShowAll)), 0, wxALL | wxEXPAND, 5);
			r->SetForegroundColour(lkCol_Yellow);
			szMid->Add(r = new wxRadioButton(box, wxID_ANY, wxT("Movies only"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_bShowMovies)), 0, wxLEFT | wxRIGHT | wxEXPAND, 5);
			r->SetForegroundColour(lkCol_Yellow);
			szMid->Add(r = new wxRadioButton(box, wxID_ANY, wxT("Series only"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_bShowSeries)), 0, wxALL | wxEXPAND, 5);
			r->SetForegroundColour(lkCol_Yellow);
			szTop->Add(szMid, 0, wxLEFT | wxRIGHT | wxEXPAND, 5);
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
	 * by MovieTitle & by Storage (combined)
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
															wxALIGN_RIGHT | wxRB_GROUP, wxGenericValidator(&m_bMovieTitle)), 0, wxFIXED_MINSIZE | wxBOTTOM, 5);
			szVer->Add(m_pRd_Storage = new wxRadioButton(parent, m_nID_Storage, wxT("Storage"), wxDefaultPosition, rdSize,
														 wxALIGN_RIGHT, wxGenericValidator(&m_bStorage)), 0, wxFIXED_MINSIZE);
			szVer->AddStretchSpacer(1);
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
	 * by Location
	 * ******************************************************************************************* */
	{
		wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
		/* ***************************************************************************************
		 * radio-button
		 * *************************************************************************************** */
		szHor->Add(m_pRd_Location = new wxRadioButton(parent, m_nID_Location, wxT("Location"), wxDefaultPosition, rdSize,
													  wxALIGN_RIGHT, wxGenericValidator(&m_bLocation)), 0, wxRIGHT | wxFIXED_MINSIZE | wxALIGN_CENTER_VERTICAL, 10);
		/* ***************************************************************************************
		 * Location Combo
		 * *************************************************************************************** */
		szHor->Add(m_pLocation = new LocationBox(parent, wxDefaultSize, lkImgComboBoxValidator(&m_nLocation)), 1, wxEXPAND);
		(dynamic_cast<LocationBox*>(m_pLocation))->SetDatabase(m_pDB);

		szBox->Add(szHor, 0, wxLEFT | wxBOTTOM | wxRIGHT | wxEXPAND, 5);
	}
	/* *******************************************************************************************
	 * by Medium
	 * ******************************************************************************************* */
	{
		wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
		/* ***************************************************************************************
		 * radio-button
		 * *************************************************************************************** */
		szHor->Add(m_pRd_Medium = new wxRadioButton(parent, m_nID_Medium, wxT("Medium"), wxDefaultPosition, rdSize,
													wxALIGN_RIGHT, wxGenericValidator(&m_bMedium)), 0, wxRIGHT | wxFIXED_MINSIZE | wxALIGN_CENTER_VERTICAL, 10);
		/* ***************************************************************************************
		 * Medium Combo
		 * *************************************************************************************** */
		szHor->Add(m_pMedium = new MediumBox(parent, wxDefaultSize, lkImgComboBoxValidator(&m_nMedium)), 1, wxEXPAND);
		(dynamic_cast<MediumBox*>(m_pMedium))->SetDatabase(m_pDB);

		szBox->Add(szHor, 0, wxLEFT | wxBOTTOM | wxRIGHT | wxEXPAND, 5);
	}
	/* *******************************************************************************************
	 * by Origine
	 * ******************************************************************************************* */
	{
		wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
		/* ***************************************************************************************
		 * radio-button
		 * *************************************************************************************** */
		szHor->Add(m_pRD_Origine = new wxRadioButton(parent, m_nID_Origine, wxT("Origine"), wxDefaultPosition, rdSize,
													 wxALIGN_RIGHT, wxGenericValidator(&m_bOrigine)), 0, wxRIGHT | wxFIXED_MINSIZE | wxALIGN_CENTER_VERTICAL, 10);
		/* ***************************************************************************************
		 * Origine Combo
		 * *************************************************************************************** */
		szHor->Add(m_pOrigine = new OrigineBox(parent, wxDefaultSize, lkImgComboBoxValidator(&m_nOrigine)), 1, wxEXPAND);
		(dynamic_cast<OrigineBox*>(m_pOrigine))->SetDatabase(m_pDB);

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
			szHor->Add(szVer, 0, wxRIGHT | wxEXPAND, 10);
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
	/* *******************************************************************************************
	 * by Audio / Sub
	 * ******************************************************************************************* */
	{
		wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
		/* ***************************************************************************************
		 * radio-button
		 * *************************************************************************************** */
		{
			wxBoxSizer* szVer = new wxBoxSizer(wxVERTICAL);
			szVer->AddStretchSpacer(2);
			szVer->Add(m_pRD_Audio = new wxRadioButton(parent, m_nID_Audio, wxT("Audio"), wxDefaultPosition, rdSize, wxALIGN_RIGHT,
													   wxGenericValidator(&m_bAudio)), 0, wxFIXED_MINSIZE);
			szVer->Add(m_pRD_Subs = new wxRadioButton(parent, m_nID_Subs, wxT("Subtitles"), wxDefaultPosition, rdSize, wxALIGN_RIGHT,
													  wxGenericValidator(&m_bSubs)), 0, wxTOP | wxFIXED_MINSIZE, 5);
			szVer->Add(m_pRD_AandS = new wxRadioButton(parent, m_nID_AandS, wxT("Audio AND Subs"), wxDefaultPosition, rdSize, wxALIGN_RIGHT,
													   wxGenericValidator(&m_bAandS)), 0, wxTOP | wxFIXED_MINSIZE, 5);
			szVer->Add(m_pRD_AorS = new wxRadioButton(parent, m_nID_AorS, wxT("Audio OR Subs"), wxDefaultPosition, rdSize, wxALIGN_RIGHT,
													  wxGenericValidator(&m_bAorS)), 0, wxTOP | wxFIXED_MINSIZE, 5);
			szVer->AddStretchSpacer(1);
			szHor->Add(szVer, 0, wxRIGHT | wxEXPAND, 10);
		}
		/* ***************************************************************************************
		 * Audio Box
		 * *************************************************************************************** */
		wxSize size =
#ifdef __WXGTK__
			wxSize(-1, 72)
#else
			wxDefaultSize
#endif
			;
		{
			wxBoxSizer* szVer = new wxBoxSizer(wxVERTICAL);
			szVer->Add(m_pAudioStat = new wxStaticText(parent, wxID_ANY, wxT("Audio :")), 0, wxBOTTOM | wxFIXED_MINSIZE, 3);
			szVer->Add(m_pAudio = new BaseCountryBox(m_pDB, true, parent, wxID_ANY, wxDefaultPosition, size, 0, lkAudioSubsValidator(&m_sA)), 1, wxEXPAND);
			szHor->Add(szVer, 1, wxRIGHT | wxEXPAND, 5);
		}
		/* ***************************************************************************************
		 * SubTitles Box
		 * *************************************************************************************** */
		{
			wxBoxSizer* szVer = new wxBoxSizer(wxVERTICAL);
			szVer->Add(m_pSubsStat = new wxStaticText(parent, wxID_ANY, wxT("Subtitles :")), 0, wxBOTTOM | wxFIXED_MINSIZE, 3);
			szVer->Add(m_pSubs = new BaseCountryBox(m_pDB, false, parent, wxID_ANY, wxDefaultPosition, size, 0, lkAudioSubsValidator(&m_sS)), 1, wxEXPAND);
			szHor->Add(szVer, 1, wxEXPAND);
		}

		szBox->Add(szHor, 0, wxLEFT | wxBOTTOM | wxRIGHT | wxEXPAND, 5);
	}

	szMain->Add(szBox, 1, wxLEFT | wxBOTTOM | wxRIGHT | wxEXPAND, 10);
	SetSizer(szMain);
}

void BaseFilterDlg::OnInitDialog(wxInitDialogEvent& event)
{
	// Getting defaults
	m_bSortMovies = true; // default

	wxString section = GetConfigSection();
	if ( !section.IsEmpty() )
	{
		bool b = GetConfigBool(section, config_FILTER_SortBase, false);

		m_bSortMovies = !b;
		m_bSortStorages = b;
	}

	DlgFilter::OnInitDialog(event);
}

//virtual
void BaseFilterDlg::SafeSettings() // safes what was been set in our ini file
{
	DlgFilter::SafeSettings();

	wxString section = GetConfigSection();
	if ( !section.IsEmpty() )
		SetConfigBool(section, config_FILTER_SortBase, m_bSortStorages);
}

//virtual
bool BaseFilterDlg::TestFilter()
{
	wxASSERT(m_pDB && m_pDB->IsOpen());
	bool bRet = false;

	wxString qry, s1, s2, s3;

	s2 = GetQueryFrom();

	s3 = GetFilter();

	s1.Printf(wxT("SELECT [%s].ROWID AS ID FROM %s HAVING %s"), t3Base, s2, s3);

	qry.Printf(wxT("SELECT COUNT([ID]) FROM (%s);"), s1);
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
void BaseFilterDlg::CorrectConfig(lkSQL3Database* dbTmp)
{
	if ( !dbTmp || !dbTmp->IsOpen() )
		return;

	wxString section = GetBaseSection();
	DlgFilter::DoCorrectCatConfig(section, dbTmp);
	DlgFilter::DoCorrectLocConfig(section, dbTmp);
	DlgFilter::DoCorrectMedConfig(section, dbTmp);
	DlgFilter::DoCorrectOrgConfig(section, dbTmp);
	DlgFilter::DoCorrectGenConfig(section, dbTmp);
}

