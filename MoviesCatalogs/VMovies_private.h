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
#ifndef __V_MOVIES_PRIVATE_H__
#define __V_MOVIES_PRIVATE_H__
#pragma once

#include "lkSQL3RecordView.h"
#include "CMovies.h"
#include "DlgFilter.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class MoviesFrame
////
class MoviesFrame : public lkSQL3RecordChildFrame
{
public:
	MoviesFrame();

	virtual wxToolBar*		OnCreateToolBar(long style, wxWindowID winid, const wxString& name) wxOVERRIDE;

protected:
	wxWindowID				m_nID_MoviesCopy;
	wxWindowID				m_nID_MoviesPaste;
	wxWindowID				m_nID_Filter;
	wxWindowID				m_nID_FilterToggle;
	wxWindowID				m_nID_Search;
	wxWindowID				m_nID_SearchNext;
	wxWindowID				m_nID_SearchPrev;

public:
	wxWindowID				GetID_MoviesCopy(void) const;
	wxWindowID				GetID_MoviesPaste(void) const;
	wxWindowID				GetID_Filter(void) const;
	wxWindowID				GetID_FilterToggle(void) const;
	wxWindowID				GetID_Search(void) const;
	wxWindowID				GetID_SearchNext(void) const;
	wxWindowID				GetID_SearchPrev(void) const;

private:
	wxDECLARE_CLASS(MoviesFrame);
	wxDECLARE_NO_COPY_CLASS(MoviesFrame);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class MoviesPanel
////
class MoviesPanel : public CMovies
{
public:
	MoviesPanel();
	virtual ~MoviesPanel();

	bool							CreateCanvas(wxWindow* parentFrame, wxWindow* parentPanel, lkSQL3RecordDocument* doc); // doc needs to be MoviesDocument
	void							SetParentView(wxView*);

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


private:
	wxView*							m_pParentView;
	lkSQL3RecordDocument*			m_pParentDoc;

	wxDECLARE_NO_COPY_CLASS(MoviesPanel);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class MoviesCanvas
////
class MoviesCanvas : public lkSQL3panel, public MoviesPanel
{
public:
	// view may be NULL if we're not associated with one yet, but parent must then be a valid pointer
	MoviesCanvas(wxView* view, wxWindow* parent = NULL);

	bool					Create(void);
	// validate the correctness of input, return true if ok
	virtual bool			Validate() wxOVERRIDE;
	virtual bool			TransferDataToWindow() wxOVERRIDE;

private:
	wxDECLARE_ABSTRACT_CLASS(MoviesCanvas);
	wxDECLARE_NO_COPY_CLASS(MoviesCanvas);
};

#endif // !__V_MOVIES_PRIVATE_H__
