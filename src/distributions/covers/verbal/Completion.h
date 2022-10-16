/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COMPLETION_H
#define SSS_COMPLETION_H

#include <vector>
#include <list>
#include <string>

using namespace std;

class Term;
class RanksNames;
enum Opponent: unsigned;


class Completion
{
  // A single combination such as 97x.

  private:

    struct CompData
    {
      unsigned char length;

      // Number of tops used.
      unsigned char topsUsed;

      // Number of ranks actively used and non-zero.
      unsigned char ranksActive;

      // Lowest active, non-zero rank.
      unsigned char lowestRankActive;

      // Highest active, non-zero rank.
      bool highestRankFlag;
      unsigned char highestRankActive;

      // Tops of full rank.
      unsigned char topsFull;

      // Range of free spaces excluding the given tops.
      unsigned char freeLower;
      unsigned char freeUpper;

      void reset()
      {
        length = 0;
        topsUsed = 0;
        ranksActive = 0;
        lowestRankActive = 0;
        highestRankFlag = false;
        highestRankActive = 0;
        topsFull = 0;
        freeLower = 0;
        freeUpper = 0;
      };

      void update(
        const unsigned char topNo,
        const unsigned char value,
        const unsigned char valueMax)
      {
        // Call this from the highest topNo on down in order.
        topsUsed += value;

        if (value)
        {
          ranksActive++;
          lowestRankActive = topNo;

          if (! highestRankFlag)
          {
            highestRankFlag = true;
            highestRankActive = topNo;
          }
        }

        if (value == valueMax)
          topsFull += valueMax;
      };
    };


    vector<unsigned char> west;
    vector<unsigned char> east;

    vector<bool> used;

    list<unsigned char> openTopNumbers;

    CompData dataWest;
    CompData dataEast;

    // Number of ranks actively used.
    unsigned char ranksUsed;

    // Lowest rank used, including with zero values.
    unsigned char lowestRankUsed;



  public:

    void resize(const size_t numTops);

    void setTop(
      const unsigned char topNo,
      const bool usedFlag,
      const unsigned char countWest,
      const unsigned char maximum);

    void updateTop(
      const unsigned char topNo,
      const unsigned char countSide,
      const unsigned char maximum,
      const Opponent side);

    void setFree(
      const unsigned char maximum,
      const Term& length);

    const list<unsigned char>& openTops() const;

    unsigned char length(const Opponent side) const;

    unsigned char getTopsFull(const Opponent side) const;

    unsigned char getTopsUsed(const Opponent side) const;

    unsigned char getFreeLower(const Opponent side) const;
    unsigned char getFreeUpper(const Opponent side) const;

    unsigned char getTotalLower(const Opponent side) const;
    unsigned char getTotalUpper(const Opponent side) const;

    unsigned char getLowestRankUsed() const;

    unsigned char getLowestRankActive(const Opponent side) const;

    bool lowestRankIsUsed(const Opponent side) const;

    bool expandable(const Opponent side) const;

    bool fullRanked(const Opponent side) const;

    bool highRanked(const Opponent side) const;

    unsigned char numOptions() const;

    Opponent preferSingleActive() const;

    Opponent preferSimpleActive() const;

    Opponent preferHighActive() const;

    bool operator < (const Completion& comp2) const;

    bool operator > (const Completion& comp2) const;

    string strSet(
      const RanksNames& ranksNames,
      const Opponent side,
      const bool explicitVoidFlag = false) const;

    string strUnset(
      const RanksNames& ranksNames,
      const Opponent side) const;

    string strXes(const Opponent side) const;
};

#endif
