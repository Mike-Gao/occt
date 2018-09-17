// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <Graphic3d_FrameStats.hxx>

#include <Graphic3d_CView.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_FrameStats, Standard_Transient)

namespace
{
  //! Format counter.
  static std::ostream& formatCounter (std::ostream& theStream,
                                      Standard_Integer theWidth,
                                      const char* thePrefix,
                                      Standard_Size theValue,
                                      const char* thePostfix = NULL)
  {
    if (thePrefix != NULL)
    {
      theStream << thePrefix;
    }
    theStream << std::setfill(' ') << std::setw (theWidth);
    if (theValue >= 1000000000)
    {
      Standard_Real aValM = Standard_Real(theValue) / 1000000000.0;
      theStream << std::fixed << std::setprecision (1) << aValM << "G";
    }
    else if (theValue >= 1000000)
    {
      Standard_Real aValM = Standard_Real(theValue) / 1000000.0;
      theStream << std::fixed << std::setprecision (1) << aValM << "M";
    }
    else if (theValue >= 1000)
    {
      Standard_Real aValK = Standard_Real(theValue) / 1000.0;
      theStream << std::fixed << std::setprecision (1) << aValK << "k";
    }
    else
    {
      theStream << theValue;
    }
    if (thePostfix != NULL)
    {
      theStream << thePostfix;
    }
    return theStream;
  }

  //! Format memory counter.
  static std::ostream& formatBytes (std::ostream& theStream,
                                    Standard_Integer theWidth,
                                    const char* thePrefix,
                                    Standard_Size theValue,
                                    const char* thePostfix = NULL)
  {
    if (thePrefix != NULL)
    {
      theStream << thePrefix;
    }
    theStream << std::setfill(' ') << std::setw (theWidth);
    if (theValue >= 1024 * 1024 * 1024)
    {
      Standard_Real aValM = Standard_Real(theValue) / (1024.0 * 1024.0 * 1024.0);
      theStream << std::fixed << std::setprecision (1) << aValM << " GiB";
    }
    else if (theValue >= 1024 * 1024)
    {
      Standard_Real aValM = Standard_Real(theValue) / (1024.0 * 1024.0);
      theStream << std::fixed << std::setprecision (1) << aValM << " MiB";
    }
    else if (theValue >= 1024)
    {
      Standard_Real aValK = Standard_Real(theValue) / 1024.0;
      theStream << std::fixed << std::setprecision (1) << aValK << " KiB";
    }
    else
    {
      theStream << theValue << " B";
    }
    if (thePostfix != NULL)
    {
      theStream << thePostfix;
    }
    return theStream;
  }

  static const Standard_Real THE_SECONDS_IN_HOUR = 3600.0;
  static const Standard_Real THE_SECONDS_IN_MINUTE = 60.0;
  static const Standard_Real THE_SECOND_IN_HOUR   = 1.0 / THE_SECONDS_IN_HOUR;
  static const Standard_Real THE_SECOND_IN_MINUTE = 1.0 / THE_SECONDS_IN_MINUTE;

  //! Format time.
  static std::ostream& formatTime (std::ostream& theStream,
                                   Standard_Integer theWidth,
                                   const char* thePrefix,
                                   Standard_Real theSeconds,
                                   const char* thePostfix = NULL)
  {
    if (thePrefix != NULL)
    {
      theStream << thePrefix;
    }

    Standard_Real aSecIn = theSeconds;
    unsigned int aHours   = (unsigned int )(aSecIn * THE_SECOND_IN_HOUR);
    aSecIn -= Standard_Real(aHours) * THE_SECONDS_IN_HOUR;
    unsigned int aMinutes = (unsigned int )(aSecIn * THE_SECOND_IN_MINUTE);
    aSecIn -= Standard_Real(aMinutes) * THE_SECONDS_IN_MINUTE;
    unsigned int aSeconds = (unsigned int )aSecIn;
    aSecIn -= Standard_Real(aSeconds);
    Standard_Real aMilliSeconds = 1000.0 * aSecIn;

    char aBuffer[64];
    theStream << std::setfill(' ') << std::setw (theWidth);
    if (aHours > 0)
    {
      Sprintf (aBuffer, "%02u:%02u:%02u", aHours, aMinutes, aSeconds);
      theStream << aBuffer;
    }
    else if (aMinutes > 0)
    {
      Sprintf (aBuffer, "%02u:%02u", aMinutes, aSeconds);
      theStream << aBuffer;
    }
    else if (aSeconds > 0)
    {
      Sprintf (aBuffer, "%2u s", aSeconds);
      theStream << aBuffer;
    }
    else
    {
      theStream << std::fixed << std::setprecision (1) << aMilliSeconds << " ms";
    }

    if (thePostfix != NULL)
    {
      theStream << thePostfix;
    }
    return theStream;
  }
}

// =======================================================================
// function : Graphic3d_FrameStats
// purpose  :
// =======================================================================
Graphic3d_FrameStats::Graphic3d_FrameStats()
: myFpsTimer (Standard_True),
  myFrameStartTime (0.0),
  myFrameDuration  (0.0),
  myUpdateInterval (1.0),
  myFpsFrameCount (0),
  myCounters (0, 0),
  myLastFrameIndex (0),
  myIsLongLineFormat (Standard_False)
{
  //
}

// =======================================================================
// function : ~Graphic3d_FrameStats
// purpose  :
// =======================================================================
Graphic3d_FrameStats::~Graphic3d_FrameStats()
{
  //
}

// =======================================================================
// function : FormatStats
// purpose  :
// =======================================================================
TCollection_AsciiString Graphic3d_FrameStats::FormatStats (Graphic3d_RenderingParams::PerfCounters theFlags) const
{
  const Standard_Integer aValWidth = 5;
  std::stringstream aBuf;
  const Standard_Boolean isCompact = theFlags == Graphic3d_RenderingParams::PerfCounters_FrameRate; // only FPS is displayed
  const Graphic3d_FrameStatsData& aStats = LastDataFrame();
  if (myIsLongLineFormat
   && (theFlags & Graphic3d_RenderingParams::PerfCounters_FrameRate) != 0
   && (theFlags & Graphic3d_RenderingParams::PerfCounters_CPU) != 0)
  {
    aBuf << "FPS: "     << std::setfill(' ') << std::setw (isCompact ? aValWidth : 9)  << std::fixed << std::setprecision (1) << aStats.FrameRate()
         << " [CPU: "   << std::setfill(' ') << std::setw (isCompact ? aValWidth : 10) << std::fixed << std::setprecision (1) << aStats.FrameRateCpu() << "]\n";
  }
  else
  {
    if ((theFlags & Graphic3d_RenderingParams::PerfCounters_FrameRate) != 0)
    {
      aBuf << "FPS:     " << std::setfill(' ') << std::setw (isCompact ? aValWidth : aValWidth + 3) << std::fixed << std::setprecision (1) << aStats.FrameRate()  << "\n";
    }
    if ((theFlags & Graphic3d_RenderingParams::PerfCounters_CPU) != 0)
    {
      aBuf << "CPU FPS: " << std::setfill(' ') << std::setw (isCompact ? aValWidth : aValWidth + 3) << std::fixed << std::setprecision (1) << aStats.FrameRateCpu() << "\n";
    }
  }
  if ((theFlags & Graphic3d_RenderingParams::PerfCounters_Layers) != 0)
  {
    if (myIsLongLineFormat)
    {
      formatCounter (aBuf, aValWidth, "Layers:  ", aStats[Graphic3d_FrameStatsCounter_NbLayers]);
      if (HasCulledLayers())
      {
        formatCounter (aBuf, aValWidth, " [rendered: ", aStats[Graphic3d_FrameStatsCounter_NbLayersNotCulled], "]");
      }
      aBuf << "\n";
    }
    else
    {
      formatCounter (aBuf, aValWidth + 3, "Layers:  ", aStats[Graphic3d_FrameStatsCounter_NbLayers], "\n");
    }
  }
  if ((theFlags & Graphic3d_RenderingParams::PerfCounters_Structures) != 0)
  {
    if (myIsLongLineFormat)
    {
      formatCounter (aBuf, aValWidth, "Structs: ", aStats[Graphic3d_FrameStatsCounter_NbStructs]);
      if (HasCulledStructs())
      {
        formatCounter (aBuf, aValWidth, " [rendered: ", aStats[Graphic3d_FrameStatsCounter_NbStructsNotCulled], "]");
      }
      aBuf << "\n";
    }
    else
    {
      formatCounter (aBuf, aValWidth + 3, "Structs: ", aStats[Graphic3d_FrameStatsCounter_NbStructs], "\n");
    }
  }
  if ((theFlags & Graphic3d_RenderingParams::PerfCounters_Groups) != 0
   || (theFlags & Graphic3d_RenderingParams::PerfCounters_GroupArrays) != 0
   || (theFlags & Graphic3d_RenderingParams::PerfCounters_Triangles) != 0
   || (theFlags & Graphic3d_RenderingParams::PerfCounters_Points) != 0
   || (!myIsLongLineFormat
    && ((theFlags & Graphic3d_RenderingParams::PerfCounters_Structures) != 0
     || (theFlags & Graphic3d_RenderingParams::PerfCounters_Layers) != 0)))
  {
    aBuf << "Rendered\n";
  }
  if (!myIsLongLineFormat
   && (theFlags & Graphic3d_RenderingParams::PerfCounters_Layers) != 0)
  {
    formatCounter (aBuf, aValWidth, "    Layers: ", aStats[Graphic3d_FrameStatsCounter_NbLayersNotCulled], "\n");
  }
  if (!myIsLongLineFormat
   && (theFlags & Graphic3d_RenderingParams::PerfCounters_Structures) != 0)
  {
    formatCounter (aBuf, aValWidth, "   Structs: ", aStats[Graphic3d_FrameStatsCounter_NbStructsNotCulled], "\n");
  }
  if ((theFlags & Graphic3d_RenderingParams::PerfCounters_Groups) != 0)
  {
    formatCounter (aBuf, aValWidth, "    Groups: ", aStats[Graphic3d_FrameStatsCounter_NbGroupsNotCulled], "\n");
  }
  if ((theFlags & Graphic3d_RenderingParams::PerfCounters_GroupArrays) != 0)
  {
    formatCounter (aBuf, aValWidth, "    Arrays: ", aStats[Graphic3d_FrameStatsCounter_NbElemsNotCulled], "\n");
    formatCounter (aBuf, aValWidth, "    [fill]: ", aStats[Graphic3d_FrameStatsCounter_NbElemsFillNotCulled], "\n");
    formatCounter (aBuf, aValWidth, "    [line]: ", aStats[Graphic3d_FrameStatsCounter_NbElemsLineNotCulled], "\n");
    formatCounter (aBuf, aValWidth, "   [point]: ", aStats[Graphic3d_FrameStatsCounter_NbElemsPointNotCulled], "\n");
    formatCounter (aBuf, aValWidth, "    [text]: ", aStats[Graphic3d_FrameStatsCounter_NbElemsTextNotCulled], "\n");
  }
  if ((theFlags & Graphic3d_RenderingParams::PerfCounters_Triangles) != 0)
  {
    formatCounter (aBuf, aValWidth, " Triangles: ", aStats[Graphic3d_FrameStatsCounter_NbTrianglesNotCulled], "\n");
  }
  if ((theFlags & Graphic3d_RenderingParams::PerfCounters_Points) != 0)
  {
    formatCounter (aBuf, aValWidth, "    Points: ", aStats[Graphic3d_FrameStatsCounter_NbPointsNotCulled], "\n");
  }
  if ((theFlags & Graphic3d_RenderingParams::PerfCounters_EstimMem) != 0)
  {
    aBuf << "GPU Memory\n";
    formatBytes (aBuf, aValWidth, "  Geometry: ", aStats[Graphic3d_FrameStatsCounter_EstimatedBytesGeom], "\n");
    formatBytes (aBuf, aValWidth, "  Textures: ", aStats[Graphic3d_FrameStatsCounter_EstimatedBytesTextures], "\n");
    formatBytes (aBuf, aValWidth, "    Frames: ", aStats[Graphic3d_FrameStatsCounter_EstimatedBytesFbos], "\n");
  }

  if ((theFlags & Graphic3d_RenderingParams::PerfCounters_FrameTime) != 0)
  {
    aBuf << "Timers Average\n";
    formatTime (aBuf, aValWidth, " Elapsed Frame: ", aStats[Graphic3d_FrameStatsTimer_ElapsedFrame], "\n");
    formatTime (aBuf, aValWidth, "     CPU Frame: ", aStats[Graphic3d_FrameStatsTimer_CpuFrame], "\n");
    if (myCountersMax[Graphic3d_FrameStatsTimer_CpuPicking] > 0.0)
    {
      formatTime (aBuf, aValWidth, "   CPU Picking: ", aStats[Graphic3d_FrameStatsTimer_CpuPicking], "\n");
    }
    if (myCountersMax[Graphic3d_FrameStatsTimer_CpuCulling] > 0.0)
    {
      formatTime (aBuf, aValWidth, "   CPU Culling: ", aStats[Graphic3d_FrameStatsTimer_CpuCulling], "\n");
    }
    if (myCountersMax[Graphic3d_FrameStatsTimer_CpuDynamics])
    {
      formatTime (aBuf, aValWidth, "  CPU Dynamics: ", aStats[Graphic3d_FrameStatsTimer_CpuDynamics], "\n");
    }
    if ((theFlags & Graphic3d_RenderingParams::PerfCounters_FrameTimeMax) != 0)
    {
      aBuf << "Timers Max\n";
      formatTime (aBuf, aValWidth, "     CPU Frame: ", myCountersMax[Graphic3d_FrameStatsTimer_CpuFrame], "\n");
      if (myCountersMax[Graphic3d_FrameStatsTimer_CpuPicking] > 0.0)
      {
        formatTime (aBuf, aValWidth, "   CPU Picking: ", myCountersMax[Graphic3d_FrameStatsTimer_CpuPicking], "\n");
      }
      if (myCountersMax[Graphic3d_FrameStatsTimer_CpuCulling] > 0.0)
      {
        formatTime (aBuf, aValWidth, "   CPU Culling: ", myCountersMax[Graphic3d_FrameStatsTimer_CpuCulling], "\n");
      }
      if (myCountersMax[Graphic3d_FrameStatsTimer_CpuDynamics])
      {
        formatTime (aBuf, aValWidth, "  CPU Dynamics: ", myCountersMax[Graphic3d_FrameStatsTimer_CpuDynamics], "\n");
      }
    }
  }

  return TCollection_AsciiString (aBuf.str().c_str());
}

// =======================================================================
// function : FrameStart
// purpose  :
// =======================================================================
void Graphic3d_FrameStats::FrameStart (const Handle(Graphic3d_CView)& theView,
                                       bool theIsImmediateOnly)
{
  const Graphic3d_RenderingParams::PerfCounters aBits = !theView.IsNull()
                                                      ? theView->RenderingParams().CollectedStats
                                                      : Graphic3d_RenderingParams::PerfCounters_NONE;
  if (theIsImmediateOnly
   && (aBits & Graphic3d_RenderingParams::PerfCounters_SkipImmediate) != 0)
  {
    return;
  }

  const Standard_Integer aNbFrames = Max (!theView.IsNull()
                                         ? theView->RenderingParams().StatsNbFrames
                                         : 1, 1);
  if (myCounters.Size() != aNbFrames)
  {
    myCounters.Resize (0, aNbFrames - 1, false);
    myCounters.Init (Graphic3d_FrameStatsData());
    myLastFrameIndex = myCounters.Upper();
  }

  // reset values at the end of frame (after data has been flushed),
  // so that application can put some counters (like picking time) before FrameStart().
  //myCountersTmp.Reset();

  myFrameStartTime = myFpsTimer.ElapsedTime();
  if (!myFpsTimer.IsStarted())
  {
    myFpsTimer.Reset();
    myFpsTimer.Start();
    myFpsFrameCount = 0;
  }
}

// =======================================================================
// function : FrameEnd
// purpose  :
// =======================================================================
void Graphic3d_FrameStats::FrameEnd (const Handle(Graphic3d_CView)& theView,
                                     bool theIsImmediateOnly)
{
  const Graphic3d_RenderingParams::PerfCounters aBits = !theView.IsNull()
                                                      ? theView->RenderingParams().CollectedStats
                                                      : Graphic3d_RenderingParams::PerfCounters_NONE;
  if (theIsImmediateOnly
   && (aBits & Graphic3d_RenderingParams::PerfCounters_SkipImmediate) != 0)
  {
    return;
  }

  const double aTime = myFpsTimer.ElapsedTime();
  myFrameDuration = aTime - myFrameStartTime;
  ++myFpsFrameCount;
  if (!theView.IsNull())
  {
    myUpdateInterval = theView->RenderingParams().StatsUpdateInterval;
  }

  if (aTime < myUpdateInterval)
  {
    myCountersTmp.FlushTimers (myFpsFrameCount, false);
    return;
  }

  if (aTime > gp::Resolution())
  {
    // update FPS
    myFpsTimer.Stop();
    const double aCpuSec = myFpsTimer.UserTimeCPU();

    myCountersTmp[Graphic3d_FrameStatsTimer_ElapsedFrame]  = aTime;
    myCountersTmp[Graphic3d_FrameStatsTimer_CpuFrame]      = aCpuSec;
    myCountersTmp.ChangeFrameRate()    = double(myFpsFrameCount) / aTime;
    myCountersTmp.ChangeFrameRateCpu() = aCpuSec > gp::Resolution()
                                       ? double(myFpsFrameCount) / aCpuSec
                                       : -1.0;
    myCountersTmp.FlushTimers (myFpsFrameCount, true);
    myCountersMax.FillMax (myCountersTmp);
    myFpsTimer.Reset();
    myFpsTimer.Start();
    myFpsFrameCount = 0;
  }

  // update structure counters
  if (theView.IsNull())
  {
    myCounters.SetValue (myLastFrameIndex, myCountersTmp);
    myCountersTmp.Reset();
    return;
  }

  updateStatistics (theView, theIsImmediateOnly);

  if (++myLastFrameIndex > myCounters.Upper())
  {
    myLastFrameIndex = myCounters.Lower();
  }
  myCounters.SetValue (myLastFrameIndex, myCountersTmp);
  myCountersTmp.Reset();
}
