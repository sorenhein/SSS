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
