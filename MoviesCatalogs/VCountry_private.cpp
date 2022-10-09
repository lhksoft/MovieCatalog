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
#include "VCountry_private.h"
#include "TCountry.h"
#include "../lkControls/lkConfigTools.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// class DlgCountryFind
////
wxIMPLEMENT_ABSTRACT_CLASS(DlgCountryFind, DlgFind)

DlgCountryFind::DlgCountryFind(lkSQL3RecordSet* pRS, lkFindList* param) : DlgFind(pRS, param)
{
}

bool DlgCountryFind::Create(wxWindow* parent, long style)
{
	return DlgFind::Create(parent, wxT("Find a Country"), style);
}

//static
wxUint64 DlgCountryFind::FindCountry(wxWindow* parent, lkSQL3RecordSet* pRS)
{
	if ( !parent || !pRS || !pRS->IsOpen() )
		return 0;

	wxUint64 row = 0;

	lkFindList paramList;
	paramList.DeleteContents(true);
	paramList.Append(new lkFindParam(t3Country_DEF, wxT("Country")));
	paramList.Append(new lkFindParam(t3Country_CODE, wxT("Code")));

	DlgCountryFind dlg(pRS, &paramList);
	if ( dlg.Create(parent) && (dlg.ShowModal() == wxID_OK) )
	{
		wxString szFind = dlg.GetFindString();
		row = pRS->FindInit(wxCLASSINFO(TCountry), szFind, dlg.IsForward());
	}

	return row;
}

//virtual
void DlgCountryFind::DoInit()
{
	wxInt64 ll = GetConfigInt(conf_COUNTRY_PATH, conf_FIND_DIRECTION);
	if ( (ll > (wxInt64)lkSQL3RecordSet::FIND_DIRECTION::fndINVALID) && (ll <= (wxInt64)lkSQL3RecordSet::FIND_DIRECTION::fndLAST) )
		m_nSrcOn = (wxUint64)ll;

	m_bForward = GetConfigBool(conf_COUNTRY_PATH, conf_FIND_FORWARD, true);

	if ( m_pParams && !m_pParams->IsEmpty() )
	{
		ll = GetConfigInt(conf_COUNTRY_PATH, conf_FIND_FIELD);
		if ( ll <= 0 )
			ll = 1;

		wxInt64 i = 0;
		for ( lkFindList::iterator iter = m_pParams->begin(); iter != m_pParams->end(); ++iter )
		{
			lkFindParam* current = *iter;
			if ( ++i == ll )
			{
				m_nField = (wxUint64)current;
				break;
			}
		}
	}
}

//virtual
void DlgCountryFind::DoOK()
{
	wxInt64 ll = (wxInt64)m_nSrcOn;
	SetConfigInt(conf_COUNTRY_PATH, conf_FIND_DIRECTION, ll, (wxInt64)lkSQL3RecordSet::FIND_DIRECTION::fndStart);

	SetConfigBool(conf_COUNTRY_PATH, conf_FIND_FORWARD, m_bForward, true);

	if ( m_pParams && !m_pParams->IsEmpty() && (m_nField != 0) )
	{
		ll = 0;
		for ( lkFindList::iterator iter = m_pParams->begin(); iter != m_pParams->end(); ++iter )
		{
			lkFindParam* current = *iter;
			ll++;
			if ( m_nField == (wxUint64)current )
			{
				SetConfigInt(conf_COUNTRY_PATH, conf_FIND_FIELD, ll, 1);
				break;
			}
		}
	}
}

