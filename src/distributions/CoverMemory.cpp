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


void CoverMemory::WestIsNotVoid(
  const unsigned char cards,
  const unsigned char tops1)
{
  CoverSpec& spec = CoverMemory::add(cards, tops1);
  spec.mode = COVER_LENGTHS_ONLY;
  spec.westLength.set(1, COVER_GREATER_EQUAL);
}


void CoverMemory::EastIsNotVoid(
  const unsigned char cards,
  const unsigned char tops1)
{
  CoverSpec& spec = CoverMemory::add(cards, tops1);
  spec.mode = COVER_LENGTHS_ONLY;
  spec.westLength.set(cards-1, COVER_LESS_EQUAL);
}


void CoverMemory::EastIsVoid(
  const unsigned char cards,
  const unsigned char tops1)
{
  CoverSpec& spec = CoverMemory::add(cards, tops1);
  spec.mode = COVER_LENGTHS_ONLY;
  spec.westLength.set(cards, COVER_EQUAL);
}


void CoverMemory::WestHasSingleton(
  const unsigned char cards,
  const unsigned char tops1)
{
  CoverSpec& spec = CoverMemory::add(cards, tops1);
  spec.mode = COVER_LENGTHS_ONLY;
  spec.westLength.set(1, COVER_EQUAL);
}


void CoverMemory::WestHasExactlyOneTop(
  const unsigned char cards,
  const unsigned char tops1)
{
  CoverSpec& spec = CoverMemory::add(cards, tops1);
  spec.mode = COVER_TOPS_ONLY;
  spec.westTop1.set(1, COVER_EQUAL);
}


void CoverMemory::EastHasExactlyOneTop(
  const unsigned char cards,
  const unsigned char tops1)
{
  CoverSpec& spec = CoverMemory::add(cards, tops1);
  spec.mode = COVER_TOPS_ONLY;
  spec.westTop1.set(tops1-1, COVER_EQUAL);
}


void CoverMemory::WestHasDoubleton(
  const unsigned char cards,
  const unsigned char tops1)
{
  CoverSpec& spec = CoverMemory::add(cards, tops1);
  spec.mode = COVER_LENGTHS_ONLY;
  spec.westLength.set(2, COVER_EQUAL);
}


void CoverMemory::WestHasTripleton(
  const unsigned char cards,
  const unsigned char tops1)
{
  CoverSpec& spec = CoverMemory::add(cards, tops1);
  spec.mode = COVER_LENGTHS_ONLY;
  spec.westLength.set(3, COVER_EQUAL);
}


void CoverMemory::WestHasCardRange(
  const unsigned char cards,
  const unsigned char tops1,
  const unsigned char lowerIncl,
  const unsigned char upperIncl)
{
  CoverSpec& spec = CoverMemory::add(cards, tops1);
  spec.mode = COVER_LENGTHS_ONLY;
  spec.westLength.set(lowerIncl, upperIncl, COVER_INSIDE_RANGE);
}


void CoverMemory::WestHasShortHonors(
  const unsigned char cards,
  const unsigned char tops1,
  const unsigned char len)
{
  CoverSpec& spec = CoverMemory::add(cards, tops1);
  spec.mode = COVER_LENGTHS_AND_TOPS;
  spec.westLength.set(len, COVER_LESS_EQUAL);
  spec.westTop1.set(tops1, COVER_EQUAL);
}


void CoverMemory::AnHonorIsShort(
  const unsigned char cards,
  const unsigned char tops1,
  const unsigned char len)
{
  // Together these two mean that an honor (there may be more) is with a
  // player with at most "len" cards.

  // West has it.
  CoverSpec& spec1 = CoverMemory::add(cards, tops1);
  spec1.mode = COVER_LENGTHS_AND_TOPS;
  spec1.westLength.set(len, COVER_LESS_EQUAL);
  spec1.westTop1.set(1, COVER_GREATER_EQUAL);

  // East has it.
  CoverSpec& spec2 = CoverMemory::add(cards, tops1);
  spec2.mode = COVER_LENGTHS_AND_TOPS;
  spec2.westLength.set(cards-len, COVER_GREATER_EQUAL);
  spec2.westTop1.set(tops1-1, COVER_LESS_EQUAL);
}


void CoverMemory::HonorsAreShort(
  const unsigned char cards,
  const unsigned char tops1,
  const unsigned char len)
{
  // Together these two mean that the honor (or all honors) is with a
  // player with at most "len" cards.

  // West has it/them.
  CoverSpec& spec1 = CoverMemory::add(cards, tops1);
  spec1.mode = COVER_LENGTHS_AND_TOPS;
  spec1.westLength.set(len, COVER_LESS_EQUAL);
  spec1.westTop1.set(tops1, COVER_EQUAL);

  // East has it/them.
  CoverSpec& spec2 = CoverMemory::add(cards, tops1);
  spec2.mode = COVER_LENGTHS_AND_TOPS;
  spec2.westLength.set(cards-len, COVER_GREATER_EQUAL);
  spec2.westTop1.set(0, COVER_EQUAL);
}


void CoverMemory::prepare_2_1()
{
  CoverMemory::WestHasSingleton(2, 1);       // 1-1
  CoverMemory::WestIsNotVoid(2, 1);          // 1-1 or 2=0
  CoverMemory::WestHasExactlyOneTop(2, 1);
}


void CoverMemory::prepare_2_2()
{
  CoverMemory::WestHasSingleton(2, 2);       // 1-1
}


void CoverMemory::prepare_3_1()
{
  CoverMemory::WestIsNotVoid(3, 1);          // 1=2, 2=1 or 3=0
  CoverMemory::EastIsNotVoid(3, 1);          // 0=3, 1=2 or 2=1
  CoverMemory::WestHasCardRange(3, 1, 1, 2); // 1=2 or 2=1

  CoverMemory::WestHasExactlyOneTop(3, 1);   // Top with West

  CoverMemory::HonorsAreShort(3, 1, 1);      // Stiff H onside + offside

  CoverMemory::WestHasShortHonors(3, 1, 2);  // H or Hx onside
}


void CoverMemory::prepare_3_2()
{
  CoverMemory::WestIsNotVoid(3, 2);          // 1=2, 2=1 or 3=0
  CoverMemory::WestHasCardRange(3, 2, 1, 2); // 1=2 or 2=1
}


void CoverMemory::prepare_3_3()
{
  CoverMemory::WestHasCardRange(3, 3, 1, 2); // 1=2 or 2=1
}


void CoverMemory::prepare_4_1()
{
  CoverMemory::WestIsNotVoid(4, 1);          // 1=3, 2=2, 3=1 or 4=0
  CoverMemory::EastIsNotVoid(4, 1);          // 0=4, 1=3, 2=2 or 3=1
  CoverMemory::WestHasDoubleton(4, 1);       // 2=2
  CoverMemory::WestHasCardRange(4, 1, 1, 3); // 1=3, 2=2 or 3=1.

  CoverMemory::WestHasExactlyOneTop(4, 1);   // Top with West
  CoverMemory::EastHasExactlyOneTop(4, 1);   // Top with East

  CoverMemory::WestHasShortHonors(4, 1, 3);  // Just not East void

  CoverMemory::HonorsAreShort(4, 1, 1);      // Stiff H onside + offside
}


void CoverMemory::prepare_4_2()
{
  CoverMemory::WestHasDoubleton(4, 2);       // 2=2
  CoverMemory::WestHasCardRange(4, 2, 1, 3); // 1=3, 2=2 or 3=1.

  CoverMemory::AnHonorIsShort(4, 2, 1);      // Stiff H onside + offside
}


void CoverMemory::prepare_4_3()
{
  CoverMemory::WestHasDoubleton(4, 3);       // 2=2
  CoverMemory::WestHasCardRange(4, 3, 1, 3); // 1=3, 2=2 or 3=1.
}


void CoverMemory::prepare_4_4()
{
  CoverMemory::WestHasDoubleton(4, 4);       // 2=2
  CoverMemory::WestHasCardRange(4, 4, 1, 3); // 1=3, 2=2 or 3=1.
}


void CoverMemory::prepare_5_1()
{
  CoverMemory::WestHasExactlyOneTop(5, 1);   // Top with West

  CoverMemory::WestHasCardRange(5, 1, 1, 4); // 1-4 cards each
  CoverMemory::WestHasCardRange(5, 1, 2, 3); // 2-3 cards each

  CoverMemory::WestHasShortHonors(5, 1, 2);  // H, Hx onside
  CoverMemory::WestHasShortHonors(5, 1, 3);  // H, Hx, Hxx onside

  CoverMemory::HonorsAreShort(5, 1, 1);      // Stiff H onside + offside
}


void CoverMemory::prepare_5_2()
{
  CoverMemory::WestHasCardRange(5, 2, 1, 4); // 1-4 cards each
  CoverMemory::WestHasCardRange(5, 2, 2, 3); // 2-3 cards each

  CoverMemory::WestHasShortHonors(5, 2, 4);  // Just not East void

  CoverMemory::HonorsAreShort(5, 2, 2);      // Stiff HH onside + offside
}


void CoverMemory::prepare_6_1()
{
  CoverMemory::WestHasExactlyOneTop(6, 1);   // Top with West

  CoverMemory::WestHasTripleton(6, 1);       // 3-3
  CoverMemory::WestHasCardRange(6, 1, 2, 4); // 2-4 cards each
  CoverMemory::WestHasCardRange(6, 1, 1, 5); // 1-5 cards each

  CoverMemory::WestHasShortHonors(6, 1, 2);  // H or Hx onside

  CoverMemory::HonorsAreShort(6, 1, 1);      // Stiff H onside + offside
  CoverMemory::HonorsAreShort(6, 1, 2);      // H or Hx onside + offside
}


void CoverMemory::prepare_6_3()
{
  CoverMemory::WestHasTripleton(6, 3);       // 3-3
}


void CoverMemory::prepare_7_1()
{
  CoverMemory::EastIsVoid(7, 1);             // East is void

  CoverMemory::WestHasExactlyOneTop(7, 1);   // Top with West

  CoverMemory::HonorsAreShort(7, 1, 1);      // Stiff H onside + offside
  CoverMemory::HonorsAreShort(7, 1, 2);      // H at most doubleton
  CoverMemory::HonorsAreShort(7, 1, 3);      // H at most tripleton
}


void CoverMemory::prepare_7_2()
{
  CoverMemory::WestHasCardRange(7, 2, 3, 4); // 3-4 cards each
  CoverMemory::WestHasCardRange(7, 2, 2, 5); // 2-5 cards each
  CoverMemory::WestHasCardRange(7, 2, 1, 6); // 1-6 cards each

  CoverMemory::AnHonorIsShort(7, 2, 1);      // Stiff H onside + offside
  CoverMemory::HonorsAreShort(7, 2, 2);      // H or Hx onside + offside
}


void CoverMemory::prepare_8_1()
{
  CoverMemory::WestHasExactlyOneTop(8, 1);   // H onside

  CoverMemory::EastIsVoid(8, 1);             // East is void

  CoverMemory::HonorsAreShort(8, 1, 1);      // Stiff H onside + offside
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

  CoverMemory::prepare_5_1();
  CoverMemory::prepare_5_2();

  CoverMemory::prepare_6_1();
  CoverMemory::prepare_6_3();

  CoverMemory::prepare_7_1();
  CoverMemory::prepare_7_2();

  CoverMemory::prepare_8_1();
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

