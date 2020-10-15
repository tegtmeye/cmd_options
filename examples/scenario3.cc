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
#include <array>
#include <iostream>

int main (int argc, char *argv[])
{
  namespace co = cmd_options;

  /*
    Application-wide, hidden options. These must be present
  */
  co::options_group app_config_grp{
    co::make_option("app-config-a",co::value<std::string>(),
      "Description of the 'app-config-a' option",
      co::constrain().occurrences(1)),
    co::make_option("app-config-b",co::value<std::string>(),
      "Description of the 'app-config-b' option",
      co::constrain().occurrences(1))
  };

  /*
    Assume these arguments came from a application-wide config file
    somewhere
  */
  std::vector<std::string> app_config{
    "--app-config-a","configure a value",
    "--app-config-b","configure b value",
  };

  /*
    Parse app-wide configuration arguments, returned vm is checkeded
    for constraints violations
  */
  co::variable_map vm;
  std::tie(std::ignore,vm) = co::parse_arguments(app_config.begin(),
    app_config.end(),app_config_grp);

  /*
    User configuration options. 'foo', 'bar', and 'baz' must be present
    for vm to be valid.
  */
  co::options_group usr_config_grp{
    co::make_option("foo,f",co::value<std::string>(),
      "Description of the 'foo' option",
      co::constrain().occurrences(1)),
    co::make_option("bar,b",co::value<std::string>(),
      "Description of the 'bar' option",
      co::constrain().occurrences(1)),
    co::make_option("path",co::value<std::string>(),
      "Set the user's PATH",
      co::constrain().occurrences(1)),
    co::make_option("pi",co::value<double>(),
      "Set the value of PI",
      co::constrain().occurrences(1)),
    co::make_operand("operand",co::value<std::string>(),
      co::constrain().occurrences(1))
  };

  /*
    Assume these arguments came from a user config file somewhere
  */
  std::vector<std::string> usr_config{
    "--foo","foo value","--bar","bar value"
  };

  /*
    Parse user configuration arguments, returned vm is NOT checked
    for constraints violations because any required arguments could
    be provided in environmental variables or on the command line.
    Specifically, only 'foo' and 'bar' are set here

    Since we didn't specify a constraint that would cause early
    termination, we can always ignore the first returned value during
    incremental parsing as it will always be true.
  */
  std::tie(std::ignore,vm) = co::parse_incremental_arguments(usr_config.begin(),
    usr_config.end(),usr_config_grp,vm);

  /*
    Translate environmental variables into option_syntax and parse
    individually so that error handling can be associated with the
    environmental variable and not the corresponding option.

    The value of the environmental variable could just be added to the
    vm directly but here it is assumed that there is a need for
    automatic translation into other types and constraints enforcement.
  */
  std::vector<std::pair<std::string,std::string> > env_map{
    {"PI","--pi"},{"PATH","--path"}
  };

  /*
    For each translated environmental variable, parse the argument but
    do not check constraints because any missing ones could be provieded
    on the command line. That is returned vm is in an invalid state.

    Since we didn't specify a constraint that would cause early
    termination, we can always ignore the first returned value during
    incremental parsing as it will always be true.
  */
  for(auto &pr : env_map) {
    const char *res = std::getenv(pr.first.c_str());
    if(res) {
      std::array<std::string,2> opt{{pr.second}};
      if(*res)
        opt[1] = res;
      try {
        std::tie(std::ignore,vm) = co::parse_incremental_arguments(opt.begin(),
          opt.end(),usr_config_grp,vm);
      }
      catch(const std::runtime_error &ex) {
        std::cerr << ex.what() << ": " << pr.first << ", received: "
          << res << "\n";
      }
    }
  }

  /*
    Parse the command-line arguments, at this point, any missing options
    or operands are considered an error so check against the
    constraints. The returned variable_map is valid
  */
  char **res = 0;
  std::tie(res,vm) = co::parse_arguments(argv+1,argv+argc,usr_config_grp,vm);

  std::cout << "VM contains:\n";
  for(auto &&key_val : vm) {
    std::cout << key_val.first << " -> ";
    if(key_val.first == "pi")
      std::cout << co::any_cast<double>(key_val.second) << "\n";
    else
      std::cout << co::any_cast<std::string>(key_val.second) << "\n";
  }

  return 0;
}
