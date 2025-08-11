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
#include "MainFrame.h"
#include "MainDocManager.h"
#include "MainDocument.h"

#include "Compact.h"
#include <wx/msgdlg.h>
#include "Defines.h"

#include "../lkControls/lkConfigTools.h"

#include <wx/aboutdlg.h>
#include "XPMs.h"

#define conf_MAINFRAME_PATH				wxT("MainFrame")
#define conf_MAINFRAME_POSX				wxT("PosX")
#define conf_MAINFRAME_POSY				wxT("PosY")

wxIMPLEMENT_CLASS(MainFrame, wxDocParentFrame)

MainFrame::MainFrame() : wxDocParentFrame()
{
	m_pThreadFrame = NULL;
}
MainFrame::MainFrame(wxDocManager* manager, wxFrame* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) :
	wxDocParentFrame(manager, parent, id, title, pos, size, style)
{
	m_pThreadFrame = NULL;

	if ( (size.GetWidth() != -1) || (size.GetHeight() != -1) )
		SetMinSize(size);
	Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnCloseWindow, this);
	Bind(wxEVT_MENU, &MainFrame::OnCompact, this, ID_MAIN_Compact);
	Bind(lkEVT_COMPACT_EVENT, &MainFrame::OnThreadFinish, this, (int)CompactEvent::EventIDs::ID_exitCompacting);
	Bind(wxEVT_MENU, &MainFrame::OnAbout, this, ID_MAIN_About);
}

void MainFrame::OnCloseWindow(wxCloseEvent& event)
{
	if ( m_docManager && m_docManager->IsKindOf(wxCLASSINFO(MainDocManager)) )
	{
		MainDocManager* man = dynamic_cast<MainDocManager*>(m_docManager);
		if (!man->lkClear(!event.CanVeto()))
			event.Veto();
		else
		{
			SetConfigPosition();
			event.Skip();
		}
	}
	else
	{
		if ( m_docManager && !m_docManager->Clear(!event.CanVeto()) )
		{
			// The user decided not to close finally, abort.
			event.Veto();
		}
		else
		{
			SetConfigPosition();
			// Just skip the event, base class handler will destroy the window.
			event.Skip();
		}
	}
}

void MainFrame::OnCompact(wxCommandEvent& event)
{
	if ( wxMessageBox(wxT("This action will take some time.\nDo you want to continue?"), wxT("Compact"), wxYES_NO | wxNO_DEFAULT | wxICON_AUTH_NEEDED | wxICON_QUESTION, this) == wxNO )
		return;

	wxDocManager* dm = GetDocumentManager();
	wxDocument* doc = (dm) ? dm->GetCurrentDocument() : NULL;
	if ( doc && doc->IsKindOf(wxCLASSINFO(MainDocument)) )
	{
		wxString path = (dynamic_cast<MainDocument*>(doc))->GetFilename();
		if ( dm->CloseDocument(doc) )
		{
			m_pThreadFrame = new CompactFrame(path);
			if ( m_pThreadFrame && (dynamic_cast<CompactFrame*>(m_pThreadFrame))->Create(this) )
			{
				(dynamic_cast<CompactFrame*>(m_pThreadFrame))->Show(true);
				if ( (dynamic_cast<CompactFrame*>(m_pThreadFrame))->DoCompact() )
					return;
			}
			// something wend wrong ?
			if ( m_pThreadFrame )
				delete m_pThreadFrame;
			m_pThreadFrame = NULL;

			dm->CreateDocument(path, wxDOC_SILENT);
		}
		// else.. could not Close document
	}
}

void MainFrame::OnThreadFinish(CompactEvent& event)
{
	wxString path = event.GetString();
	if ( m_pThreadFrame )
		delete m_pThreadFrame;
	m_pThreadFrame = NULL;

	if ( !path.IsEmpty() )
	{
		wxDocManager* dm = GetDocumentManager();
		dm->CreateDocument(path, wxDOC_SILENT);
	}
}

void MainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxAboutDialogInfo info;
	info.SetName("Movies Catalog");
	info.SetVersion("3.0.1.5", "3.0.1.5 product version 7.1.0.0");
	info.SetDescription("Movies Catalog Program using wxWidgets 3.2.4 and SQLite3");
	wxString s;
	s.Printf("CopyLeft %c 2015 - '16,2019,2021-'22, 2025 by LhK-Soft, Laurens H.Koehoorn", wxUniChar(0x00A9));
	info.SetCopyright(s);
	info.AddDeveloper("Laurens Koehoorn");
	info.SetIcon(Get_MoviesCatalog_Ico());

	wxAboutBox(info);
}

void MainFrame::SetConfigPosition() // called in 'OnCloseWindow' -- will store frame's current position in cofig-file
{
	wxPoint pos = GetPosition();
	SetConfigInt(conf_MAINFRAME_PATH, conf_MAINFRAME_POSX, (wxInt64)pos.x);
	SetConfigInt(conf_MAINFRAME_PATH, conf_MAINFRAME_POSY, (wxInt64)pos.y);
}

//static
wxPoint	MainFrame::GetConfigPosition()
{
	wxPoint pos;
	int x, y;
	x = (int)GetConfigInt(conf_MAINFRAME_PATH, conf_MAINFRAME_POSX);
	y = (int)GetConfigInt(conf_MAINFRAME_PATH, conf_MAINFRAME_POSY);

	if ( x <= 0 ) x = -1;
	if ( y <= 0 ) y = -1;

	if ( (x < 0) && (y < 0) )
		return wxDefaultPosition;
	else
	{
		pos.x = x;
		pos.y = y;
	}
	return pos;
}
