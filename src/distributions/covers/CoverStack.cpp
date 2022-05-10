/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <mutex>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "CoverStack.h"

mutex mtxCoverStack;


template<class C>
CoverStack<C>::CoverStack()
{
  CoverStack::reset();
}


template<class C>
void CoverStack<C>::reset()
{
  stack.clear();
}


template class CoverStack<Cover>;
template class CoverStack<CoverRow>;

