#include "cmd_options.h"

#include <string>
#include <iostream>

int main (int argc, char *argv[])
{
  namespace co = cmd_options;

  co::options_group grp{
    // accepts --help and -h without an argument
    co::make_option("help,h","Display a help message"),

    // accepts -b <arg> The mapped value is a string
    co::make_option(",b",co::value<std::string>(),
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

  auto brange = vm.equal_range("b");
  if(brange.first != brange.second)
    std::cout << "-b has given values:\n";
  else
    std::cout << "-b was not provided\n";
  for (auto i = brange.first; i != brange.second; ++i) {
    std::cout << "  " << i->first << ": "
      << co::any_cast<std::string>(i->second) << '\n';
  }

  auto irange = vm.equal_range("i");
  if(irange.first != irange.second)
    std::cout << "-i has given values:\n";
  else
    std::cout << "-i was not provided\n";
  for (auto i = irange.first; i != irange.second; ++i)
    std::cout << "  " << i->first << ": "
      << co::any_cast<int>(i->second) << '\n';

  auto orange = vm.equal_range("");
  if(orange.first != orange.second)
    std::cout << "Operands have values:\n";
  else
    std::cout << "An operand was not provided\n";
  for (auto i = orange.first; i != orange.second; ++i)
    std::cout << "  " << i->first << ": "
      << co::any_cast<std::string>(i->second) << '\n';

  return 0;
}
