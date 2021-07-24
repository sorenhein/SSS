#ifndef SSS_STUDY_H
#define SSS_STUDY_H

#include <vector>
#include <list>

#include "Result.h"
#include "RangesNew.h"

#include "../const.h"

using namespace std;


class Study
{
  private:

    // Used for faster comparisons on average.
    vector<unsigned> summary;
    bool studiedFlag;

    // Even more detailed pre-calculations, but requires a Ranges
    // that applies to both Strategy's being compared.

    list<unsigned> profiles;


    void setConstants();


  public:

    Study();

    ~Study();

    void reset();

    void study(const list<Result>& results);

    void scrutinize(
      const list<Result>& results,
      const RangesNew& ranges);

    void unstudy();

    bool studied() const;

    bool maybeGreaterEqual(const Study& study2) const;

    bool greaterEqualByProfile(const Study& study2) const;

    Compare compareByProfile(const Study& study2) const;
};

#endif
