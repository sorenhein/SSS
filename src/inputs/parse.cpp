#include <algorithm>

#include "parse.h"


unsigned countDelimiters(
  const string& text,
  const string& delimiters);


void tokenize(
  const string& text,
  vector<string>& tokens,
  const string& delimiters)
{
  // tokenize splits a string into tokens separated by delimiter.
  // http://stackoverflow.com/questions/236129/split-a-string-in-c

  string::size_type pos, lastPos = 0;

  while (true)
  {
    pos = text.find_first_of(delimiters, lastPos);
    if (pos == std::string::npos)
    {
      pos = text.length();
      tokens.push_back(string(text.data()+lastPos,
        static_cast<string::size_type>(pos - lastPos)));
      break;
    }
    else
    {
      tokens.push_back(string(text.data()+lastPos,
        static_cast<string::size_type>(pos - lastPos)));
    }
    lastPos = pos + 1;
  }
}


unsigned countDelimiters(
  const string& text,
  const string& delimiters)
{
  int c = 0;
  for (unsigned i = 0; i < delimiters.length(); i++)
    c += static_cast<int>
      (count(text.begin(), text.end(), delimiters.at(i)));
  return static_cast<unsigned>(c);
}


bool parseInt(
  const string& text,
  int& value)
{
  if (text == "")
    return false;

  int i;
  size_t pos;
  try
  {
    i = stoi(text, &pos, 0);
    if (pos != text.size())
      return false;
  }
  catch (...)
  {
    return false;
  }

  value = i;
  return true;
}


bool parseDouble(
  const string& text,
  double& value)
{
  if (text == "")
    return false;

  double f;
  size_t pos;
  try
  {
    f = stod(text, &pos);
    if (pos != text.size())
      return false;
  }
  catch (...)
  {
    return false;
  }

  value = f;
  return true;
}


bool parseBool(
  const string& text,
  bool& value)
{
  if (text == "yes")
  {
    value = true;
    return true;
  }
  else if (text == "no")
  {
    value = false;
    return true;
  }
  else
    return false;
}


bool parseQuotedString(
  const string& text,
  string& value)
{
  // Not much error checking here.
  const auto sp1 = text.find("\"");
  if (sp1 == string::npos)
    return false;

  const auto sp2 = text.find("\"", sp1+1);
  if (sp2 == string::npos)
    return false;

  value = text.substr(sp1+1, sp2-sp1-1);
  return true;
}


void parseDelimitedString(
  const string& text,
  const string& delimiter,
  vector<string>& fields)
{
  const size_t c = countDelimiters(text, delimiter);
  fields.resize(c+1);
  fields.clear();
  tokenize(text, fields, delimiter);
}


bool parseBitVector(
  const string& text,
  vector<int>& bitVector,
  const unsigned size)
{
  int v;
  if (! parseInt(text, v))
    return false;

  unsigned i;
  int b;
  for (i = 0, b = 1; i < size; i++, b <<= 1)
   bitVector[i] = ((v & b) != 0);
  return true;
}


string parsePath(const string& text)
{
  if (text == "")
    return "";

  const auto p = text.find_last_of('/');
  if (p == string::npos || p == 0 || p+1 == text.size())
   return text;

  return text.substr(0, p+1);
}


string parseBasename(const string& text)
{
  if (text == "")
    return "";

  const auto p = text.find_last_of('/');
  if (p == string::npos || p+1 == text.size())
   return text;

  return text.substr(p+1);
}

