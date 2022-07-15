#ifndef SSS_PARTIALVOID_H
#define SSS_PARTIALVOID_H

#include "Partial.h"


class PartialVoid: public Partial
{
  private:

    unsigned lengthWestInt;

    unsigned repeatsInt;


  
  public:

    PartialVoid();

    void setVoid(
      const unsigned lengthWestIn,
      const unsigned repeatsIn);

    unsigned lengthWest() const;

    unsigned repeats() const;
};

#endif
