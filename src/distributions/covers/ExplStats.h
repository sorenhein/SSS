
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


  string strVector3(
    const vector<vector<vector<unsigned>>>& vec,
    const string& name) const
  {
    stringstream ss;
    ss << "Covers: " << name << " stats\n\n";

    for (unsigned length = 2; length < vec.size(); length++)
    {
      for (unsigned tops1 = 1; tops1 < vec[length].size(); tops1++)
      {
        const unsigned vsize = vec[length][tops1].size();
        if (vsize == 0)
          continue;

        ss << "Length " << length << ", tops1 " << tops1 << "\n";
        for (unsigned cno = 0; cno < vsize; cno++)
        {
          const unsigned v = vec[length][tops1][cno];

          ss << 
            setw(3) << cno <<
            setw(6) << (v == 0 ? "-" : to_string(v)) << "\n";
        }
        ss << "\n";
      }
    }
    return ss.str();
  };


  string strSingles() const
  {
    return ExplStats::strVector3(singles, "Single");
  };


  string strPairs() const
  {
    stringstream ss;
    ss << "Covers: Pair stats\n\n";

    for (unsigned length = 2; length < pairs.size(); length++)
    {
      for (unsigned tops1 = 1; tops1 < pairs[length].size(); tops1++)
      {
        const auto& vec = pairs[length][tops1];
        if (vec.size() == 0)
          continue;

        ss << "Length " << length << ", tops1 " << tops1 << "\n\n";

        ss << setw(3) << "#" << " | ";
        for (unsigned cno = 0; cno < vec.size(); cno++)
          ss << setw(5) << cno;
        ss << "\n";
        ss << string(3 + 3 + 5*vec.size(), '-') << "\n";

        for (unsigned cno = 0; cno < vec.size(); cno++)
        {
          ss << setw(3) << cno << " | ";
          for (unsigned cno2 = 0; cno2 < vec.size(); cno2++)
          {
            const unsigned v = vec[cno][cno2];
            ss << setw(5) << (v == 0 ? "-" : to_string(v));
          }
          ss << "\n";
        }
        ss << "\n";
      }
    }
    return ss.str();
  };


  string strLengths() const
  {
    return ExplStats::strVector3(singles, "Length");
  }
};

#endif
