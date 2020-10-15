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

#include "cmd_options.h"

#include <iostream>
#include <fstream>

int main (int argc, char *argv[])
{
  namespace co = cmd_options;

  try {
    int opt = 10;
    int portnum = 0;
    co::options_group desc{
      co::make_option("help","produce help message",
        co::constrain().occurrences(0,1)),
      co::make_option("optimization", co::value<int>(&opt),
        "optimization level", co::constrain().occurrences(0,1)),
      co::make_option("verbose,v", co::value<int>().implicit(1),
        "enable verbosity (optionally specify level)",
          co::constrain().occurrences(0,1)),
      co::make_option("listen,l",co::value<int>(&portnum).implicit(1001),
        "listen on a port. Default is no.", co::constrain().occurrences(0,1)),
      co::make_option("include-path,I",co::value<std::string>(),
         "include path"),
      co::make_option("input-file",co::value<std::string>(),"input file"),
      co::make_operand("input-file",co::value<std::string>())
    };

    char **res = 0;
    co::variable_map vm;
    std::tie(res,vm) = co::parse_arguments(argv+1,argv+argc,desc);

    if(res != argv+argc) {
      std::cout << "Requested to stop parsing at '"
        << *res << "'\n";
    }

    if (vm.count("help")) {
      std::cout << "Usage: options_description [options]\n"
        << "Allowed options\n" << co::to_string(desc) << "\n";
      return 0;
    }

    auto &&prange = vm.equal_range("include-path");
    if(prange.first != prange.second) {
      std::cout << "Include paths are: ";
      while(prange.first != prange.second) {
        std::cout << co::any_cast<std::string>((prange.first)->second) << " ";
        ++prange.first;
      }
      std::cout << "\n";
    }

    auto &&irange = vm.equal_range("input-file");
    if(irange.first != irange.second) {
      std::cout << "Input files are: ";
      while(irange.first != irange.second) {
        std::cout << co::any_cast<std::string>((irange.first)->second) << " ";
        ++irange.first;
      }
      std::cout << "\n";
    }

    std::cout << "Optimization level is " << opt << "\n";

    std::cout << "Listen port is " << portnum << "\n";

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
