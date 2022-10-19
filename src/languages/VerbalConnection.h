/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_VERBAL_CONNECTION_H
#define SSS_VERBAL_CONNECTION_H

using namespace std;


struct VerbalConnection
{
  // For example: PLAYER_WEST, "PLAYER_WEST". 

  unsigned instance;
  string tag;
};


struct TagConnection
{
  unsigned group;
  string tag;
};

#endif
