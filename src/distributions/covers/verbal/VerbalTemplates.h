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


struct TemplateData
{
  VerbalBlank blank;

  unsigned numParams;
  unsigned char param1;
  unsigned char param2;

  void set(const VerbalBlank blankIn)
  {
    blank = blankIn;
    numParams = 0;
  };

  void set(
    const VerbalBlank blankIn,
    const unsigned char param1In)
  {
    blank = blankIn;
    numParams = 1;
    param1 = param1In;
  };

  void set(
    const VerbalBlank blankIn,
    const unsigned char param1In,
    const unsigned char param2In)
  {
    blank = blankIn;
    numParams = 2;
    param1 = param1In;
    param2 = param2In;
  };
};


class VerbalTemplates
{
  private:

    vector<VerbalTemplate> templates;

    vector<vector<string>> dictionary;

    Language language;


    string playerCap(const TemplateData& tdata) const;

    string lengthPhrase(const TemplateData& tdata) const;


  public:

    VerbalTemplates();

    void reset();

    void set(const Language languageIn);

    string get(const TemplateData& tdata) const;
};

#endif
