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
#include "MainDocManager.h"
#include "../lkControls/lkDocument.h"
#include "MainDocument.h"

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

#include <wx/app.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/filehistory.h>
//#include <wx/confbase.h>
#include <wx/config.h>
#include <wx/filename.h>

#include "Defines.h"

class lkFileHistory : public wxFileHistory
{
public:
	lkFileHistory();

	virtual void 	Load(const wxConfigBase& config) wxOVERRIDE;
	virtual void 	Save(wxConfigBase& config) wxOVERRIDE;

private:
	wxDECLARE_DYNAMIC_CLASS(lkFileHistory);
};

wxIMPLEMENT_DYNAMIC_CLASS(lkFileHistory, wxFileHistory)

lkFileHistory::lkFileHistory() : wxFileHistory()
{
}

void lkFileHistory::Load(const wxConfigBase& config)
{
	wxString oldPath = config.GetPath();
	wxConfigBase* c = (wxConfigBase*)&config;
	c->SetPath(wxT("/MRU"));

	wxFileHistory::Load(config);

	c->SetPath(oldPath);
}

void lkFileHistory::Save(wxConfigBase& config)
{
	wxString oldPath = config.GetPath();
	config.SetPath(wxT("/MRU"));

	wxFileHistory::Save(config);

	config.SetPath(oldPath);
}

// -------------------------------------------------------------------------------------------------- //

wxBEGIN_EVENT_TABLE(MainDocManager, wxDocManager)
	EVT_UPDATE_UI(ID_MAIN_Compact, MainDocManager::OnUpdateCompact)
	EVT_MENU(ID_MENU_Views_Base, MainDocManager::OnViewsBase)
	EVT_MENU(ID_MENU_Views_Category, MainDocManager::OnViewsCategory)
	EVT_MENU(ID_MENU_Views_Country, MainDocManager::OnViewsCountry)
	EVT_MENU(ID_MENU_Views_Genres, MainDocManager::OnViewsGenres)
	EVT_MENU(ID_MENU_Views_Judge, MainDocManager::OnViewsJudgement)
	EVT_MENU(ID_MENU_Views_Location, MainDocManager::OnViewsLocation)
	EVT_MENU(ID_MENU_Views_Media, MainDocManager::OnViewsMedia)
	EVT_MENU(ID_MENU_Views_Movies, MainDocManager::OnViewsMovies)
	EVT_MENU(ID_MENU_Views_Origine, MainDocManager::OnViewsOrigine)
	EVT_MENU(ID_MENU_Views_Quality, MainDocManager::OnViewsQuality)
	EVT_MENU(ID_MENU_Views_Rating, MainDocManager::OnViewsRating)
	EVT_MENU(ID_MENU_Views_Screen, MainDocManager::OnViewsScreen)
	EVT_MENU(ID_MENU_Views_Storage, MainDocManager::OnViewsStorage)
wxEND_EVENT_TABLE()

wxIMPLEMENT_DYNAMIC_CLASS(MainDocManager, wxDocManager)


MainDocManager::MainDocManager() : wxDocManager(0, false)//, m_sCompactPath()
{
	m_fileHistory = OnCreateFileHistory();
}
MainDocManager::~MainDocManager()
{
}

wxFileHistory* MainDocManager::OnCreateFileHistory()
{
	return new lkFileHistory();
}


//virtual
wxDocument* MainDocManager::CreateDocument(const wxString& pathOrig, long flags)
{
	if ( m_templates.IsEmpty() )
	{
		return NULL;
	}

	// normally user should select the template to use but wxDOC_SILENT flag we
	// choose one ourselves
	wxString path = pathOrig;   // may be modified below
	wxDocTemplate* temp;

	if ( flags & wxDOC_SILENT )
	{ // called when opening from MRU
		wxASSERT_MSG(!path.empty(), wxT("using empty path with wxDOC_SILENT doesn't make sense"));

		temp = FindTemplateForPath(path);
		if ( !temp )
		{
			wxLogWarning(_("The format of file '%s' couldn't be determined."), path);
		}
	}
	else // not silent, ask the user
	{
		// For my goal, a new file should be created when creating a new file.
		temp = DoPromptFile(path, flags);
	}

	if ( !temp )
		return NULL;

	// check whether the document with this path is already opened
	if ( !path.empty() )
	{
		wxDocument* const doc = FindDocumentByPath(path);
		if ( doc )
		{
			// file already open, just activate it and return
			doc->Activate();
			return doc;
		}
	}

	// no, we need to create a new document


	// if we've reached the max number of docs, close the first one.
	if ( (int)GetDocuments().GetCount() >= m_maxDocsOpen )
	{
		if ( !CloseDocument((wxDocument*)GetDocuments().GetFirst()->GetData()) )
		{
			// can't open the new document if closing the old one failed
			return NULL;
		}
	}


	// do create and initialize the new document finally
	wxDocument* const docNew = temp->CreateDocument(path, flags);
	if ( !docNew )
		return NULL;

	docNew->SetDocumentName(temp->GetDocumentName());

	wxTRY
	{
		// call the appropriate function depending on whether we're creating a
		// new file or opening an existing one
		if ( !(flags & wxDOC_NEW ? (docNew->IsKindOf(wxCLASSINFO(lkDocument)) ? ((lkDocument*)docNew)->OnNewDocument(path) : docNew->OnNewDocument())
								 : docNew->OnOpenDocument(path)) )
		{
			docNew->DeleteAllViews();
			return NULL;
		}
	}
	wxCATCH_ALL(docNew->DeleteAllViews(); throw; )

		// add the successfully opened file to MRU, but only if we're going to be
		// able to reopen it successfully later which requires the template for
		// this document to be retrievable from the file extension
		if ( temp->FileMatchesTemplate(path) )
			AddFileToHistory(path);

	// at least under Mac (where views are top level windows) it seems to be
	// necessary to manually activate the new document to bring it to the
	// forefront -- and it shouldn't hurt doing this under the other platforms
	docNew->Activate();
//	docNew->SetDocumentTemplate(temp);

	return docNew;
}

wxDocument* MainDocManager::CreateOrActivateChildDocument(const wxClassInfo* cI)
{
	if ( !cI )
		return NULL;

	wxDocTemplate* temp = NULL;
	for ( wxList::compatibility_iterator node = m_templates.GetFirst(); node; node = node->GetNext() )
	{
		wxDocTemplate* t = wxStaticCast(node->GetData(), wxDocTemplate);
		if ( t && t->IsKindOf(cI) )
		{
			temp = t;
			break;
		}
	}
	if ( !temp )
		return NULL;

	wxDocument* pDoc = FindDocumentByClassinfo(temp->GetDocClassInfo());
	if ( !pDoc )
		if ( (pDoc = temp->CreateDocument(wxEmptyString, wxDEFAULT_TEMPLATE_FLAGS)) != NULL )
			pDoc->SetDocumentName(temp->GetDocumentName());

	if ( !pDoc )
		return NULL;

	pDoc->Activate();

	return pDoc;
}

wxDocTemplate* MainDocManager::DoPromptFile(wxString& sPath, long flags)
{
	sPath.Clear();
	if ( m_templates.IsEmpty() )
		return NULL;

	wxList uniqueTemplates;
	uniqueTemplates.DeleteContents(false);
	bool bFound;
	// opt out identical templates
	for ( wxList::compatibility_iterator node = m_templates.GetFirst(); node; node = node->GetNext() )
	{
		wxDocTemplate* t = wxStaticCast(node->GetData(), wxDocTemplate);
		if ( t->GetFlags() & wxDOC_SILENT )
			continue;

		if ( t->GetDescription().IsEmpty() || t->GetFileFilter().IsEmpty() )
			continue; // most likely a child-template

		bFound = false;
		if ( !uniqueTemplates.IsEmpty() )
			for ( wxList::compatibility_iterator node = uniqueTemplates.GetFirst(); node; node = node->GetNext() )
			{
				wxDocTemplate* tt = wxStaticCast(node->GetData(), wxDocTemplate);
				if ( t == tt )
				{
					bFound = true;
					break;
				}
			}

		if ( !bFound )
			uniqueTemplates.Append(t);
	}

	wxString descrBuf;
	wxString defExt;
	for ( wxList::compatibility_iterator node = uniqueTemplates.GetFirst(); node; node = node->GetNext() )
	{
		wxDocTemplate* tt = wxStaticCast(node->GetData(), wxDocTemplate);
		// add a '|' to separate this filter from the previous one
		if ( !descrBuf.empty() )
			descrBuf << wxT('|');

		descrBuf << tt->GetDescription()
			<< wxT(" (") << tt->GetFileFilter() << wxT(") |")
			<< tt->GetFileFilter();
		if ( defExt.IsEmpty() )
			defExt = tt->GetDefaultExtension();
	}

	int FilterIndex = -1;

	wxString ttl = (flags & wxDOC_NEW) ? wxT("Create New File") : wxT("Open File");
	wxString pathTmp = wxFileSelectorEx(ttl, GetLastDirectory(), wxEmptyString, &FilterIndex, descrBuf, 
										(flags & wxDOC_NEW) ? (wxFD_SAVE | wxFD_OVERWRITE_PROMPT) : (wxFD_OPEN | wxFD_FILE_MUST_EXIST));

	if ( !pathTmp.IsEmpty() )
	{
		wxFileName fn(pathTmp);
		if ( fn.GetExt().IsEmpty() )
			fn.SetExt(defExt); // defExt might be empty too, but not in our app

		pathTmp = fn.GetFullPath();
	}

	wxDocTemplate* theTemplate = NULL;
	if ( !pathTmp.IsEmpty() )
	{
		if ( !(flags & wxDOC_NEW) )
		{
			if ( !wxFileExists(pathTmp) )
			{
				wxString msgTitle;
				if ( !wxTheApp->GetAppDisplayName().empty() )
					msgTitle = wxTheApp->GetAppDisplayName();
				else
					msgTitle = wxT("File error");

				wxMessageBox(wxT("Sorry, could not open this file."), msgTitle, wxOK | wxICON_EXCLAMATION | wxCENTRE);

				sPath.clear();
				return NULL;
			}
		}

		SetLastDirectory(wxPathOnly(pathTmp));

		// first choose the template using the extension, if this fails (i.e. wxFileSelectorEx() didn't fill it), then use the path
		if ( FilterIndex != -1 )
		{
			theTemplate = dynamic_cast<wxDocTemplate*>(uniqueTemplates[FilterIndex]);
			if ( theTemplate )
			{
				// But don't use this template if it doesn't match the path as
				// can happen if the user specified the extension explicitly
				// but didn't bother changing the filter.
				if ( !theTemplate->FileMatchesTemplate(pathTmp) )
					theTemplate = NULL;
			}
		}

		if ( !theTemplate )
			theTemplate = FindTemplateForPath(pathTmp);
		if ( !theTemplate )
		{
			// Since we do not add files with non-default extensions to the
			// file history this can only happen if the application changes the
			// allowed templates in runtime.
			wxString ttl = (flags & wxDOC_NEW) ? wxT("New File") : wxT("Open File");
			wxMessageBox(wxT("Sorry, the format for this file is unknown."), ttl, wxOK | wxICON_EXCLAMATION | wxCENTRE);
		}
		else
			sPath = pathTmp;
	}

	return theTemplate;
}

wxDocument* MainDocManager::FindDocumentByClassinfo(const wxClassInfo* pClassInfo)
{
	if ( (pClassInfo == NULL) || m_docs.IsEmpty() )
		return NULL;

	for ( wxList::compatibility_iterator node = m_docs.GetFirst(); node; node = node->GetNext() )
	{
		wxDocument* pDoc = wxStaticCast(node->GetData(), wxDocument);
		if ( pDoc->IsKindOf(pClassInfo) )
			return pDoc;
	}

	return NULL;
}

wxDocTemplate* MainDocManager::FindTemplateByViewClass(const wxClassInfo* viewClassInfo)
{
	if ( (viewClassInfo == NULL) || m_templates.IsEmpty() )
		return NULL;

	wxString sV = viewClassInfo->GetClassName();

	for ( wxList::compatibility_iterator node = m_templates.GetFirst(); node; node = node->GetNext() )
	{
		wxDocTemplate* pTemp = wxStaticCast(node->GetData(), wxDocTemplate);
		if ( (pTemp->GetViewClassInfo() != NULL) &&
			(wxStrcmp(pTemp->GetViewClassInfo()->GetClassName(), sV) == 0) )
			return pTemp;
	}

	return NULL;
}

wxDocument* MainDocManager::GetMainDocument(const wxClassInfo* maindocClassInfo)
{
	if ( GetDocuments().IsEmpty() )
		return NULL;

	for ( wxList::compatibility_iterator node = m_docs.GetFirst(); node; node = node->GetNext() )
	{
		wxDocument* pDoc = wxStaticCast(node->GetData(), wxDocument);
		if ( !pDoc->IsChildDocument() )
			if ( (maindocClassInfo == NULL) || (pDoc->IsKindOf(maindocClassInfo)) )
				return pDoc;
	}

	return NULL;
}

wxView* MainDocManager::CreateView(wxDocument* doc, long flags)
{
	wxASSERT(doc != NULL);
	if ( !doc )
		return NULL;

	wxDocTemplate* const temp = doc->GetDocumentTemplate();
	if ( !temp )
		return NULL;

	wxView* view = NULL;
	if ( doc->IsKindOf(wxCLASSINFO(lkDocument)) )
		view = ((lkDocument*)doc)->FindViewByClass(temp->GetViewClassInfo());

	if ( view )
		return view;

	// Nope, then create a new View
	view = temp->CreateView(doc, flags);
	if ( view )
		view->SetViewName(temp->GetViewName());
	return view;
}

// closes the specified document
// virtual
bool MainDocManager::lkCloseDocument(wxDocument* doc, bool force)
{
	// be sure the doc still exists in our docs-list, don't bother to continue if it is not
	if ( !m_docs.Member(doc) )
		return true;

	if ( !doc->Close() && !force )
		return false;

	// To really force the document to close, we must ensure that it isn't
	// modified, otherwise it would ask the user about whether it should be
	// destroyed (again, it had been already done by Close() above) and might
	// not destroy it at all, while we must do it here.
	doc->Modify(false);

	// Implicitly deletes the document when
	// the last view is deleted
	doc->DeleteAllViews();

	wxASSERT(!m_docs.Member(doc));

	return true;
}

// virtual
bool MainDocManager::lkCloseDocuments(bool force)
{
	while ( !m_docs.IsEmpty() )
	{
		wxList::compatibility_iterator node = m_docs.GetFirst();
		wxDocument* doc = (wxDocument*)node->GetData();

		if ( !lkCloseDocument(doc, force) )
			return false;

		// This assumes that documents are not connected in
		// any way, i.e. deleting one document does NOT
		// delete another.
		// --->> Well, it does! Child-Documents are automatic deleted by its parent-doc !!!
	}
	return true;
}

// virtual
bool MainDocManager::lkClear(bool force)
{
	// then close all other docs
	if ( !lkCloseDocuments(force) )
		return false;

	m_currentView = NULL;

	wxList::compatibility_iterator node = m_templates.GetFirst();
	while ( node )
	{
		wxDocTemplate* templ = (wxDocTemplate*)node->GetData();
		wxList::compatibility_iterator next = node->GetNext();
		delete templ;
		node = next;
	}
	return true;
}


void MainDocManager::OnViewsBase(wxCommandEvent& WXUNUSED(event))
{
	(void)CreateOrActivateChildDocument(wxCLASSINFO(BaseTemplate));
}
void MainDocManager::OnViewsCategory(wxCommandEvent& WXUNUSED(event))
{
	(void)CreateOrActivateChildDocument(wxCLASSINFO(CategoryTemplate));
}
void MainDocManager::OnViewsCountry(wxCommandEvent& WXUNUSED(event))
{
	(void)CreateOrActivateChildDocument(wxCLASSINFO(CountryTemplate));
}
void MainDocManager::OnViewsGenres(wxCommandEvent& WXUNUSED(event))
{
	(void)CreateOrActivateChildDocument(wxCLASSINFO(GenreTemplate));
}
void MainDocManager::OnViewsJudgement(wxCommandEvent& WXUNUSED(event))
{
	(void)CreateOrActivateChildDocument(wxCLASSINFO(JudgeTemplate));
}
void MainDocManager::OnViewsLocation(wxCommandEvent& WXUNUSED(event))
{
	(void)CreateOrActivateChildDocument(wxCLASSINFO(LocationTemplate));
}
void MainDocManager::OnViewsMedia(wxCommandEvent& WXUNUSED(event))
{
	(void)CreateOrActivateChildDocument(wxCLASSINFO(MediumTemplate));
}
void MainDocManager::OnViewsMovies(wxCommandEvent& WXUNUSED(event))
{
	(void)CreateOrActivateChildDocument(wxCLASSINFO(MoviesTemplate));
}
void MainDocManager::OnViewsOrigine(wxCommandEvent& WXUNUSED(event))
{
	(void)CreateOrActivateChildDocument(wxCLASSINFO(OrigineTemplate));
}
void MainDocManager::OnViewsQuality(wxCommandEvent& WXUNUSED(event))
{
	(void)CreateOrActivateChildDocument(wxCLASSINFO(QualityTemplate));
}
void MainDocManager::OnViewsRating(wxCommandEvent& WXUNUSED(event))
{
	(void)CreateOrActivateChildDocument(wxCLASSINFO(RatingTemplate));
}
void MainDocManager::OnViewsScreen(wxCommandEvent& WXUNUSED(event))
{
	(void)CreateOrActivateChildDocument(wxCLASSINFO(ScreenTemplate));
}
void MainDocManager::OnViewsStorage(wxCommandEvent& WXUNUSED(event))
{
	(void)CreateOrActivateChildDocument(wxCLASSINFO(StorageTemplate));
}

void MainDocManager::OnUpdateCompact(wxUpdateUIEvent& event)
{
	wxDocument* doc = GetCurrentDocument();
	bool bEnable = (doc && doc->IsKindOf(wxCLASSINFO(lkDocument)) && !(dynamic_cast<lkDocument*>(doc))->HasChildren());
	event.Enable(bEnable);
}

// /////////////////////////////////////////////////////////////////////////////////////////////
// class lkChildDocTemplate
// /////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_ABSTRACT_CLASS(lkChildDocTemplate, wxDocTemplate)

lkChildDocTemplate::lkChildDocTemplate(wxDocManager* manager, const wxString& descr, const wxString& filter, const wxString& dir, const wxString& ext,
									   const wxString& docTypeName, const wxString& viewTypeName,
									   wxClassInfo* docClassInfo, wxClassInfo* viewClassInfo, long flags) :
	wxDocTemplate(manager, descr, filter, dir, ext, docTypeName, viewTypeName, docClassInfo, viewClassInfo, flags)
{
}

lkChildDocTemplate::lkChildDocTemplate(wxDocManager* manager, wxClassInfo* parentDocClassInfo, wxClassInfo* viewClassInfo) :
	wxDocTemplate(manager, wxEmptyString, wxEmptyString, wxEmptyString, wxEmptyString, wxEmptyString, wxEmptyString, parentDocClassInfo, viewClassInfo, wxDEFAULT_TEMPLATE_FLAGS)
{
}

lkChildDocTemplate::~lkChildDocTemplate()
{
}

