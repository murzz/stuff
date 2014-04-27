#pragma once

//#include <string>
//#include <regex>
#include <tuple>
#include <boost/regex.hpp>
//#include <boost/tuple/tuple.hpp>

namespace utils
{

const unsigned short expand_env_vars(std::string& original)
{
   const boost::regex envscan("%([0-9A-Za-z\\/]*)%");
   const boost::sregex_iterator end;
   typedef std::list<std::tuple<const std::string, const std::string>> t2StrLst;
   t2StrLst replacements;
   for (boost::sregex_iterator rit(original.begin(), original.end(), envscan); rit != end; ++rit)
      replacements.push_back(std::make_pair((*rit)[0], (*rit)[1]));
   unsigned short cnt = 0;
   for (t2StrLst::const_iterator lit = replacements.begin(); lit != replacements.end(); ++lit)
   {
      const char* expanded = std::getenv(std::get < 1 > (*lit).c_str());
      if (expanded == NULL)
         continue;
      boost::replace_all(original, std::get < 0 > (*lit), expanded);
      cnt++;
   }
   return cnt;
}

}
