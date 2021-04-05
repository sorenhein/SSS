#ifndef SSS_CARD_H
#define SSS_CARD_H

#include <string>

#include "../const.h"

using namespace std;


class Card
{
  private:

    unsigned rank;
    unsigned depth; // 0 for highest card with that rank
    unsigned number; // 0 for lowest card with that player
    char name;

  public:

    Card();

    void reset();

    void set(
      const unsigned rankIn,
      const unsigned depthIn,
      const unsigned numberIn,
      const char nameIn);

    bool operator > (const Card& card2) const;
    bool operator >= (const Card& card2) const;
    bool operator == (const Card& card2) const;
    bool operator != (const Card& card2) const;
    bool operator <= (const Card& card2) const;
    bool operator < (const Card& card2) const;
    bool identical(const Card& card2) const;

    WinnerCompare compare(const Card& card2) const;

    bool isVoid() const;

    bool rankSame(const Card& card2) const;
    bool rankExceeds(const Card& card2) const;

    void operator *= (const Card& card2);

    unsigned getRank() const;
    unsigned getDepth() const;
    unsigned getNumber() const;
    char getName() const;

    void flipDepth(const unsigned maxDepth);

    string str(
      const string& side,
      const bool rankFlag = true) const;

    string strDebug(const string& side) const;
};

#endif
