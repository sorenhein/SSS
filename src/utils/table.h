/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_TABLE_H
#define SSS_TABLE_H


enum Side
{
  SIDE_NORTH = 0,
  SIDE_SOUTH = 1,
  SIDE_OPPS = 2,
  SIDE_NONE = 3
};

enum Pair
{
  PAIR_NS = 0,
  PAIR_EW = 1
};

enum Level
{
  LEVEL_LEAD = 0,
  LEVEL_LHO = 1,
  LEVEL_PARD = 2,
  LEVEL_RHO = 3,
  LEVEL_SIZE = 4
};

enum Opponent
{
  OPP_WEST = 0,
  OPP_EAST = 1
};

#endif
