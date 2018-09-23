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

#include <cmd_options.h>
#include <cmd_options/f_flag.h>

#include <iostream>

namespace co = cmd_options;
namespace co_alt = cmd_options::alt;







int main (int argc, char *argv[])
{
  try {
    co::options_group desc{
      co::make_option("help,h","produce help message",
          co::constrain().occurrences(0,1)),
      co_alt::make_f_flag<std::string>("foo","foo flag",
        co::constrain().occurrences(0,1)),
      co_alt::make_f_flag<bool>("bar","negative bar flag",true,
        co::constrain().occurrences(0,1)),
      co_alt::make_hidden_f_flag<char>("baz",
        co::constrain().occurrences(0,1))
    };

    co::variable_map vm = co::parse_arguments(argv+1,argv+argc,desc);

    if(vm.count("help")) {
      std::cout << co::to_string(desc) << "\n";
      return 0;
    }

    auto range = vm.equal_range("foo");
    if(range.first != range.second) {
      std::cout << "foo value with the value of "
        << co::any_cast<std::string>((range.first)->second) << "\n";
    }

    range = vm.equal_range("bar");
    if(range.first != range.second) {
      std::cout << "bar value with the value of "
        << co::any_cast<bool>((range.first)->second) << "\n";
    }

    range = vm.equal_range("baz");
    if(range.first != range.second) {
      std::cout << "baz value with the value of "
        << co::any_cast<char>((range.first)->second) << "\n";
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
