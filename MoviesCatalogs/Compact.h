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
#ifndef __COMPACT_H__
#define __COMPACT_H__
#pragma once

#include <wx/frame.h>
#include <wx/utils.h>
#include "CompactEvent.h"

class CompactCanvas;
class CompactThread;
class CompactFrame : public wxFrame
{
public:
	CompactFrame(const wxString& srcPath);
	virtual ~CompactFrame();
	bool							Create(wxWindow* parent);

	bool							DoCompact(void); // returning false if thread could not be started

protected:
	CompactThread*					m_pThread;
	wxCriticalSection				m_pThreadCS;    // protects the m_pThread pointer

	friend class CompactThread;
	bool							m_bThrPaused;

	CompactCanvas*					m_pCanvas;

	wxWindowDisabler*				m_DisableAllOthers; // from c'tor to d'tor all other frames will be disabled
	wxString						m_sSrcPath;

	bool							CloseThread(void);
	bool							ThreadActive(void); // true if 'm_pThread != NULL'
	void							PauseThread(void);
	void							ResumeThread(void);

	wxString						GetExitString(void) const;

// Event Handling
protected:
	void							OnClose(wxCloseEvent&);
	void							OnCharHook(wxKeyEvent& event);
	void							OnThread(CompactEvent& event);

	bool							IsEscapeKey(const wxKeyEvent& event);
	bool							IsPauseKey(const wxKeyEvent& event);

private:
	wxDECLARE_EVENT_TABLE();
	wxDECLARE_CLASS(CompactFrame);
	wxDECLARE_NO_COPY_CLASS(CompactFrame);
};

#endif // !__COMPACT_H__
