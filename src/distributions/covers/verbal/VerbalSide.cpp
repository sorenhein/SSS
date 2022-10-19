/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include "Verbalside.h"

#include "../../../languages/Dictionary.h"
#include "../../../languages/connections/cover/phrases.h"

#include "../../../utils/table.h"


PhrasesEnum VerbalSide::player() const
{
 if (side == OPP_WEST)
   return (symmFlag ? PLAYER_EITHER : PLAYER_WEST);
 else
   return (symmFlag ? PLAYER_EITHER : PLAYER_EAST);
}


PhrasesEnum VerbalSide::otherPlayer() const
{
 if (side == OPP_WEST)
   return (symmFlag ? PLAYER_EITHER : PLAYER_EAST);
 else
   return (symmFlag ? PLAYER_EITHER : PLAYER_WEST);
}


Opponent VerbalSide::otherSide() const
{
  return (side == OPP_WEST ? OPP_EAST : OPP_WEST);
}


void VerbalSide::bothPlayers(
  Opponent& side1,
  Opponent& side2) const
{
  if (symmFlag)
  {
    side1 = side;
    side2 = (side1 == OPP_WEST ? OPP_EAST : OPP_WEST);
  }
  else
  {
    side1 = OPP_WEST;
    side2 = OPP_EAST;
  }
}

