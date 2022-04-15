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

    unsigned char length;

    vector<unsigned char> tops;


  public:

    // Default: Use topsIn.size()
    void set(
      const vector<unsigned char>& topsIn,
      const unsigned char numTops = 0);

    void setSingle(
      const unsigned char numTops,
      const unsigned char lengthIn,
      const unsigned char topIn);

    void setSum(
      const vector<unsigned char>& topsWest,
      const vector<unsigned char>& topsEast);

    // TODO May eliminate this?
    void mirrorAround(const Profile& sumProfile);

    unsigned char count(const unsigned char topNo) const;

    unsigned size() const;

    unsigned char getLength() const;

    const vector<unsigned char>& getTops() const;

    unsigned long long getCode() const;

    unsigned long long getCode(
      const Profile& sumProfile,
      const Profile& lowerProfile) const;

    string strHeader() const;

    string strLine() const;
};

#endif
