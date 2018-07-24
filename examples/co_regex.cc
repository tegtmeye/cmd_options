/**
 *  Copyright (c) 2017-2018, Mike Tegtmeyer
 *  All rights reserved.
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *      * Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *      * Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *      * Neither the name of the author nor the names of its contributors may
 *        be used to endorse or promote products derived from this software
 *        without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND ANY
 *  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "cmd_options.h"

#include <iostream>
#include <regex>


/* Define a completely non-sensical class. */
struct magic_number {
  int n;
};

inline std::istream & operator>>(std::istream &in, magic_number &rhs)
{
  static std::regex r("\\d\\d\\d-(\\d\\d\\d)");

  std::istreambuf_iterator<char> first(in);
  std::istreambuf_iterator<char> last;
  std::string s(first,last);

  std::smatch match;
  if(!std::regex_match(s,match,r))
    throw std::invalid_argument(s);
  else {
    rhs = magic_number{std::stoi(match[1])};
  }

  return in;
}

inline std::ostream & operator<<(std::ostream &out, const magic_number &rhs)
{
  return (out << rhs.n);
}

int main (int argc, char *argv[])
{
  namespace co = cmd_options;

  try {
    co::options_group desc{
      co::make_option("help","produce help message",
          co::constrain().occurrences(0,1)),
      co::make_option("version,v", "print the version number",
        co::constrain().occurrences(0,1)),
      co::make_option("magic,m",co::value<magic_number>(),
        "magic value (in NNN-NNN format)",co::constrain().occurrences(0,1))
    };

    co::variable_map vm = co::parse_arguments(argv+1,argv+argc,desc);

    if(vm.count("help")) {
      std::cout << "Usage: regex [options]\n" << co::to_string(desc) << "\n";
      return 0;
    }

    if(vm.count("version")) {
      std::cout << "Version 1.\n";
      return 0;
    }

    auto &&comp = vm.lower_bound("magic");
    if(comp != vm.end()) {
      std::cout << "The magic is \""
                << co::any_cast<magic_number>(comp->second).n << "\"\n";
    }
  }
  catch(std::exception &ex) {
    std::cerr << "error: " << ex.what() << "\n";
    return 1;
  }
  catch(...) {
    std::cerr << "Exception of unknown type!\n";
  }

  return 0;
}
