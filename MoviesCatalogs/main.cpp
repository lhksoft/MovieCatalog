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
/**************************************************************************************
* \file         main.cpp
* \brief        Application source file.
*
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include "main.h"
#include <wx/config.h>
#include "lkGUIAppTraits.h"

#include "MainDocManager.h"

#include "VBase.h"
#include "VCategory.h"
#include "VCountry.h"
#include "VGenre.h"
#include "VJudge.h"
#include "VLocation.h"
#include "VMedium.h"
#include "VMovies.h"
#include "VOrigine.h"
#include "VQuality.h"
#include "VRating.h"
#include "VScreen.h"
#include "VStorage.h"

#include "MainFrame.h"

#include "Defines.h"

#include "bitmaps/MoviesCatalog.xpm"

#include <wx/menu.h>
#include <wx/stockitem.h>

#include "XPMs.h"

/****************************************************************************************
* Macro definitions
****************************************************************************************/
/* Macro which creates the application instance and starts it. */
IMPLEMENT_APP(MyApp)

/************************************************************************************//**
** \brief     Called upon application initialization. 
** \return    True if the initialization was successful. False if anything failed, which
**            prevents the application from continuing.
**
****************************************************************************************/

MyApp::MyApp() : wxApp()
{
    m_pCanvas = NULL;
}

wxAppTraits* MyApp::CreateTraits()
{
    return new lkGUIAppTraits();
}

bool MyApp::OnInit()
{
    if (!wxApp::OnInit())
        return false;

    wxInitAllImageHandlers();

    // Fill in the application information fields before creating wxConfig.
    SetVendorName("LhK-Soft");
    SetAppName("MoviesCatalog");
    SetAppDisplayName("Movies Catalog II");

	wxStandardPathsBase& stdp = wxStandardPaths::Get();
	stdp.UseAppInfo(wxStandardPathsBase::AppInfo_AppName | wxStandardPathsBase::AppInfo_VendorName);

    //// Create a document manager
    MainDocManager* docManager = new MainDocManager();

    //// Create a template relating to sqlite3 documents to their views
    new wxDocTemplate(docManager, "Movies Database files", "*.sqlite", "", "sqlite",
                      "Movies Sqlite Doc", "Movies Catalog View",
                      CLASSINFO(MainDocument), CLASSINFO(MainView));
	// Create its child-templates
	CategoryTemplate::CreateTemplate(docManager);
	CountryTemplate::CreateTemplate(docManager);
	GenreTemplate::CreateTemplate(docManager);
	JudgeTemplate::CreateTemplate(docManager);
	LocationTemplate::CreateTemplate(docManager);
	MediumTemplate::CreateTemplate(docManager);
	MoviesTemplate::CreateTemplate(docManager);
	OrigineTemplate::CreateTemplate(docManager);
	QualityTemplate::CreateTemplate(docManager);
	RatingTemplate::CreateTemplate(docManager);
	ScreenTemplate::CreateTemplate(docManager);
	StorageTemplate::CreateTemplate(docManager);
	BaseTemplate::CreateTemplate(docManager);

    docManager->SetMaxDocsOpen(1);

//    lkTestFrame* moviesWindow = new lkTestFrame();
//	SetTopWindow(moviesWindow);

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
//	moviesWindow->Show(true);

    // create the main frame window
	wxPoint pos = MainFrame::GetConfigPosition();
    wxFrame* frame = new MainFrame(docManager, NULL, wxID_ANY,
									 GetAppDisplayName(),
									 pos,
									 wxSize(500, 200));

    m_pCanvas = new MainCanvas(NULL, frame);

    // and its menu bar
    wxMenu* menuFile = new wxMenu;

    menuFile->Append(wxID_NEW);
    menuFile->Append(wxID_OPEN);
    menuFile->Append(wxID_CLOSE);
    menuFile->AppendSeparator();
	menuFile->Append(ID_MAIN_Compact, wxT("Compact"), wxT("Compact database"));
	menuFile->Append(ID_MAIN_About, wxT("About"), wxT("About.."));
	menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    // A nice touch: a history of files visited. Use this menu.
    docManager->FileHistoryUseMenu(menuFile);
    docManager->FileHistoryLoad(*wxConfig::Get());

    wxMenuBar* menubar = new wxMenuBar;
    menubar->Append(menuFile, wxGetStockLabel(wxID_FILE));

    frame->SetMenuBar(menubar);

    frame->SetIcon(Get_MoviesCatalog_Ico());
//    frame->Centre();
    frame->Show();

//	wxString s = stdp.GetAppDocumentsDir();

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned 'false' here, the
    // application would exit immediately.
    return true;
} /*** end of OnInit ***/

int MyApp::OnExit()
{
    wxDocManager* const manager = wxDocManager::GetDocumentManager();
    manager->FileHistorySave(*wxConfig::Get());
    delete manager;

    return wxApp::OnExit();
}

MainCanvas* MyApp::GetMainCanvas()
{
    return m_pCanvas;
}

/*********************************** end of main.cpp ***********************************/
