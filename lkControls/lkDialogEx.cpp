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
#include "lkDialogEx.h"
#include <wx/app.h>

#if defined(__WXGTK20__)
#include "wrapgtk.h"
#endif // __WXGTK20__

wxBEGIN_EVENT_TABLE(lkDialogEx, lkDialog)
#if wxUSE_MENUS
	EVT_MENU_OPEN(lkDialogEx::OnMenuOpen)
#if wxUSE_STATUSBAR
	EVT_MENU_CLOSE(lkDialogEx::OnMenuClose)
	EVT_MENU_HIGHLIGHT_ALL(lkDialogEx::OnMenuHighlight)
#endif // wxUSE_STATUSBAR
#endif // wxUSE_MENUS
	EVT_SIZE(lkDialogEx::OnSize)
wxEND_EVENT_TABLE()

wxIMPLEMENT_ABSTRACT_CLASS(lkDialogEx, lkDialog)

lkDialogEx::lkDialogEx() : lkDialog()
{
#if wxUSE_MENUBAR
	m_frameMenuBar = NULL;
#endif // wxUSE_MENUS

#if wxUSE_TOOLBAR
	m_frameToolBar = NULL;
#endif // wxUSE_TOOLBAR

#if wxUSE_STATUSBAR
	m_frameStatusBar = NULL;
#endif // wxUSE_STATUSBAR

	m_statusBarPane = 0;
}
lkDialogEx::~lkDialogEx()
{
	DeleteAllBars();
}

bool lkDialogEx::Create(wxWindow* parent, const wxBitmap& background, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
{
	return lkDialog::Create(parent, background, id, title, pos, size, style);
}
bool lkDialogEx::Create(wxWindow* parent, const wxImage& background, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
{
	return lkDialog::Create(parent, background, id, title, pos, size, style);
}

void lkDialogEx::DeleteAllBars()
{
#if wxUSE_MENUBAR
	wxDELETE(m_frameMenuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
	wxDELETE(m_frameStatusBar);
#endif // wxUSE_STATUSBAR

#if wxUSE_TOOLBAR
	wxDELETE(m_frameToolBar);
#endif // wxUSE_TOOLBAR
}

//virtual
bool lkDialogEx::IsOneOfBars(const wxWindow* win) const
{
#if wxUSE_MENUBAR
	if ( win == GetMenuBar() )
		return true;
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
	if ( win == GetStatusBar() )
		return true;
#endif // wxUSE_STATUSBAR

#if wxUSE_TOOLBAR
	if ( win == GetToolBar() )
		return true;
#endif // wxUSE_TOOLBAR

	wxUnusedVar(win);

	return false;
}

// get the origin of the client area in the client coordinates
wxPoint lkDialogEx::GetClientAreaOrigin() const
{
	wxPoint pt = lkDialog::GetClientAreaOrigin();

#if defined(__WXUNIVERSAL__)
#if wxUSE_MENUS
	if ( m_frameMenuBar )
	{
		pt.y += m_frameMenuBar->GetSize().y;
	}
#endif // wxUSE_MENUS
#if wxUSE_TOOLBAR
	if ( m_frameToolBar )
	{
		if ( m_frameToolBar->GetWindowStyleFlag() & wxTB_VERTICAL )
			pt.x += m_frameToolBar->GetSize().x;
		else
			pt.y += m_frameToolBar->GetSize().y;
	}
#endif // wxUSE_TOOLBAR
#else
# if defined(__WXMSW__)
#if wxUSE_TOOLBAR
	wxToolBar* const toolbar = GetToolBar();
	if ( toolbar && toolbar->IsShown() )
	{
		const wxSize sizeTB = toolbar->GetSize();
		const int directionTB = toolbar->GetDirection();

		if ( directionTB == wxTB_TOP )
		{
			pt.y += sizeTB.y;
		}
		else if ( directionTB == wxTB_LEFT )
		{
			pt.x += sizeTB.x;
		}
	}
#endif // wxUSE_TOOLBAR
# elif defined(__WXGTK20__)
	return wxPoint(0, 0);
# else
#if wxUSE_TOOLBAR
	wxToolBar* toolbar = GetToolBar();
	if ( toolbar && toolbar->IsShown() )
	{
		int w, h;
		toolbar->GetSize(&w, &h);

		if ( toolbar->GetWindowStyleFlag() & wxTB_VERTICAL )
		{
			pt.x += w;
		}
		else
		{
			pt.y += h;
		}
	}
#endif // wxUSE_TOOLBAR
# endif
#endif

	return pt;
}

//virtual
bool lkDialogEx::IsClientAreaChild(const wxWindow* child) const
{
	return (!IsOneOfBars(child) && lkDialog::IsClientAreaChild(child));
}

//static
bool lkDialogEx::ShouldUpdateMenuFromIdle()
{
	// Usually this is determined at compile time and is determined by whether
	// the platform supports wxEVT_MENU_OPEN, however in wxGTK we need to also
	// check if we're using the global menu bar as we don't get EVT_MENU_OPEN
	// for it and need to fall back to idle time updating even if normally
	// wxUSE_IDLEMENUUPDATES is set to 0 for wxGTK.
#ifdef __WXGTK20__
	if ( wxApp::GTKIsUsingGlobalMenu() )
		return true;
#endif // !__WXGTK20__

	return (wxUSE_IDLEMENUUPDATES != 0);
}

bool lkDialogEx::SendIdleEvents(wxIdleEvent& event)
{
	bool needMore = lkDialog::SendIdleEvents(event);
#ifdef __WXGTK20__
# if wxUSE_MENUS
	if ( m_frameMenuBar && m_frameMenuBar->SendIdleEvents(event) )
		needMore = true;
# endif // wxUSE_MENUS
# if wxUSE_TOOLBAR
	if ( m_frameToolBar && m_frameToolBar->SendIdleEvents(event) )
		needMore = true;
# endif // wxUSE_TOOLBAR
# if wxUSE_STATUSBAR
	if ( m_frameStatusBar && m_frameStatusBar->SendIdleEvents(event) )
		needMore = true;
# endif // wxUSE_STATUSBAR
#endif // __WXGTK20__
	return needMore;
}


// ----------------------------------------------------------------------------
// misc
// ----------------------------------------------------------------------------

#if wxUSE_MENUS

bool lkDialogEx::ProcessCommand(int winid)
{
#if wxUSE_MENUBAR
	wxMenuItem* const item = FindItemInMenuBar(winid);
	if ( !item )
		return false;

	return ProcessCommand(item);
#else
	return false;
#endif
}

bool lkDialogEx::ProcessCommand(wxMenuItem* item)
{
	wxCHECK_MSG(item, false, wxS("Menu item can't be NULL"));

	if ( !item->IsEnabled() )
		return true;

	if ( (item->GetKind() == wxITEM_RADIO) && item->IsChecked() )
		return true;

	int checked;
	if ( item->IsCheckable() )
	{
		item->Toggle();

		// use the new value
		checked = item->IsChecked();
	}
	else // Uncheckable item.
	{
		checked = -1;
	}

	wxMenu* const menu = item->GetMenu();
	wxCHECK_MSG(menu, false, wxS("Menu item should be attached to a menu"));

	return menu->SendEvent(item->GetId(), checked);
}

#endif // wxUSE_MENUS

// Do the UI update processing for this window. This is
// provided for the application to call if it wants to
// force a UI update, particularly for the menus and toolbar.

//virtual
void lkDialogEx::UpdateWindowUI(long flags)
{
	wxWindowBase::UpdateWindowUI(flags);

#if wxUSE_TOOLBAR
	if ( GetToolBar() )
		GetToolBar()->UpdateWindowUI(flags);
#endif

#if wxUSE_MENUBAR
	if ( GetMenuBar() )
	{
		// If coming from an idle event, we only want to update the menus if
		// we're in the wxUSE_IDLEMENUUPDATES configuration, otherwise they
		// will be update when the menu is opened later
		if ( !(flags & wxUPDATE_UI_FROMIDLE) || ShouldUpdateMenuFromIdle() )
			DoMenuUpdates();
	}
#endif // wxUSE_MENUS
}

// ----------------------------------------------------------------------------
// event handlers for status bar updates from menus
// ----------------------------------------------------------------------------

#if wxUSE_MENUS

void lkDialogEx::OnMenuOpen(wxMenuEvent& event)
{
	event.Skip();

	if ( !ShouldUpdateMenuFromIdle() )
	{
		// as we didn't update the menus from idle time, do it now
		DoMenuUpdates(event.GetMenu());
	}
}

#if wxUSE_STATUSBAR

void lkDialogEx::OnMenuHighlight(wxMenuEvent& event)
{
	event.Skip();
	(void)ShowMenuHelp(event.GetMenuId());
}

void lkDialogEx::OnMenuClose(wxMenuEvent& event)
{
	event.Skip();
	DoGiveHelp(wxEmptyString, false);
}

#endif // wxUSE_STATUSBAR

#endif // wxUSE_MENUS

// Implement internal behaviour (menu updating on some platforms)

//virtual
void lkDialogEx::OnInternalIdle()
{
	lkDialog::OnInternalIdle();

#if wxUSE_MENUS
	if ( ShouldUpdateMenuFromIdle() && wxUpdateUIEvent::CanUpdate(this) )
		DoMenuUpdates();
#endif
}

// ----------------------------------------------------------------------------
// status bar stuff
// ----------------------------------------------------------------------------

#if wxUSE_STATUSBAR

//virtual
wxStatusBar* lkDialogEx::CreateStatusBar(int number, long style, wxWindowID id)
{
	// the main status bar can only be created once (or else it should be
	// deleted before calling CreateStatusBar() again)
	wxCHECK_MSG(!m_frameStatusBar, NULL, wxT("recreating status bar in lkDialogEx"));

	SetStatusBar(OnCreateStatusBar(number, style, id));

	return m_frameStatusBar;
}

wxStatusBar* lkDialogEx::OnCreateStatusBar(int number, long style, wxWindowID id)
{
	wxStatusBar* statusBar = new wxStatusBar(this, id, style);
	statusBar->SetFieldsCount(number);
	return statusBar;
}

//virtual
void lkDialogEx::SetStatusText(const wxString& text, int number)
{
	wxCHECK_RET(m_frameStatusBar != NULL, wxT("no statusbar to set text for"));
	m_frameStatusBar->SetStatusText(text, number);
}

//virtual
void lkDialogEx::SetStatusWidths(int n, const int widths_field[])
{
	wxCHECK_RET(m_frameStatusBar != NULL, wxT("no statusbar to set widths for"));
	m_frameStatusBar->SetStatusWidths(n, widths_field);
	PositionStatusBar();
}

void lkDialogEx::PushStatusText(const wxString& text, int number)
{
	wxCHECK_RET(m_frameStatusBar != NULL, wxT("no statusbar to set text for"));
	m_frameStatusBar->PushStatusText(text, number);
}

void lkDialogEx::PopStatusText(int number)
{
	wxCHECK_RET(m_frameStatusBar != NULL, wxT("no statusbar to set text for"));
	m_frameStatusBar->PopStatusText(number);
}

bool lkDialogEx::ShowMenuHelp(int helpid)
{
#if wxUSE_MENUS
	// if no help string found, we will clear the status bar text
	//
	// NB: wxID_NONE is used for (sub)menus themselves by wxMSW
	wxString helpString;
	if ( (helpid != wxID_SEPARATOR) && (helpid != wxID_NONE) )
	{
		const wxMenuItem* const item = FindItemInMenuBar(helpid);
		if ( item && !item->IsSeparator() )
			helpString = item->GetHelp();

		// notice that it's ok if we don't find the item because it might
		// belong to the popup menu, so don't assert here
	}

	DoGiveHelp(helpString, true);

	return !helpString.empty();
#else // !wxUSE_MENUS
	return false;
#endif // wxUSE_MENUS/!wxUSE_MENUS
}

//virtual
void lkDialogEx::SetStatusBar(wxStatusBar* statBar)
{
#ifdef __WXGTK20__
	m_frameStatusBar = statBar;
	if ( statBar )
	{
		// statusbar goes into bottom of vbox (m_mainWidget)
		gtk_container_remove(
			GTK_CONTAINER(gtk_widget_get_parent(statBar->m_widget)), statBar->m_widget);
		gtk_box_pack_end(GTK_BOX(m_mainWidget), statBar->m_widget, false, false, 0);
		// make sure next size_allocate on statusbar causes a size event
		statBar->m_useCachedClientSize = false;
		statBar->m_clientWidth = 0;
		int h = -1;
		if ( statBar->m_wxwindow )
		{
			// statusbar is not a native widget, need to set height request
			h = statBar->m_height;
		}
		gtk_widget_set_size_request(statBar->m_widget, -1, h);
	}
	// make sure next size_allocate causes a wxSizeEvent
	m_useCachedClientSize = false;
	m_clientWidth = 0;
#else
	bool hadBar = (m_frameStatusBar != NULL);
	m_frameStatusBar = statBar;

	if ( (m_frameStatusBar != NULL) != hadBar )
	{
		PositionStatusBar();
		Layout();
	}
#endif // __WXGTK20__
}

//virtual
wxStatusBar* lkDialogEx::GetStatusBar() const
{
	return m_frameStatusBar;
}

void lkDialogEx::SetStatusBarPane(int n)
{
	m_statusBarPane = n;
}

int lkDialogEx::GetStatusBarPane() const
{
	return m_statusBarPane;
}

//virtual
void lkDialogEx::PositionStatusBar()
{
#if defined(__WXMSW__)
	if ( !m_frameStatusBar || !m_frameStatusBar->IsShown() )
		return;

	int w, h;
	GetClientSize(&w, &h);

	int x = 0;

	wxToolBar* const toolbar = GetToolBar();
	if ( toolbar )
	{
		const wxSize sizeTB = toolbar->GetSize();
		const int directionTB = toolbar->GetDirection();

		if ( toolbar->IsVertical() )
		{
			if ( directionTB == wxTB_LEFT )
				x -= sizeTB.x;

			w += sizeTB.x;
		}
		else if ( directionTB == wxTB_BOTTOM )
		{
			// we need to position the status bar below the toolbar
			h += sizeTB.y;
		}
		//else: no adjustments necessary for the toolbar on top
	}

	// GetSize returns the height of the clientSize in which the statusbar
	// height is subtracted (see wxFrame::DoGetClientSize). When the DPI of the
	// window changes, the statusbar height will likely change so we need to
	// account for this difference. If not, the statusbar will be positioned
	// too high or low.
	int shOld;
	m_frameStatusBar->GetSize(NULL, &shOld);

	// Resize the status bar to its default height, as it could have been set
	// to a wrong value before by WM_SIZE sent during the frame creation and
	// our status bars preserve their programmatically set size to avoid being
	// resized by DefWindowProc() to the full window width, so if we didn't do
	// this here, the status bar would retain the possibly wrong current height.
	m_frameStatusBar->SetSize(x, h, w, wxDefaultCoord, wxSIZE_AUTO_HEIGHT);

	int sh;
	m_frameStatusBar->GetSize(NULL, &sh);
	h += shOld - sh;

	// Since we wish the status bar to be directly under the client area,
	// we use the adjusted sizes without using wxSIZE_NO_ADJUSTMENTS.
	m_frameStatusBar->SetSize(x, h, w, sh);
#endif // __WXMSW__
}


#endif // wxUSE_STATUSBAR

#if wxUSE_MENUS || wxUSE_TOOLBAR

//virtual
void lkDialogEx::DoGiveHelp(const wxString& help, bool show)
{
#if wxUSE_STATUSBAR
	if ( m_statusBarPane < 0 )
	{
		// status bar messages disabled
		return;
	}

	wxStatusBar* statbar = GetStatusBar();
	if ( !statbar )
		return;

	wxString text;
	if ( show )
	{
		// remember the old status bar text if this is the first time we're
		// called since the menu has been opened as we're going to overwrite it
		// in our DoGiveHelp() and we want to restore it when the menu is
		// closed
		//
		// note that it would be logical to do this in OnMenuOpen() but under
		// MSW we get an EVT_MENU_HIGHLIGHT before EVT_MENU_OPEN, strangely
		// enough, and so this doesn't work and instead we use the ugly trick
		// with using special m_oldStatusText value as "menu opened" (but it is
		// arguably better than adding yet another member variable to wxFrame
		// on all platforms)
		if ( m_oldStatusText.empty() )
		{
			m_oldStatusText = statbar->GetStatusText(m_statusBarPane);
			if ( m_oldStatusText.empty() )
			{
				// use special value to prevent us from doing this the next time
				m_oldStatusText += wxT('\0');
			}
		}

		m_lastHelpShown = text = help;
	}
	else // hide help, restore the original text
	{
		// clear the last shown help string but remember its value
		wxString lastHelpShown;
		lastHelpShown.swap(m_lastHelpShown);

		// also clear the old status text but remember it too to restore it
		// below
		text.swap(m_oldStatusText);

		if ( statbar->GetStatusText(m_statusBarPane) != lastHelpShown )
		{
			// if the text was changed with an explicit SetStatusText() call
			// from the user code in the meanwhile, do not overwrite it with
			// the old status bar contents -- this is almost certainly not what
			// the user expects and would be very hard to avoid from user code
			return;
		}
	}

	statbar->SetStatusText(text, m_statusBarPane);
#else
	wxUnusedVar(help);
	wxUnusedVar(show);
#endif // wxUSE_STATUSBAR
}

#endif // wxUSE_MENUS || wxUSE_TOOLBAR

// ----------------------------------------------------------------------------
// toolbar stuff
// ----------------------------------------------------------------------------

#if wxUSE_TOOLBAR

//virtual
wxToolBar* lkDialogEx::CreateToolBar(long style, wxWindowID id, const wxString& name)
{
	// the main toolbar can't be recreated (unless it was explicitly deleted
	// before)
	wxCHECK_MSG(!m_frameToolBar, NULL, wxT("recreating toolbar in wxFrame"));

	if ( style == -1 )
	{
		// use default style
		//
		// NB: we don't specify the default value in the method declaration
		//     because
		//      a) this allows us to have different defaults for different
		//         platforms (even if we don't have them right now)
		//      b) we don't need to include wx/toolbar.h in the header then
		style = wxTB_DEFAULT_STYLE;
	}

	SetToolBar(OnCreateToolBar(style, id, name));

	return m_frameToolBar;
}

//virtual
wxToolBar* lkDialogEx::OnCreateToolBar(long style, wxWindowID id, const wxString& name)
{
	return new wxToolBar(this, id, wxDefaultPosition, wxDefaultSize, style, name);
}

//virtual
void lkDialogEx::SetToolBar(wxToolBar* toolbar)
{
#ifdef __WXGTK20__
	m_frameToolBar = toolbar;
	if ( toolbar )
	{
		gtk_container_remove(
			GTK_CONTAINER(gtk_widget_get_parent(toolbar->m_widget)), toolbar->m_widget);
		if ( toolbar->IsVertical() )
		{
			// Vertical toolbar and m_wxwindow go into an hbox, inside the
			// vbox (m_mainWidget). hbox is created on demand.
			GtkWidget* hbox = gtk_widget_get_parent(m_wxwindow);
			if ( hbox == m_mainWidget )
			{
				hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
				gtk_widget_show(hbox);
				gtk_box_pack_start(GTK_BOX(m_mainWidget), hbox, true, true, 0);
				g_object_ref(m_wxwindow);
				gtk_container_remove(GTK_CONTAINER(m_mainWidget), m_wxwindow);
				gtk_box_pack_start(GTK_BOX(hbox), m_wxwindow, true, true, 0);
				g_object_unref(m_wxwindow);
			}
			gtk_box_pack_start(GTK_BOX(hbox), toolbar->m_widget, false, false, 0);

			int pos = 0;  // left
			if ( toolbar->HasFlag(wxTB_RIGHT) )
				pos = 1;  // right
			gtk_box_reorder_child(GTK_BOX(hbox), toolbar->m_widget, pos);
		}
		else
		{
			// Horizontal toolbar goes into vbox (m_mainWidget)
			gtk_box_pack_start(GTK_BOX(m_mainWidget), toolbar->m_widget, false, false, 0);

			int pos = 0;  // top
			if ( m_frameMenuBar )
				pos = 1;  // below menubar
			if ( toolbar->HasFlag(wxTB_BOTTOM) )
				pos += 2;  // below client area (m_wxwindow)
			gtk_box_reorder_child(
				GTK_BOX(m_mainWidget), toolbar->m_widget, pos);
		}
		// reset size request to allow native sizing to work
		gtk_widget_set_size_request(toolbar->m_widget, -1, -1);
	}
	// make sure next size_allocate causes a wxSizeEvent
	m_useCachedClientSize = false;
	m_clientWidth = 0;
#else
	if ( (toolbar != NULL) != (m_frameToolBar != NULL) )
	{
		// the toolbar visibility must have changed so we need to both position
		// the toolbar itself (if it appeared) and to relayout the frame
		// contents in any case

		if ( toolbar )
		{
			// we need to assign it to m_frameToolBar for PositionToolBar() to
			// do anything
			m_frameToolBar = toolbar;
			PositionToolBar();
		}
		//else: tricky: do not reset m_frameToolBar yet as otherwise Layout()
		//      wouldn't recognize the (still existing) toolbar as one of our
		//      bars and wouldn't layout the single child of the frame correctly


		// and this is even more tricky: we want Layout() to recognize the
		// old toolbar for the purpose of not counting it among our non-bar
		// children but we don't want to reserve any more space for it so we
		// temporarily hide it
		if ( m_frameToolBar )
			m_frameToolBar->Hide();

		Layout();

		if ( m_frameToolBar )
			m_frameToolBar->Show();
	}

	// this might have been already done above but it's simpler to just always
	// do it unconditionally instead of testing for whether we already did it
	m_frameToolBar = toolbar;
#endif // __GTK20__
}

//virtual
wxToolBar* lkDialogEx::GetToolBar() const
{
	return m_frameToolBar;
}

//virtual
void lkDialogEx::PositionToolBar()
{
#if defined(__WXUNIVERSAL__)
	if ( m_frameToolBar )
	{
		wxSize size = GetClientSize();
		int tw, th, tx, ty;

		tx = ty = 0;
		m_frameToolBar->GetSize(&tw, &th);
		if ( m_frameToolBar->GetWindowStyleFlag() & wxTB_VERTICAL )
		{
			tx = -tw;
			th = size.y;
		}
		else
		{
			ty = -th;
			tw = size.x;
		}

		m_frameToolBar->SetSize(tx, ty, tw, th);
	}
#else
# if defined(__WXMSW__)
	wxToolBar* toolbar = GetToolBar();
	if ( toolbar && toolbar->IsShown() )
	{
		// don't call our (or even wxTopLevelWindow) version because we want
		// the real (full) client area size, not excluding the tool/status bar
		int width, height;
		wxWindow::DoGetClientSize(&width, &height);

		wxStatusBar* statbar = GetStatusBar();
		if ( statbar && statbar->IsShown() )
		{
			height -= statbar->GetClientSize().y;
		}

		int tx, ty, tw, th;
		toolbar->GetPosition(&tx, &ty);
		toolbar->GetBestSize(&tw, &th);

		int x, y;
		if ( toolbar->HasFlag(wxTB_BOTTOM) )
		{
			x = 0;
			y = height - th;
		}
		else if ( toolbar->HasFlag(wxTB_RIGHT) )
		{
			x = width - tw;
			y = 0;
		}
		else // left or top
		{
			x = 0;
			y = 0;
		}

		if ( toolbar->HasFlag(wxTB_BOTTOM) )
		{
			if ( ty < 0 && (-ty == th) )
				ty = height - th;
			if ( tx < 0 && (-tx == tw) )
				tx = 0;
		}
		else if ( toolbar->HasFlag(wxTB_RIGHT) )
		{
			if ( ty < 0 && (-ty == th) )
				ty = 0;
			if ( tx < 0 && (-tx == tw) )
				tx = width - tw;
		}
		else // left or top
		{
			if ( ty < 0 && (-ty == th) )
				ty = 0;
			if ( tx < 0 && (-tx == tw) )
				tx = 0;
		}

		int desiredW,
			desiredH;

		if ( toolbar->IsVertical() )
		{
			desiredW = tw;
			desiredH = height;
		}
		else
		{
			desiredW = width;
			desiredH = th;
		}

		// use the 'real' MSW position here, don't offset relatively to the
		// client area origin
		toolbar->SetSize(x, y, desiredW, desiredH, wxSIZE_NO_ADJUSTMENTS);
	}
# elif defined(__WXMOTIF__)
	if ( m_frameToolBar )
	{
		wxSize size = GetClientSize();
		int tw, th, tx, ty;

		tx = ty = 0;
		m_frameToolBar->GetSize(&tw, &th);
		if ( m_frameToolBar->GetWindowStyleFlag() & wxTB_VERTICAL )
		{
			tx = -tw;
			th = size.y;
		}
		else
		{
			ty = -th;
			tw = size.x;
		}

		m_frameToolBar->SetSize(tx, ty, tw, th);
	}
# else
	// nothing
# endif
#endif
}

#endif // wxUSE_TOOLBAR

// ----------------------------------------------------------------------------
// menus
// ----------------------------------------------------------------------------

#if wxUSE_MENUS

// update all menus

//virtual
void lkDialogEx::DoMenuUpdates(wxMenu* menu)
{
	if ( menu )
	{
		menu->UpdateUI();
	}
#if wxUSE_MENUBAR
	else
	{
		wxMenuBar* bar = GetMenuBar();
		if ( bar != NULL )
			bar->UpdateMenus();
	}
#endif
}

#if wxUSE_MENUBAR

//virtual
void lkDialogEx::PositionMenuBar()
{
#if defined(__WXUNIVERSAL__)
	if ( m_frameMenuBar )
	{
		// the menubar is positioned above the client size, hence the negative
		// y coord
		wxCoord heightMbar = m_frameMenuBar->GetSize().y;
		wxCoord heightTbar = 0;

		if ( m_frameToolBar )
			heightTbar = m_frameToolBar->GetSize().y;

		m_frameMenuBar->SetSize(0, -(heightMbar + heightTbar), GetClientSize().x, heightMbar);
	}
#endif
}

//virtual
void lkDialogEx::DetachMenuBar()
{
#ifdef __WXGTK20__
	wxASSERT_MSG((m_widget != NULL), wxT("invalid frame"));
	wxASSERT_MSG((m_wxwindow != NULL), wxT("invalid frame"));

	if ( m_frameMenuBar )
		gtk_container_remove(GTK_CONTAINER(m_mainWidget), m_frameMenuBar->m_widget);
#endif // __WXGTK20__

	if ( m_frameMenuBar )
	{
		m_frameMenuBar->Detach();
		m_frameMenuBar = NULL;
	}

#ifdef __WXGTK20__
	// make sure next size_allocate causes a wxSizeEvent
	m_useCachedClientSize = false;
	m_clientWidth = 0;
#endif // __WXGTK20__
}

//virtual
void lkDialogEx::AttachMenuBar(wxMenuBar* menubar)
{
	if ( menubar )
	{
		menubar->Attach((wxFrame*)this);
		m_frameMenuBar = menubar;
	}

#ifdef __WXGTK20__
	if ( m_frameMenuBar )
	{
		// menubar goes into top of vbox (m_mainWidget)
		gtk_box_pack_start(
			GTK_BOX(m_mainWidget), menubar->m_widget, false, false, 0);
		gtk_box_reorder_child(GTK_BOX(m_mainWidget), menubar->m_widget, 0);

		// reset size request to allow native sizing to work
		gtk_widget_set_size_request(menubar->m_widget, -1, -1);

		gtk_widget_show(m_frameMenuBar->m_widget);
	}
	// make sure next size_allocate causes a wxSizeEvent
	m_useCachedClientSize = false;
	m_clientWidth = 0;
#endif // __WXGTK20__
}

//virtual
void lkDialogEx::SetMenuBar(wxMenuBar* menubar)
{
	if ( menubar == GetMenuBar() )
	{
		// nothing to do
		return;
	}

	DetachMenuBar();

	this->AttachMenuBar(menubar);
}

//virtual
wxMenuItem* lkDialogEx::FindItemInMenuBar(int menuId) const
{
	const wxMenuBar* const menuBar = GetMenuBar();
	return (menuBar) ? menuBar->FindItem(menuId) : NULL;
}

//virtual
wxMenuBar* lkDialogEx::GetMenuBar() const
{
	return m_frameMenuBar;
}

#endif // wxUSE_MENUBAR

#endif // wxUSE_MENUS

// override base class virtuals
// ----------------------------------------------------------------------------
// client size calculations
// ----------------------------------------------------------------------------

void lkDialogEx::DoSetClientSize(int width, int height)
{
#if defined(__WXUNIVERSAL__)
	#if wxUSE_MENUS
	if ( m_frameMenuBar )
	{
		height += m_frameMenuBar->GetSize().y;
	}
	#endif // wxUSE_MENUS

	#if wxUSE_STATUSBAR
	if ( m_frameStatusBar )
	{
		height += m_frameStatusBar->GetSize().y;
	}
	#endif // wxUSE_STATUSBAR

	#if wxUSE_TOOLBAR
	if ( m_frameToolBar )
	{
		if ( m_frameToolBar->GetWindowStyleFlag() & wxTB_VERTICAL )
			width += m_frameToolBar->GetSize().x;
		else
			height += m_frameToolBar->GetSize().y;
	}
	#endif // wxUSE_TOOLBAR

	wxFrameBase::DoSetClientSize(width, height);
#else
# if defined(__WXMSW__)
	// leave enough space for the status bar if we have (and show) it
	#if wxUSE_STATUSBAR
	wxStatusBar* statbar = GetStatusBar();
	if ( statbar && statbar->IsShown() )
	{
		height += statbar->GetSize().y;
	}
	#endif // wxUSE_STATUSBAR

	// call GetClientAreaOrigin() to take the toolbar into account
	wxPoint pt = GetClientAreaOrigin();
	width += pt.x;
	height += pt.y;

	#if wxUSE_TOOLBAR
	wxToolBar* const toolbar = GetToolBar();
	if ( toolbar )
	{
		if ( toolbar->HasFlag(wxTB_RIGHT | wxTB_BOTTOM) )
		{
			const wxSize sizeTB = toolbar->GetSize();
			if ( toolbar->HasFlag(wxTB_RIGHT) )
				width -= sizeTB.x;
			else // wxTB_BOTTOM
				height -= sizeTB.y;
		}
		//else: toolbar already taken into account by GetClientAreaOrigin()
	}
	#endif // wxUSE_TOOLBAR

	wxTopLevelWindow::DoSetClientSize(width, height);
# endif // __WXMSW__
#endif // __WXUNIVERSAL__
}

// Get size *available for subwindows* i.e. excluding menu bar, toolbar etc.
void lkDialogEx::DoGetClientSize(int* x, int* y) const
{
#if defined(__WXUNIVERSAL__)
	wxTopLevelWindow::DoGetClientSize(width, height);

	#if wxUSE_MENUS
	if ( m_frameMenuBar && height )
	{
		(*height) -= m_frameMenuBar->GetSize().y;
	}
	#endif // wxUSE_MENUS

	#if wxUSE_STATUSBAR
	if ( m_frameStatusBar && height )
	{
		(*height) -= m_frameStatusBar->GetSize().y;
	}
	#endif // wxUSE_STATUSBAR

	#if wxUSE_TOOLBAR
	if ( m_frameToolBar )
	{
		if ( width && (m_frameToolBar->GetWindowStyleFlag() & wxTB_VERTICAL) )
			(*width) -= m_frameToolBar->GetSize().x;
		else if ( height )
			(*height) -= m_frameToolBar->GetSize().y;
	}
	#endif // wxUSE_TOOLBAR
#else
# if defined(__WXMSW__)
	wxTopLevelWindow::DoGetClientSize(x, y);

	// account for the possible toolbar
	wxPoint pt = GetClientAreaOrigin();
	if ( x )
		*x -= pt.x;

	if ( y )
		*y -= pt.y;

	#if wxUSE_TOOLBAR
	wxToolBar* const toolbar = GetToolBar();
	if ( toolbar )
	{
		if ( toolbar->HasFlag(wxTB_RIGHT | wxTB_BOTTOM) )
		{
			const wxSize sizeTB = toolbar->GetSize();
			if ( toolbar->HasFlag(wxTB_RIGHT) )
			{
				if ( x )
					*x -= sizeTB.x;
			}
			else // wxTB_BOTTOM
			{
				if ( y )
					*y -= sizeTB.y;
			}
		}
		//else: toolbar already taken into account by GetClientAreaOrigin()
	}
	#endif // wxUSE_TOOLBAR

	#if wxUSE_STATUSBAR
	// adjust client area height to take the status bar into account
	if ( y )
	{
		wxStatusBar* statbar = GetStatusBar();
		if ( statbar && statbar->IsShown() )
		{
			*y -= statbar->GetSize().y;
		}
	}
	#endif // wxUSE_STATUSBAR
# elif defined(__WXGTK20__)
	wxASSERT_MSG((m_widget != NULL), wxT("invalid frame"));

    lkDialog::DoGetClientSize( x, y );

	if ( m_useCachedClientSize )
		return;

	if ( y )
	{
#if wxUSE_MENUS_NATIVE
		// menu bar
		if ( m_frameMenuBar && m_frameMenuBar->IsShown() )
		{
			int hh;
			gtk_widget_get_preferred_height(m_frameMenuBar->m_widget, NULL, &hh);
			*y -= hh;
		}
#endif // wxUSE_MENUS_NATIVE

#if wxUSE_STATUSBAR
		// status bar
		if ( m_frameStatusBar && m_frameStatusBar->IsShown() )
			*y -= m_frameStatusBar->m_height;
#endif // wxUSE_STATUSBAR
	}
#if wxUSE_TOOLBAR
	// tool bar
	if (m_frameToolBar && m_frameToolBar->IsShown())
	{
		if ( m_frameToolBar->IsVertical() )
		{
			if ( x )
			{
				int w;
				gtk_widget_get_preferred_width(m_frameToolBar->m_widget, NULL, &w);
				*x -= w;
			}
		}
		else
		{
			if ( y )
			{
				int h;
				gtk_widget_get_preferred_height(m_frameToolBar->m_widget, NULL, &h);
				*y -= h;
			}
		}
	}
#endif // wxUSE_TOOLBAR

	if ( x != NULL && *x < 0 )
		*x = 0;
	if ( y != NULL && *y < 0 )
		*y = 0;
# endif
#endif
}

void lkDialogEx::OnSize(wxSizeEvent& event)
{
	PositionMenuBar();
	PositionStatusBar();
	PositionToolBar();

	event.Skip();
}

