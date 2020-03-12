// Created on : Sat May 02 12:41:14 2020 
// Created by: Irina KRYLOVA
// Generator:	Express (EXPRESS -> CASCADE/XSTEP Translator) V3.0
// Copyright (c) Open CASCADE 2020
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

#include <StepKinematics_SuParameters.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_SuParameters, StepGeom_GeometricRepresentationItem)

//=======================================================================
//function : StepKinematics_SuParameters
//purpose  : 
//=======================================================================

StepKinematics_SuParameters::StepKinematics_SuParameters ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepKinematics_SuParameters::Init (const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                                        const Standard_Real theA,
                                        const Standard_Real theAlpha,
                                        const Standard_Real theB,
                                        const Standard_Real theBeta,
                                        const Standard_Real theC,
                                        const Standard_Real theGamma)
{
  StepGeom_GeometricRepresentationItem::Init(theRepresentationItem_Name);

  myA = theA;

  myAlpha = theAlpha;

  myB = theB;

  myBeta = theBeta;

  myC = theC;

  myGamma = theGamma;
}

//=======================================================================
//function : A
//purpose  : 
//=======================================================================

Standard_Real StepKinematics_SuParameters::A () const
{
  return myA;
}

//=======================================================================
//function : SetA
//purpose  : 
//=======================================================================

void StepKinematics_SuParameters::SetA (const Standard_Real theA)
{
  myA = theA;
}

//=======================================================================
//function : Alpha
//purpose  : 
//=======================================================================

Standard_Real StepKinematics_SuParameters::Alpha () const
{
  return myAlpha;
}

//=======================================================================
//function : SetAlpha
//purpose  : 
//=======================================================================

void StepKinematics_SuParameters::SetAlpha (const Standard_Real theAlpha)
{
  myAlpha = theAlpha;
}

//=======================================================================
//function : B
//purpose  : 
//=======================================================================

Standard_Real StepKinematics_SuParameters::B () const
{
  return myB;
}

//=======================================================================
//function : SetB
//purpose  : 
//=======================================================================

void StepKinematics_SuParameters::SetB (const Standard_Real theB)
{
  myB = theB;
}

//=======================================================================
//function : Beta
//purpose  : 
//=======================================================================

Standard_Real StepKinematics_SuParameters::Beta () const
{
  return myBeta;
}

//=======================================================================
//function : SetBeta
//purpose  : 
//=======================================================================

void StepKinematics_SuParameters::SetBeta (const Standard_Real theBeta)
{
  myBeta = theBeta;
}

//=======================================================================
//function : C
//purpose  : 
//=======================================================================

Standard_Real StepKinematics_SuParameters::C () const
{
  return myC;
}

//=======================================================================
//function : SetC
//purpose  : 
//=======================================================================

void StepKinematics_SuParameters::SetC (const Standard_Real theC)
{
  myC = theC;
}

//=======================================================================
//function : Gamma
//purpose  : 
//=======================================================================

Standard_Real StepKinematics_SuParameters::Gamma () const
{
  return myGamma;
}

//=======================================================================
//function : SetGamma
//purpose  : 
//=======================================================================

void StepKinematics_SuParameters::SetGamma (const Standard_Real theGamma)
{
  myGamma = theGamma;
}
