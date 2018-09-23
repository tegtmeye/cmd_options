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


int main (int argc, char *argv[])
{
  namespace co = cmd_options;

  try {
    std::string depends = "deps_file";
    std::string sources = "src_file";
    std::string root = ".";

    co::options_group desc{
      co::make_option("help","produce help message",
          co::constrain().occurrences(0,1)),
      co::make_option("option_with_a_really_really_long_name","This option has a really really long name and a really really long description that must be wrapped.",
          co::constrain().occurrences(0,1)),
      co::make_option("output,o", co::value<std::string>(),
        "pathname for output",
        co::constrain().occurrences(0,1).
          mutually_exclusive({"two","body","mainpackage"})),
      co::make_option("macrofile,m", co::value<std::string>(),
        "full pathname of macro.h",co::constrain().occurrences(0,1)),
      co::make_option("two,t", co::value<bool>(),
        "preprocess both header and body",
        co::constrain().occurrences(0,1).
          mutually_exclusive({"mainpackage","body"})),
      co::make_option("body,b", co::value<bool>(),
        "preprocess body in the header context",
        co::constrain().occurrences(0,1).
          mutually_exclusive({"mainpackage"})),
      co::make_option("libmakfile,l", co::value<std::string>(),
        "write include makefile for library",
        co::constrain().occurrences(0,1).
          mutually_exclusive({"mainpackage"})),
      co::make_option("mainpackage,p", co::value<std::string>(),
        "output dependency information",
        co::constrain().occurrences(0,1)),
      co::make_option("depends,d", co::value<std::string>(&depends),
        "write dependencies to <pathname>",
        co::constrain().occurrences(0,1).
          mutually_inclusive({"mainpackage"})),
      co::make_option("sources,s", co::value<std::string>(&sources),
        "write source package list to <pathname>",
        co::constrain().occurrences(0,1).
          mutually_inclusive({"mainpackage"})),
      co::make_option("root,r", co::value<std::string>(&root),
        "treat <dirname> as project root directory",
        co::constrain().occurrences(0,1).
          mutually_inclusive({"mainpackage"})),
    };

    co::variable_map vm = co::parse_arguments(argv+1,argv+argc,desc);

    if(vm.count("help")) {
      std::cout << "Usage: regex [options]\n" << co::to_string(desc) << "\n";
      return 0;
    }

    std::cout << "two = ";
    auto &&res = vm.find("two");
    if(res != vm.end())
      std::cout << co::any_cast<bool>(res->second) << "\n";
    else
      std::cout << "0\n";
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
