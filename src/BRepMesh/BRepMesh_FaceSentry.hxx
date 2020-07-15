// Created: 2020-07-27 
// 
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

#ifndef BRepMesh_FaceSentry_HeaderFile
#define BRepMesh_FaceSentry_HeaderFile

#include <TopoDS_Face.hxx>

class Message_ProgressSentry;

class BRepMesh_FaceSentry
{
public:
  BRepMesh_FaceSentry()
    :mySentry(0L), isParallel(Standard_False)
  {
  }

  BRepMesh_FaceSentry (const TopoDS_Face& theFace,
                       Message_ProgressSentry* theSentry = 0L,
                       Standard_Boolean theParallel = Standard_False)
    :myFace(theFace), mySentry(theSentry),
    isParallel(theParallel)
  {
  }

  //! Get progress sentry
  Message_ProgressSentry* GetProgressEntry() const
  {
    return mySentry;
  }

  //! Return reference on TopoDS_Face
  const TopoDS_Face& GetFace() const
  {
    return myFace;
  }

  //! Returns isParallel
  Standard_Boolean IsParallel() const
  {
    return isParallel;
  }


private:
  TopoDS_Face myFace;
  Message_ProgressSentry* mySentry;
  Standard_Boolean isParallel;
};

#endif