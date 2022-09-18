/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_VERBALTEMPLATES_H
#define SSS_VERBALTEMPLATES_H

#include <vector>
#include <list>
#include <string>

#include "Language.h"

using namespace std;


enum VerbalBlank: unsigned;


enum VERBAL_TEMPLATES
{
  TEMPLATES_LENGTH_ONLY = 0,
  TEMPLATES_SIZE = 1
};


struct VerbalTemplate
{
  string pattern;

  list<VerbalBlank> blanks;
};


class VerbalTemplates
{
  private:

    vector<VerbalTemplate> templates;


  public:

    VerbalTemplates();

    void reset();

    void set(const Language language);

    const VerbalTemplate& get(const unsigned index) const;
};

#endif
