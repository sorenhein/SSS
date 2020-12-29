#ifndef SSS_DISTRIBUTION_H
#define SSS_DISTRIBUTION_H

#include <string>
#include <list>
#include <cassert>

#include "struct.h"

using namespace std;


struct DistID
{
  unsigned cards; // Smallest number of NS+EW cards needed
  unsigned holding; // Canonical EW holding
};


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

      bool operator != (const SideInfo& side2)
      {
        if (len != side2.len || counts.size() != side2.counts.size())
          return true;

        for (unsigned rank = 0; rank < counts.size(); rank++)
          if (counts[rank] != side2.counts[rank])
            return false;
        
        return true;
      };

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
        counts.resize(side1.counts.size());

        for (unsigned rank = 0; rank < side1.counts.size(); rank++)
          counts[rank] = side1.counts[rank] - side2.counts[rank];
        len = side1.len - side2.len;
      }

      void collapse1(const unsigned rank)
      {
        // rank gets collapsed onto rank-1 which must be non-void.
        assert(rank > 1);
        counts[rank-1] += counts[rank];
        counts[rank] = 0;
      };

      void collapse2(const unsigned rank)
      {
        // rank gets collapsed onto rank-2 which must be non-void.
        assert(rank > 2);
        counts[rank-2] += counts[rank];
        counts[rank] = 0;
      };

      string str(const vector<char>& names) const
      {
        string s = "";
        for (unsigned r = counts.size(); r-- > 0; )
        {
          if (counts[r] > 0)
            s += string(counts[r], names[r]);
        }
        return (s == "" ? "-" : s);
      }
    };

    struct StackInfo
    {
      SideInfo west;
      unsigned cases; // Combinatorial count

      unsigned seen; // Number of EW cards already seen
      unsigned rankNext;

      StackInfo(const unsigned ranks = 0)
      {
        west.reset(ranks);
        cases = 1;

        seen = 0;
        rankNext = 0;
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

      DistInfo()
      {
        west.len = 0;
        east.len = 0;
        cases = 1;
      }

      void add(
        const unsigned rank,
        const unsigned count,
        const unsigned factor)
      {
        west.add(rank, count);
        cases *= factor;
      }

      string str(const vector<char> &names) const
      {
        string s;
        s = "W " + to_string(west.len) + ": " + west.str(names);
        s += ", E " + to_string(east.len) + ": " + east.str(names);
        s += " (" + to_string(cases) + ")\n";
        return s;
      }
    };

    unsigned cards;

    vector<unsigned> full2reduced;
    vector<unsigned> reduced2full;
    unsigned rankSize; // Reduced ranks

    SideInfo opponents;

    vector<DistInfo> distributions;

    Distribution const * distCanonical;

    SurvivorMatrix distSurvivors;
    vector<SurvivorMatrix> distSurvivorsCollapse1;
    vector<vector<SurvivorMatrix>> distSurvivorsCollapse2;
    Survivors distSurvivorsWestVoid;
    Survivors distSurvivorsEastVoid;


    void setBinomial();

    void setNames();

    void shrink(
      const unsigned maxFullRank,
      const unsigned maxReducedRank);

    void mirror(unsigned& distIndex);

    const Survivors& survivorsReduced(
      const unsigned westRank,
      const unsigned eastRank) const;

    const Survivors& survivorsWestVoid() const;
    const Survivors& survivorsEastVoid() const;


  public:

    Distribution();

    ~Distribution();

    void reset();

    void setRanks(
      const unsigned cards,
      const unsigned holding2); // Binary, not trinary format

    void split();
    void splitAlternative(); // Does the identical thing, maybe faster

    void setPtr(Distribution const * distCanonicalIn);

    Distribution const * getPtr() const;

    unsigned size() const;

    DistID getID() const;

    void setSurvivors();

    const Survivors& survivors(
      const unsigned westRank,
      const unsigned eastRank) const;

    string str() const;

};

#endif
