// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef _XCAFPrs_AISObject_HeaderFile
#define _XCAFPrs_AISObject_HeaderFile

#include <AIS_ColoredShape.hxx>

#include <TDF_Label.hxx>
#include <XCAFPrs_Style.hxx>

//! Implements AIS_InteractiveObject functionality for shape in DECAF document.
class XCAFPrs_AISObject : public AIS_ColoredShape
{
  DEFINE_STANDARD_RTTIEXT(XCAFPrs_AISObject, AIS_ColoredShape)
public:

  //! Creates an object to visualise the shape label.
  Standard_EXPORT XCAFPrs_AISObject (const TDF_Label& theLabel);

  //! Returns the label which was visualised by this presentation
  const TDF_Label& GetLabel() const { return myLabel; }

  //! Assign the label to this presentation
  //! (but does not mark it outdated with SetToUpdate()).
  void SetLabel (const TDF_Label& theLabel)
  {
    myLabel = theLabel;
  }

  //! Fetch the Shape from associated Label and fill the map of sub-shapes styles.
  //! By default, this method is called implicitly within first ::Compute().
  //! Application might call this method explicitly to manipulate styles afterwards.
  //! @param theToSyncStyles flag indicating if method ::Compute() should call this method again
  //!                        on first compute or re-compute
  Standard_EXPORT virtual void DispatchStyles (const Standard_Boolean theToSyncStyles = Standard_False);

  //! Sets the material aspect.
  //! This method assigns the new default material without overriding XDE styles.
  //! Re-computation of existing presentation is not required after calling this method.
  Standard_EXPORT virtual void SetMaterial (const Graphic3d_MaterialAspect& theMaterial) Standard_OVERRIDE;

  //! Fills out a default style object which is used when styles are not explicitly defined in the document.
  //! By default, the style uses white color for curves and surfaces.
  virtual void DefaultStyle (XCAFPrs_Style& theStyle) const { theStyle = myDefaultStyle; }

  //! Sets default style to be used when styles are not explicitly defined in the document.
  virtual void SetDefaultStyle (const XCAFPrs_Style& theStyle) {  myDefaultStyle = theStyle; }

protected:

  //! Redefined method to compute presentation.
  Standard_EXPORT virtual  void Compute (const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                                         const Handle(Prs3d_Presentation)&           thePresentation,
                                         const Standard_Integer                      theMode) Standard_OVERRIDE;

  //! Set colors to drawer
  Standard_EXPORT void SetColors (const Handle(Prs3d_Drawer)& theDrawer,
                                  const Quantity_Color&       theColorCurv,
                                  const Quantity_ColorRGBA&   theColorSurf);

  //! Set colors to drawer
  void SetColors (const Handle(Prs3d_Drawer)& theDrawer,
                  const Quantity_Color& theColorCurv,
                  const Quantity_Color& theColorSurf) { SetColors (theDrawer, theColorCurv, Quantity_ColorRGBA (theColorSurf)); }

protected:

  XCAFPrs_Style    myDefaultStyle; //!< default presentation style
  TDF_Label        myLabel;        //!< label pointing onto the shape
  Standard_Boolean myToSyncStyles; //!< flag indicating that shape and sub-shapes should be updates within Compute()

};

DEFINE_STANDARD_HANDLE(XCAFPrs_AISObject, AIS_ColoredShape)

#endif // _XCAFPrs_AISObject_HeaderFile
