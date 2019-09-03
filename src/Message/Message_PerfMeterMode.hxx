// Created on: 2019-08-13
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _Message_PerfMeterMode_HeaderFile
#define _Message_PerfMeterMode_HeaderFile

//! Specifies kind of report information to collect
enum Message_PerfMeterMode
{
  Message_PerfMeterMode_None,           //!< no computation
  Message_PerfMeterMode_UserTimeCPU,    //!< the current CPU user time in seconds
  Message_PerfMeterMode_SystemTimeInfo, //!< the current CPU system time in seconds
  Message_PerfMeterMode_MemPrivate,     //!< OSD_MemInfo::MemPrivate
  Message_PerfMeterMode_MemVirtual,     //!< OSD_MemInfo::MemVirtual
  Message_PerfMeterMode_MemWorkingSet,   //!< OSD_MemInfo::MemWorkingSet
  Message_PerfMeterMode_MemWorkingSetPeak, //!< OSD_MemInfo::MemWorkingSetPeak
  Message_PerfMeterMode_MemSwapUsage,     //!< OSD_MemInfo::MemSwapUsage
  Message_PerfMeterMode_MemSwapUsagePeak, //!< OSD_MemInfo::MemSwapUsagePeak
  Message_PerfMeterMode_MemHeapUsage      //!< OSD_MemInfo::MemHeapUsage
};

#endif // _Message_PerfMeterKind_HeaderFile