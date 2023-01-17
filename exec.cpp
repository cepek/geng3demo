#include "geng3.h"
#include <gnu_gama/ellipsoid.h>

#include <sstream>
#include <vector>
#include <set>
#include <iomanip>


void Geng3::exec()
{
  std::string line;
  while (std::getline(*inp,line))
    {
      // remove comments and empty lines
      auto z = line.find('#');
      if (z != std::string::npos) line.erase(z);
      while (!line.empty() && std::isspace(line.back())) line.pop_back();
      if (line.empty()) continue;

      *out << line << std::endl;
      std::istringstream data(line);
      std::vector<std::string> tokens;
      std::string word;
      while (data >> word) tokens.push_back(word);

      Point p;
      if (!exec_check(tokens, p)) {
          std::cerr << "!    ignored data: " << line << std::endl;
          std::cerr.flush();
          continue;
        }

      points.push_back(p);

      // for (auto p : points)
      //   std::cout << p.x << " " << p.y << " " << p.type << std::endl;
    }

  if (points.size() == 0) return;

  double sumx {0}, sumy {0};
  for (const auto& p : points)
    {
      sumx += p.x; sumy += p.y;
    }
  sumx /= points.size();
  sumy /= points.size();

   // std::cout << "x y center " << sumx << " " << sumy << std::endl;

  using namespace GNU_gama;
  GNU_gama::Ellipsoid ellipsoid;
  // ellipsoid.set_af1(6378137, 298.257223563); // WGS 84

  for (auto& p : points)
    {
      // WGS 84  a = 6378137   b ~ 6356752.3   mean = (2a+b)/3 ~ 6371008.8
      double db = (p.x - sumx)/6371008.8;   // mean Earth radius
      double dl = (p.y - sumy)/6371008.8;

      //std::cout << "db dl : " << db << " " << dl << " " << " " << std::endl;
      //std::cout << "CBL : " << center_pseudo_b_ << " " << center_pseudo_l_ << " " << " " << std::endl;


      p.b = center_pseudo_b_ + db;
      p.l = center_pseudo_l_ + dl;
      ellipsoid.blh2xyz(p.b,p.l,p.h, p.x,p.y,p.z);

      std::cout << "BLH " <<p.b << " " << p.l << " " << p.h << std::endl;
      std::cout << std::fixed << std::setprecision(4)
          << "XYZ " << p.x << " " << p.y << " " << p.z << std::endl;
    }
}


bool Geng3::exec_check(const std::vector<std::string>& tokens, Point& p)
{
  bool test = true;

  if (tokens.size() == 0) {
      std::cerr << "! no data " << std::endl;
      test = false;
    }
  if (tokens.size() > 6 ) {
      std::cerr << "! too many tokens " << std::endl;
      test = false;
  }
  if (tokens[0] != "*"  ) {
      std::cerr << "! unknown code '" << tokens[0] << "'" << std::endl;
     test = false;
    }

  std::string id = tokens[1];

  auto sx = tokens[2];
  std::istringstream isx(sx);
  double dx;
  isx >> dx;
  // This second test is important! This makes sure that the entire string
  // was converted to a number
  if(!isx.fail() && isx.eof())
  {
      p.x = dx;
  }
  else
  {
      std::cerr << "! bad data format of x coordinate '" << sx << "'\n";
      test = false;
  }

  auto sy = tokens[3];
  std::istringstream isy(sy);
  double dy;
  isy >> dy;
  if(!isy.fail() && isy.eof())
  {
      p.y = dy;
  }
  else
  {
      std::cerr << "! bad data format of y coordinate '" << sy << "'\n";
      test = false;
  }

  auto sh = tokens[4];
  std::istringstream ish(sh);
  double dh;
  ish >> dh;
  if(!ish.fail() && ish.eof())
  {
      p.h = dh;
  }
  else
  {
      std::cerr << "! bad data format of height '" << sh << "'\n";
      test = false;
  }

  if (tokens.size() == 5)
    {
      p.type = Type::free;
    }
  else
    {
      const std::string t = tokens[5];
      auto ptr = str2type.find(t);
      if (ptr != str2type.cend())
        {
          p.type = ptr->second;
        }
      else
        {
          std::cerr << "! undefined point type '" << t << "'\n";
          test = false;
        }
    }

  return test;
}
