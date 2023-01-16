#ifndef GENG3_H
#include "geng3.h"
#endif

#include <sstream>

void Geng3::demo_data()
{
  std::string multistr = R"(
The journey
of a thousand miles
begins with one step.

)";

  auto istr = new std::istringstream(multistr);
  set_input(istr);
}
