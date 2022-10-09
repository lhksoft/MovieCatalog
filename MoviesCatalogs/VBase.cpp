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
#include "VBase.h"
#include "VBase_private.h"
#include "MainDocument.h"

#include "DMovies.h"
#include "DGroup.h"
#include "DlgBaseFilter.h"

#include "TCountry.h"
#include "TQuality.h"
#include "TOrigine.h"
#include "TScreen.h"
#include "TMedium.h"

#include "../lkSQLite3/lkSQL3Exception.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class MoviesDocument
////
wxIMPLEMENT_ABSTRACT_CLASS2(BaseDocument, lkSQL3RecordDocument, TBase)

BaseDocument::BaseDocument(wxDocument* pParent) : lkSQL3RecordDocument(pParent), TBase()
{
	TBase::SetDatabase(GetDB());

	m_bCountryChanged = false;
	m_bQualityChanged = false;
	m_bOrigineChanged = false;
	m_bScreenChanged = false;
	m_bMediumChanged = false;
}

bool BaseDocument::IsCountryChanged() const
{
	return m_bCountryChanged;
}
void BaseDocument::SetCountryChanged(bool c)
{
	m_bCountryChanged = c;
}

bool BaseDocument::IsQualityChanged() const
{
	return m_bQualityChanged;
}
void BaseDocument::SetQualityChanged(bool q)
{
	m_bQualityChanged = q;
}

bool BaseDocument::IsOrigineChanged() const
{
	return m_bOrigineChanged;
}
void BaseDocument::SetOrigineChanged(bool o)
{
	m_bOrigineChanged = o;
}

bool BaseDocument::IsScreenChanged() const
{
	return m_bScreenChanged;
}
void BaseDocument::SetScreenChanged(bool s)
{
	m_bScreenChanged = s;
}

bool BaseDocument::IsMediumChanged() const
{
	return m_bMediumChanged;
}
void BaseDocument::SetMediumChanged(bool m)
{
	m_bMediumChanged = m;
}

void BaseDocument::BindButtons()
{
	lkSQL3RecordDocument::BindButtons();

	BaseFrame* frame = NULL;
	{
		lkSQL3RecordChildFrame* f = dynamic_cast<lkSQL3RecordChildFrame*>(GetFrame());
		if ( f && f->IsKindOf(wxCLASSINFO(BaseFrame)) )
			frame = dynamic_cast<BaseFrame*>(f);
	}
	if ( !frame )
		return;

	Bind(wxEVT_UPDATE_UI, &BaseDocument::OnUpdateFind, this, frame->GetID_Search());
	Bind(wxEVT_UPDATE_UI, &BaseDocument::OnUpdateFindNext, this, frame->GetID_SearchNext());
	Bind(wxEVT_UPDATE_UI, &BaseDocument::OnUpdateFindPrev, this, frame->GetID_SearchPrev());

	Bind(wxEVT_UPDATE_UI, &BaseDocument::OnUpdateFilter, this, frame->GetID_Filter());
	Bind(wxEVT_UPDATE_UI, &BaseDocument::OnUpdateFilterToggle, this, frame->GetID_FilterToggle());

	Bind(wxEVT_UPDATE_UI, &BaseDocument::OnUpdateGrouping, this, frame->GetID_Group());

	Bind(wxEVT_TOOL, &BaseDocument::OnSearch, this, frame->GetID_Search());
	Bind(wxEVT_TOOL, &BaseDocument::OnFilter, this, frame->GetID_Filter());
	Bind(wxEVT_TOOL, &BaseDocument::OnFilterToggle, this, frame->GetID_FilterToggle());
	Bind(wxEVT_TOOL, &BaseDocument::OnSearchNext, this, frame->GetID_SearchNext());
	Bind(wxEVT_TOOL, &BaseDocument::OnSearchPrev, this, frame->GetID_SearchPrev());

	Bind(wxEVT_TOOL, &BaseDocument::OnSelectMovie, this, frame->GetID_Movies());
	Bind(wxEVT_TOOL, &BaseDocument::OnGrouping, this, frame->GetID_Group());
}

//virtual
wxString BaseDocument::GetDefaultOrder() const
{
	return wxString(wxT("[ROWID]"));
}

//virtual
lkSQL3RecordSet* BaseDocument::GetBaseSet()
{
	return dynamic_cast<lkSQL3RecordSet*>(this);
}

//virtual
void BaseDocument::DoThingsBeforeAdd()
{
	wxASSERT(IsAdding());
	wxView* v = GetFirstView();
	BaseView* view = (v && v->IsKindOf(wxCLASSINFO(BaseView))) ? dynamic_cast<BaseView*>(v) : NULL;
	lkSQL3panel* p = (view) ? view->GetCanvas() : NULL;
	BaseCanvas* c = (p && p->IsKindOf(wxCLASSINFO(BaseCanvas))) ? dynamic_cast<BaseCanvas*>(p) : NULL;

	// Setting Default values
	wxInt64 n = TOrigine::GetDefaultOrigine(GetDB());
	if ( (n > 0) && m_pFldOrigine )
			m_pFldOrigine->SetValue2(n);

	n = TScreen::GetDefaultScreen(GetDB());
	if ( (n > 0) && m_pFldScreen )
		m_pFldScreen->SetValue2(n);

	n = TQuality::GetDefaultQuality(GetDB());
	if ( (n > 0) && m_pFldQuality )
		m_pFldQuality->SetValue2(n);

	if ( c )
		c->SetBaseFocus();
}

void BaseDocument::SetFiltering(const wxString& from, const wxString& filter, const wxString& order)
{
	TBase* pRS = (m_pSet) ? dynamic_cast<TBase*>(m_pSet) : NULL;
	if ( !pRS || !pRS->IsOpen() )
		return;

	SetFltFrom(from);
	SetFltFilter(filter);
	SetFltOrder(order);
	SetFilterActive(true);

	try
	{
		Requery();
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		SetFilterActive(false);
		return;
	}

	lkSQL3RecordDocument::UpdateData(false);
}

void BaseDocument::SetFiltering(bool b)
{
	bool bDoRequery = false, bPrev = m_bFilterActive;
	if ( !b )
	{
		if ( m_bFilterActive )
		{
			m_bFilterActive = false;
			bDoRequery = true;
		}
	}
	else
	{
		if ( !m_bFilterActive && !m_sFltFrom.IsEmpty() && !m_sFltFilter.IsEmpty() )
		{
			m_bFilterActive = true;
			bDoRequery = true;
		}
	}

	if ( bDoRequery )
	{
		bool bSuccess = false;
		try
		{
			m_pSet->Requery();
			bSuccess = true;
		}
		catch ( const lkSQL3Exception& e )
		{
			((lkSQL3Exception*)&e)->ReportError();
		}

		if ( !bSuccess )
			m_bFilterActive = bPrev;
		
		lkSQL3RecordDocument::UpdateData(false);
	}
}

void BaseDocument::OnUpdateFilter(wxUpdateUIEvent& event)
{
	event.Enable(!m_bFilterActive && (m_pSet) ? (!m_pSet->IsAdding() && !m_pSet->IsEmpty()) : false);
}
void BaseDocument::OnUpdateFilterToggle(wxUpdateUIEvent& event)
{
	event.Enable((m_pSet) ? (!m_pSet->IsAdding() && !m_pSet->IsEmpty() && (m_bFilterActive || (!m_sFltFrom.IsEmpty() && !m_sFltFilter.IsEmpty()))) : false);
}

void BaseDocument::OnSearch(wxCommandEvent& WXUNUSED(event))
{
	TBase* pRS = dynamic_cast<TBase*>(GetBaseSet());

/*
SELECT [tblBase].[ROWID] AS ID, [tblStorage].[Storage] AS Stor, [tblMovies].[Title] AS Ttl, [tblMovies].[SubTitle] AS Sub, [tblMovies].[Alias]
FROM [tblBase] LEFT JOIN [tblGroup] ON [tblBase].[ROWID] = [tblGROUP].[BaseID]
LEFT JOIN [tblStorage] ON [tblGroup].[StorageID] = [tblStorage].[ROWID]
LEFT JOIN [tblMovies] ON [tblBase].[MoviesID] = [tblMovies].[ROWID]
*/

	wxUint64 _row = DlgBaseFind::FindBase(GetFrame(), pRS);

	if ( _row > 0 )
	{
		pRS->Move(_row);
		lkSQL3RecordDocument::UpdateData(false);
	}
}

void BaseDocument::OnFilter(wxCommandEvent& WXUNUSED(event))
{
	BaseFilterDlg filter(GetDB());
	if ( filter.Create(GetFrame()) && (filter.ShowModal() == wxID_OK) )
	{
		SetFiltering(filter.GetQueryFrom(), filter.GetFilter(), filter.GetOrder());
	}
}

void BaseDocument::OnFilterToggle(wxCommandEvent& WXUNUSED(event))
{
	if ( m_bFilterActive || (!m_sFltFrom.IsEmpty() && !m_sFltFilter.IsEmpty()) )
		SetFiltering(!m_bFilterActive);
}

void BaseDocument::OnSelectMovie(wxCommandEvent& WXUNUSED(event))
{
	wxView* v = GetFirstView();
	BaseView* view = (v && v->IsKindOf(wxCLASSINFO(BaseView))) ? dynamic_cast<BaseView*>(v) : NULL;
	lkSQL3panel* p = (view) ? view->GetCanvas() : NULL;
	BaseCanvas* c = (p && p->IsKindOf(wxCLASSINFO(BaseCanvas))) ? dynamic_cast<BaseCanvas*>(p) : NULL;
	wxASSERT(c); // if NULL something is terrible wrong !
	if ( !c ) return;

	DMovies dlg(m_DB);
	if ( dlg.Create(GetFrame()) )
	{
		wxUint64 n = GetMoviesID();
		dlg.SetCurRecord(n);
		if ( dlg.ShowModal() == wxID_OK )
		{
			wxUint64 m = dlg.GetCurRecord();
			if ( m != n )
			{
				SetMoviesID(m);
				c->UpdateMoviesCtrl();
			}
		}
		if ( dlg.AnythingChanged() )
		{
			lkSQL3RecordSet* pSet = dlg.GetRecordset();
			if ( pSet ) // inform others things got changed
				UpdateAllViews(v, pSet);
		}
	}
}

void BaseDocument::OnGrouping(wxCommandEvent& event)
{
	if ( !lkSQL3RecordDocument::UpdateData(true) )
		return;

	wxView* v = GetFirstView();
	BaseView* view = (v && v->IsKindOf(wxCLASSINFO(BaseView))) ? dynamic_cast<BaseView*>(v) : NULL;
	lkSQL3panel* p = (view) ? view->GetCanvas() : NULL;
	BaseCanvas* c = (p && p->IsKindOf(wxCLASSINFO(BaseCanvas))) ? dynamic_cast<BaseCanvas*>(p) : NULL;
	wxASSERT(c); // if NULL something is terrible wrong !
	if ( !c ) return;

	DGroup dlg(m_DB, GetActualCurRow());
	if ( dlg.Create(GetFrame()) )
	{
		dlg.ShowModal();
		c->UpdateGroupCtrl();
		lkSQL3RecordDocument::UpdateData(false);
	}
}

void BaseDocument::OnUpdateGrouping(wxUpdateUIEvent& event)
{
	event.Enable(m_pSet && !m_pSet->IsEmpty() && !m_pSet->IsAdding());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class BaseView
////
wxIMPLEMENT_DYNAMIC_CLASS(BaseView, lkSQL3RecordView)

BaseView::BaseView() : lkSQL3RecordView()
{
}

//virtual
bool BaseView::OnCreate(wxDocument* doc, long flags)
{
	if ( !lkSQL3RecordView::OnCreate(doc, flags) )
		return false;

	wxDocParentFrame* parent = GetParentFrame();
	if ( parent )
	{
		BaseFrame* frame = new BaseFrame();

		wxSize size =
#ifdef __WXMSW__
			wxSize(745, 535)
#else
			wxSize(745, 545)
#endif
			;
		(void)frame->Create(doc, this, static_cast<wxDocParentFrame*>(parent), wxT("Base"), wxDefaultPosition, size);

		size =
#ifdef __WXMSW__
			wxSize(729, 447)
#else
			wxSize(745, 449)
#endif
			;
		frame->SetMinClientSize(size);
		SetFrame(frame);

		// Bind events in our document
		(dynamic_cast<lkSQL3RecordDocument*>(doc))->BindButtons();

		m_pCanvas = new BaseCanvas(this, frame);
		frame->Show(true);

		InitialUpdate();
		return true;
	}

	return false;
}

//virtual
void BaseView::InitialUpdate()
{
	wxDocument* doc = GetDocument();
	BaseDocument* basDoc = (doc && doc->IsKindOf(wxCLASSINFO(BaseDocument))) ? dynamic_cast<BaseDocument*>(doc) : NULL;
	if ( basDoc )
		basDoc->MoveLast();

	lkSQL3RecordView::InitialUpdate();
}

//virtual
void BaseView::ProcessRecordset(lkSQL3RecordSet* pSet)
{
	if ( m_bClosing )
		return;

	wxDocument* doc = GetDocument();
	BaseDocument* basDoc = (doc && doc->IsKindOf(wxCLASSINFO(BaseDocument))) ? dynamic_cast<BaseDocument*>(doc) : NULL;

	if ( basDoc && pSet )
	{
		if ( pSet->IsKindOf(wxCLASSINFO(TCountry)) )
			basDoc->SetCountryChanged(true);
		else if ( pSet->IsKindOf(wxCLASSINFO(TQuality)) )
			basDoc->SetQualityChanged(true);
		else if ( pSet->IsKindOf(wxCLASSINFO(TOrigine)) )
			basDoc->SetOrigineChanged(true);
		else if ( pSet->IsKindOf(wxCLASSINFO(TScreen)) )
			basDoc->SetScreenChanged(true);
		else if ( pSet->IsKindOf(wxCLASSINFO(TMedium)) )
			basDoc->SetMediumChanged(true);
		else
			return;

		if ( UpdateData(true) )
			UpdateData(false);
	}
}


// /////////////////////////////////////////////////////////////////////////////////////////////
// class MoviesTemplate
////
wxIMPLEMENT_CLASS(BaseTemplate, lkChildDocTemplate)

BaseTemplate::BaseTemplate(MainDocManager* manager, wxClassInfo* parentDocClassInfo, wxClassInfo* viewClassInfo) :
	lkChildDocTemplate(manager, parentDocClassInfo, viewClassInfo)
{
}

//virtual
wxDocument* BaseTemplate::DoCreateDocument()
{
	wxASSERT(m_documentManager != NULL);

	wxDocument* parent = NULL;
	if ( m_documentManager && m_documentManager->IsKindOf(wxCLASSINFO(MainDocManager)) )
		parent = (dynamic_cast<MainDocManager*>(m_documentManager))->GetMainDocument(wxCLASSINFO(MainDocument));
	wxASSERT(parent != NULL);

	return new BaseDocument(parent);
}

//static
wxDocTemplate* BaseTemplate::CreateTemplate(MainDocManager* man)
{
	return new BaseTemplate(man, wxCLASSINFO(BaseDocument), wxCLASSINFO(BaseView));
}


