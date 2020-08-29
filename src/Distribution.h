#ifndef SSS_DISTRIBUTION_H
#define SSS_DISTRIBUTION_H

#include <string>
#include <list>
#include <cassert>

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
        counts.resize(side1.counts.size());

        for (unsigned rank = 0; rank < side1.counts.size(); rank++)
          counts[rank] = side1.counts[rank] - side2.counts[rank];
        len = side1.len - side2.len;
      }

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

    vector<unsigned> full2reduced;
    vector<unsigned> reduced2full;
    unsigned rankSize; // Reduced ranks

    SideInfo opponents;

    vector<DistInfo> distributions;


    void setBinomial();

    void setNames();

    void shrink(
      const unsigned maxFullRank,
      const unsigned maxReducedRank);

    void setRanks(
      const unsigned cards,
      const unsigned holding2);

    void mirror(unsigned& distIndex);


  public:

    Distribution();

    ~Distribution();

    void reset();

    unsigned set(
      const unsigned cards,
      const unsigned holding2); // Binary, not trinary format

    unsigned setAlternative(
      const unsigned cards,
      const unsigned holding2); // Does the identical thing

    string str() const;

};

#endif
