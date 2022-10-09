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
#ifndef __DLG_FILTER_H__
#define __DLG_FILTER_H__
#pragma once

#include "../lkControls/lkDialog.h"
#include "../lkSQLite3/lkSQL3Database.h"

class DlgFilter : public lkDialog
{
public:
	DlgFilter(lkSQL3Database*);

	bool							Create(wxWindow*, const wxString& title=wxT("Filter"), wxSize size = wxDefaultSize);
	virtual bool					TransferDataFromWindow() wxOVERRIDE;

	wxString						GetFilter(void) const;
	wxString						GetOrder(void) const;
	
protected:
	virtual wxString				GetConfigSection(void) const = 0;
	virtual void					AddCanvas(void) = 0; // this will be called from inside Create, should be overwritten by derived-class

	void							EnableRadio(wxWindowID isEnabledID);
	void							BuildOccuranceList(void);

	virtual void					SafeSettings(void); // safes what was been set in our ini file -- called in OnButton<OK>
	virtual bool					TestFilter(void) = 0;

public:
	static void						DoCorrectCatConfig(const wxString& section, lkSQL3Database* dbTmp);
	static void						DoCorrectGenConfig(const wxString& section, lkSQL3Database* dbTmp);
	static void						DoCorrectLocConfig(const wxString& section, lkSQL3Database* dbTmp);
	static void						DoCorrectMedConfig(const wxString& section, lkSQL3Database* dbTmp);
	static void						DoCorrectOrgConfig(const wxString& section, lkSQL3Database* dbTmp);

// Attributes
protected:
	lkSQL3Database*					m_pDB;

	wxWindow*						m_pSorting;
	wxUint64						m_nSort; // 1..3 : 1=no sort, 2=asc, 3=desc
	bool							m_bShowAll, m_bShowMovies, m_bShowSeries;

	wxString						m_sFilter, m_sMovieTitle, m_sStorage, m_sA, m_sS, m_sAudio, m_sSubs, m_sAandS_A, m_sAandS_S, m_sAorS_A, m_sAorS_S;
	wxUint64						m_nOcc, m_nMovieOcc, m_nStorageOcc;
	wxUint64						m_nCat, m_nSubCat, m_nGenres, m_nLocation, m_nMedium, m_nOrigine;

	wxWindow*						m_pFilterStrStat, *m_pFilterStr, *m_pOccStat, *m_pOccCombo;
	wxWindow*						m_pCat, *m_pSubCat, *m_pGenres, *m_pLocation, *m_pMedium, *m_pOrigine, *m_pAudio, *m_pAudioStat, *m_pSubs, *m_pSubsStat;

	// RadioCtrls
	wxWindow*						m_pRd_MovieTitle, *m_pRd_Storage, *m_pRd_Category, *m_pRd_Genres, *m_pRd_Location, *m_pRd_Medium, *m_pRD_Origine,
										*m_pRD_Audio, *m_pRD_Subs, *m_pRD_AandS, *m_pRD_AorS;
	wxWindowID						m_nID_MovieTitle, m_nID_Storage, m_nID_Category, m_nID_Genres, m_nID_Location, m_nID_Medium, m_nID_Origine,
										m_nID_Audio, m_nID_Subs, m_nID_AandS, m_nID_AorS;
	bool							m_bMovieTitle, m_bStorage, m_bCategory, m_bGenres, m_bLocation, m_bMedium, m_bOrigine, m_bAudio, m_bSubs, m_bAandS, m_bAorS;

	wxWindowID						m_nID_Prev;

	enum class IDS :wxWindowID
	{
		INVALID = 0,
		MovieTitle = 1,
		Storage,
		Category,
		Location,
		Medium,
		Genres,
		Origine,
		Audio,
		Subs,
		A_and_S,
		A_or_S
	}								m_eChoice;

	enum class Showing
	{
		INVALID = 0,
		All = 1,
		Movies,
		Series
	}								m_eShowing;

	enum class Sorting
	{
		Movies = 1,
		Storage
	}								m_eSortBy;

// Event Handling
protected:
	void							OnInitDialog(wxInitDialogEvent& event);
	void							OnRadioClick(wxCommandEvent& event);
	void							OnButton(wxCommandEvent& event);

private:
	wxDECLARE_EVENT_TABLE();
	wxDECLARE_CLASS(DlgFilter);
	wxDECLARE_NO_COPY_CLASS(DlgFilter);
};

#endif // !__DLG_FILTER_MOVIES_H__
