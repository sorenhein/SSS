/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_PRODUCTPROFILE_H
#define SSS_PRODUCTPROFILE_H

#include <vector>
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

    // The usual return (no flip) if nothing shows up.
    bool backstopFlag = false;

    if (lsum > length)
    {
      if (lowerProfile.length == 1 && upperProfile.length == length)
      {
        // Special case: This is easier to say as "not void".
        backstopFlag = true;
      }
      else
        return true;
    }
    else if (lsum < length)
      return false;

    const unsigned s = tops.size();
    assert(s == lowerProfile.tops.size());
    assert(s == upperProfile.tops.size());

    // Start from the highest top.
    for (unsigned i = s; --i > 0; )
    {
      const unsigned tsum = lowerProfile.tops[i] + upperProfile.tops[i];
      if (tsum > tops[i])
        return true;
      else if (tsum < tops[i])
        return false;
    }

    // As they are the same, we might as well not flip.
    return backstopFlag;
  };


  void mirror(const ProductProfile& pp2)
  {
    // Turn this profile into pp2 (a sum profile) minus this one.

    length = pp2.length - length;

    const unsigned s = tops.size();
    assert(pp2.tops.size() == s);
    for (unsigned i = 0; i < s; i++)
      tops[i] = pp2.tops[i] - tops[i];
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
