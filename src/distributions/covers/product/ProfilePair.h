/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/


/*
 * A ProfilePair has the data needed to fill out a Product.
 * So it is used when Product's are being generated, either 
 * algorithmically or manually.  When algoritmically, one important
 * job of this class is to eliminate potential results that would
 * have duplicative results and be more complex.
 */


#ifndef SSS_PROFILEPAIR_H
#define SSS_PROFILEPAIR_H

#include "Profile.h"

using namespace std;

class Product;


class ProfilePair
{
  private:

    Profile lowerProfile;
    Profile upperProfile;

    unsigned char topNext; // Running top number


    bool active(
      const unsigned char maxValue,
      const unsigned char actualLow,
      const unsigned char actualHigh,
      const unsigned char impliedLow,
      const unsigned char impliedHigh) const;

    bool punchTop(
      const Profile& sumProfile,
      const unsigned char topNumber,
      const unsigned char sumLower,
      const unsigned char sumHigher) const;


  public:

    ProfilePair();

    ProfilePair(const Profile& sumProfile);

    void init(const Profile& sumProfile);

    void setLength(
      const unsigned char lenLow,
      const unsigned char lenHigh);

    void setTop(
      const unsigned char topNumber,
      const unsigned char topCountLow,
      const unsigned char topCountHigh);

    void setProduct(
      Product& product,
      const Profile& sumProfile,
      const unsigned long long code) const;

    void getLengthRange(
      unsigned char& sumLower,
      unsigned char& sumHigher) const;

    unsigned char getTopLength(const Profile& sumProfile) const;

    unsigned char getNextTopNo() const;

    void incrTop();

    bool last() const;

    bool minimal(
      const Profile& sumProfile,
      const unsigned char lengthLow,
      const unsigned char lengthHigh) const;

    unsigned long long getCode(const Profile& sumProfile) const;

    string strLines() const;
};

#endif
