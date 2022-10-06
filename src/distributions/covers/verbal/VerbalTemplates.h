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
#include "Completion.h"
#include "VerbalBlank.h"

using namespace std;

class RanksNames;
class Slot;

enum VerbalBlank: unsigned;


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
  unsigned char param3;
  string text1;
  string text2;
  string text3;

  bool completionFlag;
  Completion completion;

  void setBlank(const VerbalBlank blankIn)
  {
    blank = blankIn;
  };

  void setCompletion(
    const unsigned instanceIn,
    const Completion& completionIn)
  {
    instance = instanceIn;
    numParams = 1;
    completionFlag = true;
    completion = completionIn;
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
    const string& textIn)
  {
    instance = instanceIn;
    numParams = 1;
    text1 = textIn;
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

  void setData(
    const unsigned instanceIn,
    const unsigned char& paramIn1,
    const unsigned char& paramIn2,
    const unsigned char& paramIn3)
  {
    instance = instanceIn;
    numParams = 3;
    param1 = paramIn1;
    param2 = paramIn2;
    param3 = paramIn3;
  };

  void setData(
    const unsigned instanceIn,
    const string& textIn1,
    const string& textIn2,
    const string& textIn3)
  {
    instance = instanceIn;
    numParams = 3;
    text1 = textIn1;
    text2 = textIn2;
    text3 = textIn3;
  };

  void setData(
    const unsigned instanceIn,
    const string& textIn1,
    const string& textIn2)
  {
    instance = instanceIn;
    numParams = 2;
    text1 = textIn1;
    text2 = textIn2;
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
    ss << "text1     " << text1 << "\n";
    ss << "text2     " << text2 << "\n";
    return ss.str();
  };
};


class VerbalTemplates
{
  private:

    vector<VerbalTemplate> templates;

    vector<vector<string>> dictionary;

    Language language;


    string listPhrase(const TemplateData& tdata) const;


  public:

    VerbalTemplates();

    void reset();

    void set(const Language languageIn);

    string get(
      const Sentence sentence,
      const RanksNames& ranksNames,
      const list<Completion>& completions,
      const vector<TemplateData>& tdata,
      const vector<Slot>& slots) const;
};

#endif
