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
#include "CompactEvent.h"

wxDEFINE_EVENT(lkEVT_COMPACT_EVENT, CompactEvent);

CompactEvent::CompactEvent(EventIDs id) : wxThreadEvent(lkEVT_COMPACT_EVENT, int(id))
{
	m_lkU64 = 0;
}
CompactEvent::CompactEvent(const CompactEvent& other) : wxThreadEvent(other)
{
	m_lkU64 = other.m_lkU64;
}

//virtual
wxEvent* CompactEvent::Clone() const
{
	return new CompactEvent(*this);
}

void CompactEvent::SetLongLong(wxUint64 u)
{
	m_lkU64 = u;
}
wxUint64 CompactEvent::GetLongLong() const
{
	return m_lkU64;
}

