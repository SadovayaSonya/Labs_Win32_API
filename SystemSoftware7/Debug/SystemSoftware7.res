        ��  ��                  �
  ,   V I R T U A L   ��e     0         //Microsoft Developer Studio generated resource script.
//
#include "resource.h"

#define APSTUDIO_READONLY_SYMBOLS
//////////////////////////////////////////////////////////////
// Generated from the TEXTINCLUDE 2 resource.
//
#include "afxres.h"

//////////////////////////////////////////////////////////////
#undef APSTUDIO_READONLY_SYMBOLS

//////////////////////////////////////////////////////////////
// English (U.S.) resources

#if !defined(AFX_RESOURCE_DLL) || defined(AFX_TARG_ENU)
#ifdef _WIN32
LANGUAGE LANG_ENGLISH, SUBLANG_ENGLISH_US
#pragma code_page(1252)
#endif //_WIN32

//////////////////////////////////////////////////////////////
// Menu
//
IDR_APPMENU MENU DISCARDABLE 
BEGIN
    POPUP "&File"
    BEGIN
        MENUITEM "E&xit", ID_FILE_EXIT
    END
    POPUP "&Set protection"
    BEGIN
      MENUITEM "PAGE_NOACCESS", ID_SETPROTECTION_PAGENOACCESS
      MENUITEM "PAGE_READONLY", ID_SETPROTECTION_PAGEREADONLY
      MENUITEM "PAGE_READWRITE",ID_SETPROTECTION_PAGEREADWRITE
      MENUITEM "PAGE_GUARD && PAGE_READWRITE", 
            ID_SETPROTECTION_PAGEGUARD
    END
    POPUP "&Memory"
    BEGIN
        MENUITEM "&Read",               ID_MEMORY_READ
        MENUITEM "&Write",              ID_MEMORY_WRITE
        MENUITEM "&Lock",               ID_MEMORY_LOCK
        MENUITEM "&Unlock",                     ID_MEMORY_UNLOCK
    END
    POPUP "&Help"
    BEGIN
        MENUITEM "&About...",           ID_HELP_ABOUT
    END
END

#ifdef APSTUDIO_INVOKED
//////////////////////////////////////////////////////////////
// TEXTINCLUDE
//
1 TEXTINCLUDE DISCARDABLE 
BEGIN
    "resource.h\0"
END

2 TEXTINCLUDE DISCARDABLE 
BEGIN
    "#include ""afxres.h""\r\n"
    "\0"
END

3 TEXTINCLUDE DISCARDABLE 
BEGIN
    "\r\n"
    "\0"
END

#endif    // APSTUDIO_INVOKED

//////////////////////////////////////////////////////////////
// Icon

// Icon with lowest ID value placed first to ensure application icon
// remains consistent on all systems.
IDI_APPICON             ICON    DISCARDABLE     "virtual.ico"
IDI_APPICONSM           ICON    DISCARDABLE     "virtsm.ico"

//////////////////////////////////////////////////////////////
// String Table
//
STRINGTABLE DISCARDABLE 
BEGIN
    ID_FILE_EXIT            "Quits the application"
END

#endif    // English (U.S.) resources
//////////////////////////////////////////////////////////////

#ifndef APSTUDIO_INVOKED
//////////////////////////////////////////////////////////////
// Generated from the TEXTINCLUDE 3 resource.
//////////////////////////////////////////////////////////////
#endif    // not APSTUDIO_INVOKED   