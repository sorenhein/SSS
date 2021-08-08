#ifndef SSS_WINNERS_H
#define SSS_WINNERS_H

#include <vector>
#include <list>
#include <cassert>

#include "Winner.h"
#include "Card.h"

struct Play;
class Comparer;

using namespace std;


class Winners
{
  private:

    list<Winner> winners;


    void fillComparer(
      Comparer& comparer,
      const Winners& w2) const;

    bool operator != (const Winners& w2) const;

    void operator += (const Winner& swNew);

    bool rankExceeds(const Winners& w2) const;

    void limitByRank();

    void consolidate();

    string strSingleSided(
      const string& name,
      const Card& winner) const;


  public:

    Winners();

    ~Winners();

    void reset();

    void set(
      const WinningSide sideIn,
      const Card& card);

    void set(
      const Card& north,
      const Card& south);

    void setEmpty();

    bool empty() const;

    void operator *= (const Winners& w2);

    void operator += (const Winners& w2);

    void operator |= (const Winners& w2);

    bool operator == (const Winners& w2) const;

    Compare compareForDeclarer(const Winners& w2) const;

    void flip();

    void update(const Play& play);

    string str() const;

    string strEntry() const;

    string strDebug() const;
};

#endif
