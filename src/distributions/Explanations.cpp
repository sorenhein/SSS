/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Explanations.h"

#include "../strategies/result/Result.h"


Explanations::Explanations()
{
  Explanations::reset();
}


void Explanations::reset()
{
  explanations.clear();
  fits.clear();
}


void Explanations::prepare(
  const vector<unsigned char>& lengths,
  const vector<unsigned char>& tops,
  const unsigned char maxLength,
  const unsigned char maxTops)
{
  assert(lengths.size() == tops.size());
  assert(maxLength >= 1);
  assert(maxTops >= 1);

  const unsigned explCount = EXPL_MODE_SIZE *
    ((maxLength-1) * (maxTops-1) * EXPL_OPERATOR_SIZE * EXPL_OPERATOR_SIZE +
    2 * (maxTops-1) * 1 * EXPL_OPERATOR_SIZE +
    (maxLength-1) * 2 * EXPL_OPERATOR_SIZE * 1 +
    2 * 2 * 1 * 1);

  explanations.resize(explCount);
  auto iter = explanations.begin();
  ExplanationSpec spec;

  for (unsigned char length = 0; length <= maxLength; length++)
  {
    spec.length = length;
    for (unsigned char top = 0; top <= maxTops; top++)
    {
      spec.top = top;
      for (unsigned mode = 0; mode < EXPL_MODE_SIZE; mode++)
      {
        spec.mode = static_cast<ExplanationMode>(mode);
        for (unsigned lOper = 0; lOper < EXPL_OPERATOR_SIZE; lOper++)
        {
          if ((length == 0 || length == maxLength) && lOper != EXPL_EQUAL)
            continue;

          spec.lengthOper = static_cast<ExplanationOperator>(lOper);

          for (unsigned tOper = 0; tOper < EXPL_OPERATOR_SIZE; tOper++)
          {
            if ((top == 0 || top == maxTops) && tOper != EXPL_EQUAL)
              continue;

            spec.topOper = static_cast<ExplanationOperator>(tOper);

            assert(iter != explanations.end());
            Explanation& expl = * iter;

            expl.prepare(lengths, tops, spec);
          }
        }
      }
    }
  }

  explanations.sort([](const Explanation& expl1, const Explanation& expl2)
  {
    return (expl1.getWeight() >= expl2.getWeight());
  });
}


ExplanationState Explanations::explain(const vector<Result>& results)
{
  ExplanationState state = EXPL_OPEN;
  auto iter = explanations.begin();

  vector<unsigned char> tricks(results.size());
  for (unsigned i = 0; i < results.size(); i++)
    tricks[i] = results[i].getTricks();

  while (true)
  {
    if (iter == explanations.end())
      return EXPL_IMPOSSIBLE;

    state = iter->explain(tricks);

    if (state == EXPL_DONE)
    {
      fits.push_back(&* iter);
      return EXPL_DONE;
    }
    else if (state == EXPL_OPEN)
    {
      fits.push_back(&* iter);
      continue;
    }
    else
      iter++;
  }

  // Can't happen
  return EXPL_STATE_SIZE;
}


string Explanations::str() const
{
  string s;
  for (auto& eptr: fits)
  {
    s += eptr->str() + "\n";
  }
  return s;
}

