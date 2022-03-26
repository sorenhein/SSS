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
  friend struct ProfilePair;

  private:

    unsigned char length;

    vector<unsigned char> tops;


  public:

    void resize(const unsigned numTops);

    // TODO Can we make it "size" instead, defaulting to topsIn.size()?
    void set(
      const vector<unsigned char>& topsIn,
      const unsigned char lastUsed);

    void setSingle(
      const unsigned char lengthIn,
      const unsigned char topIn);

    void mirror(const Profile& profile2);

    unsigned char count(const unsigned char topNo) const;

    unsigned size() const;

    unsigned char getLength() const;

    const vector<unsigned char>& getTops() const;

    string strHeader() const;

    string strLine() const;
};

#endif
