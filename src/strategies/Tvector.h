#ifndef SSS_TVECTOR_H
#define SSS_TVECTOR_H

#include <vector>
#include <list>
#include <cassert>

using namespace std;


enum Compare
{
  COMPARE_LESS_THAN = 0,
  COMPARE_EQUAL = 1,
  COMPARE_GREATER_THAN = 2,
  COMPARE_INCOMMENSURATE = 3
};


struct TrickEntry
{
  unsigned dist;
  unsigned tricks;

  bool operator < (const TrickEntry& te2) const
  {
    assert(dist == te2.dist);
    return(tricks < te2.tricks);
  }

  bool operator > (const TrickEntry& te2) const
  {
    return(tricks > te2.tricks);
  }
};


class Tvector
{
  private:

    list<TrickEntry> results;
    unsigned weightInt;


  public:

    Tvector();

    ~Tvector();

    list<TrickEntry>::const_iterator begin() const 
      { return results.begin(); };
    list<TrickEntry>::const_iterator end() const 
      { return results.end(); }

    void reset();

    void log(
      const vector<unsigned>& distributions,
      const vector<unsigned>& tricks);

    bool operator >= (const Tvector& tv2) const;
    bool operator > (const Tvector& tv2) const;

    Compare compare(const Tvector& tv2) const;

    void mergeDown(const Tvector& tv2);

    unsigned weight() const;

    string str(const string& title = "") const;
};

#endif
