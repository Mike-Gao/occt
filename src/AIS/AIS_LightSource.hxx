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

#ifndef _AIS_LightSource_HeaderFile
#define _AIS_LightSource_HeaderFile

#include <AIS_InteractiveObject.hxx>
#include <AIS_InteractiveContext.hxx>
#include <Graphic3d_CLight.hxx>
#include <AIS_TextLabel.hxx>
#include <AIS_Point.hxx>
#include <Geom_CartesianPoint.hxx>
#include <AIS_Line.hxx>
#include <AIS_Circle.hxx>

class AIS_LightSource : public AIS_InteractiveObject
{

public:

  //! Initializes the light source.
  Standard_EXPORT AIS_LightSource (Graphic3d_TypeOfLightSource theType, Handle(AIS_InteractiveContext) theContext);

  //! Initializes the light source by copying Graphic3d_CLight settings.
  Standard_EXPORT AIS_LightSource (const Handle(Graphic3d_CLight) & theLightSource, Handle(AIS_InteractiveContext) theContext);

  //! Returns the Type of the Light, cannot be changed after object construction.
  Graphic3d_TypeOfLightSource GetType() const { return myLightSource->Type(); }

  //! Returns light source name; empty string by default.
  const TCollection_AsciiString& Name() const { return myLightSource->Name(); }

  //! Sets light source name.
  Standard_EXPORT void SetName (const TCollection_AsciiString& theName, Standard_Boolean theIsDisplay = Standard_True);

  //! Returns the color of the light source; WHITE by default.
  const Quantity_Color& Color() const { return myLightSource->Color(); }

  //! Defines the color of a light source by giving the basic color.
  void SetColor (const Quantity_Color& theColor) { myLightSource->SetColor (theColor); }

  //! Check that the light source is turned on; TRUE by default.
  //! This flag affects all occurrences of light sources, where it was registered and activated;
  //! so that it is possible defining an active light in View which is actually in disabled state.
  Standard_Boolean IsEnabled() const { return myLightSource->IsEnabled(); }

  //! Change enabled state of the light state.
  //! This call does not remove or deactivate light source in Views/Viewers;
  //! instead it turns it OFF so that it just have no effect.
  void SetEnabled (Standard_Boolean theIsOn) { myLightSource->SetEnabled (theIsOn); }

  //! Alias for IsHeadlight().
  Standard_Boolean Headlight() const { return myLightSource->Headlight(); }

  //! Setup headlight flag.
  void SetHeadlight (Standard_Boolean theValue) { myLightSource->SetHeadlight (theValue); }

  //! @name positional/spot light properties
public:
  //! Returns location of positional/spot light; (0, 0, 0) by default.
  const gp_Pnt& Position() const { return myLightSource->Position(); }

  //! Returns location of positional/spot light.
  void Position (Standard_Real& theX, Standard_Real& theY, Standard_Real& theZ) const
  {
    myLightSource->Position (theX, theY, theZ);
  }

  //! Setup location of positional/spot light. theIsDisplay - should the position be displayed (Standard_True by default).
  Standard_EXPORT void SetPosition (const gp_Pnt& thePosition, Standard_Boolean theIsDisplay = Standard_True);

  //! Setup location of positional/spot light. theIsDisplay - should the position be displayed (Standard_True by default).
  void SetPosition (Standard_Real theX, Standard_Real theY, Standard_Real theZ, Standard_Boolean theIsDisplay = Standard_True)
  {
    SetPosition (gp_Pnt(theX, theY, theZ), theIsDisplay);
  }

  //! Returns the attenuation factors.
  void Attenuation (Standard_Real& theConstAttenuation,
                    Standard_Real& theLinearAttenuation) const
  {
    myLightSource->Attenuation (theConstAttenuation, theLinearAttenuation);
  }

  //! Defines the coefficients of attenuation; values should be >= 0.0 and their summ should not be equal to 0.
  void SetAttenuation(Standard_ShortReal theConstAttenuation, Standard_ShortReal theLinearAttenuation)
  {
    myLightSource->SetAttenuation (theConstAttenuation, theLinearAttenuation);
  }

  //! @name directional/spot light additional properties
public:
  //! Returns direction of directional/spot light.
  gp_Dir Direction() const { return myLightSource->Direction(); }

  //! Returns the theVx, theVy, theVz direction of the light source.
  void Direction (Standard_Real& theVx, Standard_Real& theVy, Standard_Real& theVz) const { myLightSource->Direction (theVx, theVy, theVz); }

  //! Sets direction of directional/spot light. theIsDisplay - should the position be displayed (Standard_True by default).
  Standard_EXPORT void SetDirection (const gp_Dir& theDir, Standard_Boolean theIsDisplay = Standard_True);

  //! Sets direction of directional/spot light. theIsDisplay - should the position be displayed (Standard_True by default).
  void SetDirection (Standard_Real theVx, Standard_Real theVy, Standard_Real theVz, Standard_Boolean theIsDisplay = Standard_True)
  {
    SetDirection (gp_Dir(theVx, theVy, theVz), theIsDisplay);
  }

  //! @name spotlight additional definition parameters
public:
  //! Returns an angle in radians of the cone created by the spot; 30 degrees by default.
  Standard_ShortReal Angle() const { return myLightSource->Angle(); }

  //! Angle in radians of the cone created by the spot, should be within range (0.0, M_PI). theIsDisplay - should the position be displayed (Standard_True by default).
  Standard_EXPORT void SetAngle(Standard_ShortReal theAngle, Standard_Boolean theIsDisplay = Standard_True);

  //! Returns intensity distribution of the spot light, within [0.0, 1.0] range; 1.0 by default.
  //! This coefficient should be converted into spotlight exponent within [0.0, 128.0] range:
  //! @code
  //!   float aSpotExponent = Concentration() * 128.0;
  //!   anAttenuation *= pow (aCosA, aSpotExponent);"
  //! @endcode
  //! The concentration factor determines the dispersion of the light on the surface, the default value (1.0) corresponds to a minimum of dispersion.
  Standard_ShortReal Concentration() const { return myLightSource->Concentration(); }

  //! Defines the coefficient of concentration; value should be within range [0.0, 1.0].
  void SetConcentration(Standard_ShortReal theConcentration) { myLightSource->SetConcentration(theConcentration); }

  //! @name Ray-Tracing / Path-Tracing light properties
public:
  //! Returns the intensity of light source; 1.0 by default.
  Standard_ShortReal Intensity() const { return myLightSource->Intensity(); }

  //! Modifies the intensity of light source, which should be > 0.0.
  void SetIntensity (Standard_ShortReal theValue) { myLightSource->SetIntensity (theValue); }

  //! Returns the smoothness of light source (either smoothing angle for directional light or smoothing radius in case of positional light); 0.0 by default.
  Standard_ShortReal Smoothness() const { return myLightSource->Smoothness(); }

  //! Modifies the smoothing radius of positional/spot light; should be >= 0.0.
  void SetSmoothRadius(Standard_ShortReal theValue) { myLightSource->SetSmoothRadius(theValue); }

  //! Modifies the smoothing angle (in radians) of directional light source; should be within range [0.0, M_PI/2].
  void SetSmoothAngle(Standard_ShortReal theValue) { myLightSource->SetSmoothAngle(theValue); }

  //! Returns maximum distance on which point light source affects to objects and is considered during illumination calculations.
  //! 0.0 means disabling range considering at all without any distance limits.
  //! Has sense only for point light sources (positional and spot).  
  Standard_ShortReal Range() const { return myLightSource->Range(); }

  //! Modifies maximum distance on which point light source affects to objects and is considered during illumination calculations.
  //! Positional and spot lights are only point light sources.
  //! 0.0 means disabling range considering at all without any distance limits.
  Standard_EXPORT void SetRange (Standard_ShortReal theValue);

  //! @name low-level access methods
public:
  //! Returns the light.
  Handle(Graphic3d_CLight) GetLight() { return myLightSource; }

  //! return light resource identifier string
  const TCollection_AsciiString& GetId() const { return myLightSource->GetId(); }

  Standard_EXPORT void DeleteAllDisplayedAttributes();

private:
  //! Display the position of light source in viewer.
  Standard_EXPORT void DisplayPosition();

  //! Display thedirection of light source in viewer.
  Standard_EXPORT void DisplayDirection();

  //! Display the angle of lightsource in viewer.
  Standard_EXPORT void DisplayAngle();

  //! Display the name of light source in viewer.
  Standard_EXPORT void DisplayName();

  //! Sets local transformation to theTransformation.
  Standard_EXPORT virtual void setLocalTransformation (const Handle(TopLoc_Datum3D)& theTransformation) Standard_OVERRIDE;

  //! Return bounding box.
  Standard_EXPORT virtual void BoundingBox (Bnd_Box& theBndBox) Standard_OVERRIDE;

  void ComputeSelection (const Handle(SelectMgr_Selection)& /*aSelection*/, const Standard_Integer /*aMode*/) Standard_OVERRIDE
  {
  }

  virtual void Compute (const Handle(PrsMgr_PresentationManager3d)& /*aPresentationManager*/, const Handle(Prs3d_Presentation)& /*aPresentation*/, const Standard_Integer /*aMode*/) Standard_OVERRIDE
  {
  }


protected:
  Handle(Graphic3d_CLight) myLightSource;

  Standard_Boolean isNameDisplayed;
  Standard_Boolean isPositionDisplayed;
  Standard_Boolean isDirectionDisplayed;
  Standard_Boolean isConeShapeDisplayed;

  Handle(AIS_Circle) myCircle;
  Handle(AIS_Line) myConeLineA;
  Handle(AIS_Line) myConeLineB;
  Handle(AIS_Line) myConeLineC;
  Handle(AIS_Line) myDirection;
  Handle(AIS_TextLabel) myName;
  Handle(AIS_Point) myPosition;
  
  Graphic3d_Vec4 myDirectionBeforeTransformation;
  gp_Pnt myPositionBeforeTransformation;
};
#endif // _AIS_LightSource_HeaderFile