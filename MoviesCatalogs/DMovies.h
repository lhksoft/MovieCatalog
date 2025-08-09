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
#ifndef __D_MOVIES_H__
#define __D_MOVIES_H__
#pragma once

#include "lkSQL3RecordDialog.h"
#include "CMovies.h"
#include "TMovies.h"

class DMovies : public lkSQL3RecordDialog, public TMovies, public CMovies
{
public:
	DMovies(lkSQL3Database*);

	bool							Create(wxWindow* parent);

// Overrides of base-class lkSQL3RecordDialog
public:
	virtual wxToolBar*				OnCreateToolBar(long style, wxWindowID winid, const wxString& name) wxOVERRIDE;
	virtual void					BindButtons(void) wxOVERRIDE;
	virtual bool					Validate() wxOVERRIDE;
	virtual bool					TransferDataToWindow() wxOVERRIDE;
	virtual wxString				GetDefaultOrder(void) const wxOVERRIDE; // default returns nothing, but you could set here a default filter -- wil be called in GetRecordset()
	virtual bool					UpdateData(bool bSaveAndValidate = false) wxOVERRIDE;

protected:
	virtual lkSQL3RecordSet*		GetBaseSet(void) wxOVERRIDE;

	// This will be called before actual DoAdd() .. e.g. to set focus to a specific control
	virtual void					DoThingsBeforeAdd(void) wxOVERRIDE;

// Overrides of base-class CMovies
protected:
	virtual bool					IsCategoryChanged(void) const wxOVERRIDE;
	virtual bool					IsJudgesChanged(void) const wxOVERRIDE;
	virtual bool					IsRatingChanged(void) const wxOVERRIDE;
	virtual bool					IsGenresChanged(void) const wxOVERRIDE;

	virtual bool					GetYearValue(void) const wxOVERRIDE;

	virtual lkSQL3Database*			GetDBase(void) wxOVERRIDE;
	virtual lkSQL3RecordSet*		GetBaseRecordset(void) wxOVERRIDE;

	virtual lkSQL3FieldData_Text*	GetFldCover(void) wxOVERRIDE;
	virtual lkSQL3FieldData_Text*	GetFldTitle(void) wxOVERRIDE;
	virtual lkSQL3FieldData_Text*	GetFldSubTtl(void) wxOVERRIDE;
	virtual lkSQL3FieldData_Text*	GetFldAlias(void) wxOVERRIDE;
	virtual lkSQL3FieldData_Int*	GetFldEpisode(void) wxOVERRIDE;
	virtual lkSQL3FieldData_Int*	GetFldCat(void) wxOVERRIDE;
	virtual lkSQL3FieldData_Int*	GetFldSubCat(void) wxOVERRIDE;
	virtual lkSQL3FieldData_Int*	GetFldGenre(void) wxOVERRIDE;
	virtual lkSQL3FieldData_Text*	GetFldCountry(void) wxOVERRIDE;
	virtual lkSQL3FieldData_Real*	GetFldDate(void) wxOVERRIDE;
	virtual lkSQL3FieldData_Int*	GetFldYear(void) wxOVERRIDE;
	virtual lkSQL3FieldData_Int*	GetFldSerie(void) wxOVERRIDE;
	virtual lkSQL3FieldData_Text*	GetFldURL(void) wxOVERRIDE;
	virtual lkSQL3FieldData_Int*	GetFldJudge(void) wxOVERRIDE;
	virtual lkSQL3FieldData_Int*	GetFldRating(void) wxOVERRIDE;
	virtual lkSQL3FieldData_Text*	GetFldInfo(void) wxOVERRIDE;

// Event Handling
protected:
	virtual bool					DoUpdateRecordFind(void) wxOVERRIDE;
	virtual bool					DoUpdateRecordFindNext(void) wxOVERRIDE;
	virtual bool					DoUpdateRecordFindPrev(void) wxOVERRIDE;

	void							OnUpdateMoviesCopy(wxUpdateUIEvent& event);
	void							OnUpdateMoviesPaste(wxUpdateUIEvent& event);

	void							OnFind(wxCommandEvent& event);

	void							OnMoviesCopy(wxCommandEvent& event);
	void							OnMoviesPaste(wxCommandEvent& event);

// Attributes
protected:
	wxWindowID						m_nID_MoviesCopy;
	wxWindowID						m_nID_MoviesPaste;
	wxWindowID						m_nID_Search;
	wxWindowID						m_nID_SearchNext;
	wxWindowID						m_nID_SearchPrev;

	bool							m_bHasMoviesCopy;

public:
	wxWindowID						GetID_MoviesCopy(void) const;
	wxWindowID						GetID_MoviesPaste(void) const;
	wxWindowID						GetID_Search(void) const;
	wxWindowID						GetID_SearchNext(void) const;
	wxWindowID						GetID_SearchPrev(void) const;

private:
	wxDECLARE_ABSTRACT_CLASS(DMovies);
	wxDECLARE_NO_COPY_CLASS(DMovies);
};

#endif // !__D_MOVIES_H__
