/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

/*
   A Product contains a set of Term's that all have to apply (they are
   multiplied together) in order for a holding to be contained.
   Like a Term, a Product does not contain information about the number
   of cards or tops that are available in a particular holding.
 */

#ifndef SSS_PRODUCT_H
#define SSS_PRODUCT_H

#include <vector>
#include <string>

#include "../term/Length.h"
#include "../term/Top.h"

using namespace std;

class Profile;
class RanksNames;
struct OppData;
enum Opponent: unsigned;
enum CoverComposition: unsigned;
enum CoverVerbal: unsigned;
enum ExplainEqual: unsigned;


class Product
{
  private:

    Length length;

    vector<Top> tops;

    unsigned long long codeInt;

    unsigned char complexity;

    unsigned char topSize; // Last used top number + 1; may be 0

    unsigned char activeCount; // Number of tops that are used


    bool singular(
      const Profile& sumProfile,
      const unsigned char canonicalShift) const;

    ExplainEqual mostlyEqual() const;

    void topRange(
      unsigned char& noLow,
      unsigned char& noHigh) const;

    bool topsSimplerThan(const Product& p2) const;

    bool simplerThan(const Product& p2) const;

    void getWestLengths(
      const Profile& sumProfile,
      const RanksNames& ranksNames,
      const Opponent simplestOpponent,
      const unsigned char canonicalShift,
      unsigned char& xesMin,
      unsigned char& xesMax,
      unsigned char& xesHidden,
      unsigned char& xesAvailable,
      unsigned char& topsExact,
      unsigned char& topsAvailable) const;

    unsigned char countHidden(
      const Profile& sumProfile,
      const unsigned char canonicalShift) const;

    void fillUsedTops(
      const Profile& sumProfile,
      const unsigned char canonicalShift,
      Product& productWest,
      Product& productEast,
      OppData& dataWest,
      OppData& dataEast) const;

    void fillUnusedTops(
      const Profile& sumProfile,
      const unsigned char canonicalShift,
      const Opponent fillOpponent,
      Product& productWest,
      Product& productEast) const;

    void fillSideBottoms(
      const Opponent fillOpponent,
      const unsigned char hidden,
      Product& productWest,
      Product& productEast) const;

    void separateSingular(
      const Profile& sumProfile,
      const unsigned char canonicalShift,
      Product& productWest,
      Product& productEast) const;

    string strEqualTopsOnly(
      const Profile& sumProfile,
      const RanksNames& ranksNames,
      const unsigned char canonicalShift) const;

    string strEqualTops(
      const Profile& sumProfile,
      const RanksNames& ranksNames,
      const string& anchor,
      const OppData& data,
      const unsigned char canonicalShift) const;

    string strExactStart(
      const Profile& sumProfile,
      const unsigned char canonicalShift) const;

    string strExactTop(
      const Profile& sumProfile,
      const RanksNames& ranksNames,
      const unsigned char canonicalShift,
      const unsigned char topNo) const;

    string strExact(
      const Profile& sumProfile,
      const RanksNames& ranksNames,
      const string& anchor,
      const unsigned char canonicalShift) const;

    string strExactTops(
      const Profile& sumProfile,
      const RanksNames& ranksNames,
      const Opponent simplestOpponent,
      const unsigned char canonicalShift,
      const bool skipUnusedFlag) const;

    string strAvailableTops(
      const Profile& sumProfile,
      const RanksNames& ranksNames,
      const unsigned char canonicalShift) const;

    string strVerbalLengthOnly(
      const Profile& sumProfile,
      const bool symmFlag,
      const unsigned char canonicalShift = 0) const;

    string strVerbalOneTopOnly(
      const Profile& sumProfile,
      const RanksNames& ranksNames,
      const bool symmFlag,
      const unsigned char canonicalShift = 0) const;

    string strVerbalLengthAndOneTop(
      const Profile& sumProfile,
      const RanksNames& ranksNames,
      const bool symmFlag,
      const unsigned char canonicalShift = 0) const;

    string strVerbalEqualTops(
      const Profile& sumProfile,
      const RanksNames& ranksNames,
      const CoverVerbal verbal,
      const bool symmFlag,
      const unsigned char canonicalShift = 0) const;

    string strVerbalSingular(
      const Profile& sumProfile,
      const RanksNames& ranksNames,
      const bool symmFlag,
      const unsigned char canonicalShift = 0) const;

  public:

    Product();

    void reset();

    void resize(const size_t topCount);

    void set(
      const Profile& sumProfile,
      const Profile& lowerProfile,
      const Profile& upperProfile,
      const unsigned long long code);

    bool includes(
      const Profile& distProfile,
      const unsigned char canonicalShift = 0) const;

    bool symmetrizable(
      const Profile& sumProfile,
      const unsigned char canonicalShift = 0) const;

    unsigned char getComplexity() const;

    unsigned long long code() const;

    unsigned char size() const;

    unsigned char effectiveDepth() const;

    bool sameTops(const Product& product2) const;

    unsigned char minimumByTops(
      const Opponent voidSide,
      const Profile& sumProfile,
      const unsigned char canonicalShift) const;

    bool discardSymmetric(
      const Profile& sumProfile,
      const unsigned char canonicalShift) const;

    CoverComposition composition() const;

    CoverVerbal verbal() const;

    bool lengthConsistent(const unsigned char specificLength) const;

    Opponent simplestOpponent(
      const Profile& sumProfile,
      const unsigned char canonicalShift = 0) const;

    Opponent simplestSingular(
      const Profile& sumProfile,
      const unsigned char canonicalShift = 0) const;

    CompareType presentOrder(const Product& product2) const;

    string strHeader(const unsigned lengthIn = 0) const;

    string strLine() const;

    string strVerbal(
      const Profile& sumProfile,
      const RanksNames& ranksNames,
      const CoverVerbal verbal,
      const bool symmFlag,
      const unsigned char canonicalShift = 0) const;

};

#endif
