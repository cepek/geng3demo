#ifndef GENG3_H
#define GENG3_H

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

class Geng3
{
public:
  Geng3();
  ~Geng3();

  void exec();

  bool help(int argc, char* argv[]);
  void set_input (std::istream* is) { inp = is; };
  void set_output(std::ostream* os) { out = os; };

private:
  // transformation from input pseudo coordinates
  double center_pseudo_x_;
  double center_pseudo_y_;
  double center_pseudo_b_;
  double center_pseudo_l_;

  void demo_data();
  std::string demo_input();

  bool demo_data_{ false };

  void set_center_pseudo_bl(double, double);

  std::istream* inp { &std::cin  };
  std::ostream* out { &std::cout };

  enum Type {free, fix, constr, unused,                // neu
             free_fix, free_constr, free_unused,       // ne_u
             fix_free, fix_constr, fix_unused,
             constr_free, constr_fix, constr_unused,
             unused_fixed, unused_fix, unused_constr};


  const std::unordered_map<std::string,Type> str2type
  {

    { "free",          Type::free },
    { "fix",           Type::fix },
    { "constr",        Type::constr },
    { "unused",        Type::unused },
    { "free_fix",      Type::free_fix },
    { "free_constr",   Type::free_constr },
    { "free_unused",   Type::free_unused },
    { "fix_free",      Type::fix_free },
    { "fix_constr",    Type:: fix_constr },
    { "fix_unused",    Type::fix_unused },
    { "fix_unused",    Type::fix_unused },
    { "constr_free",   Type::constr_free },
    { "constr_fix",    Type::constr_fix },
    { "constr_unused", Type::constr_unused },
    { "unused_fixed",  Type::unused_fixed },
    { "unused_fix",    Type::unused_fix },
    { "unused_constr", Type::unused_constr }
  };

  struct Point {
    double x{0}, y{0}, z{0};
    double b{0}, l{0}, h{0};
    Type   type {Type::free};
    double dbx {0}, dbl {0};
  };

  bool exec_check(const std::vector<std::string>& tokens, Point& p);
};

#endif // GENG3_H
