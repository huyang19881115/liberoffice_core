/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#include <odbc/ODriver.hxx>
#include <odbc/OConnection.hxx>
#include <odbc/OTools.hxx>
#include <connectivity/dbexception.hxx>
#include <cppuhelper/supportsservice.hxx>
#include <strings.hrc>
#include <resource/sharedresources.hxx>
#include <utility>

using namespace connectivity::odbc;
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::beans;
using namespace com::sun::star::sdbc;

ODBCDriver::ODBCDriver(css::uno::Reference< css::uno::XComponentContext > _xContext)
    :ODriver_BASE(m_aMutex)
    ,m_xContext(std::move(_xContext))
    ,m_pDriverHandle(SQL_NULL_HANDLE)
{
}

void ODBCDriver::disposing()
{
    ::osl::MutexGuard aGuard(m_aMutex);


    for (auto const& connection : m_xConnections)
    {
        Reference< XComponent > xComp(connection.get(), UNO_QUERY);
        if (xComp.is())
            xComp->dispose();
    }
    m_xConnections.clear();

    ODriver_BASE::disposing();
}

// static ServiceInfo

OUString ODBCDriver::getImplementationName(  )
{
    return u"com.sun.star.comp.sdbc.ODBCDriver"_ustr;
        // this name is referenced in the configuration and in the odbc.xml
        // Please take care when changing it.
}


Sequence< OUString > ODBCDriver::getSupportedServiceNames(  )
{
    return { u"com.sun.star.sdbc.Driver"_ustr };
}


sal_Bool SAL_CALL ODBCDriver::supportsService( const OUString& _rServiceName )
{
    return cppu::supportsService(this, _rServiceName);
}


Reference< XConnection > SAL_CALL ODBCDriver::connect( const OUString& url, const Sequence< PropertyValue >& info )
{
    if ( ! acceptsURL(url) )
        return nullptr;

    if(!m_pDriverHandle)
    {
        OUString aPath;
        if(!EnvironmentHandle(aPath))
            throw SQLException(aPath,*this,OUString(),1000,Any());
    }
    rtl::Reference<OConnection> pCon = new OConnection(m_pDriverHandle,this);
    pCon->Construct(url,info);
    m_xConnections.push_back(WeakReferenceHelper(*pCon));

    return pCon;
}

sal_Bool SAL_CALL ODBCDriver::acceptsURL( const OUString& url )
{
    return url.startsWith("sdbc:odbc:");
}

Sequence< DriverPropertyInfo > SAL_CALL ODBCDriver::getPropertyInfo( const OUString& url, const Sequence< PropertyValue >& /*info*/ )
{
    if ( acceptsURL(url) )
    {
        Sequence< OUString > aBooleanValues{ u"false"_ustr, u"true"_ustr };

        return
        {
            {
                u"CharSet"_ustr,
                u"CharSet of the database."_ustr,
                false,
                {},
                {}
            },
            {
                u"UseCatalog"_ustr,
                u"Use catalog for file-based databases."_ustr,
                false,
                u"false"_ustr,
                aBooleanValues
            },
            {
                u"SystemDriverSettings"_ustr,
                u"Driver settings."_ustr,
                false,
                {},
                {}
            },
            {
                u"ParameterNameSubstitution"_ustr,
                u"Change named parameters with '?'."_ustr,
                false,
                u"false"_ustr,
                aBooleanValues
            },
            {
                u"IgnoreDriverPrivileges"_ustr,
                u"Ignore the privileges from the database driver."_ustr,
                false,
                u"false"_ustr,
                aBooleanValues
            },
            {
                u"IsAutoRetrievingEnabled"_ustr,
                u"Retrieve generated values."_ustr,
                false,
                u"false"_ustr,
                aBooleanValues
            },
            {
                u"AutoRetrievingStatement"_ustr,
                u"Auto-increment statement."_ustr,
                false,
                {},
                {}
            },
            {
                u"GenerateASBeforeCorrelationName"_ustr,
                u"Generate AS before table correlation names."_ustr,
                false,
                u"false"_ustr,
                aBooleanValues
            },
            {
                u"EscapeDateTime"_ustr,
                u"Escape date time format."_ustr,
                false,
                u"true"_ustr,
                aBooleanValues
            }
        };
    }
    ::connectivity::SharedResources aResources;
    const OUString sMessage = aResources.getResourceString(STR_URI_SYNTAX_ERROR);
    ::dbtools::throwGenericSQLException(sMessage ,*this);
}

sal_Int32 SAL_CALL ODBCDriver::getMajorVersion(  )
{
    return 1;
}

sal_Int32 SAL_CALL ODBCDriver::getMinorVersion(  )
{
    return 0;
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
