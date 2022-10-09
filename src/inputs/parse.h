#ifndef SSS_PARSE_H
#define SSS_PARSE_H

#include <vector>
#include <string>

using namespace std;


void tokenize(
  const string& text,
  vector<string>& tokens,
  const string& delimiters);

bool parseInt(
  const string& text,
  int& value);

bool parseDouble(
  const string& text,
  double& value);

bool parseBool(
  const string& text,
  bool& value);

bool parseQuotedString(
  const string& text,
  string& value);

void parseDelimitedString(
  const string& text,
  const string& delimiter,
  vector<string>& fields);

bool parseBitVector(
  const string& text,
  vector<int>& bitVector,
  const unsigned size);

string parsePath(const string& text);

string parseBasename(const string& text);

#endif
