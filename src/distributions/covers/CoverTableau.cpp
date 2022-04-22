/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "CoverTableau.h"
#include "Cover.h"

#include "../../strategies/result/Result.h"


CoverTableau::CoverTableau()
{
  CoverTableau::reset();
}


void CoverTableau::reset()
{
  rows.clear();
  residuals.clear();
  residualWeight = 0;
  tricksMin = 0;
}


void CoverTableau::init(
  const Profile& sumProfileIn,
  const Tricks& tricks,
  const unsigned char tmin,
  const vector<unsigned char>& cases)
{
  sumProfile = sumProfileIn;
  residuals.set(tricks, cases, residualWeight);
  tricksMin = tmin;
}


void CoverTableau::setMinTricks(const unsigned char tmin)
{
  tricksMin = tmin;
}


void CoverTableau::attempt(
  const vector<unsigned char>& cases,
  set<Cover>::const_iterator& coverIter,
  list<StackEntry>& stack,
  list<CoverTableau>& solutions,
  unsigned char& lowestComplexity) const
{
  const Cover& cover = * coverIter;

  // explained is a dummy vector here.
  Tricks explained;
  explained.resize(cover.size());

  Tricks additions;
  additions.resize(cover.size());

  unsigned char weightAdded;
  const bool emptyStartFlag = rows.empty();

  const unsigned char complexity = CoverTableau::getComplexity();
  // Keep some extras to inspect for now.
  // TODO This doesn't actually limit the final list to this range,
  // as the complexity may shrink over time.

  if (complexity + cover.getComplexity() -2 > lowestComplexity)
  {
    // Too complex.
// cout << "XXX\n";
    return;
  }

  // First try to add a new row.
  if (cover.possible(explained, residuals, cases, additions, weightAdded))
  {
    stack.emplace_back(StackEntry());
    StackEntry& centry = stack.back();

    CoverTableau& tableau = centry.tableau;
    tableau = * this;
    tableau.rows.emplace_back(CoverRow());
    CoverRow& row = tableau.rows.back();
    row.resize(cover.size());
    row.add(cover, additions, cases, 
      tableau.residuals, tableau.residualWeight);

   centry.coverIter = coverIter;

    if (tableau.complete())
    {
      solutions.push_back(tableau);
      // Done, so eliminate.
      stack.pop_back();

      if (complexity + cover.getComplexity() < lowestComplexity)
        lowestComplexity = complexity + cover.getComplexity();
    }
  }

  // Then look for a row, or the best one, to add to.
  if (emptyStartFlag)
    return;

  unsigned rno = 0, r;
  list<CoverRow>::iterator riter;

  for (auto& row: rows)
  {
    if (row.size() < 2 &&
        row.attempt(cover, residuals, cases, additions, weightAdded) &&
        weightAdded < cover.getWeight())
    {
      // Don't want cover to be completely complementary (use new row).
      // Also don't want more than two or's in a row.

      stack.emplace_back(StackEntry());
      StackEntry& centry = stack.back();

      CoverTableau& tableau = centry.tableau;
      tableau = * this;

      // A bit fumbly: Advance to the same place in tableau.
      for (riter = tableau.rows.begin(), r = 0; r < rno; riter++, r++);

      riter->add(cover, additions, cases,
        tableau.residuals, tableau.residualWeight);

      centry.coverIter = coverIter;

      if (tableau.complete())
      {
        const unsigned char c = tableau.getComplexity();

        if (c < lowestComplexity)
          lowestComplexity = c;
        solutions.push_back(tableau);
        // Done, so eliminate.
        stack.pop_back();

      }
    }
    rno++;
  }
}


void CoverTableau::attemptManually(
  const vector<unsigned char>& cases,
  list<CoverRow>::const_iterator& rowIter,
  list<RowStackEntry>& stack,
  list<CoverTableau>& solutions,
  unsigned char& lowestComplexity) const
{
  const CoverRow& row = * rowIter;

  // explained is a dummy vector here.
  Tricks explained;
  explained.resize(residuals.size());

  Tricks additions;
  additions.resize(residuals.size());

  unsigned char weightAdded;

  const unsigned char complexity = CoverTableau::getComplexity();

  if (complexity + row.getComplexity() - 2 > lowestComplexity)
  {
    // Too complex.
    // TODO Can avoid this test if using micro-complexity per weight?
    return;
  }

  // Try to add a new row.
  if (row.possible(explained, residuals, cases, additions, weightAdded))
  {
    stack.emplace_back(RowStackEntry());
    RowStackEntry& rentry = stack.back();

    CoverTableau& tableau = rentry.tableau;
    tableau = * this;
    tableau.rows.push_back(row);

    row.subtract(additions, cases,
      tableau.residuals, tableau.residualWeight);

    rentry.rowIter = rowIter;

    if (tableau.complete())
    {
      solutions.push_back(tableau);
      // Done, so eliminate.
      // TODO This is the element we've just created.  Can we tell
      // from weightAdded and the tableau weight that we're done,
      // so we can shortcut this?
      stack.pop_back();

      // TODO Is this already tableau.getComplexity()?
      if (complexity + row.getComplexity() < lowestComplexity)
        lowestComplexity = complexity + row.getComplexity();
    }
  }
}


void CoverTableau::updateStats(
  ProductStats& productStats,
  const bool newTableauFlag) const
{
  for (auto& row: rows)
    row.updateStats(productStats, sumProfile, newTableauFlag);
}


bool CoverTableau::operator < (const CoverTableau& tableau2) const
{
  const unsigned char compl1 = CoverTableau::getComplexity();
  const unsigned char compl2 = tableau2.getComplexity();

  if (compl1 < compl2)
    return true;
  else if (compl1 > compl2)
    return false;

  // TODO Is there more we can do here in case of equality?
  // Maybe we like symmetry?
  // Do we even prefer fewer rows vs cleaner solution? Other way round?
  return (rows.size() < tableau2.rows.size());
}


bool CoverTableau::complete() const
{
  return (! rows.empty() && residualWeight == 0);
}


unsigned char CoverTableau::getComplexity() const
{
  unsigned char complexity = 0;
  for (auto& row: rows)
  {
    // TODO TEMP
    assert(complexity + row.getComplexity() > complexity);

    complexity += row.getComplexity();
  }
  return complexity;
}


unsigned CoverTableau::getWeight() const
{
  unsigned weight = 0;

  for (auto& row: rows)
    weight += row.getWeight();

  return weight;
}


unsigned char CoverTableau::getResidualWeight() const
{
  return residualWeight;
}


string CoverTableau::strResiduals() const
{
  return residuals.strSpaced();
}


string CoverTableau::str() const
{
  stringstream ss;

  ss << "Always take at least " << +tricksMin << " tricks, and more when\n";
  for (auto& row: rows)
    ss << row.str(sumProfile);

  return ss.str() + "\n";
}

