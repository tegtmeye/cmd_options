#include "cmd_options.h"

#include <string>

int main (int argc, char *argv[])
{
  namespace co = cmd_options;

  co::options_group grp{
    // accepts --foo and -f without an argument
    co::make_option("foo,f","Description of this no-arg option"),

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

  std::size_t foo_n = vm.count("foo");
  if(foo_n)
    std::cout << "--foo or -f was given " << foo_n << " times\n";

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

  return 0;
}
