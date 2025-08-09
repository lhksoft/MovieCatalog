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
#ifndef __COMPACT_EVENT_H__
#define __COMPACT_EVENT_H__
#pragma once

#include <wx/event.h>

class CompactEvent;
wxDECLARE_EVENT(lkEVT_COMPACT_EVENT, CompactEvent);

class CompactEvent : public wxThreadEvent
{
public:
	enum class EventIDs
	{
		ID_information_Main = 1,	// string will be set in 1st line
		ID_information_Sec,			// string will be set in 2nd line
		ID_progressInit,			// int=1:total records, int=2:amount of records to do of current table, using m_lkU64
		ID_progressStep,			// int=1:step in total records, int=2: step in current table, int=0:step both
		ID_threadCompleted,			// exitcode will be in internal int : 1=error, 2=<Esc> by user (TestDestroy)
		ID_infoProcessing,			// 1st line : "Processing Requirements for [...]"
		ID_infoRewriting,			// 2nd line : "Rewriting table [...]"
		ID_exitCompacting			// to inform MainFrame that CompactFrame is done
	};


	CompactEvent(EventIDs id);
	CompactEvent(const CompactEvent&);

	virtual wxEvent*			Clone() const wxOVERRIDE;

	void						SetLongLong(wxUint64);
	wxUint64					GetLongLong(void) const;

private:
	wxUint64					m_lkU64;
};

typedef void (wxEvtHandler::* ThreadEventFunction)(CompactEvent&);

#define ThreadEventHandler(func) wxEVENT_HANDLER_CAST(ThreadEventFunction, func)

#define EVT_COMPACT_EVENT(id, func) \
 	wx__DECLARE_EVT1(lkEVT_COMPACT_EVENT, id, ThreadEventHandler(func))

#endif // !__COMPACT_EVENT_H__


