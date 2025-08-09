/* lkControls - custom controls using wxWidgets
 * Copyright (C) 2022 Laurens Koehoorn (lhksoft)
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef __LK_RECORDCHILDFRAME_H__
#define __LK_RECORDCHILDFRAME_H__
#pragma once

#include <wx/docview.h>
#include <wx/icon.h>

// uses contents of following :
//#include <lk/lkRecordBar.h>

 /****************************************************************************************
 * lkRecordChildFrame -- Class definition
 ****************************************************************************************/
class lkRecordChildFrame : public wxDocChildFrame
{
public:
	lkRecordChildFrame();

	bool					Create(wxDocument* doc, wxView* view, wxFrame* parent, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
								   long style = wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT | wxFRAME_NO_TASKBAR);

	// return a new status bar
	virtual wxStatusBar*	OnCreateStatusBar(int number, long style, wxWindowID winid, const wxString& name) wxOVERRIDE;

	// return a new toolbar
	// creates the tool-items of the Toolbar -- override to replace or add items (then call this func too)
	virtual wxToolBar*		OnCreateToolBar(long style, wxWindowID winid, const wxString& name) wxOVERRIDE;

protected:
	// Creates Icon Bundle to be used as default Icon for this frame
	virtual wxIconBundle	CreateBundle_Form(void);

	// Adds entries to the frame's accelerator key table.  Derived classes should call up to their parents.
	virtual void			AddAcceleratorTableEntries(wxVector<wxAcceleratorEntry>& Entries);

	// Assembles an accelerator key table for the frame by calling down to AddAcceleratorTableEntries.
	void					DoSetAcceleratorTable(void);

	// responds to <esc> key-click, only available in wxwidgets through a wxAcceleratorTable. This command sends the keyclick to the lkSQL3RecordDocument which this class uses
	void					OnCancel(wxCommandEvent& event);
	// called in OnCancel, default does nothing
	virtual void			DoCancel(void);

	// responds to EVT_MENU_HIGHLIGHT, default calls FindItemInMenuBar(..) but whatif it's been used with popup menus? Allthough the event has the pointer to the calling (popup)menu ..
	void					OnMenuHighlight(wxMenuEvent& event);

	// responds to EVT_MENU_CLOSE so we can blank the Help-string (if any) as shown in our statusbar
	void					OnMenuClose(wxMenuEvent& event);

	// show the help string for the given menu item using DoGiveHelp() if the
	// given item does have a help string (as determined by FindInMenu()),
	// return false if there is no help for such item
	bool					ShowMenuHelp(int helpid, wxMenu*);


// Following are created, stored and destroyed (at destruction) in the internal lkRecordStatusbar class
// they're created using wxWindow::NewControlID for uniqness and only valid in the current (and any derived) class
public:
	wxWindowID				GetID_RecFirst(void) const;
	wxWindowID				GetID_RecPrev(void) const;
	wxWindowID				GetID_RecNext(void) const;
	wxWindowID				GetID_RecLast(void) const;
	wxWindowID				GetID_RecNew(void) const;
	wxWindowID				GetID_RecRemove(void) const;
	wxWindowID				GetID_RecUpdate(void) const;
	wxWindowID				GetID_RecCancel(void) const;

public:
	void					SetCurrentRecord(wxUint64);
	void					SetTotalRecords(wxUint64);

private:
	wxDECLARE_CLASS(lkRecordChildFrame);
	wxDECLARE_NO_COPY_CLASS(lkRecordChildFrame);
};


#endif // !__LK_RECORDCHILDFRAME_H__
