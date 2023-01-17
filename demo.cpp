#ifndef GENG3_H
#include "geng3.h"
#endif

#include <sstream>

std::string Geng3::demo_input()
{
  std::string multistr = R"(

# input data are read by lines
# anything from character '#' is comments and is ignored
# empty lines are ignored


* A 1000.235 2000.65 50    fix  # poin A with x, y and height
* B  800.000 1562.48 36.9  fix  #    coordinates n, e, u are fixed
* C 2000     3000    60         # implicitly n, e, u are free

)";

  return multistr;
}

void Geng3::demo_data()
{
  auto istr = new std::istringstream(demo_input());
  set_input(istr);
}
