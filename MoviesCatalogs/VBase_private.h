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
#ifndef __V_BASE_PRIVATE_H__
#define __V_BASE_PRIVATE_H__
#pragma once

#include "lkSQL3RecordView.h"
#include "CanvasCtrl.h"
#include "DlgFind.h"

#include <wx/stattext.h>

#include "TMovies.h"

const wxString GetBaseSection(void);

//////////////////////////////////////////////////////////////////////////////////////////////////////
// class DlgCountryFind
////
class DlgBaseFind : public DlgFind
{
public:
	DlgBaseFind(lkSQL3RecordSet*, lkFindList*);
	bool					Create(wxWindow*, long style = wxDEFAULT_DIALOG_STYLE);

	// returns first rowid if successful, otherwise returns 0
	static wxUint64			FindBase(wxWindow* parent, lkSQL3RecordSet* pRS);

protected:
	virtual void			DoInit(void) wxOVERRIDE;
	virtual void			DoOK(void) wxOVERRIDE;

private:
	wxDECLARE_ABSTRACT_CLASS(DlgBaseFind);
	wxDECLARE_NO_COPY_CLASS(DlgBaseFind);
};


///////////////////////////////////////////////////////////////////////////////////////////////////////
// class BaseFrame
////
class BaseFrame : public lkSQL3RecordChildFrame
{
public:
	BaseFrame();

	virtual wxToolBar*		OnCreateToolBar(long style, wxWindowID winid, const wxString& name) wxOVERRIDE;

protected:
	wxWindowID				m_nID_Filter;
	wxWindowID				m_nID_FilterToggle;
	wxWindowID				m_nID_Search;
	wxWindowID				m_nID_SearchNext;
	wxWindowID				m_nID_SearchPrev;
	wxWindowID				m_nID_Movies;
	wxWindowID				m_nID_Group;

public:
	wxWindowID				GetID_Filter(void) const;
	wxWindowID				GetID_FilterToggle(void) const;
	wxWindowID				GetID_Search(void) const;
	wxWindowID				GetID_SearchNext(void) const;
	wxWindowID				GetID_SearchPrev(void) const;
	wxWindowID				GetID_Movies(void) const;
	wxWindowID				GetID_Group(void) const;

private:
	wxDECLARE_CLASS(BaseFrame);
	wxDECLARE_NO_COPY_CLASS(BaseFrame);
};


///////////////////////////////////////////////////////////////////////////////////////////////////////
// class BaseMoviesCanvas
// Small wxPanel as a 'Control' inside BaseCanvas with some read-only Movies-fields
////
class BaseMoviesCanvas : public CanvasCtrl, public TMovies
{
public:
	BaseMoviesCanvas(wxWindow*, lkSQL3Database*, const wxValidator& validator = wxDefaultValidator);

	bool					Create(wxWindow* pParent, const wxValidator& validator = wxDefaultValidator);

	virtual void			SetInternalID(wxUint64) wxOVERRIDE;

	wxDECLARE_CLASS(BaseMoviesCanvas);
	wxDECLARE_NO_COPY_CLASS(BaseMoviesCanvas);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class BaseCanvas
////
class BaseCanvas : public lkSQL3panel
{
public:
	// view may be NULL if we're not associated with one yet, but parent must then be a valid pointer
	BaseCanvas(wxView* view, wxWindow* parent = NULL);

	bool					Create(void);
	void					SetBaseFocus(void); // required when entering Add modus, or at Creation

	virtual bool			Validate() wxOVERRIDE;
	virtual bool			TransferDataToWindow() wxOVERRIDE;

	void					UpdateMoviesCtrl(void); // will perform a m_pMovies->TransferDataToWindow
	void					UpdateGroupCtrl(void); // will perfonrm a m_pGroup->TransferDataToWindow

private:
	BaseMoviesCanvas*		m_pMovies;
	wxWindow*				m_pAudio;	// kindof 'BaseCountryBox'
	wxWindow*				m_pSubs;	// kindof 'BaseCountryBox'
	wxWindow*				m_pQuality;	// kindof 'QualityBox'
	wxWindow*				m_pOrigine;	// kindof 'OrigineBox'
	wxWindow*				m_pScreen;	// kindof 'ScreenBox'
	wxWindow*				m_pGroup;	// kindof 'GroupCtrl'

	bool					m_bReqAdd;
	wxUint64				m_nCurRow;	// curRow of Base, '0' when adding

	wxDECLARE_CLASS(BaseCanvas);
	wxDECLARE_NO_COPY_CLASS(BaseCanvas);
};


#endif // !__V_BASE_PRIVATE_H__
