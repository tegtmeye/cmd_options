/**
 *  Copyright (c) 2018, Mike Tegtmeyer
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

/*
  Equivalent behavior to boost option_groups.cpp
*/

#include "cmd_options.h"

#include <iostream>

int main (int argc, char *argv[])
{
  namespace co = cmd_options;

  try {
    // Declare three groups of options.
    co::options_group general{
      co::make_option("help","produce a help message",
        co::constrain().occurrences(0,1)),
      co::make_option("help-module",co::value<std::string>(),
        "produce a help for a given module",co::constrain().occurrences(0,1)),
      co::make_option("version","output the version number")
    };

    co::options_group gui{
      co::make_option("display",co::value<std::string>(),
        "display to use",co::constrain().occurrences(0,1))
    };

    co::options_group backend{
      co::make_option("num-threads",co::value<int>(),
        "the initial number of threads",co::constrain().occurrences(0,1))
    };

    co::options_group all;
    all.insert(all.end(),general.begin(),general.end());
    all.insert(all.end(),gui.begin(),gui.end());
    all.insert(all.end(),backend.begin(),backend.end());

    co::variable_map vm = co::parse_arguments(argv+1,argv+argc,all);

    if(vm.count("help")) {
      std::cout << "Allowed options:\n\n"
        << "General options:\n" << co::to_string(general) << "\n"
        "GUI options:\n" << co::to_string(gui);
      return 0;
    }

    auto &&prange = vm.equal_range("help-module");
    if(prange.first != prange.second) {
      const std::string &s = co::any_cast<std::string>((prange.first)->second);
      if(s == "gui") {
        std::cout << "GUI options:\n" << co::to_string(gui);
      }
      else if(s == "backend") {
        std::cout << "Backend options:\n" << co::to_string(backend);
      }
      else {
        std::cout << "Unknown module '"
          << s << "' in the --help-module option\n";
        return 1;
      }
      return 0;
    }

    auto &&trange = vm.equal_range("num-threads");
    if(trange.first != trange.second) {
      std::cout << "The 'num-threads' options was set to "
        << co::any_cast<int>((trange.first)->second) << "\n";
      return 0;
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
