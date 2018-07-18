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
    // the default values are given here but also could have been pre-populated
    // in the variable_map
    int opt = 10;
    std::string config_file{"co_multiple_sources.cfg"};

    // Declare a group of options that will be
    // allowed only on command line
    co::options_group generic{
      co::make_option("version,v", "print version string",
        co::constrain().occurrences(0,1)),
      co::make_option("help,h","Display a help message",
        co::constrain().occurrences(0,1)),
      co::make_option("config,c",co::value<std::string>(&config_file),
        "name of a file of a configuration.",co::constrain().occurrences(0,1)),
      co::make_operand("input-file",co::value<std::string>())
    };

    // Declare a group of options that will be
    // allowed both on command line and in a config file. The 'input-file'
    // parameter is hidden
    co::options_group config{
      co::make_option("optimization", co::value<int>(&opt),
        "optimization level", co::constrain().occurrences(0,1)),
      co::make_option("include-path,I",co::value<std::string>(),
         "include path"),
      co::make_option("input-file",co::value<std::string>())
    };

    co::options_group cmd_line = generic;
    cmd_line.insert(cmd_line.end(),config.begin(),config.end());

    co::variable_map vm = co::parse_arguments(argv+1,argv+argc,cmd_line);

    std::ifstream ifs(config_file.c_str());
    if(!ifs) {
      std::cout << "can not open config file: " << config_file << "\n";
      return 0;
    }
    else {
      // no config parsing functionality as part of cmd_options. Use a simple
      // config file scheme that simply pulls in args delimited by whitespace
      std::string arg;
      std::vector<std::string> config_vec;
      while((ifs >> arg))
        config_vec.push_back(arg);

      vm = co::parse_arguments(config_vec.begin(),config_vec.end(),config,vm);
    }

    if (vm.count("help")) {
        std::cout << "Generic options\n" << co::to_string(generic) << "\n"
          "Configuration\n" << co::to_string(config) << "\n";
        return 0;
    }

    if (vm.count("version")) {
        std::cout << "Multiple sources example, version 1.0\n";
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
