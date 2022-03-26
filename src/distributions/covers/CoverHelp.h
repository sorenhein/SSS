#ifndef SSS_COVERHELP_H
#define SSS_COVERHELP_H

using namespace std;


enum CoverOperator
{
  COVER_EQUAL = 0,
  COVER_INSIDE_RANGE = 1,
  COVER_GREATER_EQUAL = 2,
  COVER_LESS_EQUAL = 3,
  COVER_OPERATOR_SIZE = 4
};

enum CoverState
{
  COVER_DONE = 0,
  COVER_OPEN = 1,
  COVER_IMPOSSIBLE = 2,
  COVER_STATE_SIZE = 3
};

#endif
