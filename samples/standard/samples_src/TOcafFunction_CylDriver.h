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

#ifndef _TOcafFunction_CylDriver_HeaderFile
#define _TOcafFunction_CylDriver_HeaderFile

#include <TFunction_Driver.hxx>
#include <TFunction_Logbook.hxx>

class TOcafFunction_CylDriver;
DEFINE_STANDARD_HANDLE(TOcafFunction_CylDriver, TFunction_Driver)


class TOcafFunction_CylDriver : public TFunction_Driver
{
public:
  DEFINE_STANDARD_RTTI_INLINE(TOcafFunction_CylDriver, TFunction_Driver)

  Standard_EXPORT static const Standard_GUID& GetID ();
  Standard_EXPORT TOcafFunction_CylDriver ();
  Standard_EXPORT virtual  void Validate (Handle (TFunction_Logbook)& log) const override;
  Standard_EXPORT virtual  Standard_Boolean MustExecute (const Handle (TFunction_Logbook)& log) const override;
  Standard_EXPORT virtual  Standard_Integer Execute (Handle (TFunction_Logbook)& log) const override;
};

#endif
