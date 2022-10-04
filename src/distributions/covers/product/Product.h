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
struct VerbalSide;
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


    // Pverbal: Side comparators

    bool topsSimpler(
      const Profile& sumProfile,
      const unsigned char canonicalShift) const;

    Opponent simpler(
      const Profile& sumProfile,
      const unsigned char canonicalShift) const;


    // Pverbal: Completion methods 

    void makeCompletionBottoms(
      const Profile& sumProfile,
      const unsigned char canonicalShift,
      Completion& completion) const;

    void makeCompletion(
      const Profile& sumProfile,
      const unsigned char canonicalShift,
      Completion& completion) const;

    Opponent singularUnusedSide(
      const Profile& sumProfile,
      const unsigned char canonicalShift,
      const Completion& completion) const;

    void makeSingularCompletionBottoms(
      const Profile& sumProfile,
      const unsigned char canonicalShift,
      const Opponent side,
      Completion& completion) const;

    void makeSingularCompletion(
      const Profile& sumProfile,
      const unsigned char canonicalShift,
      const Opponent side,
      Completion& completion) const;

    bool makeCompletionList(
      const Profile& sumProfile,
      const unsigned char canonicalShift,
      const Opponent side,
      const unsigned char maxCompletions,
      list<Completion>& completions) const;


    // Pverbal: Set methods

    void setVerbalDisaster(
      const Profile& sumProfile,
      const unsigned char canonicalShift,
      const bool symmFlag,
      const RanksNames& ranksNames,
      VerbalCover& verbalCover) const;

    void setVerbalLengthOnly(
      const Profile& sumProfile,
      const unsigned char canonicalShift,
      const bool symmFlag,
      const RanksNames& ranksNames,
      VerbalCover& verbalCover) const;

    void setVerbalOneTopOnly(
      const Profile& sumProfile,
      const unsigned char canonicalShift,
      const bool symmFlag,
      const RanksNames& ranksNames,
      VerbalCover& verbalCover) const;

    void setVerbalLengthAndOneTop(
      const Profile& sumProfile,
      const unsigned char canonicalShift,
      const bool symmFlag,
      const RanksNames& ranksNames,
      VerbalCover& verbalCover) const;

    // Help method for both high tops and any tops

    void setVerbalTopsOnly(
      const Profile& sumProfile,
      const unsigned char canonicalShift,
      const bool symmFlag,
      const RanksNames& ranksNames,
      const bool flipAllowedFlag,
      VerbalCover& verbalCover) const;

    void setVerbalHighTopsEqual(
      const Profile& sumProfile,
      const unsigned char canonicalShift,
      const bool symmFlag,
      const RanksNames& ranksNames,
      VerbalCover& verbalCover) const;

    void setVerbalAnyTopsEqual(
      const Profile& sumProfile,
      const unsigned char canonicalShift,
      const bool symmFlag,
      const RanksNames& ranksNames,
      VerbalCover& verbalCover) const;

    void setVerbalSingular(
      const Profile& sumProfile,
      const unsigned char canonicalShift,
      const bool symmFlag,
      const RanksNames& ranksNames,
      VerbalCover& verbalCover) const;


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
      const unsigned char canonicalShift,
      const bool symmFlag,
      const RanksNames& ranksNames,
      const CoverVerbal verbal) const;

};

#endif
