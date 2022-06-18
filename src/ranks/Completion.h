/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COMPLETION_H
#define SSS_COMPLETION_H

#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <deque>
#include <cassert>

#include "../utils/Card.h"

using namespace std;

// This is a simple class to keep track of partner's next-higher
// card of the same rank.  If one side wins a trick with a card,
// then this subsumes that player's own higher cards as well as
// partner's higher cards.
//
// This could in principle be part of Declarer, but we'd need one
// table in each player, so this seems simpler.


class Completion
{
  private:

    vector<Card const *> completionsPtr;


  public:

    Completion()
    {
      completionsPtr.clear();
    }

    void resize(const unsigned cards)
    {
      completionsPtr.clear();
      completionsPtr.resize(cards+1, nullptr);
    }

    void set(
      const deque<Card const *> northCards,
      const deque<Card const *> southCards)
    {
      auto iter1 = northCards.begin();
      auto iter2 = southCards.begin();

      while (iter1 != northCards.end() && iter2 != southCards.end())
      {
        const Card * cptr1 = * iter1;
        const Card * cptr2 = * iter2;

        const unsigned char rank1 = cptr1->getRank();
        const unsigned char rank2 = cptr2->getRank();
        if (rank1 == 0 || rank2 == 0)
          break;

        if (rank1 < rank2)
        {
          iter1++;
          continue;
        }
        else if (rank2 < rank1)
        {
          iter2++;
          continue;
        }


        const unsigned char abs1 = cptr1->getAbsNumber();
        const unsigned char abs2 = cptr2->getAbsNumber();

        if (abs1 < abs2)
        {
          completionsPtr[abs1] = cptr2;
          iter1++;
        }
        else
        {
          completionsPtr[abs2] = cptr1;
          iter2++;
        }
      }
    }

    Card const * get(const unsigned absNumber) const
    {
      assert(absNumber < completionsPtr.size());
      return(completionsPtr[absNumber]);
    }

    string str() const
    {
      stringstream ss;
      for (unsigned i = 0; i < completionsPtr.size(); i++)
      {
        if (completionsPtr[i])
          ss << 
            setw(2) << i << 
            setw(4) << +(completionsPtr[i]->getAbsNumber()) << "\n";
      }
      return ss.str();
    }
};

#endif
