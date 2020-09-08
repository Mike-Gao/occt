// Created on: 2020-09-07
// Created by: Maria KRYLOVA
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

#include <AIS_LightSource.hxx>

#include <Geom_Circle.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>

// =======================================================================
// function : Constructor
// purpose  :
// =======================================================================
AIS_LightSource::AIS_LightSource(Graphic3d_TypeOfLightSource theType, Handle(AIS_InteractiveContext) theContext)
: myLightSource(new Graphic3d_CLight(theType)),
  isNameDisplayed(Standard_False),
  isPositionDisplayed(Standard_False),
  isDirectionDisplayed(Standard_False),
  isConeShapeDisplayed(Standard_False), 
  myCircle(NULL),
  myConeLineA(NULL),
  myConeLineB(NULL),
  myConeLineC(NULL),
  myDirection(NULL),
  myName(NULL),
  myPosition(NULL),
  myDirectionBeforeTransformation(0.0f, 0.0f, 0.0f, 0.0f),
  myPositionBeforeTransformation(myLightSource->Position())
{
  myCTXPtr = &*theContext;
}

// =======================================================================
// function : Constructor
// purpose  :
// =======================================================================
AIS_LightSource::AIS_LightSource(const Handle(Graphic3d_CLight) & theLightSource, Handle(AIS_InteractiveContext) theContext)
: myLightSource(theLightSource)
{
  myCTXPtr = &*theContext;
}

// =======================================================================
// function : SetName
// purpose  :
// =======================================================================
void AIS_LightSource::SetName (const TCollection_AsciiString& theName, Standard_Boolean theIsDisplay)
{
  myLightSource->SetName (theName);
  if (theIsDisplay)
  {
    DisplayName();
  }
  else if (!theIsDisplay && isNameDisplayed)
  {
    myCTXPtr->Remove (myName, Standard_True);
    isNameDisplayed = Standard_False;
  }
}

// =======================================================================
// function : DisplayName
// purpose  :
// =======================================================================
void AIS_LightSource::DisplayName()
{
  if (isNameDisplayed)
  {
    myCTXPtr->Remove (myName, Standard_False);
  }
  Handle(AIS_TextLabel) aLabel = new AIS_TextLabel();
  myName->SetText (myLightSource->Name());
  myName->SetPosition (myLightSource->Position());
  myCTXPtr->Display (myName, Standard_True);
  isNameDisplayed = Standard_True;
}

// =======================================================================
// function : SetPosition
// purpose  :
// =======================================================================
void AIS_LightSource::SetPosition (const gp_Pnt& thePosition, Standard_Boolean theIsDisplay)
{
  myLightSource->SetPosition (thePosition);
  myPositionBeforeTransformation = thePosition;
  if (theIsDisplay)
  {
    DisplayPosition();
  }
  else if (!theIsDisplay && isPositionDisplayed)
  {
    myCTXPtr->Remove (myPosition, Standard_True);
    isPositionDisplayed = Standard_False;
  }
  if (isNameDisplayed)
  {
    DisplayName();
  }
  if (isDirectionDisplayed)
  {
    DisplayDirection();
  }
  if (isConeShapeDisplayed)
  {
    DisplayAngle();
  }
}

// =======================================================================
// function : DisplayPosition
// purpose  :
// =======================================================================
void AIS_LightSource::DisplayPosition()
{
  if (isPositionDisplayed)
  {
    myCTXPtr->Remove (myPosition, Standard_False);
  }
  myCTXPtr->Erase (myPosition, Standard_False);
  Handle(Geom_CartesianPoint) aGeomPoint = new Geom_CartesianPoint(myLightSource->Position());
  myPosition = new AIS_Point(aGeomPoint);
  myCTXPtr->Display (myPosition, Standard_True);
  isPositionDisplayed = Standard_True;
}

// =======================================================================
// function : SetDirection
// purpose  :
// =======================================================================
void AIS_LightSource::SetDirection (const gp_Dir& theDir, Standard_Boolean theIsDisplay)
{
  myLightSource->SetDirection (theDir);
  myDirectionBeforeTransformation = Graphic3d_Vec4(theDir.X(), theDir.Y(), theDir.Z(), 0.0);
  if (theIsDisplay)
  {
    DisplayDirection();
  }
  else if (!theIsDisplay && isDirectionDisplayed)
  {
    myCTXPtr->Remove (myDirection, Standard_True);
    isDirectionDisplayed = Standard_False;
  }
  if (isConeShapeDisplayed)
  {
    SetAngle (myLightSource->Angle(), Standard_True);
  }
}

// =======================================================================
// function : DisplayDirection
// purpose  :
// =======================================================================
void AIS_LightSource::DisplayDirection()
{
  if (isDirectionDisplayed)
  {
    myCTXPtr->Remove (myDirection, Standard_False);
  }
  Standard_Real aDistance = myLightSource->Range();
  if (aDistance == 0)
  {
    aDistance = 100;
  }
  gp_Pnt aPntB(myLightSource->Position().X() + myLightSource->Direction().X() * aDistance,
               myLightSource->Position().Y() + myLightSource->Direction().Y() * aDistance,
               myLightSource->Position().Z() + myLightSource->Direction().Z() * aDistance);

  Handle(Geom_CartesianPoint) aGeomPointA = new Geom_CartesianPoint(myLightSource->Position());
  Handle(Geom_CartesianPoint) aGeomPointB = new Geom_CartesianPoint(aPntB);
  myDirection = new AIS_Line(aGeomPointA, aGeomPointB);
  myCTXPtr->Display (myDirection, Standard_True);
  isDirectionDisplayed = Standard_True;
}

// =======================================================================
// function : SetAngle
// purpose  :
// =======================================================================
void AIS_LightSource::SetAngle (Standard_ShortReal theAngle, Standard_Boolean theIsDisplay)
{
  myLightSource->SetAngle (theAngle);
  if (theIsDisplay)
  {
    DisplayAngle();
  }
  else if (!theIsDisplay && isConeShapeDisplayed)
  {
    myCTXPtr->Remove (myCircle, Standard_False);
    myCTXPtr->Remove (myConeLineA, Standard_False);
    myCTXPtr->Remove (myConeLineB, Standard_False);
    myCTXPtr->Remove (myConeLineC, Standard_True);
    isConeShapeDisplayed = Standard_False;
  }
}

// =======================================================================
// function : DisplayAngle
// purpose  :
// =======================================================================
void AIS_LightSource::DisplayAngle()
{
  if (!myCircle.IsNull())
  {
    myCTXPtr->Remove (myCircle, Standard_False);
    myCTXPtr->Remove (myConeLineA, Standard_False);
    myCTXPtr->Remove (myConeLineB, Standard_False);
    myCTXPtr->Remove (myConeLineC, Standard_False);
  }
  Standard_ShortReal aHalfAngle = myLightSource->Angle() / 2;
  Standard_Real aDistance = myLightSource->Range();
  if (aDistance == 0)
  {
    aDistance = 100;
  }
  Standard_Real aRadius = aDistance * Tan (aHalfAngle);
  gp_Pnt aCenter(myLightSource->Position().X() + myLightSource->Direction().X() * aDistance,
                 myLightSource->Position().Y() + myLightSource->Direction().Y() * aDistance,
                 myLightSource->Position().Z() + myLightSource->Direction().Z() * aDistance);
  gp_Ax2 anAxes(aCenter, myLightSource->Direction());
  Handle(Geom_Circle) aCircle = new Geom_Circle(anAxes, aRadius);
  myCircle = new AIS_Circle(aCircle);
  gp_Dir aYDir = aCircle->YAxis().Direction();
  gp_Pnt aPntCircle1(aCenter.X() + aYDir.X() * aRadius,
                     aCenter.Y() + aYDir.Y() * aRadius,
                     aCenter.Z() + aYDir.Z() * aRadius);
  gp_Pnt aPntCircle2(aCenter.X() - aYDir.X() * aRadius,
                     aCenter.Y() - aYDir.Y() * aRadius,
                     aCenter.Z() - aYDir.Z() * aRadius);
  Handle(Geom_CartesianPoint) aGeomPointA = new Geom_CartesianPoint(myLightSource->Position());
  Handle(Geom_CartesianPoint) aGeomPointB = new Geom_CartesianPoint(aPntCircle1);
  Handle(Geom_CartesianPoint) aGeomPointC = new Geom_CartesianPoint(aPntCircle2);
  myConeLineA = new AIS_Line(aGeomPointA, aGeomPointB);
  myConeLineB = new AIS_Line(aGeomPointA, aGeomPointC);
  myConeLineC = new AIS_Line(aGeomPointB, aGeomPointC);

  myCTXPtr->Display (myCircle, Standard_True);
  myCTXPtr->Display (myConeLineA, Standard_True);
  myCTXPtr->Display (myConeLineB, Standard_True);
  myCTXPtr->Display (myConeLineC, Standard_True);
  isConeShapeDisplayed = Standard_True;
}

// =======================================================================
// function : SetRange
// purpose  :
// =======================================================================
void AIS_LightSource::SetRange(Standard_ShortReal theValue)
{
  myLightSource->SetRange (theValue);
  if (isDirectionDisplayed)
  {
    DisplayDirection();
  }
  if (isConeShapeDisplayed)
  {
    DisplayAngle();
  }
}

// =======================================================================
// function : DeleteAllDisplayedAttributes
// purpose  :
// =======================================================================
void AIS_LightSource::DeleteAllDisplayedAttributes()
{
  if (!myName.IsNull()) SetName (myLightSource->Name(), Standard_False);
  if (!myPosition.IsNull()) SetPosition (myLightSource->Position(), Standard_False);
  if (!myDirection.IsNull()) SetDirection (myLightSource->Direction(), Standard_False);
  if (!myCircle.IsNull()) SetAngle (myLightSource->Angle(), Standard_False);
}

// =======================================================================
// function : setLocalTransformation
// purpose  :
// =======================================================================
void AIS_LightSource::setLocalTransformation(const Handle(TopLoc_Datum3D)& theTransformation)
{
  myLocalTransformation = theTransformation;
  if (myLightSource->Type() == Graphic3d_TOLS_SPOT && isConeShapeDisplayed)
  {
    myConeLineA->SetLocalTransformation (theTransformation);
    myConeLineB->SetLocalTransformation (theTransformation);
    myConeLineC->SetLocalTransformation (theTransformation);
    myCircle->SetLocalTransformation (theTransformation);
  }
  // Transformation Matrix.
  NCollection_Mat4<Standard_Real> aMatrix;
  theTransformation->Transformation().GetMat4 (aMatrix);

  if ((myLightSource->Type() == Graphic3d_TOLS_POSITIONAL || myLightSource->Type() == Graphic3d_TOLS_SPOT))
  {
    // Calculating a new position.
    Standard_Real aX = myPositionBeforeTransformation.X();
    Standard_Real aY = myPositionBeforeTransformation.Y();
    Standard_Real aZ = myPositionBeforeTransformation.Z();
    gp_Pnt newPosition(aMatrix.GetValue(0, 0) * aX + aMatrix.GetValue(0, 1) * aY + aMatrix.GetValue(0, 2) * aZ + aMatrix.GetValue(0, 3),
                       aMatrix.GetValue(1, 0) * aX + aMatrix.GetValue(1, 1) * aY + aMatrix.GetValue(1, 2) * aZ + aMatrix.GetValue(1, 3),
                       aMatrix.GetValue(2, 0) * aX + aMatrix.GetValue(2, 1) * aY + aMatrix.GetValue(2, 2) * aZ + aMatrix.GetValue(2, 3));
    myLightSource->SetPosition (newPosition);
    if (isPositionDisplayed)
    {
      DisplayPosition();
    }
  }
  if ((myLightSource->Type() == Graphic3d_TOLS_SPOT || myLightSource->Type() == Graphic3d_TOLS_DIRECTIONAL))
  {
    // Calculating a new direction.
    Standard_Real aXDir = myDirectionBeforeTransformation.x();
    Standard_Real aYDir = myDirectionBeforeTransformation.y();
    Standard_Real aZDir = myDirectionBeforeTransformation.z();
    gp_Dir newDirection(aMatrix.GetValue (0, 0) * aXDir + aMatrix.GetValue (0, 1) * aYDir + aMatrix.GetValue (0, 2) * aZDir,
                        aMatrix.GetValue (1, 0) * aXDir + aMatrix.GetValue (1, 1) * aYDir + aMatrix.GetValue (1, 2) * aZDir,
                        aMatrix.GetValue (2, 0) * aXDir + aMatrix.GetValue (2, 1) * aYDir + aMatrix.GetValue (2, 2) * aZDir);
    myLightSource->SetDirection (newDirection);
    if (isDirectionDisplayed)
    {
      myDirection->SetLocalTransformation (theTransformation);
    }
  }
  if (isNameDisplayed)
  {
    DisplayName();
  }
}

void AIS_LightSource::BoundingBox (Bnd_Box& theBndBox)
{
  NCollection_Vec3<double> aMinPoint(myLightSource->Position().X(), myLightSource->Position().Y(), myLightSource->Position().Z());
  SelectMgr_Vec3 mde (aMinPoint);
  const Graphic3d_BndBox3d aBndBox = Graphic3d_BndBox3d(mde, mde);
  if (aBndBox.IsValid())
  {
    theBndBox.Update (aBndBox.CornerMin().x(), aBndBox.CornerMin().y(), aBndBox.CornerMin().z(),
                      aBndBox.CornerMax().x(), aBndBox.CornerMax().y(), aBndBox.CornerMax().z());
  }
  else
  {
    theBndBox.SetVoid();
  }
  return;
}
