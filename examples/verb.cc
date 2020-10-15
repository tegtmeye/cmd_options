/**
 *  Copyright (c) 2020, Mike Tegtmeyer
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

int main (int argc, char *argv[])
{
  namespace co = cmd_options;

  co::options_group prog_grp{
    // accepts --bar and -b without an argument
    co::make_option("bar,b","Description of the bar flag"),

    // accepts --foo <arg> or -f <arg> The mapped value is a string
    co::make_option("foo,f",co::value<std::string>(),
      "Description of option that requires a string argument",
      co::constrain().occurrences(1)),

    // accept all operands, trigger preempt, and ignore it
    co::make_operand("",
      co::constrain().preempt().ignore())
  };

  std::map<std::string,co::options_group> verb_opts;

  verb_opts["verb1"] =
    co::options_group{
      co::make_operand("verb",co::value<std::string>(),
        co::constrain().at_position(0)),

      // accepts --bar and -b without an argument
      co::make_option("v1_bar","Description of the v1_bar flag"),

      // accepts --foo <arg> or -f <arg> The mapped value is a string
      co::make_option("v1_foo",co::value<std::string>(),
        "Description of option that requires a string argument",
        co::constrain().occurrences(1)),

      // accept all operands, trigger preempt, and ignore it
      co::make_operand("",
        co::constrain().preempt().ignore())
    };

  verb_opts["verb2"] =
    co::options_group{
      co::make_operand("verb",co::value<std::string>(),
        co::constrain().at_position(0)),

      // accepts --bar and -b without an argument
      co::make_option("v2_bar","Description of the v1_bar flag"),

      // accepts --foo <arg> or -f <arg> The mapped value is a string
      co::make_option("v2_foo",co::value<std::string>(),
        "Description of option that requires a string argument",
        co::constrain().occurrences(1)),

      // accept all operands, trigger preempt, and ignore it
      co::make_operand("",
        co::constrain().preempt().ignore())
    };


  char **res = 0;
  co::variable_map prog_vm;
  std::tie(res,prog_vm) = co::parse_arguments(argv+1,argv+argc,prog_grp);

  std::vector<std::pair<std::string,co::variable_map> > verb_vm_list;

  while(res != argv+argc) {
    auto loc = verb_opts.find(*res);
    if(loc == verb_opts.end()) {
      std::cerr << "Invalid verb!\n";
      return 1;
    }

    verb_vm_list.emplace_back(std::make_pair(*res,co::variable_map()));
    std::tie(res,verb_vm_list.back().second) =
      co::parse_arguments(res,argv+argc,loc->second);
  }


  std::cerr << "program options are:\n";
  for(auto &val : prog_vm) {
    std::cerr << val.first << " -> ";
    if(!co::is_empty(val.second))
      std::cerr << co::any_cast<std::string>(val.second) << "\n";
    else
      std::cerr << "[empty]\n";
  }
  std::cerr << '\n';

  for(auto &verb_pair : verb_vm_list) {
    std::cerr << "received verb: '" << verb_pair.first << "' with options:\n";

    for(auto &val : verb_pair.second) {
      std::cerr << val.first << " -> ";
      if(!co::is_empty(val.second))
        std::cerr << co::any_cast<std::string>(val.second) << "\n";
      else
        std::cerr << "[empty]\n";
    }

    std::cerr << '\n';
  }


  return 0;
}
