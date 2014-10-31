
/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * TOra - An Oracle Toolkit for DBA's and developers
 *
 * Shared/mixed copyright is held throughout files in this product
 *
 * Portions Copyright (C) 2000-2001 Underscore AB
 * Portions Copyright (C) 2003-2005 Quest Software, Inc.
 * Portions Copyright (C) 2004-2013 Numerous Other Contributors
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation;  only version 2 of
 * the License is valid for this program.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program as the file COPYING.txt; if not, please see
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt.
 *
 *      As a special exception, you have permission to link this program
 *      with the Oracle Client libraries and distribute executables, as long
 *      as you follow the requirements of the GNU GPL in regard to all of the
 *      software in the executable aside from Oracle client libraries.
 *
 * All trademarks belong to their respective owners.
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include <windows.h>

#define OCI_ERROR -1
typedef signed int sword;

#define TO_EXPORT __declspec(dllexport)

extern "C"
{

    BOOL APIENTRY DllMain(Qt::HANDLE hModule,
                          DWORD ul_reason_for_call,
                          LPVOID lpReserved)
    {
        switch (ul_reason_for_call)
        {
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
        }
        return TRUE;
    }

    TO_EXPORT sword OCIEnvInit()
    {
        return OCI_ERROR;
    }

    TO_EXPORT sword OCIHandleAlloc()
    {
        return OCI_ERROR;
    }

    TO_EXPORT sword OCIServerAttach()
    {
        return OCI_ERROR;
    }

    TO_EXPORT sword OCIAttrGet()
    {
        return OCI_ERROR;
    }

    TO_EXPORT sword OCIAttrSet()
    {
        return OCI_ERROR;
    }

    TO_EXPORT sword OCISessionBegin()
    {
        return OCI_ERROR;
    }

    TO_EXPORT sword OCISessionEnd()
    {
        return OCI_ERROR;
    }

    TO_EXPORT sword OCIBreak()
    {
        return OCI_ERROR;
    }

    TO_EXPORT sword OCIBindByName()
    {
        return OCI_ERROR;
    }

    TO_EXPORT sword OCIInitialize()
    {
        return OCI_ERROR;
    }

    TO_EXPORT sword OCIHandleFree()
    {
        return OCI_ERROR;
    }

    TO_EXPORT sword OCILobGetLength()
    {
        return OCI_ERROR;
    }

    TO_EXPORT sword OCILobLocatorIsInit()
    {
        return OCI_ERROR;
    }

    TO_EXPORT sword OCILobRead()
    {
        return OCI_ERROR;
    }

    TO_EXPORT sword OCIStmtPrepare()
    {
        return OCI_ERROR;
    }

    TO_EXPORT sword OCIStmtExecute()
    {
        return OCI_ERROR;
    }

    TO_EXPORT sword OCITransCommit()
    {
        return OCI_ERROR;
    }


    TO_EXPORT sword OCITransRollback()
    {
        return OCI_ERROR;
    }

    TO_EXPORT sword OCIDescriptorAlloc()
    {
        return OCI_ERROR;
    }

    TO_EXPORT sword OCIDescriptorFree()
    {
        return OCI_ERROR;
    }

    TO_EXPORT sword OCIParamGet()
    {
        return OCI_ERROR;
    }

    TO_EXPORT sword OCIServerDetach()
    {
        return OCI_ERROR;
    }

    TO_EXPORT sword OCILobWrite()
    {
        return OCI_ERROR;
    }

    TO_EXPORT sword OCIStmtFetch()
    {
        return OCI_ERROR;
    }

    TO_EXPORT sword OCIDefineByPos()
    {
        return OCI_ERROR;
    }

    TO_EXPORT sword OCIErrorGet()
    {
        return OCI_ERROR;
    }

}
