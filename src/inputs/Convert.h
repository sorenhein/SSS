#ifndef SSS_CONVERT_H
#define SSS_CONVERT_H

#include <string>


using namespace std;


class Convert
{
  private:


  public:

    Convert();

    ~Convert();

    void reset();

    bool holding2cards(
      const unsigned holding,
      const unsigned cards,
      string& north,
      string& south);

    bool cards2holding(
      const string& north,
      const string& south,
      const unsigned cards,
      unsigned& holding);
};

#endif
