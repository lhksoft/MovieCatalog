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
#include "DlgFilter.h"
#include "Backgrounds.h"

#include "SimpleBox.h"
#include "CategoryBox.h"
#include "FilterCtrls.h"
#include "GenresBox.h"
#include "VBase_controls.h" // need 'BaseCountryBox' for Audio/Sub controls

#include <wx/msgdlg.h>

#include "../lkControls/lkImgComboBox.h"
#include "../lkSQLite3/lkSQL3RecordSet.h"
#include "../lkControls/lkColour.h"
#include "../lkControls/lkConfigTools.h"


#include "TBase.h"
#include "TGroup.h"
#include "TMovies.h"
#include "TStorage.h"

#include "TTemp.h"			// required for ::CorrectConfig
#include "TCategory.h"		// required for ::CorrectConfig
#include "TGenre.h"			// required for ::CorrectConfig
#include "TLocation.h"		// required for ::CorrectConfig
#include "TMedium.h"		// required for ::CorrectConfig
#include "TOrigine.h"		// required for ::CorrectConfig

#define config_FILTER_Sort			wxT("FltSort")
#define config_FILTER_Showing		wxT("FltShowing")
#define config_FILTER_Radio			wxT("FltChoice")
#define config_FILTER_Title			wxT("FltTitle")
#define config_FILTER_Occurance		wxT("FltOccurance")
#define config_FILTER_Category		wxT("FltCategory")
#define config_FILTER_SubCat		wxT("FltSubCat")
#define config_FILTER_Genres		wxT("FltGenres")
#define config_FILTER_Storage		wxT("FltStorage")
#define config_FILTER_StorageOcc	wxT("FltStorageOcc")
#define config_FILTER_Location		wxT("FltLocation")
#define config_FILTER_Medium		wxT("FltMedium")
#define config_FILTER_Origine		wxT("FltOrigine")
#define config_FILTER_Audio			wxT("FltAudio")
#define config_FILTER_Subs			wxT("FltSubs")
#define config_FILTER_AandS_A		wxT("FltAudAndSub_A")
#define config_FILTER_AandS_S		wxT("FltAudAndSub_S")
#define config_FILTER_AorS_A		wxT("FltAudOrSub_A")
#define config_FILTER_AorS_S		wxT("FltAudOrSub_S")



wxIMPLEMENT_CLASS(DlgFilter, lkDialog)

wxBEGIN_EVENT_TABLE(DlgFilter, lkDialog)
	EVT_INIT_DIALOG(DlgFilter::OnInitDialog)
	EVT_RADIOBUTTON(wxID_ANY, DlgFilter::OnRadioClick)
	EVT_BUTTON(wxID_ANY, DlgFilter::OnButton)
wxEND_EVENT_TABLE()

DlgFilter::DlgFilter(lkSQL3Database* pDB) : lkDialog(),
	m_sFilter(), m_sMovieTitle(), m_sStorage(), m_sA(), m_sS(), m_sAudio(), m_sSubs(),
	m_sAandS_A(), m_sAandS_S(), m_sAorS_A(), m_sAorS_S()
{
	m_pDB = pDB;

	m_eChoice = IDS::INVALID;
	m_eShowing = Showing::INVALID;
	m_eSortBy = Sorting::Movies;

	m_pSorting = NULL;
	m_nSort = 0;
	m_bShowAll = m_bShowMovies = m_bShowSeries = false;
	m_nOcc = m_nMovieOcc = m_nStorageOcc = 0;

	m_nCat = m_nSubCat = m_nGenres = m_nLocation = m_nMedium = m_nOrigine = 0;

	m_pFilterStrStat = m_pFilterStr = m_pOccStat = m_pOccCombo = NULL;
	m_pCat = m_pSubCat = m_pGenres = m_pLocation = m_pMedium = m_pOrigine = m_pAudio = m_pAudioStat = m_pSubs = m_pSubsStat = NULL;

	// RadioCtrls
	m_pRd_MovieTitle = m_pRd_Category = m_pRd_Genres = m_pRd_Storage = m_pRd_Location = m_pRd_Medium = m_pRD_Origine = m_pRD_Audio = m_pRD_Subs = m_pRD_AandS = m_pRD_AorS = NULL;
	m_nID_MovieTitle = m_nID_Category = m_nID_Genres = m_nID_Storage = m_nID_Location = m_nID_Medium = m_nID_Origine = m_nID_Audio = m_nID_Subs = m_nID_AandS = m_nID_AorS = 0;
	m_bMovieTitle = m_bCategory = m_bGenres = m_bStorage = m_bLocation = m_bMedium = m_bOrigine = m_bAudio = m_bSubs = m_bAandS = m_bAorS = false;

	m_nID_Prev = 0;
}

bool DlgFilter::Create(wxWindow* parent, const wxString& title, wxSize size)
{
	long style = wxDEFAULT_DIALOG_STYLE;
#ifdef __WXDEBUG__
	style |= wxRESIZE_BORDER;
#endif // __WXDEBUG__

	if ( !lkDialog::Create(parent, GetImage_FBG_FILTER(), wxID_ANY, title, wxDefaultPosition, size, style) )
		return false;
	SetForegroundColour(lkCol_White);

	AddCanvas();
	Layout();

	return true;
}

void DlgFilter::EnableRadio(wxWindowID isEnabledID)
{
	struct _radio_groups_
	{
		wxWindowID	nID[2];
		wxWindow* pWnds[4];
	} RadioGroups[] = {
		{ { wxWindowID(IDS::MovieTitle), wxWindowID(IDS::Storage) },	{ m_pFilterStrStat, m_pFilterStr, m_pOccStat, m_pOccCombo }},
		{ { wxWindowID(IDS::Category), -1 },							{ m_pCat, m_pSubCat, 0 }},
		{ { wxWindowID(IDS::Location), -1},								{ m_pLocation, 0 }},
		{ { wxWindowID(IDS::Medium), -1},								{ m_pMedium, 0 }},
		{ { wxWindowID(IDS::Genres), -1 },								{ m_pGenres, 0 }},
		{ { wxWindowID(IDS::Origine), -1 },								{ m_pOrigine, 0 }},
		{ { wxNOT_FOUND },												{ 0 }}
	};

	struct _radios_
	{
		wxWindowID	nID;
		wxWindow*	pWnd;
	} Radios[] = {
		{ wxWindowID(IDS::MovieTitle),			m_pRd_MovieTitle },
		{ wxWindowID(IDS::Storage),				m_pRd_Storage },
		{ wxWindowID(IDS::Category),			m_pRd_Category },
		{ wxWindowID(IDS::Genres),				m_pRd_Genres },
		{ wxWindowID(IDS::Location),			m_pRd_Location },
		{ wxWindowID(IDS::Medium),				m_pRd_Medium },
		{ wxWindowID(IDS::Origine),				m_pRD_Origine },
		{ wxWindowID(IDS::Audio),				m_pRD_Audio },
		{ wxWindowID(IDS::Subs),				m_pRD_Subs },
		{ wxWindowID(IDS::A_and_S),				m_pRD_AandS },
		{ wxWindowID(IDS::A_or_S),				m_pRD_AorS },
		{ wxNOT_FOUND, NULL }
	};

	int i, j;
	bool bEnable;
	for ( i = 0; RadioGroups[i].nID[0] != wxNOT_FOUND; i++ )
	{
		if ( !RadioGroups[i].pWnds[0] )
			continue;

		bEnable = false;
		for ( j = 0; j < 2 && RadioGroups[i].nID[j] != -1; j++ )
		{
			if ( RadioGroups[i].nID[j] == 0 )
				continue;
			if ( (bEnable = (RadioGroups[i].nID[j] == isEnabledID)) )
				break;
		}

		for ( j = 0; j < 4 && RadioGroups[i].pWnds[j]; j++ )
			if ( (bEnable && !RadioGroups[i].pWnds[j]->IsEnabled()) || // previous was disabled and should be enabled
				(!bEnable && RadioGroups[i].pWnds[j]->IsEnabled()) )   // previous was enabled and should be disabled
			{
				RadioGroups[i].pWnds[j]->Enable(bEnable);
				RadioGroups[i].pWnds[j]->Refresh();
			}
	}

	if ( m_pAudio )
	{
		bool bEnAudio = ((isEnabledID == wxWindowID(IDS::Audio)) || (isEnabledID == wxWindowID(IDS::A_and_S)) || (isEnabledID == wxWindowID(IDS::A_or_S)));
		if ( (bEnAudio && !m_pAudio->IsEnabled()) || (!bEnAudio && m_pAudio->IsEnabled()) )
		{
			m_pAudio->Enable(bEnAudio);
			m_pAudio->Refresh();
			if ( m_pAudioStat )
			{
				m_pAudioStat->Enable(bEnAudio);
				m_pAudioStat->Refresh();
			}
		}
	}
	if ( m_pSubs )
	{
		bool bEnSubs = ((isEnabledID == wxWindowID(IDS::Subs)) || (isEnabledID == wxWindowID(IDS::A_and_S)) || (isEnabledID == wxWindowID(IDS::A_or_S)));
		if ( (bEnSubs && !m_pSubs->IsEnabled()) || (!bEnSubs && m_pSubs->IsEnabled()) )
		{
			m_pSubs->Enable(bEnSubs);
			m_pSubs->Refresh();
			if ( m_pSubsStat )
			{
				m_pSubsStat->Enable(bEnSubs);
				m_pSubsStat->Refresh();
			}
		}
	}

	// Mark active Choice with a special color
	if ( m_nID_Prev != 0 )
		for ( i = 0; Radios[i].nID != wxNOT_FOUND; i++ )
			if ( Radios[i].nID == m_nID_Prev )
			{
				if ( Radios[i].pWnd )
				{
					Radios[i].pWnd->SetForegroundColour(GetForegroundColour());
					Radios[i].pWnd->Refresh();
				}
				break;
			}

	m_nID_Prev = isEnabledID;
	for ( i = 0; Radios[i].nID != wxNOT_FOUND; i++ )
		if ( Radios[i].nID == isEnabledID )
		{
			if ( Radios[i].pWnd )
			{
				Radios[i].pWnd->SetForegroundColour(MAKE_RGB(0xad, 0xff, 0x40)); // light lime
				Radios[i].pWnd->Refresh();
			}
			break;
		}
}

void DlgFilter::BuildOccuranceList()
{
	if ( m_pOccCombo )
	{
		struct _search_opt_
		{
			lkSQL3RecordSet::FIND_DIRECTION	nVal;
			const wxChar* sName;
		} SearchOpt[] = {
			{ lkSQL3RecordSet::FIND_DIRECTION::fndStart, wxT("Start") },
			{ lkSQL3RecordSet::FIND_DIRECTION::fndEnd, wxT("End") },
			{ lkSQL3RecordSet::FIND_DIRECTION::fndAnywhere, wxT("Anywhere") },
			{ lkSQL3RecordSet::FIND_DIRECTION::fndEntire, wxT("Entire") },
			{ lkSQL3RecordSet::FIND_DIRECTION::fndINVALID }
		};

		lkBoxInitList initList;
		initList.DeleteContents(true);
		for ( int i = 0; SearchOpt[i].nVal != lkSQL3RecordSet::FIND_DIRECTION::fndINVALID; i++ )
			initList.Append(new lkBoxInit(SearchOpt[i].sName, (wxUint64)SearchOpt[i].nVal));

		if ( (dynamic_cast<lkImgComboBox*>(m_pOccCombo))->InitializeFromList(&initList, true) )
			(dynamic_cast<lkImgComboBox*>(m_pOccCombo))->Populate();
	}
}

//virtual
void DlgFilter::SafeSettings() // safes what was been set in our ini file
{
	wxString section = GetConfigSection();
	if ( !section.IsEmpty() )
	{
		// Setting defaults
		if ( m_nSort != SortComboBox::NoSort )
			SetConfigInt(section, config_FILTER_Sort, m_nSort);
		else
			DeleteKey(section, config_FILTER_Sort);

		wxInt64 show = wxInt64(m_eShowing);
		if ( show != wxInt64(Showing::All) )
			SetConfigInt(section, config_FILTER_Showing, show);
		else
			DeleteKey(section, config_FILTER_Showing);

		wxInt64 choice = wxInt64(m_eChoice);
		if ( (m_pRd_MovieTitle && (choice != wxInt64(IDS::MovieTitle))) || (!m_pRd_MovieTitle && m_pRd_Storage && (choice != wxInt64(IDS::Storage))) )
			SetConfigInt(section, config_FILTER_Radio, choice);
		else
			DeleteKey(section, config_FILTER_Radio);

		//////////////////////////////////////////////////////////////////////////////

		if ( m_bStorage || m_bMovieTitle )
		{
			if ( m_pFilterStr && m_pFilterStr->IsEnabled() )
			{
				wxString key = (m_bStorage) ? config_FILTER_Storage : config_FILTER_Title;
				if ( !m_sFilter.IsEmpty() )
					SetConfigString(section, key, m_sFilter);
				else
					DeleteKey(section, key);
			}

			if ( m_pOccCombo && m_pOccCombo->IsEnabled() )
			{
				wxString key = (m_bStorage) ? config_FILTER_StorageOcc : config_FILTER_Occurance;
				if ( m_nOcc != (wxUint64)lkSQL3RecordSet::FIND_DIRECTION::fndStart )
					SetConfigInt(section, key, m_nOcc);
				else
					DeleteKey(section, key);
			}
		}

		if ( m_pCat && m_pCat->IsEnabled() )
		{
			SetConfigInt(section, config_FILTER_Category, m_nCat);
			if ( m_nCat == 0 )
				m_nSubCat = 0;

			SetConfigInt(section, config_FILTER_SubCat, m_nSubCat);
		}

		if ( m_pGenres && m_pGenres->IsEnabled() )
			SetConfigInt(section, config_FILTER_Genres, m_nGenres);

		if ( m_pLocation && m_pLocation->IsEnabled() )
		{
			if ( m_nLocation > 1 ) // 1 allways is default
				SetConfigInt(section, config_FILTER_Location, m_nLocation);
			else
				DeleteKey(section, config_FILTER_Location);
		}

		if ( m_pMedium && m_pMedium->IsEnabled() )
		{
			if ( m_nMedium > 1 ) // 1 allways is default
				SetConfigInt(section, config_FILTER_Medium, m_nMedium);
			else
				DeleteKey(section, config_FILTER_Medium);
		}

		if ( m_pOrigine && m_pOrigine->IsEnabled() )
		{
			if ( m_nOrigine > 1 ) // 1 allways is default
				SetConfigInt(section, config_FILTER_Origine, m_nOrigine);
			else
				DeleteKey(section, config_FILTER_Origine);
		}

		if ( m_pAudio && m_pAudio->IsEnabled() )
		{
			wxString key = (m_bAandS) ? config_FILTER_AandS_A : (m_bAorS) ? config_FILTER_AorS_A : config_FILTER_Audio;
			if ( !m_sA.IsEmpty() )
				SetConfigString(section, key, m_sA);
			else
				DeleteKey(section, key);
		}

		if ( m_pSubs && m_pSubs->IsEnabled() )
		{
			wxString key = (m_bAandS) ? config_FILTER_AandS_S : (m_bAorS) ? config_FILTER_AorS_S : config_FILTER_Subs;
			if ( !m_sS.IsEmpty() )
				SetConfigString(section, key, m_sS);
			else
				DeleteKey(section, key);
		}
	}
}

//static
void DlgFilter::DoCorrectCatConfig(const wxString& section, lkSQL3Database* dbTmp)
{
	if ( section.IsEmpty() || !dbTmp || !dbTmp->IsOpen() )
		return;

	wxUint64 oCat, nCat = 0, oSub, nSub = 0;
	oCat = GetConfigInt(section, config_FILTER_Category);
	oSub = GetConfigInt(section, config_FILTER_SubCat);
	if ( oCat > 0 )
	{
		nCat = TTempCat::FindNewID(dbTmp, t3Category, oCat, 0);
		if ( oSub > 0 )
			nSub = TTempCat::FindNewID(dbTmp, t3Category, oCat, oSub);
	}

	SetConfigInt(section, config_FILTER_Category, nCat);
	SetConfigInt(section, config_FILTER_SubCat, nSub);
}

//static
void DlgFilter::DoCorrectGenConfig(const wxString& section, lkSQL3Database* dbTmp)
{
	if ( section.IsEmpty() || !dbTmp || !dbTmp->IsOpen() )
		return;

	wxUint64 oGenre, nGenre = 0;
	if ( (oGenre = GetConfigInt(section, config_FILTER_Genres)) > 0 )
		nGenre = TTemp::FindNewGenre(dbTmp, t3Genre, oGenre);
	SetConfigInt(section, config_FILTER_Genres, nGenre);
}

//static
void DlgFilter::DoCorrectLocConfig(const wxString& section, lkSQL3Database* dbTmp)
{
	if ( section.IsEmpty() || !dbTmp || !dbTmp->IsOpen() )
		return;

	wxUint64 oLoc, nLoc = 0;
	oLoc = GetConfigInt(section, config_FILTER_Location);
	if ( oLoc > 1 )
		nLoc = TTemp::FindNewID(dbTmp, t3Location, oLoc);

	if ( nLoc > 1 ) // 1 allways is default
		SetConfigInt(section, config_FILTER_Location, nLoc);
	else
		DeleteKey(section, config_FILTER_Location);
}

//static
void DlgFilter::DoCorrectMedConfig(const wxString& section, lkSQL3Database* dbTmp)
{
	if ( section.IsEmpty() || !dbTmp || !dbTmp->IsOpen() )
		return;

	wxUint64 oMed, nMed = 0;
	oMed = GetConfigInt(section, config_FILTER_Medium);
	if ( oMed > 1 )
		nMed = TTemp::FindNewID(dbTmp, t3Medium, oMed);

	if ( nMed > 1 ) // 1 allways is default
		SetConfigInt(section, config_FILTER_Medium, nMed);
	else
		DeleteKey(section, config_FILTER_Medium);
}

//static
void DlgFilter::DoCorrectOrgConfig(const wxString& section, lkSQL3Database* dbTmp)
{
	if ( section.IsEmpty() || !dbTmp || !dbTmp->IsOpen() )
		return;

	wxUint64 oOrg, nOrg = 1;
	oOrg = GetConfigInt(section, config_FILTER_Origine);
	if ( oOrg > 1 )
		nOrg = TTemp::FindNewID(dbTmp, t3Orig, oOrg);

	if ( nOrg > 1 ) // 1 allways is default
		SetConfigInt(section, config_FILTER_Origine, nOrg);
	else
		DeleteKey(section, config_FILTER_Origine);
}

//virtual
bool DlgFilter::TransferDataFromWindow()
{
	bool bRet = lkDialog::TransferDataFromWindow();

	if ( bRet )
	{
		m_eChoice = IDS::INVALID;
		if ( m_bAorS )
			m_eChoice = IDS::A_or_S;
		else if ( m_bAandS )
			m_eChoice = IDS::A_and_S;
		else if ( m_bSubs )
			m_eChoice = IDS::Subs;
		else if ( m_bAudio )
			m_eChoice = IDS::Audio;
		else if ( m_bOrigine )
			m_eChoice = IDS::Origine;
		else if ( m_bMedium )
			m_eChoice = IDS::Medium;
		else if ( m_bLocation )
			m_eChoice = IDS::Location;
		else if ( m_bGenres )
			m_eChoice = IDS::Genres;
		else if ( m_bCategory )
			m_eChoice = IDS::Category;
		else if ( m_bStorage )
			m_eChoice = IDS::Storage;
		else if ( m_bMovieTitle )
			m_eChoice = IDS::MovieTitle;

		if ( m_eChoice == IDS::INVALID )
		{
			if ( m_pRd_Storage )
				m_eChoice = IDS::Storage;
			else if ( m_pRd_MovieTitle )
				m_eChoice = IDS::MovieTitle;
		}

		if ( m_bShowMovies )
			m_eShowing = Showing::Movies;
		else if ( m_bShowSeries )
			m_eShowing = Showing::Series;
		else
			m_eShowing = Showing::All;

		if ( m_pFilterStr && m_pFilterStr->IsEnabled() )
		{
			if ( m_bStorage )
				m_sStorage = m_sFilter;
			else // if ( m_bMovieTitle )
				m_sMovieTitle = m_sFilter;
		}

		if ( m_pOccCombo && m_pOccCombo->IsEnabled() )
		{
			if ( m_bStorage )
				m_nStorageOcc = m_nOcc;
			else // if ( m_bMovieTitle )
				m_nMovieOcc = m_nOcc;
		}
	}

	return bRet;
}

void DlgFilter::OnInitDialog(wxInitDialogEvent& event)
{
	if ( m_pSorting )
		(dynamic_cast<SortComboBox*>(m_pSorting))->BuildList();
	BuildOccuranceList();
	if ( m_pCat )
		(dynamic_cast<CategoryBox*>(m_pCat))->BuildCatList();
	if ( m_pLocation )
		(dynamic_cast<LocationBox*>(m_pLocation))->BuildList();
	if ( m_pMedium )
		(dynamic_cast<MediumBox*>(m_pMedium))->BuildList();
	if ( m_pGenres )
		(dynamic_cast<GenresBox*>(m_pGenres))->BuildList();
	if ( m_pOrigine )
		(dynamic_cast<OrigineBox*>(m_pOrigine))->BuildList();
	if ( m_pAudio )
		(dynamic_cast<BaseCountryBox*>(m_pAudio))->BuildList(lkExpandFlags::EXPAND_Right);
	if ( m_pSubs )
		(dynamic_cast<BaseCountryBox*>(m_pSubs))->BuildList(lkExpandFlags::EXPAND_Right);

	// Getting defaults
	m_eChoice = (m_pRd_Storage && !m_pRd_MovieTitle) ? IDS::Storage : IDS::MovieTitle;
	m_nSort = SortComboBox::NoSort; // default : No Sort
	m_eShowing = Showing::All;
	m_nMovieOcc = (wxUint64)lkSQL3RecordSet::FIND_DIRECTION::fndStart; // default
	m_nStorageOcc = (wxUint64)lkSQL3RecordSet::FIND_DIRECTION::fndStart; // default
	m_nCat = m_nSubCat = 0; // default : <unset>
	m_nLocation = 1; // default : "Kast"
	m_nMedium = 1; // default : "VHS" (first in line)
	m_nGenres = 0; // default to <unset>
	m_nOrigine = 1; // default to 1st in list

	wxString section = GetConfigSection();
	if ( !section.IsEmpty() )
	{
		wxInt64 i = GetConfigInt(section, config_FILTER_Sort);
		if ( (i == SortComboBox::NoSort) || (i == SortComboBox::Ascending) || (i == SortComboBox::Descending) )
			m_nSort = i;

		i = GetConfigInt(section, config_FILTER_Showing);
		if ( i == (wxInt64)Showing::Movies )
			m_eShowing = Showing::Movies;
		else if ( i == (wxInt64)Showing::Series )
			m_eShowing = Showing::Series;
		else
			m_eShowing = Showing::All;

		i = GetConfigInt(section, config_FILTER_Radio);
		if ( i == wxInt64(IDS::Genres) )
			m_eChoice = IDS::Genres;
		else if ( i == wxInt64(IDS::Medium) )
			m_eChoice = IDS::Medium;
		else if ( i == wxInt64(IDS::Location) )
			m_eChoice = IDS::Location;
		else if ( i == wxInt64(IDS::Category) )
			m_eChoice = IDS::Category;
		else if ( i == wxInt64(IDS::Storage) )
			m_eChoice = IDS::Storage;
		else if ( i == wxInt64(IDS::Origine) )
			m_eChoice = IDS::Origine;
		else if ( i == wxInt64(IDS::Audio) )
			m_eChoice = IDS::Audio;
		else if ( i == wxInt64(IDS::Subs) )
			m_eChoice = IDS::Subs;
		else if ( i == wxInt64(IDS::A_and_S) )
			m_eChoice = IDS::A_and_S;
		else if ( i == wxInt64(IDS::A_or_S) )
			m_eChoice = IDS::A_or_S;
		else
			m_eChoice = (m_pRd_Storage && !m_pRd_MovieTitle) ? IDS::Storage : IDS::MovieTitle;

		// Movies / Storages
		//////////////////////////////////////////////////////////////////////////////
		if ( m_pFilterStr )
		{
			m_sMovieTitle = GetConfigString(section, config_FILTER_Title);
			m_sStorage = GetConfigString(section, config_FILTER_Storage);
		}

		if ( m_pOccCombo )
		{
			i = GetConfigInt(section, config_FILTER_Occurance);
			if ( (i == (wxInt64)lkSQL3RecordSet::FIND_DIRECTION::fndStart) || (i == (wxInt64)lkSQL3RecordSet::FIND_DIRECTION::fndEnd) ||
				(i == (wxInt64)lkSQL3RecordSet::FIND_DIRECTION::fndAnywhere) || (i == (wxInt64)lkSQL3RecordSet::FIND_DIRECTION::fndEntire) )
				m_nMovieOcc = (wxUint64)i;

			i = GetConfigInt(section, config_FILTER_StorageOcc);
			if ( (i == (wxInt64)lkSQL3RecordSet::FIND_DIRECTION::fndStart) || (i == (wxInt64)lkSQL3RecordSet::FIND_DIRECTION::fndEnd) ||
				(i == (wxInt64)lkSQL3RecordSet::FIND_DIRECTION::fndAnywhere) || (i == (wxInt64)lkSQL3RecordSet::FIND_DIRECTION::fndEntire) )
				m_nStorageOcc = (wxUint64)i;
		}

		// Movies
		//////////////////////////////////////////////////////////////////////////////
		if ( m_pCat )
			if ( (i = GetConfigInt(section, config_FILTER_Category)) > 0 )
				m_nCat = (wxUint64)i;

		if ( (m_nCat > 0) && m_pSubCat )
			if ( (i = GetConfigInt(section, config_FILTER_SubCat)) > 0 )
				m_nSubCat = (wxUint64)i;

		if ( m_pGenres )
			if ( (i = GetConfigInt(section, config_FILTER_Genres)) > 0 )
				m_nGenres = (wxUint64)i;

		// Storages
		//////////////////////////////////////////////////////////////////////////////
		if ( m_pLocation )
			if ( (i = GetConfigInt(section, config_FILTER_Location)) > 1 )
				m_nLocation = (wxUint64)i;

		if ( m_pMedium )
			if ( (i = GetConfigInt(section, config_FILTER_Medium)) > 1 )
				m_nMedium = (wxUint64)i;

		// Base
		//////////////////////////////////////////////////////////////////////////////
		if ( m_pOrigine )
			if ( (i = GetConfigInt(section, config_FILTER_Origine)) > 1 )
				m_nOrigine = (wxUint64)i;

		if ( m_pAudio && m_pSubs )
		{
			m_sAudio = GetConfigString(section, config_FILTER_Audio);
			m_sSubs = GetConfigString(section, config_FILTER_Subs);
			m_sAandS_A = GetConfigString(section, config_FILTER_AandS_A);
			m_sAandS_S = GetConfigString(section, config_FILTER_AandS_S);
			m_sAorS_A = GetConfigString(section, config_FILTER_AorS_A);
			m_sAorS_S = GetConfigString(section, config_FILTER_AorS_S);
		}
	}

	m_bShowAll = (m_eShowing == Showing::All);
	m_bShowMovies = (m_eShowing == Showing::Movies);
	m_bShowSeries = (m_eShowing == Showing::Series);

	m_bMovieTitle = (m_eChoice == IDS::MovieTitle);
	m_bStorage = (m_eChoice == IDS::Storage);
	m_bCategory = (m_eChoice == IDS::Category);
	m_bLocation = (m_eChoice == IDS::Location);
	m_bMedium = (m_eChoice == IDS::Medium);
	m_bGenres = (m_eChoice == IDS::Genres);
	m_bOrigine = (m_eChoice == IDS::Origine);
	m_bAudio = (m_eChoice == IDS::Audio);
	m_bSubs = (m_eChoice == IDS::Subs);
	m_bAandS = (m_eChoice == IDS::A_and_S);
	m_bAorS = (m_eChoice == IDS::A_or_S);

	if (( m_pRd_Storage && !m_pRd_MovieTitle ) || (m_pRd_Storage && m_pRd_MovieTitle && m_bStorage))
	{
		m_sFilter = m_sStorage;
		m_nOcc = m_nStorageOcc;
	}
	else // if (( !m_pRd_Storage && m_pRd_MovieTitle ) || (m_pRd_Storage && m_pRd_MovieTitle && m_bMovieTitle) )
	{
		m_sFilter = m_sMovieTitle;
		m_nOcc = m_nMovieOcc;
	}

	m_sA = m_sAudio;
	m_sS = m_sSubs;
	if ( m_bAandS )
	{
		m_sA = m_sAandS_A;
		m_sS = m_sAandS_S;
	}
	else if ( m_bAorS )
	{
		m_sA = m_sAorS_A;
		m_sS = m_sAorS_S;
	}

	lkDialog::OnInitDialog(event);

	wxWindowID id = (m_bCategory) ? wxWindowID(IDS::Category) : (m_bGenres) ? wxWindowID(IDS::Genres) : (m_bLocation) ? wxWindowID(IDS::Location) :
		(m_bMedium) ? wxWindowID(IDS::Medium) : (m_bStorage) ? wxWindowID(IDS::Storage) : (m_bOrigine) ? wxWindowID(IDS::Origine) :
		(m_bAudio) ? wxWindowID(IDS::Audio) : (m_bSubs) ? wxWindowID(IDS::Subs) : (m_bAandS) ? wxWindowID(IDS::A_and_S) : (m_bAorS) ? wxWindowID(IDS::A_or_S) : wxWindowID(IDS::MovieTitle);
	EnableRadio(id);
}

void DlgFilter::OnRadioClick(wxCommandEvent& event)
{
	if ( event.GetId() < (wxID_HIGHEST + 1) )
	{
		event.Skip();
		return;
	}
	//else..

	// if previous was MovieTitle or Storage, safe whatever is been set
	if ( m_bMovieTitle || m_bStorage )
	{
		if ( m_pFilterStr )
		{
			m_pFilterStr->TransferDataFromWindow();
			if ( m_bMovieTitle )
				m_sMovieTitle = m_sFilter;
			else
				m_sStorage = m_sFilter;
		}

		if ( m_pOccCombo )
		{
			m_pOccCombo->TransferDataFromWindow();
			if ( m_bMovieTitle )
				m_nMovieOcc = m_nOcc;
			else
				m_nStorageOcc = m_nOcc;
		}
	}

	if ( m_bAudio || m_bAandS || m_bAorS )
	{
		if ( m_pAudio )
		{
			m_pAudio->TransferDataFromWindow();
			if ( m_bAorS )
				m_sAorS_A = m_sA;
			else if ( m_bAandS )
				m_sAandS_A = m_sA;
			else // if ( m_bAudio )
				m_sAudio = m_sA;
		}
	}

	if ( m_bSubs || m_bAandS || m_bAorS )
	{
		if ( m_pSubs )
		{
			m_pSubs->TransferDataFromWindow();
			if ( m_bAorS )
				m_sAorS_S = m_sS;
			else if ( m_bAandS )
				m_sAandS_S = m_sS;
			else // if ( m_bSubs )
				m_sSubs = m_sS;
		}
	}

	EnableRadio(event.GetId() - wxID_HIGHEST);

	if ( m_pRd_MovieTitle )
		m_pRd_MovieTitle->TransferDataFromWindow();
	if ( m_pRd_Storage )
		m_pRd_Storage->TransferDataFromWindow();
	if ( m_pRD_Audio )
		m_pRD_Audio->TransferDataFromWindow();
	if ( m_pRD_Subs )
		m_pRD_Subs->TransferDataFromWindow();
	if ( m_pRD_AandS )
		m_pRD_AandS->TransferDataFromWindow();
	if ( m_pRD_AorS )
		m_pRD_AorS->TransferDataFromWindow();

	// if now is MovieTitle or Storage, restore whatever was been set
	if ( m_bMovieTitle || m_bStorage )
	{
		if ( m_pFilterStr )
		{
			if ( m_bMovieTitle )
				m_sFilter = m_sMovieTitle;
			else
				m_sFilter = m_sStorage;
			m_pFilterStr->TransferDataToWindow();
		}

		if ( m_pOccCombo )
		{
			if ( m_bMovieTitle )
				m_nOcc = m_nMovieOcc;
			else
				m_nOcc = m_nStorageOcc;
			m_pOccCombo->TransferDataToWindow();
		}
	}

	if ( m_bAudio || m_bAandS || m_bAorS )
	{
		if ( m_pAudio )
		{
			if ( m_bAorS )
				m_sA = m_sAorS_A;
			else if ( m_bAandS )
				m_sA = m_sAandS_A;
			else // if ( m_bAudio )
				m_sA = m_sAudio;
			m_pAudio->TransferDataToWindow();
		}
	}

	if ( m_bSubs || m_bAandS || m_bAorS )
	{
		if ( m_pSubs )
		{
			if ( m_bAorS )
				m_sS = m_sAorS_S;
			else if ( m_bAandS )
				m_sS = m_sAandS_S;
			else // if ( m_bSubs )
				m_sS = m_sSubs;
			m_pSubs->TransferDataToWindow();
		}
	}
}

void DlgFilter::OnButton(wxCommandEvent& event)
{
	const int id = event.GetId();
	if ( id == GetAffirmativeId() )
	{
		if ( !Validate() )
			return;

		if ( TransferDataFromWindow() )
		{
			if ( !TestFilter() )
				wxMessageBox(wxT("Nothing found with specified filter.\nRephrase your filter."), wxT("Filter"), wxICON_EXCLAMATION | wxOK);
			else
			{
				SafeSettings();
				EndDialog(m_affirmativeId);
			}
			return;
		}
	}
	event.Skip();
}

wxString DlgFilter::GetFilter() const
{
	wxString s1, s2;
	if ( m_bMovieTitle )
	{
		wxString corrStr = m_sMovieTitle;
		corrStr.Replace(wxT("'"), wxT("''"), true);
		if ( m_nMovieOcc == wxUint64(lkSQL3RecordSet::FIND_DIRECTION::fndStart) )
			s2 = wxT("(([%s].[%s] LIKE '%s%%') OR ([%s].[%s] LIKE '%s%%') OR ([%s].[%s] LIKE '%s%%'))");
		else if ( m_nMovieOcc == wxUint64(lkSQL3RecordSet::FIND_DIRECTION::fndEnd))
			s2 = wxT("(([%s].[%s] LIKE '%%%s') OR ([%s].[%s] LIKE '%%%s') OR ([%s].[%s] LIKE '%%%s'))");
		else if ( m_nMovieOcc == wxUint64(lkSQL3RecordSet::FIND_DIRECTION::fndAnywhere))
			s2 = wxT("(([%s].[%s] LIKE '%%%s%%') OR ([%s].[%s] LIKE '%%%s%%') OR ([%s].[%s] LIKE '%%%s%%'))");
		else // if ( m_nMovieOcc == wxUint64(lkSQL3RecordSet::FIND_DIRECTION::fndEntire))
			s2 = wxT("((LOWER([%s].[%s]) = LOWER('%s')) OR (LOWER([%s].[%s]) = LOWER('%s')) OR (LOWER([%s].[%s]) = LOWER('%s')))");

		s1.Printf(s2, t3Movies, t3Movies_TITLE, corrStr, t3Movies, t3Movies_SUBTITLE, corrStr, t3Movies, t3Movies_AKATITLE, corrStr);
	}
	else if ( m_bStorage )
	{
		if ( m_nStorageOcc == wxUint64(lkSQL3RecordSet::FIND_DIRECTION::fndStart) )
			s2 = wxT("([%s].[%s] LIKE '%s%%')");
		else if ( m_nStorageOcc == wxUint64(lkSQL3RecordSet::FIND_DIRECTION::fndEnd) )
			s2 = wxT("([%s].[%s] LIKE '%%%s')");
		else if ( m_nStorageOcc == wxUint64(lkSQL3RecordSet::FIND_DIRECTION::fndAnywhere) )
			s2 = wxT("([%s].[%s] LIKE '%%%s%%')");
		else // if ( m_nStorageOcc == wxUint64(lkSQL3RecordSet::FIND_DIRECTION::fndEntire))
			s2 = wxT("(LOWER([%s].[%s]) = LOWER('%s'))");

		s1.Printf(s2, t3Storage, t3Storage_NAME, m_sStorage);
	}
	else if ( m_bCategory )
	{
		if ( m_nSubCat == 0 )
			s1.Printf(wxT("([%s].[%s] = %I64u)"), t3Movies, t3Movies_CAT, m_nCat);
		else
			s1.Printf(wxT("(([%s].[%s] = %I64u) AND ([%s].[%s] = %I64u))"), t3Movies, t3Movies_CAT, m_nCat, t3Movies, t3Movies_SUBCAT, m_nSubCat);
	}
	else if ( m_bGenres )
	{
		if ( m_nGenres != 0 )
			s1.Printf(wxT("(([%s].[%s] & %I64u) = %I64u)"), t3Movies, t3Movies_GENRE, m_nGenres, m_nGenres);
		else
			s1.Printf(wxT("([%s].[%s] = 0)"), t3Movies, t3Movies_GENRE);
	}
	else if ( m_bLocation )
	{
		s1.Printf(wxT("([%s].[%s] = %I64u)"), t3Storage, t3Storage_LOCATION, m_nLocation);
	}
	else if ( m_bMedium )
	{
		s1.Printf(wxT("([%s].[%s] = %I64u)"), t3Storage, t3Storage_MEDIUM, m_nMedium);
	}
	else if ( m_bOrigine )
	{
		s1.Printf(wxT("([%s].[%s] = %I64u)"), t3Base, t3Base_ORIGINE, m_nOrigine);
	}
	else if ( m_bAudio )
	{
		wxASSERT(m_pAudio);
		s2.Printf(wxT("[%s].[%s]"), t3Base, t3Base_AUDIO);
		s1 = (dynamic_cast<BaseCountryBox*>(m_pAudio))->GetFilter(s2);
	}
	else if ( m_bSubs )
	{
		wxASSERT(m_pSubs);
		s2.Printf(wxT("[%s].[%s]"), t3Base, t3Base_SUBS);
		s1 = (dynamic_cast<BaseCountryBox*>(m_pSubs))->GetFilter(s2);
	}
	else if ( m_bAandS )
	{
		wxASSERT(m_pAudio && m_pSubs);
		wxString s3;
		s1.Printf(wxT("[%s].[%s]"), t3Base, t3Base_AUDIO);
		s2 = (dynamic_cast<BaseCountryBox*>(m_pAudio))->GetFilter(s1);
		s1.Printf(wxT("[%s].[%s]"), t3Base, t3Base_SUBS);
		s3 = (dynamic_cast<BaseCountryBox*>(m_pSubs))->GetFilter(s1);

		s1.Printf(wxT("(%s AND %s)"), s2, s3);
	}
	else if ( m_bAorS )
	{
		wxASSERT(m_pAudio && m_pSubs);
		wxString s3;
		s1.Printf(wxT("[%s].[%s]"), t3Base, t3Base_AUDIO);
		s2 = (dynamic_cast<BaseCountryBox*>(m_pAudio))->GetFilter(s1);
		s1.Printf(wxT("[%s].[%s]"), t3Base, t3Base_SUBS);
		s3 = (dynamic_cast<BaseCountryBox*>(m_pSubs))->GetFilter(s1);

		s1.Printf(wxT("(%s OR %s)"), s2, s3);
	}

	if ( m_pRd_MovieTitle )
	{ // if ==NULL, ignore following
		wxString s3;
		s2 = s1;

		if ( m_bShowMovies )
			s3.Printf(wxT("([%s].[%s] = 0)"), t3Movies, t3Movies_SERIE);
		else if ( m_bShowSeries )
			s3.Printf(wxT("([%s].[%s] = 1)"), t3Movies, t3Movies_SERIE);

		if ( !s2.IsEmpty() && !s3.IsEmpty() )
			s1.Printf(wxT("(%s AND %s)"), s3, s2);
		else if ( s2.IsEmpty() && !s3.IsEmpty() )
			s1 = s3;
		else if ( !s2.IsEmpty() && s3.IsEmpty() )
			s1 = s2;
	}

	return s1;
}

wxString DlgFilter::GetOrder() const
{
	wxString s, order;

	if (m_nSort == SortComboBox::Ascending)
		s = wxT(" ASC");
	else if ( m_nSort == SortComboBox::Descending )
		s = wxT(" DESC");

	if ( !s.IsEmpty() )
	{
		if ( m_eSortBy == Sorting::Storage )
			order.Printf(wxT("[%s].[%s]%s"), t3Storage, t3Storage_NAME, s);
		else // if (m_eSortBy == Sorting::Movies )
			order.Printf(wxT("[%s].[%s] COLLATE lkUTF8compare%s, [%s].[%s]%s"), t3Movies, t3Movies_TITLE, s, t3Movies, t3Movies_EPISODE, s);
	}

	return order;
}
