/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

// Despite the file name, this file implements Product methods.
// They are separate as there are so many of them.

#include <cassert>

#include "VerbalTemplates.h"
#include "VerbalBlank.h"


VerbalTemplates::VerbalTemplates()
{
  VerbalTemplates::reset();
}


void VerbalTemplates::reset()
{
  templates.clear();
}


void VerbalTemplates::set(const Language language)
{
  templates.resize(TEMPLATES_SIZE);

  if (language == LANGUAGE_ENGLISH_US)
  {
    templates[TEMPLATES_LENGTH_ONLY] =
      { "%1 %2", { BLANK_PLAYER_UC, BLANK_LENGTH_PHRASE }};
  }
  else if (language == LANGUAGE_GERMAN_DE)
  {
    templates[TEMPLATES_LENGTH_ONLY] =
      { "%1 %2", { BLANK_PLAYER_UC, BLANK_LENGTH_PHRASE }};
  }
  else
    assert(false);
}


const VerbalTemplate& VerbalTemplates::get(const unsigned index) const
{
  assert(index < templates.size());
  return templates[index];
}

