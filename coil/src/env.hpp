#pragma once

struct env
{
   int argc_;
   char** argv_;

   std::string url_;
   std::string name_;
   std::string pass_;

   static env & get()
   {
      static env impl;
      return impl;
   }
};

