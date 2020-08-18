#ifndef SSS_COMBINATIONS_H
#define SSS_COMBINATIONS_H

#include <map>
#include <list>
#include <vector>
#include <string>

#include "const.h"

using namespace std;


class Combinations
{
  private:

    struct CountEntry
    {
      int total;
      int unique;
    };

    int maxCards;

    vector<vector<CombEntry>> combinations;
    vector<vector<int>> uniques; // Probably gets more structure

    vector<CountEntry> counts;


  public:

    Combinations();

    ~Combinations();

    void reset();

    void resize(const int maxCardsIn);

    void runUniques(const int cards);

    string strUniques(const int cards = 0) const;

};

#endif
