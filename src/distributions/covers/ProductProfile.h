/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_PRODUCTPROFILE_H
#define SSS_PRODUCTPROFILE_H

#include <string>

using namespace std;


struct ProductProfile
{
  unsigned char length;
  vector<unsigned char> tops;

  bool flip(
    const ProductProfile& lowerProfile,
    const ProductProfile& upperProfile) const
  {
    // We want to express a Product in terms that make as much
    // intuitive sense as possible.  I think this tends to be in
    // terms of shortness.

    // With 6 cards, we generally want 1-4 to remain, but 2-5 to be 
    // considered as 1-4 from the other side.
    const unsigned char lsum = lowerProfile.length + upperProfile.length;
    if (lsum > length)
       return false;
    else if (lsum < length)
      return true;

    const unsigned s = tops.size();
    assert(s == lowerProfile.tops.size());
    assert(s == upperProfile.tops.size());

    // Start from the highest top.
    for (unsigned i = s; --i > 0; )
    {
      const unsigned tsum = lowerProfile.tops[i] + upperProfile.tops[i];
      if (tsum > tops[i])
        return false;
      else if (tsum < tops[i])
        return false;
    }

    // As they are the same, we might as well not flip.
    return false;
  };


  string str() const
  {
    stringstream ss;
    ss << +length << ":";
    for (auto& t: tops)
      ss << " " << +t;
    ss << "\n";
    return ss.str();
  };
};

#endif
