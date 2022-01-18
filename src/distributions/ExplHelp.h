/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_EXPLHELP_H
#define SSS_EXPLHELP_H

enum ExplanationMode
{
  EXPL_LENGTHS_ONLY = 0,
  EXPL_TOPS_ONLY = 1,
  EXPL_LENGTHS_OR_TOPS = 2,
  EXPL_LENGTHS_AND_TOPS = 3,
  EXPL_MODE_SIZE = 4
};

enum ExplanationOperator
{
  EXPL_LESS_EQUAL = 0,
  EXPL_EQUAL = 1,
  EXPL_GREATER_EQUAL = 2,
  EXPL_OPERATOR_SIZE = 3
};

enum ExplanationState
{
  EXPL_DONE = 0,
  EXPL_OPEN = 1,
  EXPL_IMPOSSIBLE = 2,
  EXPL_STATE_SIZE = 3
};

struct ExplanationSpec
{
  ExplanationMode mode;
  unsigned char length;
  unsigned char top;
  ExplanationOperator lengthOper;
  ExplanationOperator topOper;
};

#endif
