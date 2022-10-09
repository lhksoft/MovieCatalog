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
#ifndef __LK_DIALOG_EX_H__
#define __LK_DIALOG_EX_H__
#pragma once

#include "lkDialog.h"

#include <wx/statusbr.h>
#include <wx/menu.h>
#include <wx/toolbar.h>

class lkDialogEx : public lkDialog
{
public:
	lkDialogEx();
	virtual ~lkDialogEx();

	bool							Create(wxWindow* parent, const wxBitmap& background, wxWindowID id, const wxString& title,
										   const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE);
	bool							Create(wxWindow* parent, const wxImage& background, wxWindowID id, const wxString& title, 
										   const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE);


	// get the origin of the client area (which may be different from (0, 0)
	// if the frame has a toolbar) in client coordinates
	virtual wxPoint					GetClientAreaOrigin() const wxOVERRIDE;

	// menu bar functions -- from wxFrame
	// ------------------
#if wxUSE_MENUS
#if wxUSE_MENUBAR
	virtual void					SetMenuBar(wxMenuBar* menubar);
	virtual wxMenuBar*				GetMenuBar(void) const;

	// find the item by id in the frame menu bar: this is an internal function
	// and exists mainly in order to be overridden in the MDI parent frame
	// which also looks at its active child menu bar
	virtual wxMenuItem*				FindItemInMenuBar(int menuId) const;
#endif

	// generate menu command corresponding to the given menu item
	//
	// returns true if processed
	bool							ProcessCommand(wxMenuItem* item);

	// generate menu command corresponding to the given menu command id
	//
	// returns true if processed
	bool							ProcessCommand(int winid);
#else
	bool ProcessCommand(int WXUNUSED(winid))
	{
		return false;
	}
#endif // wxUSE_MENUS


	// status bar functions -- from wxFrame
	// --------------------
#if wxUSE_STATUSBAR
	// create the main status bar by calling OnCreateStatusBar()
	virtual wxStatusBar*			CreateStatusBar(int number = 1, long style = wxSTB_DEFAULT_STYLE, wxWindowID winid = 0);
	// return a new status bar
	virtual wxStatusBar*			OnCreateStatusBar(int number, long style, wxWindowID winid);
	// get the main status bar
	virtual wxStatusBar*			GetStatusBar(void) const;

	// sets the main status bar
	virtual void					SetStatusBar(wxStatusBar* statBar);

	// forward these to status bar
	virtual void					SetStatusText(const wxString& text, int number = 0);
	virtual void					SetStatusWidths(int n, const int widths_field[]);
	void							PushStatusText(const wxString& text, int number = 0);
	void							PopStatusText(int number = 0);

	// set the status bar pane the help will be shown in
	void							SetStatusBarPane(int n);
	int								GetStatusBarPane(void) const;
#endif // wxUSE_STATUSBAR

	// toolbar functions -- from wxFrame
	// -----------------
#if wxUSE_TOOLBAR
	// create main toolbar bycalling OnCreateToolBar()
	virtual wxToolBar*				CreateToolBar(long style = -1, wxWindowID winid = wxID_ANY, const wxString& name = wxASCII_STR(wxToolBarNameStr));
	// return a new toolbar
	virtual wxToolBar*				OnCreateToolBar(long style, wxWindowID winid, const wxString& name);

	// get/set the main toolbar
	virtual void					SetToolBar(wxToolBar* toolbar);
	virtual wxToolBar*				GetToolBar(void) const;
#endif // wxUSE_TOOLBAR


// Implementation
public:
	virtual bool					SendIdleEvents(wxIdleEvent& event) wxOVERRIDE;

	// event handlers
#if wxUSE_MENUS
	void							OnMenuOpen(wxMenuEvent& event);
#if wxUSE_STATUSBAR
	void							OnMenuClose(wxMenuEvent& event);
	void							OnMenuHighlight(wxMenuEvent& event);
#endif // wxUSE_STATUSBAR

	// send wxUpdateUIEvents for all menu items in the menubar,
	// or just for menu if non-NULL
	virtual void					DoMenuUpdates(wxMenu* menu = NULL);
#endif // wxUSE_MENUS

	// do the UI update processing for this window
	virtual void					UpdateWindowUI(long flags = wxUPDATE_UI_NONE) wxOVERRIDE;

	// Implement internal behaviour (menu updating on some platforms)
	virtual void					OnInternalIdle() wxOVERRIDE;

#if wxUSE_MENUS || wxUSE_TOOLBAR
	// show help text for the currently selected menu or toolbar item
	// (typically in the status bar) or hide it and restore the status bar text
	// originally shown before the menu was opened if show == false
	virtual void					DoGiveHelp(const wxString& text, bool show);
#endif

	virtual bool					IsClientAreaChild(const wxWindow* child) const wxOVERRIDE;

protected:
	// override base class virtuals
	virtual void					DoGetClientSize(int* width, int* height) const wxOVERRIDE;
	virtual void					DoSetClientSize(int width, int height) wxOVERRIDE;

	void							OnSize(wxSizeEvent& event);
	// the frame main menu/status/tool bars
	// ------------------------------------

	// this (non virtual!) function should be called from dtor to delete the
	// main menubar, statusbar and toolbar (if any)
	void							DeleteAllBars(void);

	// test whether this window makes part of the frame
	virtual bool					IsOneOfBars(const wxWindow* win) const wxOVERRIDE;

#if wxUSE_MENUBAR
	// override to update menu bar position when the frame size changes
	virtual void					PositionMenuBar(void);

	// override to do something special when the menu bar is being removed
	// from the frame
	virtual void					DetachMenuBar(void);

	// override to do something special when the menu bar is attached to the frame
	virtual void					AttachMenuBar(wxMenuBar* menubar);
#endif // wxUSE_MENUBAR

	// Return true if we should update the menu item state from idle event
	// handler or false if we should delay it until the menu is opened.
	static bool						ShouldUpdateMenuFromIdle(void);

#if wxUSE_MENUBAR
	wxMenuBar*						m_frameMenuBar;
#endif // wxUSE_MENUBAR

#if wxUSE_STATUSBAR && (wxUSE_MENUS || wxUSE_TOOLBAR)
	// the saved status bar text overwritten by DoGiveHelp()
	wxString						m_oldStatusText;

	// the last help string we have shown in the status bar
	wxString						m_lastHelpShown;
#endif

#if wxUSE_STATUSBAR
	// override to update status bar position (or anything else) when something changes
	virtual void					PositionStatusBar(void);

	// show the help string for the given menu item using DoGiveHelp() if the
	// given item does have a help string (as determined by FindInMenuBar()),
	// return false if there is no help for such item
	bool							ShowMenuHelp(int helpid);

	wxStatusBar*					m_frameStatusBar;
#endif // wxUSE_STATUSBAR


	int								m_statusBarPane;

#if wxUSE_TOOLBAR
	// override to update status bar position (or anything else) when something changes
	virtual void					PositionToolBar(void);

	wxToolBar*						m_frameToolBar;
#endif // wxUSE_TOOLBAR

	wxDECLARE_EVENT_TABLE();
	wxDECLARE_ABSTRACT_CLASS(lkDialogEx);
	wxDECLARE_NO_COPY_CLASS(lkDialogEx);
};

#endif // !__LK_DIALOG_EX_H__
