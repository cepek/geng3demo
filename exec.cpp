#include "geng3.h"

void Geng3::exec()
{
  char c;
  while (inp->get(c)) {
        *out << c;
    }
}
