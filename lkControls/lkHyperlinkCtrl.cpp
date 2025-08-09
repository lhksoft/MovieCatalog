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
#include "lkHyperlinkCtrl.h"
#include "lkHyperlinkCtrl_private.h"

#include <wx/utils.h> // for wxLaunchDefaultBrowser
#include <wx/dcclient.h>
#include <wx/clipbrd.h>
#include <wx/renderer.h>
#include <wx/menuitem.h>

const char	lkHyperlinkCtrlNameStr[] = "lkHyperlinkCtrl";

wxIMPLEMENT_DYNAMIC_CLASS(lkHyperlinkCtrl, wxControl)

lkHyperlinkCtrl::lkHyperlinkCtrl() : wxControl()
{
	Init();
}
lkHyperlinkCtrl::lkHyperlinkCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator) : wxControl()
{
	Init();
	(void)Create(parent, id, pos, size, style, validator);
}
lkHyperlinkCtrl::~lkHyperlinkCtrl()
{
}

void lkHyperlinkCtrl::Init()
{
	m_rollover = false;
	m_clicking = false;
	m_visited = false;

	// colours
	m_normalColour = *wxBLUE;
	m_hoverColour = *wxRED;
	m_visitedColour = wxColour("#551a8b");
}

bool lkHyperlinkCtrl::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator)
{
	if ( !(style & wxFULL_REPAINT_ON_RESIZE) )
		style |= wxFULL_REPAINT_ON_RESIZE;

	if ( !wxControl::Create(parent, id, pos, size, style, validator, lkHyperlinkCtrlNameStr) )
		return false;

	// do some own creation..

	// by default the font of a hyperlink control is underlined
	if ( !(style & lkHL_FONT_NO_UNDERLINE) )
	{
		wxFont f = GetFont();
		f.SetUnderlined(true);
		SetFont(f);
	}

	SetForegroundColour(m_normalColour);

	SetInitialSize(size);

	// connect our event handlers:
	Bind(wxEVT_PAINT, &lkHyperlinkCtrl::OnPaint, this);
	Bind(wxEVT_SET_FOCUS, &lkHyperlinkCtrl::OnFocus, this);
	Bind(wxEVT_KILL_FOCUS, &lkHyperlinkCtrl::OnFocus, this);
	Bind(wxEVT_CHAR, &lkHyperlinkCtrl::OnChar, this);
	Bind(wxEVT_LEAVE_WINDOW, &lkHyperlinkCtrl::OnLeaveWindow, this);

	Bind(wxEVT_LEFT_DOWN, &lkHyperlinkCtrl::OnLeftDown, this);
	Bind(wxEVT_LEFT_UP, &lkHyperlinkCtrl::OnLeftUp, this);
	Bind(wxEVT_RIGHT_UP, &lkHyperlinkCtrl::OnRightUp, this);
	Bind(wxEVT_MOTION, &lkHyperlinkCtrl::OnMotion, this);

	ConnectMenuHandlers();

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// get/set
wxColour lkHyperlinkCtrl::GetHoverColour() const
{
	return m_hoverColour;
}
void lkHyperlinkCtrl::SetHoverColour(const wxColour& colour)
{
	m_hoverColour = colour;
}

wxColour lkHyperlinkCtrl::GetNormalColour() const
{
	return m_normalColour;
}
void lkHyperlinkCtrl::SetNormalColour(const wxColour& colour)
{
	m_normalColour = colour;
	if ( !m_visited )
	{
		SetForegroundColour(m_normalColour);
		Refresh();
	}
}

wxColour lkHyperlinkCtrl::GetVisitedColour() const
{
	return m_visitedColour;
}
void lkHyperlinkCtrl::SetVisitedColour(const wxColour& colour)
{
	m_visitedColour = colour;
	if ( m_visited )
	{
		SetForegroundColour(m_visitedColour);
		Refresh();
	}
}

wxString lkHyperlinkCtrl::GetURL() const
{
	return m_url;
}
void lkHyperlinkCtrl::SetURL(const wxString& url)
{
	m_url = url;
}

void lkHyperlinkCtrl::SetVisited(bool visited)
{
	m_visited = visited;

	if ( !m_visited )
		SetForegroundColour(m_normalColour);
	else
		SetForegroundColour(m_visitedColour);
}
bool lkHyperlinkCtrl::GetVisited() const
{
	return m_visited;
}

void lkHyperlinkCtrl::SetEmpty()
{
	SetURL(wxEmptyString);
	SetLabel(wxEmptyString);
	Refresh();
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation

//virtual
void lkHyperlinkCtrl::ConnectMenuHandlers()
{
	// Connect the event handlers for the context menu.
/*
	Bind(wxEVT_MENU, &lkHyperlinkCtrl::OnUrlModify, this, m_idModify);
	Bind(wxEVT_MENU, &lkHyperlinkCtrl::OnUrlFollow, this, m_idFollow);
	Bind(wxEVT_MENU, &lkHyperlinkCtrl::OnUrlCopy, this, m_idCopy);
	Bind(wxEVT_MENU, &lkHyperlinkCtrl::OnUrlRemove, this, m_idRemove);
*/
}

wxRect lkHyperlinkCtrl::GetLabelRect() const
{
	wxClientDC dc(const_cast<lkHyperlinkCtrl*>(this));
	wxSize sz = dc.GetTextExtent(GetLabel());

	// our best size is always the size of the label without borders
	wxSize c(GetClientSize()), b(sz); // b(GetBestSize());
	wxPoint offset;

	// the label is always centered vertically
	offset.y = (c.GetHeight() - b.GetHeight()) / 2;

	if ( (m_windowStyle & lkHL_ALIGN_CENTER) == lkHL_ALIGN_CENTER )
		offset.x = (c.GetWidth() - b.GetWidth()) / 2;
	else
	{
		if ( HasFlag(lkHL_ALIGN_RIGHT) )
			offset.x = c.GetWidth() - b.GetWidth();
		else // if ( HasFlag(lkHL_ALIGN_LEFT) )
			offset.x = 0;
	}

	return wxRect(offset, b);
}

//virtual
void lkHyperlinkCtrl::DoContextMenu(const wxPoint& pos)
{
	wxMenu* menuPopUp = new wxMenu(wxEmptyString, wxMENU_TEAROFF);
	CreateContextMenuItems(menuPopUp);
	menuPopUp->Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(lkHyperlinkCtrl::OnMenu), NULL, this);
	PopupMenu(menuPopUp, pos);
	delete menuPopUp;
}
//virtual
void lkHyperlinkCtrl::CreateContextMenuItems(wxMenu* popup)
{
	if ( !popup )
		return;

	wxMenuItem* item;
	popup->Append(item = new wxMenuItem(popup, (wxWindowID)lkHyperlinkCtrl::menu_ids::ID_Modify, wxT("&Modify"), wxT("Change/Set the URL")));

	popup->Append(item = new wxMenuItem(popup, (wxWindowID)lkHyperlinkCtrl::menu_ids::ID_Follow, wxT("&Follow"), wxT("Open this link in your web-browser")));
	item->Enable(!GetLabel().IsEmpty());
	popup->AppendSeparator();
#if wxUSE_CLIPBOARD
	popup->Append(item = new wxMenuItem(popup, (wxWindowID)lkHyperlinkCtrl::menu_ids::ID_Copy, wxT("&Copy"), wxT("Copy this link to the Clipboard")));
	item->Enable(!GetLabel().IsEmpty());
#endif // wxUSE_CLIPBOARD
	popup->Append(item = new wxMenuItem(popup, (wxWindowID)lkHyperlinkCtrl::menu_ids::ID_Remove, wxT("&Remove"), wxT("Remove the current URL")));
	item->Enable(!GetLabel().IsEmpty());
}

//virtual
void lkHyperlinkCtrl::DoUrlModify()
{
	lkHyperlinkDialog dlg;
	dlg.SetUriLabel(GetLabel());
	dlg.SetURL(GetURL());

	if ( dlg.Create(this) )
		if ( dlg.ShowModal() == wxID_OK )
		{
			SetURL(dlg.GetURL());
			SetLabel(dlg.GetUriLabel());
//			Refresh();
		}
}

//virtual
void lkHyperlinkCtrl::DoUrlFollow()
{
	SetForegroundColour(m_visitedColour);
	m_visited = true;
	m_clicking = false;

	// send the event
	SendEvent();
}

//virtual
void lkHyperlinkCtrl::DoUrlCopy()
{
#if wxUSE_CLIPBOARD
	if ( !wxTheClipboard->Open() )
		return;

	wxTextDataObject* data = new wxTextDataObject(m_url);
	wxTheClipboard->SetData(data);
	wxTheClipboard->Close();
#endif // wxUSE_CLIPBOARD
}

//virtual
void lkHyperlinkCtrl::DoUrlRemove()
{
	m_clicking = false;
	SetEmpty();
}

void lkHyperlinkCtrl::SendEvent()
{
	const wxString& url = GetURL();
	lkHyperlinkEvent linkEvent(this, GetId(), url);
	if ( !GetEventHandler()->ProcessEvent(linkEvent) )     // was the event skipped ?
		wxLaunchDefaultBrowser(url);
}


////////////////////////////////////////////////////////////////////////////////////////////////
// event handlers

void lkHyperlinkCtrl::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	wxPaintDC dc(this);
	dc.SetFont(GetFont());
	dc.SetTextForeground(GetForegroundColour());
	if ( !HasTransparentBackground() )
		dc.SetTextBackground(GetBackgroundColour());

	const wxString text = wxControl::Ellipsize(GetLabel(), dc, wxELLIPSIZE_MIDDLE, GetClientSize().GetWidth());
	dc.DrawText(text, GetLabelRect().GetTopLeft());
	if ( HasFocus() )
	{
		wxRendererNative::Get().DrawFocusRect(this, dc, GetClientRect(), wxCONTROL_SELECTED);
	}
}

void lkHyperlinkCtrl::OnFocus(wxFocusEvent& event)
{
	Refresh();
	event.Skip();
}

void lkHyperlinkCtrl::OnChar(wxKeyEvent& event)
{
	switch ( event.m_keyCode )
	{
		default:
			event.Skip();
			break;
		case WXK_SPACE:
		case WXK_NUMPAD_SPACE:
			SetForegroundColour(m_visitedColour);
			m_visited = true;
			SendEvent();
			break;
	}
}

void lkHyperlinkCtrl::OnLeftDown(wxMouseEvent& event)
{
	// the left click must start from the hyperlink rect
	m_clicking = GetLabelRect().Contains(event.GetPosition());
}

void lkHyperlinkCtrl::OnLeftUp(wxMouseEvent& event)
{
	// the click must be started and ended in the hyperlink rect
	if ( !m_clicking || !GetLabelRect().Contains(event.GetPosition()) )
		return;

	SetForegroundColour(m_visitedColour);
	m_visited = true;
	m_clicking = false;

	// send the event
	SendEvent();
}

void lkHyperlinkCtrl::OnRightUp(wxMouseEvent& event)
{
	if ( GetWindowStyle() & lkHL_CONTEXTMENU )
//		if ( GetLabelRect().Contains(event.GetPosition()) )
			DoContextMenu(wxPoint(event.m_x, event.m_y));
}

void lkHyperlinkCtrl::OnMotion(wxMouseEvent& event)
{
	wxRect textrc = GetLabelRect();

	if ( textrc.Contains(event.GetPosition()) )
	{
		if ( !m_rollover )
		{
			SetCursor(wxCursor(wxCURSOR_HAND));
			SetForegroundColour(m_hoverColour);
			m_rollover = true;
			Refresh();
		}
		if ( !GetURL().IsEmpty() && !GetLabel().IsEmpty() && !GetURL().IsSameAs(GetLabel(), false) )
			SetToolTip(GetURL());
	}
	else if ( m_rollover )
	{
		SetCursor(*wxSTANDARD_CURSOR);
		SetForegroundColour(!m_visited ? m_normalColour : m_visitedColour);
		m_rollover = false;
		Refresh();

		UnsetToolTip();
	}
}

void lkHyperlinkCtrl::OnLeaveWindow(wxMouseEvent& WXUNUSED(event))
{
	// NB: when the label rect and the client size rect have the same
	//     height this function is indispensable to remove the "rollover"
	//     effect as the OnMotion() event handler could not be called
	//     in that case moving the mouse out of the label vertically...

	if ( m_rollover )
	{
		SetCursor(*wxSTANDARD_CURSOR);
		SetForegroundColour(!m_visited ? m_normalColour : m_visitedColour);
		m_rollover = false;
		Refresh();
	}
}

void lkHyperlinkCtrl::OnMenu(wxCommandEvent& event)
{
	switch ( (lkHyperlinkCtrl::menu_ids)event.GetId() )
	{
		case lkHyperlinkCtrl::menu_ids::ID_Modify:
			DoUrlModify();
			break;
		case lkHyperlinkCtrl::menu_ids::ID_Follow:
			DoUrlFollow();
			break;
		case lkHyperlinkCtrl::menu_ids::ID_Copy:
			DoUrlCopy();
			break;
		case lkHyperlinkCtrl::menu_ids::ID_Remove:
			DoUrlRemove();
			break;
		default:
			event.Skip();
			break;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////
// overridden base class virtuals

// virtual
wxSize lkHyperlinkCtrl::DoGetBestClientSize() const
{
	wxClientDC dc(const_cast<lkHyperlinkCtrl*>(this));
	wxString s = (!GetLabel().IsEmpty()) ? GetLabel() : wxT("//AAaabbBBppjjQQGgg");
	wxSize sz = dc.GetTextExtent(s);
	// add some more space vertically
	sz.y += 2;
	return sz;
}

//virtual
wxBorder lkHyperlinkCtrl::GetDefaultBorder() const
{
	return wxBORDER_NONE;
}

//virtual
bool lkHyperlinkCtrl::HasTransparentBackground()
{
	return true;
}

//virtual
void lkHyperlinkCtrl::SetLabel(const wxString& label)
{
	wxControl::SetLabel(label);
	Refresh();
}


///////////////////////////////////////////////////////////////////////////////
// wxHyperlinkEvent
///////////////////////////////////////////////////////////////////////////////
wxDEFINE_EVENT(lkEVT_HYPERLINK, lkHyperlinkEvent);

wxIMPLEMENT_DYNAMIC_CLASS(lkHyperlinkEvent, wxCommandEvent)

lkHyperlinkEvent::lkHyperlinkEvent() : wxCommandEvent(), m_URL()
{
}
lkHyperlinkEvent::lkHyperlinkEvent(wxObject* generator, wxWindowID id, const wxString& url) : wxCommandEvent(lkEVT_HYPERLINK, id), m_URL(url)
{
	SetEventObject(generator);
}
lkHyperlinkEvent::lkHyperlinkEvent(const lkHyperlinkEvent& other) : wxCommandEvent(other), m_URL()
{
	m_URL = other.m_URL;
}

wxString lkHyperlinkEvent::GetURL() const
{
	return m_URL;
}
void lkHyperlinkEvent::SetURL(const wxString& url)
{
	m_URL = url;
}

wxEvent* lkHyperlinkEvent::Clone() const
{
	return new lkHyperlinkEvent(*this);
}
