/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_PROFILE_H
#define SSS_PROFILE_H

#include <vector>
#include <string>

using namespace std;


class Profile
{
  friend class ProfilePair;

  private:

    unsigned char lengthInt;

    vector<unsigned char> tops;


  public:

    void set(const vector<unsigned char>& topsIn);

    void setSum(
      const vector<unsigned char>& topsWest,
      const vector<unsigned char>& topsEast);

    // Only leave the highest top as non-zero.
    // This is used for manual cover rows.
    void limit();

    unsigned char length() const;

    unsigned char operator [] (const unsigned char topNo) const;

    // Only unused tops or tops with upper == lower.
    bool onlyEquals(
      const Profile& upperProfile,
      const Profile& sumProfile) const;

    unsigned size() const;

    unsigned long long getLowerCode() const;

    unsigned long long getUpperCode(
      const Profile& sumProfile,
      const Profile& lowerProfile) const;

    string strHeader() const;

    string strLine() const;
};

#endif
