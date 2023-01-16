#ifndef GENG3_H
#include "geng3.h"
#endif

#include <set>
#include <string>
#include <iostream>

bool Geng3::help(int argc, char* argv[])
{
  const std::set<std::string> hset {"-h", "--h", "-help", "--help"};
  const std::set<std::string> dset {"-d", "--d", "-demo", "--demo"};

  bool bhelp {false};
  for (int n=2; n<=argc; n++)
    {
      if (hset.find(argv[n-1]) != hset.cend()) bhelp = true;
      if (dset.find(argv[n-1]) != dset.cend()) demo_data_ = true;
    }

  if (demo_data_)
    {
      demo_data();
    }

  return bhelp;
}
