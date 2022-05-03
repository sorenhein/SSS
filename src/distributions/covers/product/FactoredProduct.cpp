/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "FactoredProduct.h"
#include "Product.h"

using namespace std;


unsigned char FactoredProduct::getComplexity() const
{
  const unsigned char c = noncanonicalPtr->getComplexity();
  /*  */
  if (c != canonicalPtr->getComplexity())
  {
cout << "non: " << +c << "\n";
cout << noncanonicalPtr->strHeader() << "\n";
cout << noncanonicalPtr->strLine() << "\n";
cout << "can: " << +canonicalPtr->getComplexity() << "\n";
cout << canonicalPtr->strHeader() << "\n";
cout << canonicalPtr->strLine() << endl;
    assert(c == canonicalPtr->getComplexity());
    }
    /* */
  return c;
}

