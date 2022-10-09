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
#include "VMovies.h"
#include "VMovies_private.h"
#include "MainDocument.h"
#include "lkSQL3Valid.h"
//#include "SimpleBox.h"
#include "DlgDate.h"
#include "Defines.h"
#include "XPMs.h" // resources

#include "TCategory.h"
#include "TGenre.h"
#include "TJudge.h"
#include "TRating.h"

#include "DlgFind.h"
#include "DlgMoviesFilter.h"

#include "../lkSQLite3/lkSQL3Exception.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class MoviesDocument
////
wxIMPLEMENT_ABSTRACT_CLASS2(MoviesDocument, lkSQL3RecordDocument, TMovies)

MoviesDocument::MoviesDocument(wxDocument* pParent) : lkSQL3RecordDocument(pParent), TMovies()
{
	TMovies::SetDatabase(GetDB());

	m_bCatChanged = false;
	m_bGenresChanged = false;
	m_bJudgesChanged = false;
	m_bRatingChanged = false;

	m_bHasMoviesCopy = false;
}

void MoviesDocument::BindButtons()
{
	lkSQL3RecordDocument::BindButtons();

	MoviesFrame* frame = NULL;
	{
		lkSQL3RecordChildFrame* f = dynamic_cast<lkSQL3RecordChildFrame*>(GetFrame());
		if ( f && f->IsKindOf(wxCLASSINFO(MoviesFrame)) )
			frame = dynamic_cast<MoviesFrame*>(f);
	}
	if ( !frame )
		return;

	Bind(wxEVT_UPDATE_UI, &MoviesDocument::OnUpdateFind, this, frame->GetID_Search());
	Bind(wxEVT_UPDATE_UI, &MoviesDocument::OnUpdateFindNext, this, frame->GetID_SearchNext());
	Bind(wxEVT_UPDATE_UI, &MoviesDocument::OnUpdateFindPrev, this, frame->GetID_SearchPrev());

	Bind(wxEVT_UPDATE_UI, &MoviesDocument::OnUpdateFilter, this, frame->GetID_Filter());
	Bind(wxEVT_UPDATE_UI, &MoviesDocument::OnUpdateFilterToggle, this, frame->GetID_FilterToggle());

	Bind(wxEVT_UPDATE_UI, &MoviesDocument::OnUpdateMoviesCopy, this, frame->GetID_MoviesCopy());
	Bind(wxEVT_UPDATE_UI, &MoviesDocument::OnUpdateMoviesPaste, this, frame->GetID_MoviesPaste());

	Bind(wxEVT_TOOL, &MoviesDocument::OnMoviesCopy, this, frame->GetID_MoviesCopy());
	Bind(wxEVT_TOOL, &MoviesDocument::OnMoviesPaste, this, frame->GetID_MoviesPaste());

	Bind(wxEVT_TOOL, &MoviesDocument::OnSearch, this, frame->GetID_Search());
	Bind(wxEVT_TOOL, &MoviesDocument::OnSearchNext, this, frame->GetID_SearchNext());
	Bind(wxEVT_TOOL, &MoviesDocument::OnSearchPrev, this, frame->GetID_SearchPrev());

	Bind(wxEVT_TOOL, &MoviesDocument::OnFilter, this, frame->GetID_Filter());
	Bind(wxEVT_TOOL, &MoviesDocument::OnFilterToggle, this, frame->GetID_FilterToggle());
}

void MoviesDocument::OnUpdateFilter(wxUpdateUIEvent& event)
{
	event.Enable(!m_bFilterActive && (m_pSet) ? (!m_pSet->IsAdding() && !m_pSet->IsEmpty()) : false);
}
void MoviesDocument::OnUpdateFilterToggle(wxUpdateUIEvent& event)
{
	event.Enable((m_pSet) ? (!m_pSet->IsAdding() && !m_pSet->IsEmpty() && (m_bFilterActive || !m_sFltFilter.IsEmpty())) : false);
}

void MoviesDocument::OnUpdateMoviesCopy(wxUpdateUIEvent& event)
{
	event.Enable((m_pSet) ? !m_pSet->IsAdding() : false);
}
void MoviesDocument::OnUpdateMoviesPaste(wxUpdateUIEvent& event)
{
	event.Enable((m_pSet) ? (m_pSet->IsAdding() && m_bHasMoviesCopy) : false);
}


void MoviesDocument::OnMoviesCopy(wxCommandEvent& event)
{
	wxASSERT(!m_pSet->IsAdding());
	if ( m_pSet->IsAdding() )
		return;

	wxView* v = GetFirstView();
	MoviesView* view = (v && v->IsKindOf(wxCLASSINFO(MoviesView))) ? dynamic_cast<MoviesView*>(v) : NULL;
	lkSQL3panel* p = (view) ? view->GetCanvas() : NULL;
	MoviesCanvas* c = (p && p->IsKindOf(wxCLASSINFO(MoviesCanvas))) ? dynamic_cast<MoviesCanvas*>(p) : NULL;

	if ( !c )
		return;

	c->MoviesCopyEnable();
	c->TransferDataFromWindow();
	m_bHasMoviesCopy = true;
	c->MoviesCopyDisable();
}
void MoviesDocument::OnMoviesPaste(wxCommandEvent& event)
{
	wxASSERT(m_pSet->IsAdding());
	if ( !m_pSet->IsAdding() )
		return;

	wxView* v = GetFirstView();
	MoviesView* view = (v && v->IsKindOf(wxCLASSINFO(MoviesView))) ? dynamic_cast<MoviesView*>(v) : NULL;
	lkSQL3panel* p = (view) ? view->GetCanvas() : NULL;
	MoviesCanvas* c = (p && p->IsKindOf(wxCLASSINFO(MoviesCanvas))) ? dynamic_cast<MoviesCanvas*>(p) : NULL;

	if ( !c )
		return;

	m_bHasMoviesCopy = false;
	c->MoviesCopyEnable();
	c->TransferDataToWindow();
	c->MoviesCopyDisable();
}

void MoviesDocument::OnSearch(wxCommandEvent& WXUNUSED(event))
{
	TMovies* pRS = dynamic_cast<TMovies*>(GetBaseSet());
	wxUint64 _row = DlgMoviesFind::FindMovie(GetFrame(), pRS);
	
	if ( _row > 0 )
	{
		pRS->Move(_row);
		lkSQL3RecordDocument::UpdateData(false);
	}
}

void MoviesDocument::OnFilter(wxCommandEvent& WXUNUSED(event))
{
	MoviesFilterDlg filter(GetDB());
	if ( filter.Create(GetFrame()) && (filter.ShowModal() == wxID_OK) )
	{
		SetFltFilter(filter.GetFilter());
		SetFltOrder(filter.GetOrder());
		SetFilterActive(true);

		try
		{
			Requery();
		}
		catch ( const lkSQL3Exception& e )
		{
			((lkSQL3Exception*)&e)->ReportError();
			SetFilterActive(false);
			SetFltFilter(wxEmptyString);
			SetFltOrder(wxEmptyString);
		}

		lkSQL3RecordDocument::UpdateData(false);
	}
}

void MoviesDocument::OnFilterToggle(wxCommandEvent& WXUNUSED(event))
{
	if ( m_bFilterActive || !m_sFltFilter.IsEmpty() )
	{
		SetFilterActive(!m_bFilterActive);

		try
		{
			Requery();
		}
		catch ( const lkSQL3Exception& e )
		{
			((lkSQL3Exception*)&e)->ReportError();
			SetFilterActive(!m_bFilterActive);
		}

		lkSQL3RecordDocument::UpdateData(false);
	}
}


bool MoviesDocument::IsCategoryChanged() const
{
	return m_bCatChanged;
}
void MoviesDocument::SetCategoryChanged(bool c)
{
	m_bCatChanged = c;
}

bool MoviesDocument::IsGenresChanged() const
{
	return m_bGenresChanged;
}
void MoviesDocument::SetGenresChanged(bool g)
{
	m_bGenresChanged = g;
}

bool MoviesDocument::IsJudgesChanged() const
{
	return m_bJudgesChanged;
}
void MoviesDocument::SetJudgesChanged(bool j)
{
	m_bJudgesChanged = j;
}

bool MoviesDocument::IsRatingChanged() const
{
	return m_bRatingChanged;
}
void MoviesDocument::SetRatingChanged(bool r)
{
	m_bRatingChanged = r;
}

//virtual
wxString MoviesDocument::GetDefaultOrder() const
{
	return wxString(wxT("[ROWID]"));
}

//virtual
lkSQL3RecordSet* MoviesDocument::GetBaseSet()
{
	return dynamic_cast<lkSQL3RecordSet*>(this);
}

//virtual
void MoviesDocument::DoThingsBeforeAdd()
{
	wxView* v = GetFirstView();
	MoviesView* view = (v && v->IsKindOf(wxCLASSINFO(MoviesView))) ? dynamic_cast<MoviesView*>(v) : NULL;
	lkSQL3panel* p = (view) ? view->GetCanvas() : NULL;
	MoviesCanvas* c = (p && p->IsKindOf(wxCLASSINFO(MoviesCanvas))) ? dynamic_cast<MoviesCanvas*>(p) : NULL;

	if ( c )
		c->SetMoviesFocus();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
// class MoviesView
////
wxIMPLEMENT_DYNAMIC_CLASS(MoviesView, lkSQL3RecordView)

MoviesView::MoviesView() : lkSQL3RecordView()
{
}

//virtual
bool MoviesView::OnCreate(wxDocument* doc, long flags)
{
	if ( !lkSQL3RecordView::OnCreate(doc, flags) )
		return false;

	wxDocParentFrame* parent = GetParentFrame();
	if ( parent )
	{
		MoviesFrame* frame = new MoviesFrame();

		wxSize size =
#ifdef __WXMSW__
			wxSize(867, 464)
#else
			wxSize(867, 505)
#endif
			;
		(void)frame->Create(doc, this, static_cast<wxDocParentFrame*>(parent), wxT("Movies"), wxDefaultPosition, size);

		size =
#ifdef __WXMSW__
			wxSize(851, 376)
#else
			wxSize(867, 409)
#endif
			;
		frame->SetMinClientSize(size);
		SetFrame(frame);

		// Bind events in our document
		(dynamic_cast<lkSQL3RecordDocument*>(doc))->BindButtons();

		m_pCanvas = new MoviesCanvas(this, frame);
		frame->Show(true);

		InitialUpdate();
		return true;
	}

	return false;
}

//virtual
void MoviesView::InitialUpdate()
{
	wxDocument* doc = GetDocument();
	MoviesDocument* movDoc = (doc && doc->IsKindOf(wxCLASSINFO(MoviesDocument))) ? dynamic_cast<MoviesDocument*>(doc) : NULL;
	if ( movDoc )
		movDoc->MoveLast();

	lkSQL3RecordView::InitialUpdate();
}

bool MoviesView::UpdateData(bool bSaveAndValidate)
{
	// When doing a MoviesCopy, this should be handled by calling the TransferData..-routines directly, not via UpdateData !
	if ( m_pCanvas && m_pCanvas->IsKindOf(wxCLASSINFO(MoviesCanvas)) )
		if ( (dynamic_cast<MoviesCanvas*>(m_pCanvas))->MoviesCopyInProgress() )
			(dynamic_cast<MoviesCanvas*>(m_pCanvas))->MoviesCopyDisable();

	return lkSQL3RecordView::UpdateData(bSaveAndValidate);
}

//virtual
void MoviesView::ProcessRecordset(lkSQL3RecordSet* pSet)
{
	if ( m_bClosing )
		return;

	wxDocument* doc = GetDocument();
	MoviesDocument* movDoc = (doc && doc->IsKindOf(wxCLASSINFO(MoviesDocument))) ? dynamic_cast<MoviesDocument*>(doc) : NULL;

	if ( movDoc && pSet )
	{
		if ( pSet->IsKindOf(wxCLASSINFO(TCategory)) )
			movDoc->SetCategoryChanged(true);
		else if ( pSet->IsKindOf(wxCLASSINFO(TGenre)) )
			movDoc->SetGenresChanged(true);
		else if ( pSet->IsKindOf(wxCLASSINFO(TJudge)) )
			movDoc->SetJudgesChanged(true);
		else if ( pSet->IsKindOf(wxCLASSINFO(TRating)) )
			movDoc->SetRatingChanged(true);
		else
			return;

		if ( UpdateData(true) )
			UpdateData(false);
	}
}


// /////////////////////////////////////////////////////////////////////////////////////////////
// class MoviesTemplate
////

wxIMPLEMENT_CLASS(MoviesTemplate, lkChildDocTemplate)

MoviesTemplate::MoviesTemplate(MainDocManager* manager, wxClassInfo* parentDocClassInfo, wxClassInfo* viewClassInfo) :
	lkChildDocTemplate(manager, parentDocClassInfo, viewClassInfo)
{
}

//virtual
wxDocument* MoviesTemplate::DoCreateDocument()
{
	wxASSERT(m_documentManager != NULL);

	wxDocument* parent = NULL;
	if ( m_documentManager && m_documentManager->IsKindOf(wxCLASSINFO(MainDocManager)) )
		parent = (dynamic_cast<MainDocManager*>(m_documentManager))->GetMainDocument(wxCLASSINFO(MainDocument));
	wxASSERT(parent != NULL);

	return new MoviesDocument(parent);
}

//static
wxDocTemplate* MoviesTemplate::CreateTemplate(MainDocManager* man)
{
	return new MoviesTemplate(man, wxCLASSINFO(MoviesDocument), wxCLASSINFO(MoviesView));
}

