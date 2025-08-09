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
#include "lkCtrlData.h"

lkCtrlData::lkCtrlData() : wxStringClientData(), wxClientDataContainer()
{
    m_bChecked = false;
}
lkCtrlData::lkCtrlData(const wxString& data) : wxStringClientData(data), wxClientDataContainer()
{
    m_bChecked = false;
}
lkCtrlData::lkCtrlData(const lkCtrlData& item) : wxStringClientData(), wxClientDataContainer()
{
    // string
    SetData(item.GetData());

    // obj / void*
    switch ( item.m_clientDataType )
    {
        case wxClientDataType::wxClientData_Object :
            SetClientObject(item.GetClientObject());
            break;
        case wxClientDataType::wxClientData_Void :
            SetClientData(item.GetClientData());
            break;
        default:
            break;
    }

    m_bChecked = item.IsChecked();
}

lkCtrlData::~lkCtrlData()
{
    // we only delete object data, not untyped
    if ( m_clientDataType == wxClientDataType::wxClientData_Object )
        if ( m_clientObject )
        {
            delete m_clientObject;
            m_clientObject = NULL;
        }
    // must change data-type to prevent base-class trying to delete the object again
    m_clientDataType = wxClientDataType::wxClientData_None;
}

wxClientData* lkCtrlData::DetachClientObject()
{
    wxClientData* pO = NULL;
    if ( m_clientDataType == wxClientDataType::wxClientData_Object )
    {
        pO = m_clientObject;
        m_clientObject = NULL;
    }

    return pO;
}

bool lkCtrlData::IsChecked() const
{
    return m_bChecked;
}
void lkCtrlData::SetCheck(bool c)
{
    m_bChecked = c;
}

//virtual
void lkCtrlData::DoSetClientObject(wxClientData* data)
{
    if ( m_clientDataType == wxClientDataType::wxClientData_Object )
        if ( m_clientObject )
            delete m_clientObject;

    m_clientObject = data; // can be NULL
    m_clientDataType = wxClientDataType::wxClientData_Object;
}

//virtual
wxClientData* lkCtrlData::DoGetClientObject() const
{
    // it's not an error to call GetClientObject() on a window which doesn't have client data at all - NULL will be returned
    if ( m_clientDataType == wxClientDataType::wxClientData_Void )
        return NULL;

    return m_clientObject;
}

//virtual
void lkCtrlData::DoSetClientData(void* data)
{
    if ( m_clientDataType == wxClientDataType::wxClientData_Object )
        if ( m_clientObject )
            delete m_clientObject;

    m_clientData = data; // can be NULL
    m_clientDataType = wxClientDataType::wxClientData_Void;
}

//virtual
void* lkCtrlData::DoGetClientData() const
{
    // it's not an error to call GetClientData() on a window which doesn't have client data at all - NULL will be returned
    if ( m_clientDataType == wxClientDataType::wxClientData_Object )
        return NULL;

    return m_clientData;
}

