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
#ifndef __SIMPLEBOX_H__
#define __SIMPLEBOX_H__
#pragma once

#include "../lkControls/lkImgComboBox.h"
#include "../lkSQLite3/lkSQL3Database.h"

class SimpleBox : public lkImgComboBox
{
public:
	SimpleBox();
	SimpleBox(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
			  long style = 0, const wxValidator& validator = wxDefaultValidator);

	void				SetDatabase(lkSQL3Database*);
	virtual bool		BuildList(bool bKeepSel = false) = 0;

protected:
	lkSQL3Database*		m_pDB;

private:
	wxDECLARE_ABSTRACT_CLASS(SimpleBox);
};

/* ***************************************************************
 * class JudgeBox -- used in CMovies
 * *************************************************************** */

class JudgeBox : public SimpleBox
{
public:
	JudgeBox();
	JudgeBox(wxWindow* parent, lkSQL3Database* pDB, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
			 long style = 0, const wxValidator& validator = wxDefaultValidator);
	virtual bool	BuildList(bool bKeepSel = false) wxOVERRIDE;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(JudgeBox);
};

/* ***************************************************************
 * class RatingBox -- used in CMovies
 * *************************************************************** */

class RatingBox : public SimpleBox
{
public:
	RatingBox();
	RatingBox(wxWindow* parent, lkSQL3Database* pDB, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
			  long style = 0, const wxValidator& validator = wxDefaultValidator);
	virtual bool	BuildList(bool bKeepSel = false) wxOVERRIDE;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(RatingBox);
};

/* ***************************************************************
 * class MediumBox -- used in VStorage (canvas) + DlgStorageFilter (canvas)
 * *************************************************************** */

class MediumBox : public SimpleBox
{
public:
	MediumBox();
	MediumBox(wxWindow* parent, const wxSize& size = wxDefaultSize, const wxValidator& validator = wxDefaultValidator);

	virtual bool	BuildList(bool bKeepSel = false) wxOVERRIDE;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(MediumBox);
};

/* ***************************************************************
 * class LocationBox -- used in VStorage (canvas) + DlgStorageFilter (canvas)
 * *************************************************************** */

class LocationBox : public SimpleBox
{
public:
	LocationBox();
	LocationBox(wxWindow* parent, const wxSize& size = wxDefaultSize, const wxValidator& validator = wxDefaultValidator);

	virtual bool	BuildList(bool bKeepSel = false) wxOVERRIDE;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(LocationBox);
};


/* ***************************************************************
 * class QualityBox -- used in VBase_private (canvas)
 * *************************************************************** */

class QualityBox : public SimpleBox
{
public:
	QualityBox();
	QualityBox(wxWindow* parent, const wxSize& size = wxDefaultSize, const wxValidator& validator = wxDefaultValidator);

	virtual bool	BuildList(bool bKeepSel = false) wxOVERRIDE;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(QualityBox);
};


/* ***************************************************************
 * class OrigineBox -- used in VBase_private (canvas)
 * *************************************************************** */

class OrigineBox : public SimpleBox
{
public:
	OrigineBox();
	OrigineBox(wxWindow* parent, const wxSize& size = wxDefaultSize, const wxValidator& validator = wxDefaultValidator);

	virtual bool	BuildList(bool bKeepSel = false) wxOVERRIDE;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(OrigineBox);
};


/* ***************************************************************
 * class ScreenBox -- used in VBase_private (canvas)
 * *************************************************************** */

class ScreenBox : public SimpleBox
{
public:
	ScreenBox();
	ScreenBox(wxWindow* parent, const wxSize& size = wxDefaultSize, const wxValidator& validator = wxDefaultValidator);

	virtual bool	BuildList(bool bKeepSel = false) wxOVERRIDE;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(ScreenBox);
};


#endif // !__SIMPLEBOX_H__