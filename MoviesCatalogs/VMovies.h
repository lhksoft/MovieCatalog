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
#ifndef __V_MOVIES_H__
#define __V_MOVIES_H__
#pragma once

#include "lkSQL3RecordView.h"
#include "MainDocManager.h"
#include "TMovies.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class MoviesDocument
////
class MoviesDocument : public lkSQL3RecordDocument, public TMovies
{
public:
	MoviesDocument(wxDocument* pParent);

	virtual wxString				GetDefaultOrder(void) const wxOVERRIDE; // default returns nothing, but you could set here a default filter -- wil be called in GetRecordset()

	bool							IsCategoryChanged(void) const;
	void							SetCategoryChanged(bool);

	bool							IsGenresChanged(void) const;
	void							SetGenresChanged(bool);

	bool							IsJudgesChanged(void) const;
	void							SetJudgesChanged(bool);

	bool							IsRatingChanged(void) const;
	void							SetRatingChanged(bool);

	virtual void					BindButtons(void) wxOVERRIDE;

// Attributes
protected:
	virtual lkSQL3RecordSet*		GetBaseSet(void) wxOVERRIDE;
	virtual void					DoThingsBeforeAdd(void) wxOVERRIDE;

	bool							m_bCatChanged, m_bGenresChanged, m_bJudgesChanged, m_bRatingChanged;
	bool							m_bHasMoviesCopy;

// Events
protected:
	void							OnUpdateFilter(wxUpdateUIEvent& event);
	void							OnUpdateFilterToggle(wxUpdateUIEvent& event);
	void							OnUpdateMoviesCopy(wxUpdateUIEvent& event);
	void							OnUpdateMoviesPaste(wxUpdateUIEvent& event);

	void							OnMoviesCopy(wxCommandEvent& event);
	void							OnMoviesPaste(wxCommandEvent& event);
	void							OnSearch(wxCommandEvent& event);
	void							OnFilter(wxCommandEvent& event);
	void							OnFilterToggle(wxCommandEvent& event);

private:
	wxDECLARE_ABSTRACT_CLASS(MoviesDocument);
	wxDECLARE_NO_COPY_CLASS(MoviesDocument);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class MoviesView
////
class MoviesView : public lkSQL3RecordView
{
public:
	MoviesView();

	virtual bool					OnCreate(wxDocument* doc, long flags) wxOVERRIDE;
	virtual void					InitialUpdate(void) wxOVERRIDE;
	virtual bool					UpdateData(bool bSaveAndValidate = false) wxOVERRIDE;

// Overrides
protected:
	// Required for setting config-options
	virtual wxString				GetConfigPath(void) const wxOVERRIDE;
	virtual void					ProcessRecordset(lkSQL3RecordSet*) wxOVERRIDE;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(MoviesView);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class MoviesTemplate
////
class MoviesTemplate : public lkChildDocTemplate
{
public:
	MoviesTemplate(MainDocManager* manager, wxClassInfo* parentDocClassInfo, wxClassInfo* viewClassInfo);

	virtual wxDocument* DoCreateDocument() wxOVERRIDE;

	static wxDocTemplate* CreateTemplate(MainDocManager*);

private:
	wxDECLARE_CLASS(MoviesTemplate);
	wxDECLARE_NO_COPY_CLASS(MoviesTemplate);
};

#endif // !__V_MOVIES_H__
