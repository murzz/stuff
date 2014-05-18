#pragma once

//#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <boost/optional.hpp>

struct env
{
   int argc_;
   char** argv_;

   std::string url_;
   std::string name_;
   std::string pass_;

   //unsigned pool_size_;
   //typedef boost::optional<unsigned> pool_size_type;
   typedef unsigned pool_size_type;
   pool_size_type pool_size_;

   //boost::filesystem::path level_dir_;
   boost::thread_group threads_;

   static env & get()
   {
      static env impl;
      return impl;
   }
};

//std::ostream & operator<<(std::ostream & os, env::pool_size_type const & op)
//{
//   if (op)
//   {
//      os << *op;
//   }
//   return os;
//}

