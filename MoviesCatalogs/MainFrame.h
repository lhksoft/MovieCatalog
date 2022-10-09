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
#ifndef __MAIN_FRAME_H__
#define __MAIN_FRAME_H__
#pragma once

#include <wx/docview.h>
#include "CompactEvent.h"

class MainFrame : public wxDocParentFrame
{
public:
	MainFrame();
	MainFrame(wxDocManager* manager, wxFrame* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
			  long style = wxDEFAULT_FRAME_STYLE);
protected:
	void			OnCloseWindow(wxCloseEvent& event);
	void			OnCompact(wxCommandEvent& event);
	void			OnThreadFinish(CompactEvent& event);

	void			OnAbout(wxCommandEvent& event);

	wxFrame*		m_pThreadFrame;

	void			SetConfigPosition(void); // called in 'OnCloseWindow' -- will store frame's current position in cofig-file

public:
	static wxPoint	GetConfigPosition(void);

private:
	wxDECLARE_CLASS(MainFrame);
	wxDECLARE_NO_COPY_CLASS(MainFrame);
};

#endif // !__MAIN_FRAME_H__