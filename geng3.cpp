#ifndef GENG3_H
#include "geng3.h"
#endif

Geng3::Geng3()
{
  set_center_pseudo_bl(45,90);

  inp = &std::cin;
  out = &std::cout;
}

Geng3::~Geng3()
{
  //if (inp != &std::cin ) delete inp;
  if (out != &std::cout) delete out;
}

void Geng3::set_center_pseudo_bl(double bp, double pl)
{
  // center position is not important for generating demo data,
  // it just needs to be set somewhere on the ellipsoid
  center_pseudo_b_ = bp/180.0*3.14;
  center_pseudo_l_ = pl/180.0*3.14;
}
