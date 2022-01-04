/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_DEBUGPLAY_H
#define SSS_DEBUGPLAY_H

enum DebugPlay
{
  DEBUGPLAY_NONE = 0x0,
  DEBUGPLAY_CONSTANT_PLAYS = 0x1,
  DEBUGPLAY_RHO_DETAILS = 0x2,
  DEBUGPLAY_PARD_DETAILS = 0x4,
  DEBUGPLAY_LHO_DETAILS = 0x8,
  DEBUGPLAY_LEAD_DETAILS = 0x10,
  DEBUGPLAY_NODE_COUNTS = 0x20
};

#endif
