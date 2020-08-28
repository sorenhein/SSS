#ifndef SSS_DISTRIBUTION_H
#define SSS_DISTRIBUTION_H

#include <string>
#include <list>

#include "struct.h"

using namespace std;


class Distribution
{
  private:

    struct SideInfo
    {
      vector<unsigned> counts; // For each (reduced) rank
      unsigned len; // Sum of rank counts

      void reset(const unsigned ranks)
      {
        counts.resize(ranks);
        for (unsigned rank = 0; rank < ranks; rank++)
          counts[rank] = 0;
        len = 0;
      }

      void add(
        const unsigned rank,
        const unsigned count)
      {
        counts[rank] = count;
        len += count;
      }

      void diff(
        const SideInfo& side1,
        const SideInfo& side2)
      {
        for (unsigned rank = 0; rank < side1.counts.size(); rank++)
          counts[rank] = side1.counts[rank] - side2.counts[rank];
        len = side1.len - side2.len;
      }
    };

    struct StackInfo
    {
      SideInfo west;
      unsigned cases; // Combinatorial count

      unsigned seen; // Number of EW cards already seen
      unsigned rankNext;

      StackInfo(const unsigned ranks)
      {
        west.reset(ranks);
        cases = 1;
      }

      void add(
        const unsigned rank,
        const unsigned count,
        const unsigned factor)
      {
        west.add(rank, count);
        rankNext++;
        cases *= factor;
      }
    };

    struct DistInfo
    {
      SideInfo west;
      SideInfo east;
      unsigned cases; // Combinatorial count

      DistInfo(unsigned cards = 0)
      {
        if (cards)
        {
          west.counts.resize(cards);
          east.counts.resize(cards);
          for (unsigned r = 0; r < cards; r++)
          {
            west.counts[r] = 0;
            east.counts[r] = 0;
          }
        }
        west.len = 0;
        east.len = 0;
        cases = 1;
      }

      string str() const
      {
        string s;
        s = "W " + to_string(west.len) + ": ";
        for (unsigned i = west.counts.size(); i-- > 0; )
        {
          if (west.counts[i] > 0)
          {
            for (unsigned j = 0; j < west.counts[i]; j++)
              s += to_string(i);
          }
        }
        s += ", E " + to_string(east.len) + ": ";
        for (unsigned i = east.counts.size(); i-- > 0; )
        {
          if (east.counts[i] > 0)
          {
            for (unsigned j = 0; j < east.counts[i]; j++)
              s += to_string(i);
          }
        }
        s += " (" + to_string(cases) + ")\n";
        return s;
      }
    };

    vector<unsigned> full2reduced;
    vector<unsigned> reduced2full;
    unsigned rankSize; // Reduced ranks
    SideInfo opponents;

    vector<vector<unsigned>> binomial;

    vector<DistInfo> distributions;



    void setBinomial();

    void setRanks(
      const unsigned cards,
      const unsigned holding2);

    void mirror(
      const unsigned len,
      const unsigned lenMid,
      unsigned& distIndex);

    string strStack(const list<DistInfo>& stack) const;

    string strDist(const unsigned DistIndex) const;


  public:

    Distribution();

    ~Distribution();

    void reset();

    unsigned set(
      const unsigned cards,
      const unsigned holding2); // Binary, not trinary format

    string rank2str(
      const vector<unsigned>& counts,
      const vector<string>& names) const;

    string str() const;

};

#endif
