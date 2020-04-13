#include "cmd_options.h"

#include <string>
#include <iostream>


int main (int argc, char *argv[])
{
  namespace co = cmd_options;

  co::options_group grp{
    // accepts --bar and -b without an argument
    co::make_option("bar,b","Description of the bar flag"),

    // accepts --foo <arg> or -f <arg> The mapped value is a string
    co::make_option("foo,f",co::value<std::string>(),
      "Description of option that requires a string argument",
      co::constrain().occurrences(1)),

    // accepts -z <arg> or -z The mapped value is a string with default
    // "implicit'
    co::make_option(",z",co::value<std::string>().implicit("implicit"),
      "Description of the 'z' option that takes an optional string"),

    // accepts an operand in any position. The mapped value is a string
    co::make_operand("operand_key",co::value<std::string>())
  };


  /*
    Assume conf was created from some other source, a configuration file
    for example.
  */
  std::vector<std::string> conf{
    "--bar"
  };

  /*
    The there should be exactly 1 'foo' option in a valid variable_map
    instance but since the parse was incremental, there is no error.

    Since we didn't specify a constraint that would cause early
    termination, we can always ignore the first returned value during
    incremental parsing as it will always be true.
  */
  co::variable_map vm;
  std::tie(std::ignore,vm) = co::parse_incremental_arguments(conf.begin(),
    conf.end(),grp);


  /*
    parse the main arguments and put into the variable map. Don't forget
    that the first argument (argv[0]) is the program name! Since this is
    a full parse, there will be an error if 'foo' has not been given.
  */
  vm = co::parse_arguments(argv+1,argv+argc,grp,vm);

  for(auto &key_val : vm) {
    std::cout << "key: " << key_val.first;
    if(!co::is_empty(key_val.second))
      std::cout << " value: " << co::any_cast<std::string>(key_val.second);
    std::cout << "\n";
  }

  return 0;
}
