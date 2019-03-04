// Created on: 2013-01-29
// Created by: Kirill GAVRILOV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#include <Font_TextFormatter.hxx>

#include <Font_FTFont.hxx>

#include <Precision.hxx>

IMPLEMENT_STANDARD_RTTIEXT (Font_TextFormatter, Standard_Transient)

namespace
{
  typedef NCollection_Vec2<Standard_ShortReal> Vec2f;

  //! Auxiliary function to translate corners by the vector.
  inline void move (NCollection_Vector< Vec2f >& theCorners,
                    const Vec2f&                 theMoveVec,
                    Standard_Integer             theCharLower,
                    const Standard_Integer       theCharUpper)
  {
    for(; theCharLower <= theCharUpper; ++theCharLower)
    {
      theCorners.ChangeValue (theCharLower) += theMoveVec;
    }
  }

  //! Auxiliary function to translate corners in vertical direction.
  inline void moveY (NCollection_Vector<Vec2f>& theCorners,
                     const Standard_ShortReal   theMoveVec,
                     Standard_Integer           theCharLower,
                     const Standard_Integer     theCharUpper)
  {
    for(; theCharLower <= theCharUpper; ++theCharLower)
    {
      theCorners.ChangeValue (theCharLower).y() += theMoveVec;
    }
  }

}

// =======================================================================
// function : Font_TextFormatter
// purpose  :
// =======================================================================
Font_TextFormatter::Font_TextFormatter()
: myAlignX (Graphic3d_HTA_LEFT),
  myAlignY (Graphic3d_VTA_TOP),
  myTabSize (8),
  myWrappingWidth (0.0f),
  myLastSymbolWidth (0.0f),
  myMaxSymbolWidth (0.0f),
  //
  myPen (0.0f, 0.0f),
  myLineSpacing (0.0f),
  myAscender (0.0f),
  myIsFormatted (false),
  //
  myLinesNb (0),
  myRectLineStart (0),
  myNewLineNb(0),
  myPenCurrLine (0.0f),
  myBndTop   (0.0f),
  myBndWidth (0.0f),
  myMoveVec (0.0f, 0.0f)
{
  //
}

// =======================================================================
// function : SetupAlignment
// purpose  :
// =======================================================================
void Font_TextFormatter::SetupAlignment (const Graphic3d_HorizontalTextAlignment theAlignX,
                                         const Graphic3d_VerticalTextAlignment   theAlignY)
{
  myAlignX = theAlignX;
  myAlignY = theAlignY;
}

// =======================================================================
// function : Reset
// purpose  :
// =======================================================================
void Font_TextFormatter::Reset()
{
  myIsFormatted = false;
  myString.Clear();
  myPen.x() = myPen.y() = 0.0f;
  myLineSpacing = myAscender = 0.0f;
  myCorners.Clear();
  myNewLines.Clear();

  myLastSymbolWidth = 0.0f;
  myMaxSymbolWidth = 0.0f;
}

// =======================================================================
// function : Append
// purpose  :
// =======================================================================
void Font_TextFormatter::Append (const NCollection_String& theString,
                                 Font_FTFont&              theFont)
{
  if (theString.IsEmpty())
  {
    return;
  }

  myAscender    = Max (myAscender,    theFont.Ascender());
  myLineSpacing = Max (myLineSpacing, theFont.LineSpacing());
  myString     += theString;

  int aSymbolsCounter = 0; // special counter to process tabulation symbols

  // first pass - render all symbols using associated font on single ZERO baseline
  Handle(Font_TextFormatter) aFormatter (this);
  Standard_Utf32Char aCharThis;
  for (Font_TextFormatter::Iterator aFormatterIt (aFormatter); aFormatterIt .More(); aFormatterIt .Next())
  {
    aCharThis = aFormatterIt.Symbol();
    const Standard_Utf32Char aCharNext = aFormatterIt.SymbolNext();

    Standard_ShortReal anAdvanceX = 0;
    if (Font_TextFormatter::IsCommandSymbol (aCharThis))
    {
      continue; // skip unsupported carriage control codes
    }
    else if (aCharThis == '\x0A') // LF (line feed, new line)
    {
      aSymbolsCounter = 0;
      myNewLines.Append (myPen.x());
      anAdvanceX = 0; // the symbol has null width
    }
    else if (aCharThis == ' ')
    {
      ++aSymbolsCounter;
      anAdvanceX = theFont.AdvanceX (' ', aCharNext);
    }
    else if (aCharThis == '\t')
    {
      const Standard_Integer aSpacesNum = (myTabSize - (aSymbolsCounter - 1) % myTabSize);
      anAdvanceX = theFont.AdvanceX (' ', aCharNext) * Standard_ShortReal(aSpacesNum);
      aSymbolsCounter += aSpacesNum;
    }
    else
      anAdvanceX = theFont.AdvanceX (aCharThis, aCharNext);

    ++aSymbolsCounter;
    myCorners.Append (myPen);
    myPen.x() += anAdvanceX;
    myMaxSymbolWidth = Max (myMaxSymbolWidth, anAdvanceX);
  }
  myLastSymbolWidth = myPen.x() - myCorners.Last().x();
}

// =======================================================================
// function : newLine
// purpose  :
// =======================================================================
void Font_TextFormatter::newLine (const Standard_Integer theLastRect,
                                  const Standard_ShortReal theMaxLineWidth)
{
  Standard_Integer aFirstCornerId = myRectLineStart;
  Standard_Integer aLastCornerId = theLastRect;

  if (aFirstCornerId >= getRectsNb())
  {
    ++myLinesNb;
    myPenCurrLine -= myLineSpacing;
    return;
  }

  Standard_ShortReal aXMin = BottomLeft (aFirstCornerId).x();
  Font_Rect aBndBox;
  BndBox (aLastCornerId, aBndBox);
  Standard_ShortReal aXMax = aBndBox.Right;

  myMoveVec.y() = myPenCurrLine;
  switch (myAlignX)
  {
    default:
    case Graphic3d_HTA_LEFT:   myMoveVec.x() = -aXMin; break;
    case Graphic3d_HTA_RIGHT:  myMoveVec.x() = -aXMin +        (theMaxLineWidth - (aXMax - aXMin)); break;
    case Graphic3d_HTA_CENTER: myMoveVec.x() = -aXMin + 0.5f * (theMaxLineWidth - (aXMax - aXMin)); break;
  }

  move (myCorners, myMoveVec, myRectLineStart, theLastRect);

  ++myLinesNb;
  myPenCurrLine -= myLineSpacing;
  myRectLineStart = theLastRect + 1;
}

// =======================================================================
// function : Format
// purpose  :
// =======================================================================
void Font_TextFormatter::Format()
{
  if (getRectsNb() == 0 || myIsFormatted)
  {
    return;
  }

  myIsFormatted = true;
  myLinesNb = myRectLineStart = 0;
  myBndTop     = 0.0f;
  myBndWidth   = 0.0f;
  myMoveVec.x() = myMoveVec.y() = 0.0f;

  // split text into lines and apply horizontal alignment
  myPenCurrLine = -myAscender;
  Standard_Integer aRectIter = 0;
  myNewLineNb = 0;

  Standard_ShortReal aMaxLineWidth = Wrapping();
  if (HasWrapping())
  {
    aMaxLineWidth = Max (aMaxLineWidth, MaximumSymbolWidth()); // it is not possible to wrap less than symbol width
  }
  else
  {
    if (myNewLines.IsEmpty()) // If only one line
      aMaxLineWidth = myPen.x();
    else
    {
      for (int aLineIt = 0; aLineIt < myLinesNb; aLineIt++)
        aMaxLineWidth = Max (aMaxLineWidth, getLineWidth (aLineIt));
    }
  }

  Handle(Font_TextFormatter) aFormatter (this);
  for (Font_TextFormatter::Iterator aFormatterIt (aFormatter);
       aFormatterIt .More(); aFormatterIt .Next())
  {
    const Standard_Utf32Char aCharThis = aFormatterIt.Symbol();
    aRectIter = aFormatterIt.SymbolPosition();

    if (aCharThis == '\x0A') // LF (line feed, new line)
    {
      const Standard_Integer aLastRect = aRectIter; // last rect on current line
      newLine (aLastRect, aMaxLineWidth);
      ++myNewLineNb;
      continue;
    }
    else if (HasWrapping()) // wrap lines longer than maximum width
    {
      Standard_Integer aFirstCornerId = myRectLineStart;

      Font_Rect aBndBox;
      BndBox (aRectIter, aBndBox);
      const Standard_ShortReal aNextXPos = aBndBox.Right - BottomLeft (aFirstCornerId).x();
      if (aNextXPos > aMaxLineWidth) // wrap the line and do processing of the symbol
      {
        const Standard_Integer aLastRect = aRectIter - 1; // last rect on current line
        newLine (aLastRect, aMaxLineWidth);
      }
    }
  }

  myBndWidth = aMaxLineWidth;

  // move last line
  newLine (getRectsNb() - 1, aMaxLineWidth);

  // apply vertical alignment style
  if (myAlignY == Graphic3d_VTA_BOTTOM)
  {
    myBndTop = -myLineSpacing - myPenCurrLine;
  }
  else if (myAlignY == Graphic3d_VTA_CENTER)
  {
    myBndTop = 0.5f * (myLineSpacing * Standard_ShortReal(myLinesNb));
  }
  else if (myAlignY == Graphic3d_VTA_TOPFIRSTLINE)
  {
    myBndTop = myAscender;
  }

  if (myAlignY != Graphic3d_VTA_TOP)
  {
    moveY (myCorners, myBndTop, 0, getRectsNb() - 1);
  }
}

// =======================================================================
// function : BndBox
// purpose  :
// =======================================================================
Standard_Boolean Font_TextFormatter::BndBox (const Standard_Integer theIndex, Font_Rect& theBndBox) const
{
  if (theIndex < 0 || theIndex >= GetCorners().Size())
    return Standard_False;

  const NCollection_Vec2<Standard_ShortReal>& aLeftCorner = BottomLeft (theIndex);
  if (theIndex + 1 < myCorners.Length()) // not the last symbol
  {
    const NCollection_Vec2<Standard_ShortReal>& aNextLeftCorner = BottomLeft (theIndex + 1);
    theBndBox.Left = aLeftCorner.x();
    theBndBox.Bottom = aLeftCorner.y();
    theBndBox.Top = theBndBox.Bottom + myLineSpacing;
    if (Abs (aLeftCorner.y() - aNextLeftCorner.y()) < Precision::Confusion()) // in the same row
    {
      theBndBox.Right = aNextLeftCorner.x();
    }
    else
    {
      // the next symbol is on the next row either by '\n' or by wrapping
      Standard_ShortReal aLineWidth = getLineWidth (LineIndex (theIndex));
      theBndBox.Left = aLeftCorner.x();
      switch (myAlignX)
      {
        case Graphic3d_HTA_LEFT:   theBndBox.Right = aLineWidth; break;
        case Graphic3d_HTA_RIGHT:  theBndBox.Right = myBndWidth; break;
        case Graphic3d_HTA_CENTER: theBndBox.Right = 0.5f * (myBndWidth + aLineWidth); break;
      }
    }
  }
  else // the last symbol
  {
    theBndBox.Left = aLeftCorner.x();
    theBndBox.Right = aLeftCorner.x() + myLastSymbolWidth;
    theBndBox.Bottom = aLeftCorner.y();
    theBndBox.Top = theBndBox.Bottom + myLineSpacing;
  }
  return Standard_True;
}


// =======================================================================
// function : IsLFSymbol
// purpose  :
// =======================================================================
Standard_Boolean Font_TextFormatter::IsLFSymbol (const Standard_Integer theIndex) const
{
  Font_Rect aBndBox;
  if (!BndBox (theIndex, aBndBox))
    return Standard_False;

  return Abs (aBndBox.Right - aBndBox.Left) < Precision::Confusion();
}

// =======================================================================
// function : GetFirstPosition
// purpose  :
// =======================================================================
Standard_ShortReal Font_TextFormatter::GetFirstPosition() const
{
  switch (myAlignX)
  {
    default:
    case Graphic3d_HTA_LEFT:   return 0; break;
    case Graphic3d_HTA_RIGHT:  return myBndWidth; break;
    case Graphic3d_HTA_CENTER: return 0.5f * myBndWidth; break;
  }
}

// =======================================================================
// function : LinePositionIndex
// purpose  :
// =======================================================================
Standard_Integer Font_TextFormatter::LinePositionIndex (const Standard_Integer theIndex) const
{
  Standard_Integer anIndex = 0;

  Standard_ShortReal anIndexHeight = BottomLeft (theIndex).y();
  for (Standard_Integer aPrevIndex = theIndex-1; aPrevIndex >= 0; aPrevIndex--)
  {
    if (BottomLeft (aPrevIndex).y() > anIndexHeight)
      break;
    anIndex++;
  }
  return anIndex;
}

// =======================================================================
// function : LineIndex
// purpose  :
// =======================================================================
Standard_Integer Font_TextFormatter::LineIndex (const Standard_Integer theIndex) const
{
  if (myLineSpacing < 0.0f)
    return 0;

  return (Standard_Integer)Abs((BottomLeft (theIndex).y() + myAscender) / myLineSpacing);
}

// =======================================================================
// function : IsCommandSymbol
// purpose  :
// =======================================================================
Standard_Boolean Font_TextFormatter::IsCommandSymbol (const Standard_Utf32Char& theSymbol)
{
  if (theSymbol == '\x0D' // CR  (carriage return)
   || theSymbol == '\a'   // BEL (alarm)
   || theSymbol == '\f'   // FF  (form feed) NP (new page)
   || theSymbol == '\b'   // BS  (backspace)
   || theSymbol == '\v')  // VT  (vertical tab)
   return Standard_True;

  return Standard_False;
}

// =======================================================================
// function : getLineWidth
// purpose  :
// =======================================================================
Standard_ShortReal Font_TextFormatter::getLineWidth (const Standard_Integer theIndex) const
{
  if (theIndex < 0)
    return 0;

  if (theIndex < myNewLines.Length())
    return theIndex == 0 ? myNewLines[0] : myNewLines[theIndex] - myNewLines[theIndex -1];

  if (theIndex == myNewLines.Length()) // the last line
    return myPen.x() - getLineWidth (theIndex - 1);

  return 0;
}
