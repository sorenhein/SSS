#ifndef SSS_WINNER_H
#define SSS_WINNER_H

#include <vector>
#include <list>
#include <cassert>

#include "Winner.h"

using namespace std;


enum WinningSide
{
  WIN_NORTH = 0,
  WIN_SOUTH = 1,
  WIN_EITHER = 2,
  WIN_NONE = 3
};


class Winner
{
  private:

    unsigned rank;
    WinningSide side;


  public:

    Winner();

    ~Winner();

    void reset();

    void set(
      const unsigned rankIn,
      const WinningSide sideIn);

    string str() const;
};

#endif
