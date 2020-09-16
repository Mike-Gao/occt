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

#include "TOcaf_Application.h"
#include <TDF_Label.hxx>
#include <TPrsStd_AISPresentation.hxx>
#include <TPrsStd_AISViewer.hxx>
#include <TNaming_NamedShape.hxx>
#include <TFunction_DriverTable.hxx>

#include "TOcafFunction_BoxDriver.h"
#include "TOcafFunction_CylDriver.h"
#include "TOcafFunction_CutDriver.h"


//=======================================================================
//function : TOcaf_Application
//purpose  : 
//=======================================================================

TOcaf_Application::TOcaf_Application()
{
  // Instanciate a TOcafFunction_BoxDriver and add it to the TFunction_DriverTable
  TFunction_DriverTable::Get()->AddDriver (TOcafFunction_BoxDriver::GetID(),
                                           new TOcafFunction_BoxDriver());

  // Instanciate a TOcafFunction_Cyl  Driver and add it to the TFunction_DriverTable
  TFunction_DriverTable::Get()->AddDriver (TOcafFunction_CylDriver::GetID(),
                                           new TOcafFunction_CylDriver());

  // Instanciate a TOcafFunction_CutDriver and add it to the TFunction_DriverTable
  Handle(TOcafFunction_CutDriver) myCutDriver = new TOcafFunction_CutDriver();
  TFunction_DriverTable::Get()->AddDriver (TOcafFunction_CutDriver::GetID(),
                                           new TOcafFunction_CutDriver());
}
