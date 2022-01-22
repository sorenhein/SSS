/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <cassert>

#include "CoverMemory.h"


CoverMemory::CoverMemory()
{
  CoverMemory::reset();
}


void CoverMemory::reset()
{
  specs.clear();
}


CoverSpec& CoverMemory::add(
  const unsigned char cards,
  const unsigned char tops1)
{
  assert(cards < specs.size());
  assert(tops1 < specs[cards].size());
  specs[cards][tops1].emplace_back(CoverSpec());
  CoverSpec& spec = specs[cards][tops1].back();
  spec.oppsLength = cards;
  spec.oppsTops1 = tops1;
  return spec;
}


void CoverMemory::prepare_2_1()
{
  CoverSpec& spec1 = CoverMemory::add(2, 1);
  spec1.mode = COVER_LENGTHS_ONLY;
  spec1.westLength.set(1, COVER_EQUAL);

  // East void.  Can we say that more idiomatically?
  CoverSpec& spec2 = CoverMemory::add(2, 1);
  spec2.mode = COVER_LENGTHS_ONLY;
  spec2.westLength.set(1, COVER_GREATER_EQUAL);

  CoverSpec& spec3 = CoverMemory::add(2, 1);
  spec3.mode = COVER_TOPS_ONLY;
  spec3.westTop1.set(1, COVER_EQUAL);
}


void CoverMemory::prepare_2_2()
{
  // Exactly 1-1.
  CoverSpec& spec1 = CoverMemory::add(2, 2);
  spec1.mode = COVER_LENGTHS_ONLY;
  spec1.westLength.set(1, COVER_EQUAL);
}


void CoverMemory::prepare_3_1()
{
  // These two together mean "stiff top".  Can we combine?
  CoverSpec& spec1 = CoverMemory::add(3, 1);
  spec1.mode = COVER_LENGTHS_AND_TOPS;
  spec1.westLength.set(1, COVER_EQUAL);
  spec1.westTop1.set(1, COVER_EQUAL);

  CoverSpec& spec2 = CoverMemory::add(3, 1);
  spec2.mode = COVER_LENGTHS_AND_TOPS;
  spec2.westLength.set(2, COVER_EQUAL);
  spec2.westTop1.set(0, COVER_EQUAL);

  // Top with West.
  CoverSpec& spec3 = CoverMemory::add(3, 1);
  spec3.mode = COVER_TOPS_ONLY;
  spec3.westTop1.set(1, COVER_EQUAL);

  // 1-2 cards each.
  CoverSpec& spec4 = CoverMemory::add(3, 1);
  spec4.mode = COVER_LENGTHS_ONLY;
  spec4.westLength.set(1, 2, COVER_INSIDE_RANGE);
}


void CoverMemory::prepare_3_2()
{
  // 1-2 cards each.
  CoverSpec& spec1 = CoverMemory::add(3, 2);
  spec1.mode = COVER_LENGTHS_ONLY;
  spec1.westLength.set(1, 2, COVER_INSIDE_RANGE);
}


void CoverMemory::prepare_3_3()
{
  // 1-2 cards each.
  CoverSpec& spec1 = CoverMemory::add(3, 3);
  spec1.mode = COVER_LENGTHS_ONLY;
  spec1.westLength.set(1, 2, COVER_INSIDE_RANGE);
}


void CoverMemory::prepare_4_1()
{
  // These two together mean "stiff top".  Can we combine?
  CoverSpec& spec1 = CoverMemory::add(4, 1);
  spec1.mode = COVER_LENGTHS_AND_TOPS;
  spec1.westLength.set(1, COVER_EQUAL);
  spec1.westTop1.set(1, COVER_EQUAL);

  CoverSpec& spec2 = CoverMemory::add(4, 1);
  spec2.mode = COVER_LENGTHS_AND_TOPS;
  spec2.westLength.set(3, COVER_EQUAL);
  spec2.westTop1.set(0, COVER_EQUAL);
}


void CoverMemory::prepare_4_2()
{
  // 1-3 cards each.
  CoverSpec& spec1 = CoverMemory::add(4, 2);
  spec1.mode = COVER_LENGTHS_ONLY;
  spec1.westLength.set(1, 3, COVER_INSIDE_RANGE);

  // Exactly 2-2.
  CoverSpec& spec2 = CoverMemory::add(4, 2);
  spec2.mode = COVER_LENGTHS_ONLY;
  spec2.westLength.set(2, COVER_EQUAL);
}


void CoverMemory::prepare_4_3()
{
  // 1-3 cards each.
  CoverSpec& spec1 = CoverMemory::add(4, 3);
  spec1.mode = COVER_LENGTHS_ONLY;
  spec1.westLength.set(1, 3, COVER_INSIDE_RANGE);

  // Exactly 2-2.
  CoverSpec& spec2 = CoverMemory::add(4, 3);
  spec2.mode = COVER_LENGTHS_ONLY;
  spec2.westLength.set(2, COVER_EQUAL);
}


void CoverMemory::prepare_4_4()
{
  // 1-3 cards each.
  CoverSpec& spec1 = CoverMemory::add(4, 4);
  spec1.mode = COVER_LENGTHS_ONLY;
  spec1.westLength.set(1, 3, COVER_INSIDE_RANGE);

  // Exactly 2-2.
  CoverSpec& spec2 = CoverMemory::add(4, 4);
  spec2.mode = COVER_LENGTHS_ONLY;
  spec2.westLength.set(2, COVER_EQUAL);
}


void CoverMemory::prepare_7_1()
{
  CoverSpec& spec1 = CoverMemory::add(7, 1);
  spec1.mode = COVER_LENGTHS_AND_TOPS;
  spec1.westLength.set(4, COVER_GREATER_EQUAL);
  spec1.westTop1.set(0, COVER_EQUAL);

  CoverSpec& spec2 = CoverMemory::add(7, 1);
  spec2.mode = COVER_LENGTHS_AND_TOPS;
  spec2.westLength.set(3, COVER_LESS_EQUAL);
  spec2.westTop1.set(1, COVER_EQUAL);

  CoverSpec& spec3 = CoverMemory::add(7, 1);
  spec3.mode = COVER_LENGTHS_ONLY;
  spec3.westLength.set(7, COVER_EQUAL);

  CoverSpec& spec4 = CoverMemory::add(7, 1);
  spec4.mode = COVER_TOPS_ONLY;
  spec4.westTop1.set(1, COVER_EQUAL);

  CoverSpec& spec5 = CoverMemory::add(7, 1);
  spec5.mode = COVER_LENGTHS_AND_TOPS;
  spec5.westLength.set(6, COVER_EQUAL);
  spec5.westTop1.set(0, COVER_EQUAL);
}



void CoverMemory::prepare(const unsigned char maxCards)
{
  specs.resize(maxCards+1);
  for (unsigned char c = 0; c <= maxCards; c++)
    // Just to have enough for now
    specs[c].resize(14);

  CoverMemory::prepare_2_1();
  CoverMemory::prepare_2_2();

  CoverMemory::prepare_3_1();
  CoverMemory::prepare_3_2();
  CoverMemory::prepare_3_3();

  CoverMemory::prepare_4_1();
  CoverMemory::prepare_4_2();
  CoverMemory::prepare_4_3();
  CoverMemory::prepare_4_4();

  CoverMemory::prepare_7_1();
}


list<CoverSpec>::const_iterator CoverMemory::begin(
  const unsigned cards,
  const unsigned tops1) const
{
  assert(cards < specs.size());
if (tops1 >= specs[cards].size())
{
cout << "cards " << cards << endl;
cout << "tops1 " << tops1 << ", size " << specs[cards].size() << endl;
  assert(tops1 < specs[cards].size());
}

  return specs[cards][tops1].begin();
}


list<CoverSpec>::const_iterator CoverMemory::end(
  const unsigned cards,
  const unsigned tops1) const
{
  assert(cards < specs.size());
  assert(tops1 < specs[cards].size());

  return specs[cards][tops1].end();
}


string CoverMemory::str(
  const unsigned cards,
  const unsigned tops1) const
{
  string s = "";

  for (auto iter = CoverMemory::begin(cards, tops1); 
      iter != CoverMemory::end(cards, tops1); iter++)
  {
    s += iter->strLength() + + " (oper) " + iter->strTop1() + "\n";
  }
    
  return s; 
}


/*

void Covers::prepareSpecific(
  const vector<unsigned char>& lengths,
  const vector<unsigned char>& tops,
  const vector<unsigned char>& cases,
  const unsigned char maxLength,
  const unsigned char maxTops,
  list<Cover>::iterator& iter)
{
  iter = covers.begin();
  CoverSpec spec;

  // This loop does combinations of the form <=, ==, >=.

  for (unsigned char length = 0; length <= maxLength; length++)
  {
    spec.westLength.setValue(length);
    for (unsigned char top = 0; top <= maxTops; top++)
    {
      spec.westTop1.setValue(top);
      for (unsigned mode = 0; mode < COVER_MODE_SIZE; mode++)
      {
        spec.mode = static_cast<CoverMode>(mode);
        for (unsigned lOper = 0; lOper < COVER_OPERATOR_SIZE-1; lOper++)
        {
          if ((length == 0 || length == maxLength) && lOper != COVER_EQUAL)
            continue;

          spec.westLength.setOperator(static_cast<CoverOperator>(lOper));

          for (unsigned tOper = 0; tOper < COVER_OPERATOR_SIZE-1; tOper++)
          {
            if ((top == 0 || top == maxTops) && tOper != COVER_EQUAL)
              continue;

            spec.westTop1.setOperator(static_cast<CoverOperator>(tOper));

            assert(iter != covers.end());
            iter->prepare(lengths, tops, cases, spec);
            iter++;
          }
        }
      }
    }
  }
}


void Covers::prepareMiddles(
  const vector<unsigned char>& lengths,
  const vector<unsigned char>& tops,
  const vector<unsigned char>& cases,
  const unsigned char maxLength,
  const unsigned char maxTops,
  list<Cover>::iterator& iter)
{
  // This loop does more global distributions of the form
  // "4-2 or better".

  CoverSpec spec;
  spec.westLength.setOperator(static_cast<CoverOperator>(COVER_INSIDE_RANGE));

  // With 5 or 6 cards, we run from 1 to 2 as the lower end.
  const unsigned char middleCount = (maxLength-1) >> 1;

  for (unsigned char length = 1; length <= middleCount; length++)
  {
    spec.westLength.setValues(length, maxLength-length);
    for (unsigned char top = 0; top <= maxTops; top++)
    {
      spec.westTop1.setValue(top);
      for (unsigned mode = 0; mode < COVER_MODE_SIZE; mode++)
      {
        spec.mode = static_cast<CoverMode>(mode);
        for (unsigned tOper = 0; tOper < COVER_OPERATOR_SIZE-1; tOper++)
        {
          if ((top == 0 || top == maxTops) && tOper != COVER_EQUAL)
            continue;

          spec.westTop1.setOperator(static_cast<CoverOperator>(tOper));

          assert(iter != covers.end());
          iter->prepare(lengths, tops, cases, spec);
          iter++;
        }
      }
    }
  }
}


void Covers::prepare(
  const vector<unsigned char>& lengths,
  const vector<unsigned char>& tops,
  const vector<unsigned char>& cases,
  const unsigned char maxLength,
  const unsigned char maxTops)
{
  assert(lengths.size() == tops.size());
  assert(maxLength >= 2);
  assert(maxTops >= 1);

  // We consider two kinds of distribution information:
  // (1) <=, ==, >= a specific number of West cards.
  // (2) Both West and East in a certain middle range.
  // 
  // For (1) there are generally 3 operators for each of lengths and tops;
  // 4 ways to combine them; and a number of possible lengths and tops.
  // For the maximum and minimum value of lengths and tops there is
  // only one operator, ==.
  //
  // For (2) there are (maxLength-1) >> 1 interesting splits.
  // For example, for 7 cards there are any 4-3; up to 5-2; up to 6-1,
  // and (7-1) >> 1 is 3.  For 8 cards there are also 3, as 4-4 is
  // already covered by the "exactly 4 West cards" split above.

  const unsigned middleCount = (maxLength-1) >> 1;

  unsigned count1, count2;
  if (maxTops == 1)
  {
    count1 = COVER_MODE_SIZE *
      ((maxLength-1) * 2 * (COVER_OPERATOR_SIZE-1) * 1 + 
      2 * 2 * 1 * 1);
    
    count2 = COVER_MODE_SIZE * middleCount * 2 * 1 * 1;
  }
  else
  {
    count1 = COVER_MODE_SIZE *
      ((maxLength-1) * (maxTops-1) * 
        (COVER_OPERATOR_SIZE-1) * (COVER_OPERATOR_SIZE-1) +
      2 * (maxTops-1) * 1 * (COVER_OPERATOR_SIZE-1) +
      (maxLength-1) * 2 * (COVER_OPERATOR_SIZE-1) * 1 +
      2 * 2 * 1 * 1);

    count2 = COVER_MODE_SIZE * middleCount *
      ((maxTops-1) * 1 * (COVER_OPERATOR_SIZE-1) +
      2 * 1 * 1 );
  }

  const unsigned coverCount = count1 + count2;

  covers.resize(coverCount);

  list<Cover>::iterator iter;
  Covers::prepareSpecific(lengths, tops, cases, maxLength, maxTops, iter);
  Covers::prepareMiddles(lengths, tops, cases, maxLength, maxTops, iter);
  assert(iter == covers.end());

  covers.sort([](const Cover& cover1, const Cover& cover2)
  {
    return (cover1.getWeight() >= cover2.getWeight());
  });
}

*/

