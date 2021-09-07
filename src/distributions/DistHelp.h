/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_DISTHELP_H
#define SSS_DISTHELP_H

// These helper structures are only intended for Distribution.h
// within the private section.  Therefore the file does not include
// files that are needed (they are already included in Distribution.h).


enum SideCompare
{
  SC_SAME = 0,
  SC_DIFFERENT_LENGTH = 1,
  SC_DIFFERENT_VALUES = 2
};

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

  void makeSentinel()
  {
    len = numeric_limits<unsigned>::max();
  }

  bool operator != (const SideInfo& side2) const
  {
    if (len != side2.len || counts.size() != side2.counts.size())
      return true;

    for (unsigned rank = 0; rank < counts.size(); rank++)
      if (counts[rank] != side2.counts[rank])
        return true;
    
    return false;
  };

  SideCompare compare(const SideInfo& side2) const
  {
    if (len != side2.len || counts.size() != side2.counts.size())
      return SC_DIFFERENT_LENGTH;
    
    for (unsigned rank = 0; rank < counts.size(); rank++)
      if (counts[rank] != side2.counts[rank])
        return SC_DIFFERENT_VALUES;
    
    return SC_SAME;
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
    assert(rank > 0);
    counts[rank-1] += counts[rank];
    counts[rank] = 0;
  };

  void collapse2(const unsigned rank)
  {
    // rank gets collapsed onto rank-2 which must be non-void.
    assert(rank > 1);
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

struct SurvivorMatrix
{
  vector<vector<SurvivorList>> data;

  void resize(const unsigned len)
  {
    data.resize(len);
    for (unsigned w = 0; w < len; w++)
    {
      data[w].resize(len);
      for (unsigned e = 0; e < len; e++)
        data[w][e].reducedSize = 0;
    }
  };
};

#endif
