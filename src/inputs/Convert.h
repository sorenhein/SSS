#ifndef SSS_CONVERT_H
#define SSS_CONVERT_H

#include <map>
#include <list>
#include <vector>
#include <string>

#include "../const.h"


using namespace std;


class Convert
{
  private:


  public:

    Convert();

    ~Convert();

    void reset();

    bool holding2cards(
      const int holding,
      const int cards,
      string& north,
      string& south);

    bool cards2holding(
      const string& north,
      const string& south,
      const int cards,
      int& holding);
};

#endif
