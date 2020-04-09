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

namespace co = cmd_options;

int main (int argc, char *argv[])
{
  try {
    co::options_group grp{
      /*
        Accepts --help and -h without an argument. If seen, stop further
        options parsing
      */
      co::make_option("help,h","Display a help message",
        co::constrain().preempt()),

      /*
        Accepts --foo <arg> or -f <arg> with a required string argument.
        This option can be given any number of times.
      */
      co::make_option("foo,f",co::value<std::string>(),
        "Description of option that requires a string argument"),

      /*
        Accepts --bar or -b strictly without and argument
        This option can be only be given 0 or 1 time
      */
      co::make_option("bar,b","Description of option without an argument",
        co::constrain().occurrences(0,1)),

      /*
        Accepts -i <arg> or -i with an optional int argument indexed in
        the resulting multimap by the 'meaning_of_life' key. If the
        argument is not provided, the mapped value will default 42.
        This option can be given zero or one time.
      */
      co::make_option(",i,meaning_of_life",co::value<int>().implicit(42),
        "Description of option with an optional integer argument",
        co::constrain().occurrences(0,1)),

      /*
        Specify a non-optional operand at index position 0 only. The
        mapped value is a string.
      */
      co::make_operand("operand1",co::value<std::string>(),
        co::constrain().at_position(0).occurrences(1)),

      /*
        Specify an optional operand at index position 1 only. The
        mapped value is a string. If this operand is given, '-i' cannot
        also be given.
      */
      co::make_operand("operand2",co::value<std::string>(),
        co::constrain().at_position(1).mutually_exclusive({"meaning_of_life"})),

      /*
        Accepts an optional operand in any position but since it is
        assigned to the options group after 'operand1' and 'operand2',
        it is only considered in the position index 2 and greater. The
        mapped value is a string. This operand can be given any number
        of times if it is, '--bar' must also be given.
      */
      co::make_operand("operand3",co::value<std::string>(),
        co::constrain().mutually_inclusive({"bar"})),
    };

    // parse the options and put into the variable map. Don't forget that
    // the first argument (argv[0]) is the program name!
    co::variable_map vm = co::parse_arguments(argv+1,argv+argc,grp);

    if(vm.count("help")) {
      std::cout << co::to_string(grp) << "\n";
      return 0;
    }

    auto range = vm.equal_range("foo");
    for (auto i = range.first; i != range.second; ++i) {
      std::cout << i->first << " was given: "
        << co::any_cast<std::string>(i->second) << '\n';
    }

    range = vm.equal_range("meaning_of_life");
    for (auto i = range.first; i != range.second; ++i) {
      std::cout << i->first << " was given: "
        << co::any_cast<int>(i->second) << '\n';
    }

    range = vm.equal_range("operand1");
    for (auto i = range.first; i != range.second; ++i)
      std::cout << i->first << " was given: "
        << co::any_cast<std::string>(i->second) << '\n';

    range = vm.equal_range("operand2");
    for (auto i = range.first; i != range.second; ++i)
      std::cout << i->first << " was given: "
        << co::any_cast<std::string>(i->second) << '\n';

    range = vm.equal_range("operand3");
    for (auto i = range.first; i != range.second; ++i)
      std::cout << i->first << " was given: "
        << co::any_cast<std::string>(i->second) << '\n';
  }
  catch(const co::unknown_option_error &ex) {
    std::cerr << "unrecognized option '" << argv[ex.argument()+1] << "'\n"
      << "Try 'impatient --help' for more information.\n";
  }
  catch(const co::missing_argument_error &ex) {
    std::cerr << "option '" << argv[ex.argument()+1]
      << "' requires an argument\n"
      << "Try 'impatient --help' for more information.\n";
  }
  catch(const co::unexpected_argument_error &ex) {
    std::cerr << "option '" << argv[ex.argument()+1]
      << "' doesn't allow an argument\n"
      << "Try 'impatient --help' for more information.\n";
  }
  catch(const co::invalid_argument_error &ex) {
    std::string nested_msg;
    try {
      std::rethrow_if_nested(ex);
    }
    catch(const std::exception& nested) {
      nested_msg = nested.what();
    }
    catch(...) {
      nested_msg = "unknown error";
    }

    std::cerr << "invalid argument to '" << argv[ex.argument()+1] << "', "
      << nested_msg.c_str() << "\n"
      << "Try 'impatient --help' for more information.\n";
  }
  catch(const co::unexpected_operand_error &ex) {
    std::cerr << "unexpected operand '" << argv[ex.argument()+1] << "'\n"
      << "Try 'impatient --help' for more information.\n";
  }
  catch(const co::occurrence_error &ex) {
    if(ex.min() == ex.max()) {
      std::cerr << ex.mapped_key()
        << " must be provided exactly " << ex.max() << " time"
        << (ex.max() == 1?"":"s") << ", received "
        << ex.occurrences() << "\n";
    }
    else if(ex.min() == 0) {
      std::cerr << ex.mapped_key()
        << " cannot be provided more than " << ex.max() << " time"
        << (ex.max() == 1?"":"s") << ", received "
        << ex.occurrences() << "\n";
    }
    else if(ex.max() == std::numeric_limits<std::size_t>::max()) {
      std::cerr << ex.mapped_key()
        << " must be provided a minimum of " << ex.min() << " time"
        << (ex.min() == 1?"":"s") << ", received "
        << ex.occurrences() << "\n";
    }
    else {
      std::cerr << ex.mapped_key()
        << " must be provided a minimum of " << ex.min() << " but no more than "
        << ex.max() << " time" << (ex.max() == 1?"":"s") << ", received "
        << ex.occurrences() << "\n";
    }

    std::cerr << "Try 'impatient --help' for more information.\n";
  }
  catch(const co::mutually_exclusive_error &ex) {
    std::cerr << ex.mapped_key() << " cannot be used with "
      << ex.exclusive_mapped_key() << "\n"
      << "Try 'impatient --help' for more information.\n";
  }
  catch(const co::mutually_inclusive_error &ex) {
    std::cerr << ex.mapped_key() << " must be used with "
      << ex.inclusive_mapped_key() << "\n"
      << "Try 'impatient --help' for more information.\n";
  }

  return 0;
}
