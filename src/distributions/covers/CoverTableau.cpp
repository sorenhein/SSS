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

// TMP
extern unsigned numCompare, numStack, numSolutions;
extern unsigned numCompareManual, numStackManual, numSolutionsManual;


CoverTableau::CoverTableau()
{
  CoverTableau::reset();
}


void CoverTableau::reset()
{
  rows.clear();
  tricksMin = 0;

  residuals.clear();
  residualWeight = 0;

  complexity = 0;
  maxComplexity = 0;
}


void CoverTableau::init(
  const Tricks& tricks,
  const unsigned char tmin,
  const vector<unsigned char>& cases)
{
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

  // const unsigned char complexity = CoverTableau::getComplexity();

numCompare++;
  // First try to add a new row.
  if (cover.possible(explained, residuals, cases, additions, weightAdded))
  {
    stack.emplace_back(StackEntry());
    StackEntry& centry = stack.back();

numStack++;
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
 numSolutions++;
      solutions.push_back(tableau);
      // Done, so eliminate.
      stack.pop_back();

      CoverTableau& solution = solutions.back();
      solution.complexity += row.getComplexity();
      if (row.getComplexity() > solution.maxComplexity)
        solution.maxComplexity = row.getComplexity();

      if (complexity + row.getComplexity() < lowestComplexity)
        lowestComplexity = complexity + row.getComplexity();
    }
    else
    {
      tableau.complexity += row.getComplexity();
      if (row.getComplexity() > tableau.maxComplexity)
        tableau.maxComplexity = row.getComplexity();
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
numStack++;

      CoverTableau& tableau = centry.tableau;
      tableau = * this;

      // A bit fumbly: Advance to the same place in tableau.
      for (riter = tableau.rows.begin(), r = 0; r < rno; riter++, r++);

      riter->add(cover, additions, cases,
        tableau.residuals, tableau.residualWeight);

      centry.coverIter = coverIter;

      if (tableau.complete())
      {
numSolutions++;

        solutions.push_back(tableau);

        CoverTableau& solution = solutions.back();
        solution.complexity += cover.getComplexity();
        if (riter->getComplexity() > solution.maxComplexity)
          solution.maxComplexity = riter->getComplexity();

        // const unsigned char c = tableau.getComplexity();
        const unsigned char c = solution.getComplexity();

        if (c < lowestComplexity)
          lowestComplexity = c;
        // Done, so eliminate.
        stack.pop_back();
      }
      else
      {
        tableau.complexity += cover.getComplexity();
        if (riter->getComplexity() > tableau.maxComplexity)
          tableau.maxComplexity = riter->getComplexity();
      }
    }
    rno++;
  }
}


bool CoverTableau::attemptManually(
  const vector<unsigned char>& cases,
  list<CoverRow>::const_iterator& rowIter,
  list<RowStackEntry>& stack,
  CoverTableau& solution,
  [[maybe_unused]] unsigned char& lowestComplexity) const
{
  // Return true if a solution is found.

  Tricks additions;
  additions.resize(residuals.size());
  unsigned char weightAdded;

numCompareManual++;
  if (! rowIter->possible(residuals, cases, additions, weightAdded))
    return false;

  if (weightAdded == residualWeight)
  {
numSolutionsManual++;
    // Done, so we have a solution.

    if (solution.rows.empty())
    {
      solution = * this;
      solution.rows.push_back(* rowIter);

      solution.complexity += rowIter->getComplexity();
      lowestComplexity = solution.complexity;
      if (rowIter->getComplexity() > solution.maxComplexity)
        solution.maxComplexity = rowIter->getComplexity();
    }
    else
    {
      CoverTableau tmp = * this;

      tmp.rows.push_back(* rowIter);

      tmp.complexity += rowIter->getComplexity();
      if (rowIter->getComplexity() > tmp.maxComplexity)
        tmp.maxComplexity = rowIter->getComplexity();

      if (tmp < solution)
      {
        solution = tmp;
        lowestComplexity = solution.complexity;
      }
    }
    
    return true;
  }
  else
  {
  numStackManual++;
  // TODO Could not push if new complexity is >= solution
    stack.emplace_back(RowStackEntry());
    RowStackEntry& rentry = stack.back();

    CoverTableau& tableau = rentry.tableau;
    tableau = * this;
    tableau.rows.push_back(* rowIter);

    tableau.residuals -= additions;
    tableau.residualWeight -= weightAdded;

    rentry.rowIter = rowIter;

    tableau.complexity += rowIter->getComplexity();
    if (rowIter->getComplexity() > tableau.maxComplexity)
      tableau.maxComplexity = rowIter->getComplexity();

    return false;
  }
}


unsigned char CoverTableau::complexityHeadroom(
  const CoverTableau& solution) const
{
  if (solution.complexity == 0)
    return numeric_limits<unsigned char>::max();
  else if (complexity >= solution.complexity)
    return 0;
  else
    return solution.complexity - complexity;
}


void CoverTableau::updateStats(
  const Profile& sumProfile,
  ProductStats& productStats,
  const bool newTableauFlag) const
{
  for (auto& row: rows)
    row.updateStats(productStats, sumProfile, newTableauFlag);
}


bool CoverTableau::operator < (const CoverTableau& tableau2) const
{
  if (complexity < tableau2.complexity)
    return true;
  else if (complexity > tableau2.complexity)
    return false;
  else
    return (maxComplexity < tableau2.maxComplexity);
}


bool CoverTableau::complete() const
{
  return (! rows.empty() && residualWeight == 0);
}


unsigned char CoverTableau::getComplexity() const
{
  return complexity;
}


unsigned char CoverTableau::maxRowComplexity() const
{
  return maxComplexity;
}


unsigned char CoverTableau::getResidualWeight() const
{
  return residualWeight;
}


string CoverTableau::strBracket() const
{
  unsigned weight = 0;

  for (auto& row: rows)
    weight += row.getWeight();

  stringstream ss;
  ss << 
    "[c " << +complexity << 
    "/" << +maxComplexity <<
    ", w " << weight << "]";
  return ss.str();
}


string CoverTableau::strResiduals() const
{
  return residuals.strSpaced();
}


string CoverTableau::str(const Profile& sumProfile) const
{
  stringstream ss;

  ss << "Always take at least " << +tricksMin << " tricks, and more when\n";
  for (auto& row: rows)
    ss << row.str(sumProfile);

  return ss.str() + "\n";
}

