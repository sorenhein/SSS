#ifndef SSS_COVEROPERATOR_H
#define SSS_COVEROPERATOR_H

using namespace std;


enum CoverOperator: unsigned
{
  COVER_EQUAL = 0,
  COVER_INSIDE_RANGE = 1,
  COVER_GREATER_EQUAL = 2,
  COVER_LESS_EQUAL = 3,
  COVER_OPERATOR_SIZE = 4
};

#endif
