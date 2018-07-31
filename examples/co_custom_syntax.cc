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

/*
  option_pack has the following interface. Custom unpack functions must
  populate the members accordingly.

  template<typename CharT>
  struct basic_option_pack {
    typedef std::basic_string<CharT> string_type;
    typedef std::vector<string_type> packed_arg_seq;

    bool did_unpack;
    bool value_provided;
    string_type prefix;
    string_type raw_key;
    packed_arg_seq packed_arguments;
    string_type value;
  };
*/
namespace co = cmd_options;


co::option_pack unpack_f(const std::string &s)
{
  if (s.find("-f") == 0) {
    if (s.substr(2, 3) == "no-")
      return co::option_pack{true,true,"-",s.substr(5),{},"false"};
    else
      return co::option_pack{true,true,"-",s.substr(2),{},"true"};
  }

  return co::option_pack{false};
}

int main (int argc, char *argv[])
{
  try {
    co::option_description bar_desc =
      co::make_option("foo", co::value<std::string>(),
        "just an option", co::constrain().occurrences(0,1));
    bar_desc.unpack_option = unpack_f;

    co::options_group desc{
      co::make_option("help","produce help message",
          co::constrain().occurrences(0,1)),
      bar_desc
    };

    co::variable_map vm = co::parse_arguments(argv+1,argv+argc,desc);

    if(vm.count("help")) {
      std::cout << co::to_string(desc)
        << "\nIn addition -ffoo and -fno-foo syntax are recognized.\n";
      return 0;
    }

    auto &&range = vm.equal_range("foo");
    if(range.first != range.second) {
      std::cout << "foo value with the value of "
        << co::any_cast<std::string>((range.first)->second) << "\n";
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
