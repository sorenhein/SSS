/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <cassert>

#include "Phrase.h"
#include "Completion.h"

#include "../../../languages/Dictionary.h"
#include "../../../languages/connections/words.h"

#include "../../../languages/PhraseExpansion.h"

#include "../../../ranks/RanksNames.h"

extern Dictionary dictionary;


Phrase::Phrase()
{
  numOpp = 0;
  numUchars = 0;
  numBools = 0;

  side = static_cast<Opponent>(0);
  uchars.clear();
  bools.clear();
}


void Phrase::setPhrase(const PhrasesEnum phraseIn)
{
  phrase = phraseIn;
}


void Phrase::setSide(const Opponent sideIn)
{
  numOpp = 1;
  side = sideIn;
}


void Phrase::setValues(const unsigned char value1)
{
  numUchars = 1;
  uchars.resize(1);
  uchars[0] = value1;
}


void Phrase::setValues(
  const unsigned char value1,
  const unsigned char value2)
{
  numUchars = 2;
  uchars.resize(2);
  uchars[0] = value1;
  uchars[1] = value2;
}


void Phrase::setValues(
  const unsigned char value1,
  const unsigned char value2,
  const unsigned char value3)
{
  numUchars = 3;
  uchars.resize(3);
  uchars[0] = value1;
  uchars[1] = value2;
  uchars[2] = value3;
}


void Phrase::setBools(const bool bool1)
{
  numBools = 1;
  bools.resize(1);
  bools[0] = bool1;
}


PhrasesEnum Phrase::getPhrase() const
{
  return phrase;
}


bool Phrase::has(
  const unsigned char actOpp,
  const unsigned char actUchars,
  const unsigned char actBools) const
{
  return (numOpp == actOpp &&
    numUchars == actUchars &&
    numBools == actBools);
}


void Phrase::replace(
  string& s,
  const string& percent,
  const string& repl) const
{
  auto p = s.find(percent);
  assert(p != string::npos);
  s.replace(p, 2, repl);
}


void Phrase::replace(
  string& s,
  const string& percent,
  const unsigned char uchar) const
{
  Phrase::replace(s, percent, to_string(uchar));
}


void Phrase::replace(
  string& s,
  const unsigned char field,
  const string& repl) const
{
  Phrase::replace(s, "%" + to_string(field), repl);
}


void Phrase::replace(
  string& s,
  const unsigned char field,
  const unsigned char uchar) const
{
  Phrase::replace(s, "%" + to_string(field), to_string(uchar));
}


string Phrase::str(
  const PhraseExpansion expansion,
  const string& text,
  const RanksNames& ranksNames,
  const Completion& completion) const
{
  string s = text;

  if (expansion == PHRASE_NONE)
  {
    assert(Phrase::has(0, 0, 0));
    return s;
  }
  else if (expansion == PHRASE_NUMERICAL)
  {
    assert(Phrase::has(0, 1, 0) || Phrase::has(0, 2, 0));

    for (unsigned char field = 0; field < numUchars; field++)
      Phrase::replace(s, field, dictionary.numerals.get(uchars[field]).text);

    return s;
  }
  else if (expansion == PHRASE_ORDINAL)
  {
    assert(Phrase::has(0, 1, 0) || Phrase::has(0, 2, 0));

    for (unsigned char field = 0; field < numUchars; field++)
      Phrase::replace(s, field, dictionary.ordinals.get(uchars[field]).text);

    return s;
  }
  else if (expansion == PHRASE_RANKS)
  {
    assert(Phrase::has(0, 1, 0));
    Phrase::replace(s, "%0", ranksNames.lowestCard(uchars[0]));
    return s;
  }
  else if (expansion == PHRASE_TEXT_LOWER)
  {
    assert(Phrase::has(0, 1, 0));
    Phrase::replace(s, "%0", dictionary.words.get(
      uchars[0] == 1 ? WORDS_CARD : WORDS_CARDS).text);
    return s;
  }
  else if (expansion == PHRASE_TEXT_BELOW)
  {
    // If we permit full card names here, they should be in dative
    // (in German, and it doesn't hurt in English and Danish).
    // I don't have a good general mechanism for this.
    assert(Phrase::has(0, 2, 0));
    Phrase::replace(s, "%0", dictionary.words.get(
      uchars[0] == 1 ? WORDS_CARD : WORDS_CARDS).text);
    Phrase::replace(s, "%1", ranksNames.lowestCard(uchars[1]));
    return s;
  }
  else if (expansion == PHRASE_ADJACENT)
  {
    // Dative.
    assert(Phrase::has(0, 3, 0));
    Phrase::replace(s, "%0", dictionary.numerals.get(uchars[0]).text);
    Phrase::replace(s, "%1", dictionary.numerals.get(uchars[1]).text);
    Phrase::replace(s, "%2", ranksNames.strComponent(
      RANKNAME_ACTUAL_FULL, uchars[2], uchars[1] > 1));
    return s;
  }
  else if (expansion == PHRASE_RANGE_OF)
  {
    // Dative.
    assert(Phrase::has(0, 3, 0));
    Phrase::replace(s, "%0", uchars[0]);
    Phrase::replace(s, "%1", uchars[1]);
    Phrase::replace(s, "%2", ranksNames.strComponent(
      RANKNAME_ACTUAL_FULL, uchars[2], uchars[1] > 1));
    return s;
  }
  else if (expansion == PHRASE_SOME_OF)
  {
    // Dative.
    assert(Phrase::has(0, 2, 0));
    Phrase::replace(s, "%0", dictionary.numerals.get(uchars[0]).text);
    Phrase::replace(s, "%1", ranksNames.strComponent(
      RANKNAME_ACTUAL_FULL, uchars[1], uchars[0] > 1));
    return s;
  }
  else if (expansion == PHRASE_SOME_RANK_SET)
  {
    assert(Phrase::has(0, 2, 0));
    Phrase::replace(s, "%0", dictionary.numerals.get(uchars[0]).text);
    Phrase::replace(s, "%1", ranksNames.strComponent(
      RANKNAME_ACTUAL_FULL, uchars[1], uchars[0] > 1));
    return s;
  }
  else if (expansion == PHRASE_FULL_RANK_SET)
  {
    assert(Phrase::has(0, 1, 0));
    Phrase::replace(s, "%0", ranksNames.getOpponents(uchars[0]).
      strComponent(RANKNAME_ACTUAL_FULL_DEF));
    return s;
  }
  else if (expansion == PHRASE_COMPLETION_SET)
  {
    assert(Phrase::has(1, 0, 0) || Phrase::has(1, 0, 1));

    if (numBools == 0)
      Phrase::replace(s, "%0", completion.strSet(ranksNames, side));
    else
      Phrase::replace(s, "%0", completion.strSet(ranksNames, side, bools[0]));

    return s;
  }
  else if (expansion == PHRASE_COMPLETION_BOTH)
  {
    assert(Phrase::has(1, 0, 1));

    Phrase::replace(s, "%0", completion.strSet(ranksNames, side, bools[0]));

    Phrase::replace(s, "%1", completion.strUnset(ranksNames, side));

    return s;
  }
  else if (expansion == PHRASE_COMPLETION_XES)
  {
    assert(Phrase::has(1, 0, 0));
    Phrase::replace(s, "%0", completion.strXes(side));
    return s;
  }
  else
  {
    assert(false);
    return "";
  }
}
