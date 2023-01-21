#include "geng3.h"
#include <gnu_gama/radian.h>

Geng3::Geng3()
{
  set_center_pseudo_bl(45,90);

  inp = &std::cin;
  out = &std::cout;
}

Geng3::~Geng3()
{
  if (inp != &std::cin ) delete inp;
  if (out != &std::cout) delete out;
}

void Geng3::set_center_pseudo_bl(double bp, double pl)
{
  // center position is not important for generating demo data,
  // it just needs to be set somewhere on the ellipsoid
  center_pseudo_b_ = bp/180.0*M_PI;
  center_pseudo_l_ = pl/180.0*M_PI;
}

// Help ...................................................

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

// Demo ...................................................

std::string Geng3::demo_input()
{
  std::string multistr = R"(

# input data are read by lines
# anything from character '#' is comments and is ignored
# empty lines are ignored


* A 1000.235 2000.65 50    fix  # poin A with x, y and height
* B  800.000 1562.48 36.9  fix  #    coordinates n, e, u are fixed
* C 2000     3000    60         # implicitly n, e, u are free


# </vectors>
#
#? not a point ### points and here

)";

  return multistr;
}

void Geng3::demo_data()
{
  auto istr = new std::istringstream(demo_input());
  set_input(istr);
}

// Exec ...................................................

void Geng3::exec()
{
  const std::set<std::string> obs_blocks {"<vectors>"};

  std::string line;
  std::deque<std::string> lines;

  while (std::getline(*inp,line))
    {
      // remove comments and empty lines
      auto z = line.find('#');
      if (z != std::string::npos) line.erase(z);
      while (!line.empty() && std::isspace(line.back())) line.pop_back();

      if (!line.empty()) lines.push_back(line);
  }


  // *** points section
  while (!lines.empty())
    {
      line = lines.front();

      std::istringstream data(line);
      std::vector<std::string> tokens;
      std::string word;
      while (data >> word) tokens.push_back(word);

      if (obs_blocks.find(tokens[0]) != obs_blocks.cend()) break;

      lines.pop_front();
      std::cout << line << std::endl;

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


  std::cout << "// observations section" <<std::endl;
  while (!lines.empty())
    {
      line = lines.front();
      lines.pop_front();
      std::cout << line << std::endl;
    }



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
      double db = std::asin((p.x - sumx)/6371008.8);   // mean Earth radius
      double dl = std::asin((p.y - sumy)/6371008.8);

      //std::cout << "db dl : " << db << " " << dl << " " << " " << std::endl;
      //std::cout << "CBL : " << center_pseudo_b_ << " "
      //          << center_pseudo_l_ << " " << " " << std::endl;


      p.b = center_pseudo_b_ + db;
      p.l = center_pseudo_l_ + dl;
      ellipsoid.blh2xyz(p.b,p.l,p.h, p.x,p.y,p.z);

#if 0
      std::cout << "BLH "
                << "pseudo b l : " << center_pseudo_b_ << " " << center_pseudo_l_
                << "   rad " << p.b << " " << p.l << " .... "
                << GNU_gama::rad2deg_str(p.b,3,6) << " "
                << GNU_gama::rad2deg_str(p.l,3,6) << " "
                << p.h << std::endl;
      std::cout << std::fixed << std::setprecision(4)
                << "XYZ " << p.x << " " << p.y << " " << p.z << std::endl
                << std::endl;
#endif

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

