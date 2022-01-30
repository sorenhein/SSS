
/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_EXPLSTATS_H
#define SSS_EXPLSTATS_H

#include <iostream>
#include <iomanip>
#include <sstream>

#include <vector>
#include <string>


struct ExplStat
{
  vector<unsigned> lengths;
  vector<unsigned> singles;
  vector<vector<unsigned>> pairs;
};


struct ExplStats
{
  vector<vector<ExplStat>> explStats;

  void resize(const vector<vector<list<CoverSpec>>>& specs)
  {
    const unsigned ssize = specs.size();

    explStats.resize(ssize);

    for (unsigned s = 0; s < ssize; s++)
    {
      const unsigned s2size = specs[s].size();

      explStats[s].resize(s2size);

      for (unsigned t = 0; t < s2size; t++)
      {
        const unsigned csize = specs[s][t].size();

        ExplStat& explStat = explStats[s][t];
        explStat.lengths.resize(20, 0); // <= 20 explanations per strat
        explStat.singles.resize(csize, 0);

        explStat.pairs.resize(csize);

        for (unsigned c = 0; c < csize; c++)
          explStat.pairs[c].resize(csize, 0);
      }
    }
  };

  string str() const
  {
    stringstream ss;

    for (unsigned length = 2; length < explStats.size(); length++)
    {
      for (unsigned tops1 = 1; tops1 < explStats[length].size(); tops1++)
      {
        const ExplStat& explStat = explStats[length][tops1];
        const unsigned vsize = explStat.singles.size();
        if (vsize == 0)
          continue;

        ss << "Cover counts " << length << "-" << tops1 << "\n";
        unsigned cumSum = 0, sum = 0;
        for (unsigned cno = 0; cno < explStat.lengths.size(); cno++)
        {
          const unsigned v = explStat.lengths[cno];
          if (v > 0)
          {
            ss << setw(3) << cno << setw(6) << v << "\n";
            sum += v;
            cumSum += v * cno;
          }
        }
        ss << string(9, '-') << "\n";
        ss << setw(9) << fixed << setprecision(2) <<
          cumSum / static_cast<double>(sum) << "\n\n";

        ss << "Pairs " << length << "-" << tops1 << "\n";

        ss << setw(3) << "#" << " | ";
        for (unsigned cno = 0; cno < explStat.pairs.size(); cno++)
          ss << setw(5) << cno;
        ss << " | " << setw(5) << "Sing" << "\n";
        ss << string(3 + 3 + 5 * explStat.pairs.size() + 3 + 5, '-') << "\n";

        for (unsigned cno = 0; cno < explStat.pairs.size(); cno++)
        {
          ss << setw(3) << cno << " | ";
          for (unsigned cno2 = 0; cno2 < explStat.pairs.size(); cno2++)
          {
            const unsigned v = explStat.pairs[cno][cno2];
            ss << setw(5) << (v == 0 ? "-" : to_string(v));
          }
          ss << " | " << setw(5) << explStat.singles[cno] << "\n";
        }
        ss << "\n";
      }
    }

    return ss.str();
  }
};

#endif
