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

#include "../term/Term.h"

using namespace std;

class Profile;
class RanksNames;
struct VerbalData;
class Completion;
class VerbalCover;
enum Opponent: unsigned;
enum BlankPlayerCap: unsigned;
enum CoverComposition: unsigned;
enum CoverVerbal: unsigned;
enum ExplainEqual: unsigned;


class Product
{
  private:

    Term length;

    vector<Term> tops;

    unsigned long long codeInt;

    unsigned char complexity;

    unsigned char topSize; // Last used top number + 1; may be 0

    unsigned char activeCount; // Number of tops that are used


    ExplainEqual mostlyEqual() const;


    bool singular(
      const Profile& sumProfile,
      const unsigned char canonicalShift) const;


    bool topsSimpler(
      const Profile& sumProfile,
      const unsigned char canonicalShift) const;

    Opponent simpler(
      const Profile& sumProfile,
      const unsigned char canonicalShift) const;


    void study(
      const Profile& sumProfile,
      const unsigned char canonicalShift,
      VerbalData& data) const;

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


    // Simple set methods (no branches)

    void setVerbalLengthOnly(
      const Profile& sumProfile,
      const bool symmFlag,
      VerbalCover& verbalCover) const;

    void setVerbalOneTopOnly(
      const Profile& sumProfile,
      const unsigned char canonicalShift,
      const bool symmFlag,
      VerbalCover& verbalCover) const;

    void setVerbalLengthAndOneTop(
      const Profile& sumProfile,
      const unsigned char canonicalShift,
      const bool symmFlag,
      VerbalCover& verbalCover) const;



    void setVerbalSingular(
      const Profile& sumProfile,
      const bool symmFlag,
      const unsigned char canonicalShift,
      VerbalCover& verbalCover) const;

    // Both high tops and any tops

    string strVerbalTopsOnly(
      const Profile& sumProfile,
      const unsigned char canonicalShift,
      const bool symmFlag,
      const RanksNames& ranksNames,
      const Product& productWest,
      const Product& productEast,
      const VerbalData& dataWest,
      const VerbalData& dataEast,
      const bool flipAllowedFlag) const;


    // Compositions

    void makeCompletion(
      const Profile& sumProfile,
      const unsigned char canonicalShift,
      Completion& completion) const;

    void makeSingularCompletion(
      const Profile& sumProfile,
      const unsigned char canonicalShift,
      const Opponent side,
      Completion& completion) const;

    bool makeCompletionList(
      const Profile& sumProfile,
      const unsigned char canonicalShift,
      const VerbalData& data,
      const unsigned char maxCompletions,
      list<Completion>& completions) const;


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

    string strVerbalHighTopsSide(
      const Profile& sumProfile,
      const RanksNames& ranksNames,
      const Opponent simplestOpponent,
      const bool symmFlag,
      const VerbalData& data,
      const unsigned char canonicalShift) const;

    string strVerbalHighTops(
      const Profile& sumProfile,
      const RanksNames& ranksNames,
      const bool symmFlag,
      const unsigned char canonicalShift) const;

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
