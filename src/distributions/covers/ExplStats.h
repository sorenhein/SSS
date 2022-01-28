
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

        ss << "Length " << length << ", tops1 " << tops1 << " (singles)\n";
        for (unsigned cno = 0; cno < vsize; cno++)
        {
          const unsigned v = vecSingles[cno];
          ss << 
            setw(3) << cno <<
            setw(6) << (v == 0 ? "-" : to_string(v)) << "\n";
        }
        ss << "\n";

        ss << "Length " << length << ", tops1 " << tops1 << " (lengths)\n";
        for (unsigned cno = 0; cno < vsize; cno++)
        {
          const unsigned v = vecLengths[cno];
          if (v > 0)
            ss << 
              setw(3) << cno <<
              setw(6) << (v == 0 ? "-" : to_string(v)) << "\n";
        }
        ss << "\n";

        ss << "Length " << length << ", tops1 " << tops1 << " (pairs)\n";

        ss << setw(3) << "#" << " | ";
        for (unsigned cno = 0; cno < vecPairs.size(); cno++)
          ss << setw(5) << cno;
        ss << "\n";
        ss << string(3 + 3 + 5 * vecPairs.size(), '-') << "\n";

        for (unsigned cno = 0; cno < vecPairs.size(); cno++)
        {
          ss << setw(3) << cno << " | ";
          for (unsigned cno2 = 0; cno2 < vecPairs.size(); cno2++)
          {
            const unsigned v = vecPairs[cno][cno2];
            ss << setw(5) << (v == 0 ? "-" : to_string(v));
          }
          ss << "\n";
        }
        ss << "\n";
      }
    }

    return ss.str();
  }
};

#endif
