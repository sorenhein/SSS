
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

struct ExplStats
{
  vector<vector<vector<unsigned>>> singles;
  vector<vector<vector<vector<unsigned>>>> pairs;
  vector<vector<vector<unsigned>>> lengths;

  void resize(const vector<vector<list<CoverSpec>>>& specs)
  {
    const unsigned ssize = specs.size();

    singles.resize(ssize);
    lengths.resize(ssize);
    pairs.resize(ssize);

    for (unsigned s = 0; s < ssize; s++)
    {
      const unsigned s2size = specs[s].size();

      singles[s].resize(s2size);
      lengths[s].resize(s2size);
      pairs[s].resize(s2size);

      for (unsigned t = 0; t < s2size; t++)
      {
        const unsigned csize = specs[s][t].size();

        singles[s][t].resize(csize, 0);
        lengths[s][t].resize(20, 0); // 20 explanations per strat
  
        pairs[s][t].resize(csize);

        for (unsigned c = 0; c < csize; c++)
          pairs[s][t][c].resize(csize, 0);
      }
    }
  };

  string str() const
  {
    stringstream ss;

    for (unsigned length = 2; length < pairs.size(); length++)
    {
      for (unsigned tops1 = 1; tops1 < pairs[length].size(); tops1++)
      {
        const auto& vecSingles = singles[length][tops1];
        const auto& vecPairs = pairs[length][tops1];
        const auto& vecLengths = lengths[length][tops1];
        const unsigned vsize = vecSingles.size();
        if (vsize == 0)
          continue;

        ss << "Cover counts " << length << "-" << tops1 << "\n";
        unsigned cumSum = 0, sum = 0;
        for (unsigned cno = 0; cno < vecLengths.size(); cno++)
        {
          const unsigned v = vecLengths[cno];
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
        for (unsigned cno = 0; cno < vecPairs.size(); cno++)
          ss << setw(5) << cno;
        ss << " | " << setw(5) << "Sing" << "\n";
        ss << string(3 + 3 + 5 * vecPairs.size() + 3 + 5, '-') << "\n";

        for (unsigned cno = 0; cno < vecPairs.size(); cno++)
        {
          ss << setw(3) << cno << " | ";
          for (unsigned cno2 = 0; cno2 < vecPairs.size(); cno2++)
          {
            const unsigned v = vecPairs[cno][cno2];
            ss << setw(5) << (v == 0 ? "-" : to_string(v));
          }
          ss << " | " << setw(5) << vecSingles[cno] << "\n";
        }
        ss << "\n";
      }
    }

    return ss.str();
  }
};

#endif
