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
#ifndef __MAIN_VIEW_H__
#define __MAIN_VIEW_H__
#pragma once

#include "MainDocument.h"
#include "../lkControls/lkCanvas.h"

////////////////////////////////////////////////////////////////////////////////////////////////
// class MainCanvas
////
class MainCanvas : public lkCanvas
{
public:
	// view may be NULL if we're not associated with one yet, but parent must then be a valid pointer
	MainCanvas(wxView* view, wxWindow* parent = NULL);
	virtual ~MainCanvas();

	bool					Create(void);
	void					UpdateData(void);

private:
	wxWindow*				m_pText;
	wxWindow*				m_pTablesList;

	void					BuildTablesList(void);

private:
	wxDECLARE_CLASS(MainCanvas);
	wxDECLARE_NO_COPY_CLASS(MainCanvas);
};

class MainView : public wxView
{
public:
	MainView();

	virtual bool			OnCreate(wxDocument* doc, long flags) wxOVERRIDE;
	virtual void			OnDraw(wxDC* dc) wxOVERRIDE;
	virtual void			OnUpdate(wxView* sender, wxObject* hint = NULL) wxOVERRIDE;
	virtual bool			OnClose(bool deleteWindow = true) wxOVERRIDE;

	MainDocument*			GetDocument(void);

private:
	MainCanvas*			m_pCanvas;

	wxDECLARE_DYNAMIC_CLASS_NO_COPY(MainView);
};


#endif // !__MAIN_VIEW_H__
