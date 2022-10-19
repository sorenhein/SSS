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
  // For example: 
  // PLAYER_WEST, 
  // "PLAYER_WEST", 
  // GROUP_PLAYER, 
  // PHRASE_NONE.

  unsigned instance;
  string tag;
  unsigned group;
  unsigned expansion;
};


struct TagConnection
{
  unsigned group;
  string tag;
};

#endif
