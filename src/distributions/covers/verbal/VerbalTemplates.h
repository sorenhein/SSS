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


enum TemplateSentence
{
  TEMPLATES_LENGTH_ONLY = 0,
  TEMPLATES_SIZE = 1
};


struct VerbalTemplate
{
  string pattern;

  list<VerbalBlank> blanks;

  string str() const
  {
    stringstream ss;
    ss << "pattern '" << pattern << "'\n";
    for (auto i: blanks)
    ss << i << " ";
    ss << "\n";
    return ss.str();
  };
};


struct TemplateData
{
  VerbalBlank blank;

  // Each VerbalBlank has a different enum, so we just used unsigned here.
  unsigned instance;

  unsigned numParams;
  unsigned char param1;
  unsigned char param2;

  void setBlank(const VerbalBlank blankIn)
  {
    blank = blankIn;
  };

  void setData(const unsigned instanceIn)
  {
    instance = instanceIn;
    numParams = 0;
  };

  void setData(
    const unsigned instanceIn,
    const unsigned char param1In)
  {
    instance = instanceIn;
    numParams = 1;
    param1 = param1In;
  };

  void setData(
    const unsigned instanceIn,
    const unsigned char param1In,
    const unsigned char param2In)
  {
    instance = instanceIn;
    numParams = 2;
    param1 = param1In;
    param2 = param2In;
  };

  void set(
    const VerbalBlank blankIn,
    const unsigned instanceIn)
  {
    blank = blankIn;
    instance = instanceIn;
    numParams = 0;
  };

  string str() const
  {
    stringstream ss;
    ss << "blank     " << blank << "\n";
    ss << "instance  " << instance << "\n";
    ss << "numParams " << numParams << "\n";
    ss << "param1    " << +param1 << "\n";
    ss << "param2    " << +param2 << "\n";
    return ss.str();
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

    string get(
      const TemplateSentence sentence,
      const vector<TemplateData>& tdata) const;
};

#endif
