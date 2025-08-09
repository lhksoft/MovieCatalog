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
#ifndef __LK_HYPERLINK_CTRL_H__
#define __LK_HYPERLINK_CTRL_H__
#pragma once

#include <wx/control.h>
#include <wx/menu.h>
#include <wx/event.h>

#define lkHL_ALIGN_LEFT         0x0001
#define lkHL_ALIGN_RIGHT        0x0002
#define lkHL_ALIGN_CENTER       0x0003		// LEFT | RIGHT = CENTER
#define lkHL_CONTEXTMENU        0x0004
#define lkHL_FONT_NO_UNDERLINE	0x0008
#define lkHL_DEFAULT_STYLE      (lkHL_CONTEXTMENU|wxBORDER_SIMPLE|lkHL_ALIGN_CENTER)

extern const char	lkHyperlinkCtrlNameStr[];

class lkHyperlinkCtrl : public wxControl
{
public:
	lkHyperlinkCtrl();
	lkHyperlinkCtrl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
					long style = lkHL_DEFAULT_STYLE, const wxValidator& validator = wxDefaultValidator);
	virtual ~lkHyperlinkCtrl();

	// Creation function (for two-step construction).
	bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
				long style = lkHL_DEFAULT_STYLE, const wxValidator& validator = wxDefaultValidator);


// get/set
public:
	wxColour				GetHoverColour(void) const;
	void					SetHoverColour(const wxColour& colour);

	wxColour				GetNormalColour(void) const;
	void					SetNormalColour(const wxColour& colour);

	wxColour				GetVisitedColour(void) const;
	void					SetVisitedColour(const wxColour& colour);

	wxString				GetURL(void) const;
	void					SetURL(const wxString& url);

	void					SetVisited(bool visited = true);
	bool					GetVisited(void) const;

	void					SetEmpty(void);

// Implementation
protected:
	// Returns the wxRect of the label of this hyperlink.
	// This is different from the clientsize's rectangle when
	// clientsize != bestsize and this rectangle is influenced
	// by the alignment of the label (wxHL_ALIGN_*).
	wxRect					GetLabelRect() const;

	// Override this handler to bind more menu-options
	virtual void			ConnectMenuHandlers(void);

	// creates a context menu
	void					DoContextMenu(const wxPoint&);
	// creates default menuitems, override to add or replace menuitems
	virtual void			CreateContextMenuItems(wxMenu*);

	// handles "Modify URL" menuitem
	virtual void			DoUrlModify(void);
	// handles "Follow link" menuitem
	virtual void			DoUrlFollow(void);
	// handles "Copy URL" menuitem
	virtual void			DoUrlCopy(void);
	// handles "Remove URL" menuitem
	virtual void			DoUrlRemove(void);
public:
	// Send wxHyperlinkEvent and open our link in the default browser if it
	// wasn't handled.
	//
	// not part of the public API but needs to be public as used by
	// GTK+ callbacks:
	void					SendEvent(void);

// event handlers
protected:
	// Renders the hyperlink.
	void					OnPaint(wxPaintEvent& event);

	// Handle set/kill focus events (invalidate for painting focus rect)
	void					OnFocus(wxFocusEvent& event);

	// Fire a HyperlinkEvent on space
	void					OnChar(wxKeyEvent& event);

	// If the click originates inside the bounding box of the label,
	// a flag is set so that an event will be fired when the left
	// button is released.
	void					OnLeftDown(wxMouseEvent& event);

	// If the click both originated and finished inside the bounding box
	// of the label, a HyperlinkEvent is fired.
	void					OnLeftUp(wxMouseEvent& event);

	// This will launch the popup-menu with options
	void					OnRightUp(wxMouseEvent& event);

	// Changes the cursor to a hand, if the mouse is inside the label's bounding box.
	void					OnMotion(wxMouseEvent& event);

	// Changes the cursor back to the default, if necessary.
	void					OnLeaveWindow(wxMouseEvent& event);

	// handles all internal menuitems
	void					OnMenu(wxCommandEvent&);

// overridden base class virtuals
protected:
	// Returns the best size for the window, which is the size needed to display the text label.
	virtual wxSize			DoGetBestClientSize() const wxOVERRIDE;

	virtual wxBorder		GetDefaultBorder() const wxOVERRIDE;

public:
	virtual bool			HasTransparentBackground() wxOVERRIDE;
	virtual void			SetLabel(const wxString& label) wxOVERRIDE;

// Attributes
protected:
	// URL associated with the link. This is transmitted inside
	// the HyperlinkEvent fired when the user clicks on the label.
	wxString				m_url;

	// Foreground colours for various link types.
	// NOTE: wxWindow::m_backgroundColour is used for background,
	//       wxWindow::m_foregroundColour is used to render non-visited links
	wxColour				m_hoverColour;
	wxColour				m_normalColour;
	wxColour				m_visitedColour;

	// True if the mouse cursor is inside the label's bounding box.
	bool					m_rollover;

	// True if the link has been clicked before.
	bool					m_visited;

	// True if a click is in progress (left button down) and the click
	// originated inside the label's bounding box.
	bool					m_clicking;

private:
	enum class menu_ids
	{
		ID_Modify = wxID_HIGHEST + 1,
		ID_Follow,
		ID_Copy,
		ID_Remove
	};

	wxWindowID				m_idModify;
	wxWindowID				m_idFollow;
	wxWindowID				m_idCopy;
	wxWindowID				m_idRemove;

private:
	// Common part of all ctors.
	void					Init(void);

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(lkHyperlinkCtrl);
};


///////////////////////////////////////////////////////////////////////////////
// lkHyperlinkEvent
///////////////////////////////////////////////////////////////////////////////

class lkHyperlinkEvent;

wxDECLARE_EVENT(lkEVT_HYPERLINK, lkHyperlinkEvent);

//
// An event fired when the user clicks on the label in a hyperlink control.
// See HyperlinkControl for details.
//
class lkHyperlinkEvent : public wxCommandEvent
{
public:
	lkHyperlinkEvent();
	lkHyperlinkEvent(wxObject* generator, wxWindowID id, const wxString& url);
	lkHyperlinkEvent(const lkHyperlinkEvent&);
		

	// Returns the URL associated with the hyperlink control
	// that the user clicked on.
	wxString				GetURL(void) const;
	void					SetURL(const wxString& url);

	virtual wxEvent*		Clone() const wxOVERRIDE;

private:

	// URL associated with the hyperlink control that the used clicked on.
	wxString				m_URL;

	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(lkHyperlinkEvent);
};


// ----------------------------------------------------------------------------
// event types and macros
// ----------------------------------------------------------------------------

typedef void (wxEvtHandler::* lkHyperlinkEventFunction)(lkHyperlinkEvent&);

#define lkHyperlinkEventHandler(func) \
    wxEVENT_HANDLER_CAST(lkHyperlinkEventFunction, func)

#define EVT_lkHYPERLINK(id, fn) \
    wx__DECLARE_EVT1(lkEVT_HYPERLINK, id, lkHyperlinkEventHandler(fn))


#endif // !__LK_HYPERLINK_CTRL_H__
