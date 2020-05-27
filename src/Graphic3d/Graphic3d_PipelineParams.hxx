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

#ifndef _Graphic3d_PipelineParams_HeaderFile
#define _Graphic3d_PipelineParams_HeaderFile

#include <Graphic3d_TypeOfPrimitiveArray.hxx>

//! Face culling modes.
enum Graphic3d_FaceCulling
{
  Graphic3d_FaceCulling_Back = 0, //!< cull back  faces
  Graphic3d_FaceCulling_Front,    //!< cull front faces
  Graphic3d_FaceCulling_Both      //!< cull front and back faces
};

//! Depth/stencil test function.
enum Graphic3d_CompareFunc
{
  Graphic3d_CompareFunc_Always = 0,  //!< always passes
  Graphic3d_CompareFunc_Never,       //!< always fails
  Graphic3d_CompareFunc_Less,        //!< stencil: passes if (Ref & Mask) <  (Stencil & Mask)
  Graphic3d_CompareFunc_Equal,       //!< stencil: passes if (Ref & Mask) =  (Stencil & Mask)
  Graphic3d_CompareFunc_LessEqual,   //!< stencil: passes if (Ref & Mask) <= (Stencil & Mask)
  Graphic3d_CompareFunc_Greater,     //!< stencil: passes if (Ref & Mask) >  (Stencil & Mask)
  Graphic3d_CompareFunc_NotEqual,    //!< stencil: passes if (Ref & Mask) != (Stencil & Mask)
  Graphic3d_CompareFunc_GreaterEqual //!< stencil: passes if (Ref & Mask) >= (Stencil & Mask)
};

//! Supported stencil test operations.
enum Graphic3d_StencilOp
{
  Graphic3d_StencilOp_Keep = 0,  //!< keeps the current value
  Graphic3d_StencilOp_Zero,      //!< sets the stencil buffer value to 0
  Graphic3d_StencilOp_Replace,   //!< sets the stencil buffer value to Ref
  Graphic3d_StencilOp_IncrClamp, //!< increments the current stencil buffer value; clamps to the maximum representable unsigned value
  Graphic3d_StencilOp_DecrClamp, //!< decrements the current stencil buffer value; clamps to 0
  Graphic3d_StencilOp_Invert,    //!< bitwise inverts the current stencil buffer value
  Graphic3d_StencilOp_IncrWrap,  //!< increments the current stencil buffer value; wraps stencil buffer value to zero when incrementing the maximum representable unsigned value
  Graphic3d_StencilOp_DecrWrap   //!< decrements the current stencil buffer value; wraps stencil buffer value to the maximum representable unsigned value when decrementing a stencil buffer value of zero
};

//! Stencil test parameters.
class Graphic3d_StencilState
{
public:
  Graphic3d_StencilOp   FailOp;      //!< stencil operation on stencil test fail
  Graphic3d_StencilOp   DepthFailOp; //!< stencil operation on depth   test fail
  Graphic3d_StencilOp   PassOp;      //!< stencil operation on stencil test success
  Graphic3d_CompareFunc CmpFunc;     //!< stencil test function

  //! Set all operations to specified one.
  void SetOperations (Graphic3d_StencilOp theOp)
  {
    FailOp = DepthFailOp = PassOp = theOp;
  }

  //! Default constructor.
  Graphic3d_StencilState()
  : FailOp     (Graphic3d_StencilOp_Keep),
    DepthFailOp(Graphic3d_StencilOp_Keep),
    PassOp     (Graphic3d_StencilOp_Keep),
    CmpFunc    (Graphic3d_CompareFunc_Always) {}

  //! Equality check.
  bool operator== (const Graphic3d_StencilState& theOther) const
  {
    return FailOp  == theOther.FailOp
        && DepthFailOp == theOther.DepthFailOp
        && PassOp  == theOther.PassOp
        && CmpFunc == theOther.CmpFunc;
  }

  //! Non-equality check.
  bool operator!= (const Graphic3d_StencilState& theOther) const { return !operator== (theOther); }
};

//! Pipeline drawing parameters.
struct Graphic3d_PipelineParams
{
public:

  Graphic3d_TypeOfPrimitiveArray PrimType;         //!< primitive type
  Graphic3d_CompareFunc          DepthCmpFunc;     //!< depth test function
  bool                           ToWriteColor;     //!< flag indicating writing into color buffer
  bool                           ToWriteDepth;     //!< flag indicating writing into depth buffer
  bool                           ToEnableBlending; //!< standard blending on/off
  Graphic3d_FaceCulling          CullFace;         //!< face culling side
  bool                           CullFaceEnabled;  //!< face culling enabled state
  Graphic3d_StencilState         StencilFront;     //!< stencil parameters
  Graphic3d_StencilState         StencilBack;      //!< stencil parameters
  uint8_t                        StencilMask;      //!< stencil mask; hypothetically can be larger then 8 bit, but not practically
  uint8_t                        StencilRef;       //!< stencil reference value; hypothetically can be larger then 8 bit, but not practically
  bool                           StencilEnabled;   //!< stencil test enabled state

public:

  //! Enable depth test.
  void EnableDepthTest (Graphic3d_CompareFunc theCompare = Graphic3d_CompareFunc_LessEqual)
  {
    ToWriteDepth = true;
    DepthCmpFunc = theCompare;
  }

  //! Enable normal blending.
  void EnableBlending()
  {
    ToEnableBlending = true;
  }

  //! Enable face culling.
  void EnableFaceCulling (Graphic3d_FaceCulling theFace)
  {
    CullFaceEnabled = true;
    CullFace = theFace;
  }

  //! Default constructor.
  Graphic3d_PipelineParams()
  : PrimType (Graphic3d_TOPA_TRIANGLES),
    DepthCmpFunc (Graphic3d_CompareFunc_Always),
    ToWriteColor (true),
    ToWriteDepth (false),
    ToEnableBlending (false),
    CullFace (Graphic3d_FaceCulling_Back),
    CullFaceEnabled (false),
    StencilMask (0xFF),
    StencilRef (0),
    StencilEnabled (false) {}

};

#endif // _Graphic3d_PipelineParams_HeaderFile
