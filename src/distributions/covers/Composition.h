
/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COMPOSITION_H
#define SSS_COMPOSITION_H

#include <vector>
#include <string>

using namespace std;

/* A composition is a split of a length (such as 5) into ordered
 * numbers of ranks (such as 1+3+1).  There are 2^(n-1) of them
 * for length n, and for a given precise number of terms (such as 
 * 3 in the example) there are the binomial coefficient C(n-1, k-1), 
 * so C(4, 2) = 6 in the example.
 * (https://en.wikipedia.org/wiki/Composition_(combinatorics)
 */


class Composition
{
  private:

    unsigned char length;

    vector<unsigned char> tops;


  public:

    Composition();

    void reset();

    void set(
      const vector<unsigned char>& topsIn,
      const unsigned char lastUsed);

    unsigned char count(const unsigned char topNo) const;

    unsigned size() const;

    string strHeader(const unsigned char width) const;

    string strLine() const;
};

#endif
