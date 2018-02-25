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
    co::make_option(",i",co::value<int>().implicit(42),
      "Description of option with an optional integer argument"),

    // accepts an operand in any position. The mapped value is a string
    co::make_operand("Description of this operand that takes a string",
      co::value<std::string>())
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

  auto irange = vm.equal_range("i");
  for (auto i = irange.first; i != irange.second; ++i) {
    std::cout << i->first << " was given: "
      << co::any_cast<int>(i->second) << '\n';
  }

  auto oprange = vm.equal_range("");
  for (auto i = oprange.first; i != oprange.second; ++i)
    std::cout << "operand was given with value: "
      << co::any_cast<std::string>(i->second) << '\n';

  return 0;
}
