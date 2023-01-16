#ifndef GENG3_H
#define GENG3_H

#include <iostream>

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

  bool demo_data_{ false };

  void set_center_pseudo_bl(double, double);

  std::istream* inp { &std::cin  };
  std::ostream* out { &std::cout };
};

#endif // GENG3_H
