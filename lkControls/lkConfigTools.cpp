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
#include "lkConfigTools.h"
#include <wx/confbase.h>

bool DoFlush()
{
	wxConfigBase* conf = wxConfigBase::Get();
	if ( conf )
		return conf->Flush();

	return false;
}

bool DeleteKey(const wxString& section, const wxString& key)
{
	wxASSERT(!key.IsEmpty());
	if ( key.IsEmpty() )
		return false;

	bool bRet = false;
	wxConfigBase* conf = wxConfigBase::Get();
	if ( conf )
	{
		wxString oldP;
		if ( !section.IsEmpty() )
		{
			oldP = conf->GetPath();
			conf->SetPath(section);
		}

		bRet = conf->DeleteEntry(key, true);
		conf->SetPath(oldP);

		conf->Flush();
	}
	return bRet;
}

wxString GetConfigString(const wxString& section, const wxString& key)
{
	wxASSERT(!key.IsEmpty());
	wxString P;
	if ( key.IsEmpty() )
		return P;

	wxConfigBase* conf = wxConfigBase::Get();
	if ( conf )
	{
		wxString oldP;
		if ( !section.IsEmpty() )
		{
			oldP = conf->GetPath();
			conf->SetPath(section);
		}

		P = conf->Read(key);
		conf->SetPath(oldP);
	}

	return P;
}

void SetConfigString(const wxString& section, const wxString& key, const wxString& P)
{
	wxASSERT(!key.IsEmpty());
	if ( key.IsEmpty() )
		return;

	wxConfigBase* conf = wxConfigBase::Get();
	if ( conf )
	{
		wxString oldP;
		if ( !section.IsEmpty() )
		{
			oldP = conf->GetPath();
			conf->SetPath(section);
		}

		conf->Write(key, P);
		conf->SetPath(oldP);

		conf->Flush();
	}
}

wxInt64 GetConfigInt(const wxString& section, const wxString& key)
{
	wxASSERT(!key.IsEmpty());
	wxLongLong_t I = 0;
	if ( key.IsEmpty() )
		return static_cast<wxInt64>(I);

	wxConfigBase* conf = wxConfigBase::Get();
	if ( conf )
	{
		wxString oldP;
		if ( !section.IsEmpty() )
		{
			oldP = conf->GetPath();
			conf->SetPath(section);
		}

		if ( !conf->Read(key, &I) )
			I = 0;
		conf->SetPath(oldP);
	}

	return static_cast<wxInt64>(I);
}
void SetConfigInt(const wxString& section, const wxString& key, wxInt64 I, wxInt64 defVal)
{
	wxASSERT(!key.IsEmpty());
	if ( key.IsEmpty() )
		return;

	wxConfigBase* conf = wxConfigBase::Get();
	if ( conf )
	{
		wxString oldP;
		if ( !section.IsEmpty() )
		{
			oldP = conf->GetPath();
			conf->SetPath(section);
		}

		if ( I != defVal )
		{
			wxLongLong_t L = static_cast<wxLongLong_t>(I);
			conf->Write(key, L);
		}
		else
			conf->DeleteEntry(key);

		conf->SetPath(oldP);

		conf->Flush();
	}
}

bool GetConfigBool(const wxString& section, const wxString& key, bool defVal)
{
	wxASSERT(!key.IsEmpty());
	bool bRet = defVal;
	if ( key.IsEmpty() )
		return bRet;

	wxConfigBase* conf = wxConfigBase::Get();
	if ( conf )
	{
		wxString oldP;
		if ( !section.IsEmpty() )
		{
			oldP = conf->GetPath();
			conf->SetPath(section);
		}

		bRet = conf->ReadBool(key, defVal);
		conf->SetPath(oldP);
	}

	return bRet;
}
void SetConfigBool(const wxString& section, const wxString& key, bool val, bool defVal)
{
	wxASSERT(!key.IsEmpty());
	if ( key.IsEmpty() )
		return;

	wxConfigBase* conf = wxConfigBase::Get();
	if ( conf )
	{
		wxString oldP;
		if ( !section.IsEmpty() )
		{
			oldP = conf->GetPath();
			conf->SetPath(section);
		}

		if ( val != defVal )
			conf->Write(key, val);
		else
			conf->DeleteEntry(key);

		conf->SetPath(oldP);
		conf->Flush();
	}
}

