/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include "Verbalside.h"

#include "VerbalDimensions.h"

#include "../../../utils/table.h"


VerbalPhrase VerbalSide::player() const
{
 if (side == OPP_WEST)
   return (symmFlag ? PLAYER_EITHER : PLAYER_WEST);
 else
   return (symmFlag ? PLAYER_EITHER : PLAYER_EAST);
}
