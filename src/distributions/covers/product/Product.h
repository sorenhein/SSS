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
#include <list>
#include <string>

#include "../term/Length.h"
#include "../term/Top.h"

using namespace std;

class Profile;
class RanksNames;
struct VerbalData;
class Completion;
class VerbalCover;
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

    // The following private methods are associated with
    // strVerbal and live in Pverbal.cpp
    // TODO Reorder according to Pverbal.cpp

    Opponent simplestOpponent(
      const Profile& sumProfile,
      const unsigned char canonicalShift = 0) const;

    Opponent simplestSingular(
      const Profile& sumProfile,
      const unsigned char canonicalShift = 0) const;

    void topRange(
      unsigned char& noLow,
      unsigned char& noHigh) const;

    bool topsSimplerThan(const Product& p2) const;

    bool simplerThan(const Product& p2) const;

    unsigned char countBottoms(
      const Profile& sumProfile,
      const unsigned char canonicalShift) const;

    void fillUsedTops(
      const Profile& sumProfile,
      const unsigned char canonicalShift,
      Product& productWest,
      Product& productEast,
      VerbalData& dataWest,
      VerbalData& dataEast) const;

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


    string strUsedTops(
      const Profile& sumProfile,
      const RanksNames& ranksNames,
      const unsigned char canonicalShift,
      const bool expandFlag,
      const bool singleRankFlag,
      const bool onlyFullFlag) const;

    string strUsedBottoms(
      const Profile& sumProfile,
      const RanksNames& ranksNames,
      const unsigned char canonicalShift,
      const bool allFlag) const;

    string strAddBottom(
      const RanksNames& ranksNames,
      const unsigned char canonicalShift,
      const string& base,
      const bool emptyFlag) const;


    string strVerbalLengthOnly(
      const Profile& sumProfile,
      const RanksNames& ranksNames,
      const bool symmFlag) const;

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

    // Both high tops and any tops

    string strVerbalTops(
      const Profile& sumProfile,
      const RanksNames& ranksNames,
      const unsigned char canonicalShift,
      const string& side,
      const string& sideOther,
      const VerbalData& data,
      const bool singleActiveRank,
      const bool flipAllowedFlag) const;

    string strVerbalTopsDual(
      const Profile& sumProfile,
      const RanksNames& ranksNames,
      const unsigned char canonicalShift,
      const Product& productOther,
      const string& side,
      const VerbalData& data,
      const VerbalData& dataOther) const;

    string strVerbalTopsOnly(
      const Profile& sumProfile,
      const RanksNames& ranksNames,
      const unsigned char canonicalShift,
      const Product& productWest,
      const Product& productEast,
      const VerbalData& dataWest,
      const VerbalData& dataEast,
      const bool flipAllowedFlag) const;


    // Compositions

    void makePartialProfile(
      const Profile& sumProfile,
      const unsigned char canonicalShift,
      Completion& completion) const;

    bool makeCompletions(
      const Profile& sumProfile,
      const unsigned char canonicalShift,
      const VerbalData& data,
      const unsigned char maxCompletions,
      VerbalCover& completions) const;


    // Any tops

    string strVerbalAnyTops(
      const Profile& sumProfile,
      const RanksNames& ranksNames,
      const bool symmFlag,
      const unsigned char canonicalShift = 0) const;

    // High tops

    string strVerbalHighTopsOnlySide(
      const Profile& sumProfile,
      const RanksNames& ranksNames,
      const unsigned char canonicalShift,
      const string& side,
      const string& sideOther,
      const VerbalData& data,
      const bool singleActiveRank) const;

    string strVerbalHighTopsOnlyBothSides(
      const Profile& sumProfile,
      const RanksNames& ranksNames,
      const unsigned char canonicalShift,
      const Product& productOther,
      const string& side,
      const VerbalData& data,
      const VerbalData& dataOther) const;

    string strVerbalHighTopsSide(
      const Profile& sumProfile,
      const RanksNames& ranksNames,
      const string& side,
      const VerbalData& data,
      const unsigned char canonicalShift) const;

    string strVerbalHighTops(
      const Profile& sumProfile,
      const RanksNames& ranksNames,
      const bool symmFlag,
      const unsigned char canonicalShift) const;

    // Singular

    string strVerbalSingularQualifier(
      const Profile& sumProfile,
      const unsigned char canonicalShift) const;

    string strVerbalSingularSide(
      const Profile& sumProfile,
      const RanksNames& ranksNames,
      const string& side,
      const unsigned char canonicalShift) const;

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

    CompareType presentOrder(const Product& product2) const;

    string strHeader(const unsigned lengthIn = 0) const;

    string strLine() const;

    // This method and all its private sub-methods live in a separate
    // implementation file, not Product.cpp but Pverbal.cpp

    string strVerbal(
      const Profile& sumProfile,
      const RanksNames& ranksNames,
      const CoverVerbal verbal,
      const bool symmFlag,
      const unsigned char canonicalShift = 0) const;

};

#endif
