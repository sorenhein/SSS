/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

// A simple timer class.

#ifndef SSS_TIMER_H
#define SSS_TIMER_H

#pragma warning(push)
#pragma warning(disable: 4365 4571 4625 4626 4774 5026 5027)
#include <chrono>
#pragma warning(pop)

#include <string>

using namespace std;


class Timer
{
  private:

    unsigned no;

    double sum;

    string label;

    std::chrono::time_point<std::chrono::high_resolution_clock> begin;


  public:

    Timer();

    ~Timer();

    void reset();

    void name(const string& nameIn);

    void start();

    void stop();

    void operator += (const Timer& timer2); // number changes

    void accum(const Timer& timer2); // number unchanged

    bool empty() const;

    string strNumber() const;

    string strRatio(
      const Timer& timerNum,
      const int prec) const;

    string str(const int prec = 1) const;
};

#endif
