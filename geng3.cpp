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

# Geng3 reads GNU Gama 3D scatch definition from standard input
# and writes generated XML file to the standard output.

# Input data are read by lines
# anything from character '#' is a comment and is ignored
# empty lines are ignored.

# Points section
# --------------

* A 1000.235 2000.65 50    fix       # poin A with x, y and height (neu fixed)
* B  800.000 1562.48 36.9  fix_free  # coordinates n, e are fixed, u is free
* C 2000     3000    60              # implicitly n, e, u are free

# Points defined in this section defines only a rough scatch of a 3D network.
# For each point we define local xy corrdinates and an ellipsoidal height.
# From xy coordinates the center point is calculated and differences
# dx and dy. The 2D plane distances dx and dy are transformat to radians
# db = asin(dx/6371008.8), dl = asin(dy/6371008.8). Radian differences
# are added to the given ellipsoidal center point (45 and 90 degrees)
# leading to pseudo ellipsoidal coordintes BLH. These are transformed
# to the carthesian space coordinas XYZ and are used for generated XML input
# data for gama-g3.

# Observations in the generated XML are calculated from the given
# coordinates thus adjusted corrections must be zero (if we avoid
# rounding errors and set the output precision to 16 digits).

# Observations section
# --------------------

<vector> A C
<vector> B C
<distance> B C   # space distance
<xyz> C          # point coordinate XYZ with the given covatiance matrix

)";

  return multistr;
}

void Geng3::demo_data()
{
  auto istr = new std::istringstream(demo_input());
  set_input(istr);
}

// Exec ...................................................

std::string Geng3::type2definition(int n)
{
  std::string t = "<UNKNOWN TYPE>";
  switch(n)
    {
    case Type::free:
      t = "<free><n/><e/><u/></free>"; break;
    case Type::fix:
      t = "<fixed><n/><e/><u/></fixed>"; break;
    case Type::constr:
      t = "<constr><n/><e/><u/></constr>"; break;
    case Type::unused:
      t = "<unused><n/><e/><u/></unused>"; break;
    case Type::free_fix:
      t = "<free><n/><e/></free>  <fixed><u/></fixed>"; break;
    case Type::free_constr:
      t = "<free><n/><e/></free>  <constr><u/></constr>"; break;
    case Type::free_unused:
      t = "<free><n/><e/></free> <unused><u/></unused>"; break;
    case Type::fix_free:
      t = "<fixed><n/><e/></fixed> <free><u/></free>"; break;
    case Type:: fix_constr:
      t = "<fixed><n/><e/></fixed> <constr><u/></constr>"; break;
    case Type::fix_unused:
      t = "<fixed><n/><e/></fixed> <unused><u/></unused>"; break;
    case Type::constr_free:
      t = "<constr><n/<u/</constr> <free><u/></free>"; break;
    case Type::constr_fix:
      t = "<constr><n/<u/</constr> <fixed><u/></fixed>"; break;
    case Type::constr_unused:
      t = "<constr><n/<u/</> <free><u/></unused>"; break;
    case Type::unused_free:
      t = "<unused><n/><e/></unused> <free><e></free>"; break;
    case Type::unused_fix:
      t = "<unused><n/><e/></unused> <fixed><e/></fixed>"; break;
    case Type::unused_constr:
      t = "<unused><n/><e/></unused> <constr><e></constr>"; break;
    }

  return t;
  };

void Geng3::exec()
{
  *out << xml_start();

  const std::set<std::string> observation_types
  {
    "<vector>", "<distance>", "<xyz>"
  };

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

      // first observation block ends the points section
      if (observation_types.find(tokens[0]) != observation_types.cend()) break;

      lines.pop_front();
      // std::cout << line << std::endl;

      Point p;
      if (!exec_point_check(tokens, p)) {
          std::cerr << "!    ignored data: " << line << std::endl;
          std::cerr.flush();
          continue;
        }

      points.push_back(p);

      // for (auto p : points)
      //   std::cout << p.x << " " << p.y << " " << p.type << std::endl;
    }

  if (points.size() == 0) return;


  // set XML points type

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

  bool first_point = true;
  auto previous_point_type = Type::free;

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

      if (first_point || previous_point_type != p.type)
        {

          *out << "\n" << type2definition(p.type) << std::endl;
          previous_point_type = p.type;
        }
      first_point = false;

      *out << std::fixed << std::setprecision(16) // 3)
           << "<point> <id>" << p.id << "</id> "
           << "<x>" << p.x << "</x> <y>" << p.y << "</y> <z>" << p.z << "</z>\n"
           << "</point>\n";

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
  *out << "\n";


  // observations

  while (!lines.empty())
    {
      auto obs = lines.front();
      lines.pop_front();

      std::vector<std::string> tokens;
      std::istringstream istr(obs);
      std::string token;
      while (istr >> token) tokens.push_back(token);

      bool test = false;
      if      (tokens[0] == "<vector>")   test = check_vector(tokens);
      else if (tokens[0] == "<distance>") test = check_distance(tokens);
      else if (tokens[0] == "<xyz>")      test = check_xyz(tokens);

      if (!test) *out << "! Unknown observation: " << obs << "\n";

      // *out << obs << std::endl;
    }

  *out << xml_end();
}


bool Geng3::exec_point_check(const std::vector<std::string>& tokens, Point& p)
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

  p.id = tokens[1];

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

  p.type = Type::free;
  if (tokens.size() == 6) {
      auto type_ptr = str2type.find(tokens[5]);
      if (type_ptr != str2type.end()) {
          p.type = type_ptr->second;
        } else {
          std::cerr << "! unknown coordinate type '" << tokens[5] << "'\n";
          test = false;
        }
    }

  return test;
}

bool Geng3::check_vector(const std::vector<std::string>& tokens)
{
  if (tokens.size() != 3) return false;
  if (tokens[0] != "<vector>") return false;

  Point a, b;
  for (const auto& point : points)
    {
      if (point.id == tokens[1]) a = point;
      if (point.id == tokens[2]) b = point;
    }

  if (a.id.empty() || b.id.empty()) return false;
  if (a.id == b.id) return false;

  double dx = b.x - a.x;
  double dy = b.y - a.y;
  double dz = b.z - a.z;

  *out << "\n<obs>\n<vector>\n"
       << "<from>" << a.id << "</from> <to>" << b.id << "</to>\n"
       << "<dx>" << dx << "</dx> <dy>" << dy << "</dy> <dz>" << dz << "</dz>\n"
       << "</vector>\n"
       << "<cov-mat> <dim>3</dim> <band>0</band>\n"
       << "<flt>0.6</flt>  <flt>0.4</flt>  <flt>0.7</flt>\n"
       << "</cov-mat>\n"
       << "</obs>\n";


  return true;
}

bool Geng3::check_distance(const std::vector<std::string>& tokens)
{
  if (tokens.size() != 3) return false;
  if (tokens[0] != "<distance>") return false;

  Point a, b;
  for (const auto& point : points)
    {
      if (point.id == tokens[1]) a = point;
      if (point.id == tokens[2]) b = point;
    }

  if (a.id.empty() || b.id.empty()) return false;
  if (a.id == b.id) return false;

  double dx = b.x - a.x;
  double dy = b.y - a.y;
  double dz = b.z - a.z;
  double val = std::sqrt(dx*dx + dy*dy + dz*dz);

  *out << "\n<obs>\n<distance>\n"
       << "<from>" << a.id << "</from> <to>" << b.id << "</to>\n"
       << "<val>" << val << "</val>\n"
       << "<stdev>15</stdev>\n"
       << "<from-dh>0</from-dh> <to-dh>0</to-dh>"
       << "</distance>\n"
       << "</obs>\n";

  /*
       << "<cov-mat> <dim>1</dim> <band>0</band>\n"
       << "<flt>0.8</flt>\n"
       << "</cov-mat>\n"

  */

  // or <stdev>15</stdev>

  return true;
}


bool Geng3::check_xyz(const std::vector<std::string>& tokens)
{
  if (tokens.size() != 2) return false;
  if (tokens[0] != "<xyz>") return false;

  Point p;
  for (const auto& point : points)
    {
      if (point.id == tokens[1]) p = point;
    }

  if (p.id.empty()) return false;

  *out << "\n<obs>\n<xyz>\n"
       << "<id>" << p.id << "</id> "
       << "<x>" << p.x << "</x> <y>" << p.y << "</y> <z>" << p.z << "</z>\n"
       << "</xyz>\n"
       << "<cov-mat> <dim>3</dim> <band>2</band>\n"
       << "<flt>0.31</flt> <flt>0.10</flt> <flt>0.04</flt>\n"
       << "<flt>0.35</flt> <flt>0.13</flt>\n"
       << "<flt>0.36</flt>\n"
       << "</cov-mat>\n"
       << "</obs>\n";

  /*


  */

  // or <stdev>15</stdev>

  return true;
}


// XML output .............................................

std::string Geng3::xml_start()
{
  std::string xmlstart = R"xml_start(<?xml version="1.0" ?>

<gnu-gama-data xmlns="http://www.gnu.org/software/gama/gnu-gama-data">

<text>
Geng3 generated testing file
</text>

<g3-model>

  <constants>
     <apriori-standard-deviation>10</apriori-standard-deviation>
     <confidence-level>0.95</confidence-level>
     <!-- angular-units-degrees/ -->
     <angular-units-gons/>

     <ellipsoid>
        <id>wgs84</id>
        <!-- a>6378137</a -->
        <!-- b>6356752.31425</b -->
        <!-- inv-f>298.257223563</inv-f -->
     </ellipsoid>
  </constants>
)xml_start";

  return xmlstart;
}

std::string Geng3::xml_end()
{

  return "\n</g3-model>\n"
         "</gnu-gama-data>\n";
}
