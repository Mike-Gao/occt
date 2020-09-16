// Copyright (c) 2020 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _TOcaf_Application_HeaderFile
#define _TOcaf_Application_HeaderFile

#include <TDocStd_Application.hxx>
#include <TColStd_SequenceOfExtendedString.hxx>

class TOcaf_Application;
DEFINE_STANDARD_HANDLE(TOcaf_Application, TDocStd_Application)

class TOcaf_Application : public TDocStd_Application
{
public:
  DEFINE_STANDARD_RTTI_INLINE(TOcaf_Application, TDocStd_Application)

  Standard_EXPORT TOcaf_Application ();
};

#endif
