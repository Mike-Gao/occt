// Created on: 2002-10-31
// Created by: Michael SAZONOV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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


#include <BinDrivers.hxx>
#include <BinDrivers_DocumentRetrievalDriver.hxx>
#include <BinLDrivers_DocumentSection.hxx>
#include <BinMDataStd.hxx>
#include <BinMDF_ADriverTable.hxx>
#include <BinMNaming.hxx>
#include <BinMNaming_NamedShapeDriver.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_IStream.hxx>
#include <Standard_Type.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TNaming_NamedShape.hxx>


#include <string>

//=======================================================================
//function : BinDrivers_DocumentRetrievalDriver
//purpose  : Constructor
//=======================================================================
BinDrivers_DocumentRetrievalDriver::BinDrivers_DocumentRetrievalDriver ()
{
}

//=======================================================================
//function : AttributeDrivers
//purpose  :
//=======================================================================

Handle(BinMDF_ADriverTable) BinDrivers_DocumentRetrievalDriver::AttributeDrivers
       (const Handle(CDM_MessageDriver)& theMessageDriver)
{
  return BinDrivers::AttributeDrivers (theMessageDriver);
}

//=======================================================================
//function : ReadShapeSection
//purpose  : 
//=======================================================================

void BinDrivers_DocumentRetrievalDriver::ReadShapeSection( BinLDrivers_DocumentSection& theSection,
                                                           const Handle(Storage_IODevice)& theDevice,
                                                           const Standard_Boolean /*isMess*/)

{
  // Read Shapes
  Standard_Character* aBuf = 0;
  Handle(BinMNaming_NamedShapeDriver) aNamedShapeDriver;
  if (myDrivers->GetDriver(STANDARD_TYPE(TNaming_NamedShape), aNamedShapeDriver))
  {
    try {
      OCC_CATCH_SIGNALS

      aBuf = (Standard_Character*)malloc( theSection.Length() + 1 );
      Standard_Size aSize = theDevice->Read( (Standard_Address)aBuf, theSection.Length() );
      if ( aSize == theSection.Length() )
      {
          Standard_SStream aStrStream( std::string( aBuf, aSize ), Standard_SStream::in | Standard_SStream::binary );
          aNamedShapeDriver->ReadShapeSection( aStrStream );
      }
      else
        WriteMessage( "BinDrivers_DocumentRetrievalDriver: can't read all shape section data." );
    }
    catch(Standard_Failure) {
      if ( aBuf )
      {
        free( aBuf );
        aBuf = 0;
      }

      Handle(Standard_Failure) aFailure = Standard_Failure::Caught();
      const TCollection_ExtendedString aMethStr ("BinDrivers_DocumentRetrievalDriver: ");
      WriteMessage (aMethStr + "error of Shape Section " +
      aFailure->GetMessageString());
    }
  }
  if ( aBuf )
    free( aBuf );
}

//=======================================================================
//function : CheckShapeSection
//purpose  : 
//=======================================================================
void BinDrivers_DocumentRetrievalDriver::CheckShapeSection(
                                          const Storage_Position& /*ShapeSectionPos*/, 
                                          const Handle(Storage_IODevice)& /*theDevice*/)
{}

//=======================================================================
//function : PropagateDocumentVersion
//purpose  : 
//=======================================================================
void BinDrivers_DocumentRetrievalDriver::PropagateDocumentVersion(
				    const Standard_Integer theDocVersion )
{
  BinMDataStd::SetDocumentVersion(theDocVersion);
  BinMNaming::SetDocumentVersion(theDocVersion);
}

