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
#ifndef __C_MOVIES_H__
#define __C_MOVIES_H__
#pragma once

#include "CategoryBox.h"
#include "GenresBox.h"
#include "SimpleBox.h"
#include "ImageCtrl.h"
#include "DlgFind.h"

#include <wx/image.h>
#include <wx/button.h>

#include "../lkControls/lkTransTextCtrl.h"
#include "../lkSQLite3/lkDateTime.h"

#include "../lkSQLite3/lkSQL3RecordSet.h"
#include "../lkSQLite3/lkSQL3Field.h"

///////////////////////////////////////////////////////////////
// class DlgMoviesFind
////
class DlgMoviesFind : public DlgFind
{
public:
	DlgMoviesFind(lkSQL3RecordSet*, lkFindList*);
	bool					Create(wxWindow*, long style = wxDEFAULT_DIALOG_STYLE);

	// returns first actual_rowid if successful, otherwise returns 0
	static wxUint64			FindMovie(wxWindow* parent, lkSQL3RecordSet* pRS);

protected:
	virtual void			DoInit(void) wxOVERRIDE;
	virtual void			DoOK(void) wxOVERRIDE;

private:
	wxDECLARE_ABSTRACT_CLASS(DlgMoviesFind);
	wxDECLARE_NO_COPY_CLASS(DlgMoviesFind);
};

///////////////////////////////////////////////////////////////
// class CMovies
////
class MoviesCopy;
class CMovies
{
public:
	CMovies();
	virtual ~CMovies();

	static wxString					GetConfigKeyString(void);

	wxImage							GetBackground(void) const;

	bool							CreateCanvas(wxWindow* parentFrame, wxWindow* parentPanel);

	void							SetMoviesFocus(void);

	bool							mvTransferDataToWindow(bool bInitial);

	bool							MoviesCopyInProgress(void) const;
	void							MoviesCopyDisable(void);
	void							MoviesCopyEnable(void);

// Event Handling -- usualy is protected, but in here it needs to be public or it cannot be linked by derived classes in an EvtHandler
public:
	void							OnSelectDate(wxCommandEvent& event);
	void							OnYearCheck(wxCommandEvent& event);
	void							OnPickCountry(wxCommandEvent& event);

protected:
	virtual bool					IsCategoryChanged(void) const = 0;
	virtual bool					IsJudgesChanged(void) const = 0;
	virtual bool					IsRatingChanged(void) const = 0;
	virtual bool					IsGenresChanged(void) const = 0;

	virtual bool					GetYearValue(void) const = 0;

	virtual lkSQL3Database*			GetDBase(void) = 0;
	virtual lkSQL3RecordSet*		GetBaseRecordset(void) = 0;

	virtual lkSQL3FieldData_Text*	GetFldCover(void) = 0;
	virtual lkSQL3FieldData_Text*	GetFldTitle(void) = 0;
	virtual lkSQL3FieldData_Text*	GetFldSubTtl(void) = 0;
	virtual lkSQL3FieldData_Text*	GetFldAlias(void) = 0;
	virtual lkSQL3FieldData_Int*	GetFldEpisode(void) = 0;
	virtual lkSQL3FieldData_Int*	GetFldCat(void) = 0;
	virtual lkSQL3FieldData_Int*	GetFldSubCat(void) = 0;
	virtual lkSQL3FieldData_Int*	GetFldGenre(void) = 0;
	virtual lkSQL3FieldData_Text*	GetFldCountry(void) = 0;
	virtual lkSQL3FieldData_Real*	GetFldDate(void) = 0;
	virtual lkSQL3FieldData_Int*	GetFldYear(void) = 0;
	virtual lkSQL3FieldData_Int*	GetFldSerie(void) = 0;
	virtual lkSQL3FieldData_Text*	GetFldURL(void) = 0;
	virtual lkSQL3FieldData_Int*	GetFldJudge(void) = 0;
	virtual lkSQL3FieldData_Int*	GetFldRating(void) = 0;
	virtual lkSQL3FieldData_Text*	GetFldInfo(void) = 0;

protected:
	ImgPathCtrl*					m_pImage;
	lkTransTextCtrl*				m_pTitleCtrl; // when adding a new record, this control will get focus
	lkTransTextCtrl*				m_pCountry;
	CategoryBox*					m_pCategory;
	CategoryBox*					m_pSubCat;
	GenresBox*						m_pGenres;
	JudgeBox*						m_pJudge;
	RatingBox*						m_pRating;
	lkTransTextCtrl*				m_pDateTxt, *m_pYearTxt; // in wxMSW one cannot dynamicaly change ReadOnly style, so I need 2 controls - 1 readonly for full date, and 1 one editable for Year-only
	wxButton*						m_pChangeDateBtn; // to hide/show the control (hide when m_bYear==true)

	wxWindowID						m_ID_CountryBtn;
	wxWindowID						m_ID_YearChkb;
	wxWindowID						m_ID_DateBtn;
	bool							m_bYear;
	lkDateTime						m_Date;

	bool							m_bReqAdd;

	MoviesCopy*						m_pMoviesCopy;

private:
	wxWindow*						m_pParentFrame; // either a wxFrame or wxDialog
	wxWindow*						m_pParentPanel; // either a wxPanel or wxDialog

	wxDECLARE_NO_COPY_CLASS(CMovies);
};

#endif // !__C_MOVIES_H__
