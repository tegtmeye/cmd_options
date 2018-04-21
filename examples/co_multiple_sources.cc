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

int main (int argc, char *argv[])
{
  namespace co = cmd_options;

  // Declare a group of options that will be
  // allowed only on command line
  co::options_group generic{
    co::make_option("version,v", "print version string"),
    co::make_option("help,h","Display a help message"),
    co::make_option("config,c",
      co::value<std::string>().implicit("multiple_sources.cfg"),
      "name of a file of a configuration.")
  };

  // Declare a group of options that will be
  // allowed both on command line and in a config file
  co::options_group config{
    co::make_option("optimization", co::value<int>().implicit(10),
      "optimization level"),
    co::make_option("include-path,I",co::value<std::string>(),
       "include path")
  };

  co::options_group cmd_line = generic;
  cmd_line.emplace_back(
    co::make_operand("input-file",co::value<std::string>()));

  co::variable_map vm = co::parse_arguments(argv+1,argv+argc,cmd_line);

  return 0;
}
