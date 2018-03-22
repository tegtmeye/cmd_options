/**
 *  Copyright (c) 2017, Mike Tegtmeyer
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

#include <string>
#include <iostream>

int main (int argc, char *argv[])
{
  namespace co = cmd_options;

  co::options_group grp{
    // accepts --help and -h without an argument
    co::make_option("help,h","Display a help message"),

    // accepts --foo <arg> or -f <arg> The mapped value is a string
    co::make_option("foo,f",co::value<std::string>(),
      "Description of option that requires a string argument"),

    // accepts -i <arg> or -i The mapped value is an int with default 42
    // indexed by the 'meaning_of_life' key
    co::make_option(",i,meaning_of_life",co::value<int>().implicit(42),
      "Description of option with an optional integer argument"),

    // accepts an operand in any position. The mapped value is a string
    co::make_operand("operand_key",co::value<std::string>())
  };

  // parse the options and put into the variable map. Don't forget that
  // the first argument (argv[0]) is the program name!
  co::variable_map vm = co::parse_arguments(argv+1,argv+argc,grp);

  if(vm.count("help")) {
    std::cout << co::to_string(grp) << "\n";
    return 0;
  }

  auto brange = vm.equal_range("foo");
  for (auto i = brange.first; i != brange.second; ++i) {
    std::cout << i->first << " was given: "
      << co::any_cast<std::string>(i->second) << '\n';
  }

  auto irange = vm.equal_range("meaning_of_life");
  for (auto i = irange.first; i != irange.second; ++i) {
    std::cout << i->first << " was given: "
      << co::any_cast<int>(i->second) << '\n';
  }

  auto oprange = vm.equal_range("operand_key");
  for (auto i = oprange.first; i != oprange.second; ++i)
    std::cout << i->first << " was given: "
      << co::any_cast<std::string>(i->second) << '\n';

  return 0;
}
