#ifndef SSS_RANKS_H
#define SSS_RANKS_H

#include <vector>

#include "strategies/Winner.h"
#include "strategies/Card.h"

#include "const.h"

using namespace std;

struct TrickEntry;
struct CombEntry;
class Plays;


class Ranks
{
  private:

    struct ReducedRankInfo
    {
      unsigned rank;
      unsigned count;

      ReducedRankInfo()
      {
        count = 0;
      }

      void add(const unsigned rankIn)
      {
        rank = rankIn;
        count++;
      }
    };

    struct PositionInfo
    {
      vector<ReducedRankInfo> ranks;
      unsigned maxPos;
      unsigned minPos;

      vector<unsigned> fullCount;
      unsigned maxRank;
      unsigned minRank;

      bool singleRank;
      unsigned len;

      // remainders maps winners in subsequent tricks to winners
      // in the current reference frame.
      vector<vector<Sidewinner>> remainders;

      // The best card(s) that NS play to this trick, whether or not
      // they win it.  If they win, then the winner is interesting.
      // The indices are the first and second plays to this trick.
      vector<vector<Winner>> best;

      // The numbers of each relevant NS card.
      vector<unsigned> numRank;

      void resize(const unsigned cardsIn)
      {
        // Worst case, leaving room for voids at rank 0.
        ranks.resize(cardsIn+1);
        maxPos = cardsIn;

        fullCount.resize(cardsIn+1);
        maxRank = cardsIn;
      }

      void clear()
      {
        ranks.clear();
        fullCount.clear();
      }

      void zero()
      {
        for (unsigned pos = 0; pos <= maxPos; pos++)
          ranks[pos].count = 0;

        for (unsigned rank = 0; rank <= maxRank; rank++)
          fullCount[rank] = 0;
        
        len = 0;
      }

      void update(
        const unsigned position,
        const unsigned rank,
        bool& firstFlag)
      {
        ranks[position].add(rank);
        maxPos = position;

        fullCount[rank]++;
        maxRank = rank;

        if (firstFlag)
        {
          minPos = position;
          minRank = rank;
          firstFlag = false;
        }

        len++;
      }

      void setVoid(const bool forceFlag)
      {
        if (forceFlag || len == 0)
        {
          ranks[0].add(0);
          minPos = 0;
          maxPos = 0;

          fullCount[0] = 1;
          minRank = 0;
          maxRank = 0;
        }
      }

      void setSingleRank()
      {
        singleRank = (len >= 1 && minRank == maxRank);
      }

      bool operator >= (const PositionInfo& p2) const
      {
        // The rank vectors may not be of the same effective size.
        unsigned pos1 = maxPos + 1;  // One beyond end
        unsigned pos2 = p2.maxPos + 1;

        while (true)
        {
          while (true)
          {
            // If we run out of p2, this wins even if it also runs out.
            if (pos2 == 0)
              return true;
      
            if (p2.ranks[--pos2].count)
              break;
          }

          while (true)
          {
            // Otherwise p2 wins.
            if (pos1 == 0)
              return false;

            if (ranks[--pos1].count)
              break;
          }

          if (ranks[pos1].rank > p2.ranks[pos2].rank)
            return true;
          if (ranks[pos1].rank < p2.ranks[pos2].rank)
            return false;
          if (ranks[pos1].count > p2.ranks[pos2].count)
            return true;
          if (ranks[pos1].count < p2.ranks[pos2].count)
            return false;
        }
      }

      bool greater(
        const PositionInfo& p2,
        const PositionInfo& opps) const
      {
        /*
        const unsigned l = fullCount.size();
        if (l == 0)
          return true;

        for (unsigned rank = l-1; rank > 0; rank--) // Exclude void
        {
          if (fullCount[rank] > p2.fullCount[rank])
            return true;
          if (fullCount[rank] < p2.fullCount[rank])
            return false;
        }
        return true;
        */

        // There may be rank collapses from played EW cards.
        unsigned run1 = 0;
        unsigned run2 = 0;
        for (unsigned r = max(maxRank, p2.maxRank); ; r -= 2)
        {
          run1 += fullCount[r];
          run2 += p2.fullCount[r];
          if (r > 2 && opps.fullCount[r-1] == 0)
            continue;  // EW collapse
          else if (run1 > run2)
            return true;
          else if (run1 < run2)
            return false;
          else if (r <= 2)
            return true; // Nothing else happens, so equality
          
          run1 = 0;
          run2 = 0;
        }
      }
    };


    PositionInfo north;
    PositionInfo south;
    PositionInfo opps;

    unsigned holding;
    unsigned cards;
    unsigned maxRank;

    vector<string> namesNorthNew;
    vector<string> namesSouthNew;
    vector<string> namesOppsNew;

    vector<Card> cardsNorth;
    vector<Card> cardsSouth;
    vector<Card> cardsOpps;


    void setConstants();

    void zero();

    void setRanks();
    void setRanksNew(); // Also does strings.  Will replace setRanks

    void countNumbers(
      vector<unsigned>& numbers,
      const PositionInfo& posInfo) const;

    void setOrderTablesRemainder(PositionInfo& posInfo);

    void setOrderTablesWin(
      PositionInfo& posInfo,
      const WinningSide side,
      const PositionInfo& otherInfo,
      const WinningSide otherSide);

    void setOrderTables();

    // void resizeOrderTablesLose(PositionInfo& posInfo);

    void resizeOrderTablesWin(
      PositionInfo& posInfo,
      const PositionInfo& otherInfo);

    void resizeOrderTables();

    bool dominates(
      const PositionInfo& first,
      const PositionInfo& second) const;

    unsigned canonicalTrinary(
      const vector<unsigned>& fullCount1,
      const vector<unsigned>& fullCount2) const;

    void canonicalBoth(
      const vector<unsigned>& fullCount1,
      const vector<unsigned>& fullCount2,
      unsigned& holding3,
      unsigned& holding2) const;

    void trivialRanked(
      const unsigned tricks,
      const unsigned winRank,
      TrickEntry& trivialEntry) const;

    bool trivial(TrickEntry& trivialEntry) const;

    bool leadOK(
      const PositionInfo& leader,
      const PositionInfo& partner,
      const unsigned lead) const;

    bool pardOK(
      const PositionInfo& partner,
      const unsigned toBeat,
      const unsigned pard) const;

    void updateHoldings(
      const PositionInfo& leader,
      const PositionInfo& partner,
      const SidePosition side,
      unsigned& holding3,
      bool& rotateFlag) const;

    void logPlay(
      Plays& plays,
      const PositionInfo& leader,
      const PositionInfo& partner,
      const SidePosition side,
      const unsigned lead,
      const unsigned lho,
      const unsigned pard,
      const unsigned rho,
      const bool leadCollapse,
      const bool pardCollapse,
      const unsigned holding3,
      const bool rotateFlag) const;

    void setPlaysLeadWithVoid(
      PositionInfo& leader,
      PositionInfo& partner,
      const SidePosition side,
      const unsigned lead,
      const bool leadCollapse,
      Plays& plays);

    void setPlaysLeadWithoutVoid(
      PositionInfo& leader,
      PositionInfo& partner,
      const SidePosition side,
      const unsigned lead,
      const bool leadCollapse,
      Plays& plays);

    void setPlaysSide(
      PositionInfo& leader,
      PositionInfo& partner,
      const SidePosition side,
      Plays& plays);

    void strSetFullNames(
      vector<string>& namesNorth,
      vector<string>& namesSouth,
      vector<string>& namesOpps) const;

    string strPosition(
      const string& cards,
      const string& player) const;

    wstring strPlays(
      const PositionInfo& posInfo,
      const vector<string>& names) const;

  public:

    Ranks();

    ~Ranks();

    void resize(const unsigned cardsIn);

    unsigned size() const;

    void set(
      const unsigned holdingIn,
      CombEntry& combEntry);

    CombinationType setPlays(
      Plays& plays,
      TrickEntry& trivialEntry);

    string str() const;

    wstring strDiagram() const;
};

#endif
