#include <iostream>
#include <sstream>
#include <string>
#include "geng3.h"

using namespace std;

int main(int argc, char* argv[])
{
  // cerr << "Hello geng3demo!" << endl;

  Geng3 geng3;
  // geng3.set_input (inp);
  // geng3.set_output(out);

  if (geng3.help(argc, argv)) return 1;

  // std::istream* inp {};
  // std::ostream* out {};

  geng3.exec();

  return 0;
}
