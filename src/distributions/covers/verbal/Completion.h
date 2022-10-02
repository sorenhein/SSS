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

class RanksNames;
enum Opponent: unsigned;


class Completion
{
  // A single combination such as 97x.

  private:

    struct CompData
    {
      unsigned char length;

      // Number of tops used, including with zero values.
      unsigned char topsUsed;

      // Number of ranks actively used and non-zero.
      unsigned char ranksActive;

      // Lowest active, non-zero rank.
      unsigned char lowestRankActive;

      // Tops of full rank.
      unsigned char topsFull;

      void reset()
      {
        length = 0;
        topsUsed = 0;
        ranksActive = 0;
        lowestRankActive = 0;
        topsFull = 0;
      };

      void update(
        const unsigned char topNo,
        const unsigned char value,
        [[maybe_unused]] const unsigned char valueMax)
      {
        // Call this from the highest topNo on down in order.
        topsUsed += value;
        if (value)
        {
          ranksActive++;
          lowestRankActive = topNo;
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



  public:

    void resize(const size_t numTops);

    void setTop(
      const unsigned char topNo,
      const bool usedFlag,
      const unsigned char countWest,
      const unsigned char maximum);

    void updateTop(
      const unsigned char topNo,
      const unsigned char count,
      const unsigned char maximum);

    const list<unsigned char>& openTops() const;

    unsigned char length(const Opponent side) const;

    unsigned char getTopsFull(const Opponent side) const;

    unsigned char numOptions() const;

    Opponent preferSingleActive() const;

    Opponent preferSimpleActive() const;

    bool operator < (const Completion& comp2) const;

    bool operator == (const Completion& comp2) const;

    string strDebug() const;

    string strSet(
      const RanksNames& ranksNames,
      const Opponent side,
      const bool expandFlag,
      const bool singleRankFlag,
      const bool explicitVoidFlag = false) const;

    string strSetNew(
      const RanksNames& ranksNames,
      const Opponent side,
      const bool enableExpandFlag,
      const bool enableSingleRankFlag,
      const bool explicitVoidFlag = false) const;

    string strUnset(
      const RanksNames& ranksNames,
      const Opponent side) const;
};

#endif
