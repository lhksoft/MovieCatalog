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
#include "MainView.h"
#include "Backgrounds.h"

#include <wx/filename.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/listctrl.h>
#include <wx/menu.h>

#include "../lkControls/lkColour.h"
#include "../lkControls/lkStaticText.h"

#include "../lkSQLite3/lkTSqliteMaster.h"
#include "../lkSQLite3/lkSQL3Exception.h"

#include "main.h"
#include "bitmaps/Table.xpm"

//#include <lk/lkTransTextCtrl.h>

#include "Defines.h"

// /////////////////////////////////////////////////////////////////////////////////////////////
// class MainCanvas
// /////////////////////////////////////////////////////////////////////////////////////////////
wxIMPLEMENT_CLASS(MainCanvas, lkCanvas)

MainCanvas::MainCanvas(wxView* view, wxWindow* parent) : lkCanvas(view, parent)
{
	m_pText = NULL;
	m_pTablesList = NULL;

	Create();
}
MainCanvas::~MainCanvas()
{
}

bool MainCanvas::Create()
{
	if ( !lkCanvas::Create(GetParent(), GetImage_FBG_DATABASE()) )
		return false;

	wxBoxSizer* szVer = new wxBoxSizer(wxVERTICAL);
	{
		long border = wxBORDER_SUNKEN;
#ifdef __WXMSW__
		border = wxBORDER_STATIC;
#endif // __WXMSW__
		wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
		szHor->Add(new wxStaticText(this, wxID_ANY, wxT("Path :"), wxDefaultPosition, wxSize(60, -1), wxALIGN_RIGHT), 0, wxRIGHT | wxBOTTOM | wxALIGN_CENTER, 5);
		szHor->Add((m_pText = new lkStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, border | wxST_NO_AUTORESIZE)), 1, wxBOTTOM | wxEXPAND, 5);
		szVer->Add(szHor, 0, wxLEFT | wxTOP | wxRIGHT | wxEXPAND, 10);
	}
	{
		long border = wxBORDER_SIMPLE;
#ifdef __WXMSW__
		border = wxBORDER_STATIC;
#endif // __WXMSW__
		wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
		szHor->Add(new wxStaticText(this, wxID_ANY, wxT("Tables :"), wxDefaultPosition, wxSize(60, -1), wxALIGN_RIGHT), 0, wxRIGHT | wxALIGN_TOP, 5);
		szHor->Add(m_pTablesList = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, border | wxLC_LIST | wxLC_SINGLE_SEL), 1, wxEXPAND, 0);
		szVer->Add(szHor, 1, wxLEFT | wxBOTTOM | wxRIGHT | wxEXPAND, 10);
	}
	{
		wxImage img(TABLE_XPM);
		wxBitmap bmp(img);
		wxImageList* imgList = new wxImageList(16, 16);
		imgList->Add(bmp, MAKE_RGB(0x00, 0x80, 0x80));
		(dynamic_cast<wxListCtrl*>(m_pTablesList))->AssignImageList(imgList, wxIMAGE_LIST_SMALL);
	}

	SetSizer(szVer);
	Layout();
	return true;
}

void MainCanvas::UpdateData()
{
	if ( m_pView )
	{
		// we should initialise the controls with the data of the just yet opened document
		wxDocument* pDoc = m_pView->GetDocument();
		if ( pDoc )
		{
			if ( m_pText )
			{
				lkStaticText* txtC = dynamic_cast<lkStaticText*>(m_pText);
				txtC->SetLabel(pDoc->GetFilename());
			}

			BuildTablesList();
		}
	}
	else
	{
		if ( m_pText )
		{
			lkStaticText* txtC = dynamic_cast<lkStaticText*>(m_pText);
			txtC->SetLabel(wxEmptyString);
		}
		if ( m_pTablesList )
		{
			wxListCtrl* pTablesList = dynamic_cast<wxListCtrl*>(m_pTablesList);
			pTablesList->DeleteAllItems();
		}
	}
}

void MainCanvas::BuildTablesList()
{
	wxASSERT(m_pTablesList != NULL);
	if ( !m_pTablesList )
		return;
	MainDocument* pDoc = NULL;
	if ( m_pView )
		pDoc = dynamic_cast<MainDocument*>(m_pView->GetDocument());
	else
		return;
	if ( !pDoc || !pDoc->IsOk() )
		return;

	wxListCtrl* pTablesList = dynamic_cast<wxListCtrl*>(m_pTablesList);
	// be sure Tables List is empty
	pTablesList->DeleteAllItems();

	wxString sql;
	lkTSqliteMaster rs(pDoc->GetDB());
	sql.Printf(wxT("[%s] = 'table'"), t3SqliteMaster_TYPE);
	rs.SetFilter(sql);
	sql.Printf(wxT("[%s]"), t3SqliteMaster_NAME);
	rs.SetOrder(sql);

	long idx, cnt;
	//	wxUint64 rowcnt;
	try
	{
		rs.Open();
		while ( !rs.IsEOF() )
		{
			cnt = pTablesList->GetItemCount();

			sql = rs.GetFldName();
			//			rowcnt = lkTSqliteMaster::GetRowCount(&(pDoc->m_DB), sql);
			//			sql.Printf(wxT("%I64u"), rowcnt);
			idx = pTablesList->InsertItem(cnt, sql, 0);

			//			sql = rs.GetFldName();
			//			pTablesList->SetItem(idx, 1, sql);

			rs.MoveNext();
		}
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
	}
}


// /////////////////////////////////////////////////////////////////////////////////////////////
// class MainView
// /////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_DYNAMIC_CLASS(MainView, wxView)

MainView::MainView() : wxView()
{
	m_pCanvas = NULL;
}

//virtual
bool MainView::OnCreate(wxDocument* doc, long flags)
{
	if ( !wxView::OnCreate(doc, flags) )
		return false;

	MyApp& app = wxGetApp();

	wxWindow* top = app.GetTopWindow();
	if ( top && top->IsKindOf(wxCLASSINFO(wxFrame)) )
		SetFrame(top);

	m_pCanvas = app.GetMainCanvas();
	wxASSERT(m_pCanvas != NULL);
	m_pCanvas->SetView(this);

	return true;
}

//virtual
void MainView::OnDraw(wxDC* dc)
{
	// nothing to do here, m_pCanvas draws itself
}

wxMenu* CreateViewsMenu()
{
	// Main menu Views
	wxMenu* menu = new wxMenu();
	// Sub Menu Maintenance
	wxMenu* sub = new wxMenu();
	sub->Append(ID_MENU_Views_Category, wxT("C&ategory"), wxT("Display/Edit Categories table"));
	sub->Append(ID_MENU_Views_Country, wxT("&Country"), wxT("Display/Edit Country table"));
	sub->Append(ID_MENU_Views_Genres, wxT("&Genres"), wxT("Display/Edit Genres table"));
	sub->Append(ID_MENU_Views_Judge, wxT("&Judgement"), wxT("Display/Edit Judgement table"));
	sub->Append(ID_MENU_Views_Location, wxT("&Location"), wxT("Display/Edit Location table"));
	sub->Append(ID_MENU_Views_Media, wxT("&Media"), wxT("Display/Edit Media table"));
	sub->Append(ID_MENU_Views_Origine, wxT("&Origine"), wxT("Display/Edit Origine table"));
	sub->Append(ID_MENU_Views_Quality, wxT("&Quality"), wxT("Display/Edit Quality table"));
	sub->Append(ID_MENU_Views_Rating, wxT("&Rating"), wxT("Display/Edit Rating table"));
	sub->Append(ID_MENU_Views_Screen, wxT("Scr&een"), wxT("Display/Edit Screen table"));
	sub->Append(ID_MENU_Views_Storage, wxT("&Storages"), wxT("Display/Edit Storages table"));
	menu->AppendSubMenu(sub, wxT("M&aintenance"), wxT("Minor Views"));
	menu->Append(ID_MENU_Views_Base, wxT("&Base"), wxT("Display/Edit Main table"));
	menu->Append(ID_MENU_Views_Movies, wxT("&Movies"), wxT("Display/Edit Movies table"));

	return menu;
}

//virtual
void MainView::OnUpdate(wxView* sender, wxObject* hint)
{
	wxView::OnUpdate(sender, hint);

	if ( sender == NULL )
	{
		if ( hint == NULL || !hint->IsKindOf(wxCLASSINFO(lkStringObject)) )
			return; // then nothing to do

		if ( m_pCanvas )
		{
			// if we're here, a document just got opened or created as new
			wxWindow* pFrame = GetFrame();
			if ( pFrame )
			{
				if ( hint->IsKindOf(wxCLASSINFO(lkStringObject)) )
				{
					wxString sPath = ((lkStringObject*)hint)->GetString();
					wxString fn, fe;
					wxFileName::SplitPath(sPath, NULL, NULL, &fn, &fe);

					wxString sTitle;
					sTitle.Printf(wxT("%s -- %s%s%s"), wxTheApp->GetAppDisplayName(), fn, (fe.IsEmpty()) ? wxT("") : wxT("."), fe);

					wxStaticCast(pFrame, wxFrame)->SetTitle(sTitle);
				}

				if ( pFrame->IsKindOf(wxCLASSINFO(wxFrame)) )
				{
					wxMenuBar* bar = (dynamic_cast<wxFrame*>(pFrame))->GetMenuBar();
					if ( bar )
					{
						int vwsMnu = bar->FindMenu(wxT("Views"));
						if ( vwsMnu != wxNOT_FOUND )
						{
							wxMenu* menu = bar->Remove((size_t)vwsMnu);
							if ( menu )
								delete menu;
						}

						wxMenu* viewsMenu = CreateViewsMenu();
						bar->Append(viewsMenu, wxT("&Views"));
					}
				}
			}
			m_pCanvas->UpdateData();
			m_pCanvas->Refresh();
		}
	}
}

// Clean up windows used for displaying the view.
bool MainView::OnClose(bool deleteWindow)
{
	if ( !wxView::OnClose(deleteWindow) )
		return false;

	wxWindow* frame = GetFrame();
	if ( frame && frame->IsKindOf(wxCLASSINFO(wxFrame)) )
	{
		wxMenuBar* bar = (dynamic_cast<wxFrame*>(frame))->GetMenuBar();
		if ( bar )
		{
			int vwsMnu = bar->FindMenu(wxT("Views"));
			if ( vwsMnu != wxNOT_FOUND )
			{
				wxMenu* menu = bar->Remove((size_t)vwsMnu);
				if ( menu )
					delete menu;
			}
		}
	}

	Activate(false);

	// Clear the canvas in single-window mode in which it stays alive
	if ( m_pCanvas )
	{
		//		m_pCanvas->ClearBackground();
		m_pCanvas->ResetView();
		m_pCanvas->UpdateData();
		m_pCanvas = NULL;

		if ( GetFrame() )
		{
			wxStaticCast(GetFrame(), wxFrame)->SetTitle(wxTheApp->GetAppDisplayName());
			SetFrame(NULL);
		}
	}

	return true;
}

MainDocument* MainView::GetDocument()
{
	wxASSERT(wxView::GetDocument() && wxView::GetDocument()->IsKindOf(wxCLASSINFO(MainDocument)));
	return wxStaticCast(wxView::GetDocument(), MainDocument);
}

