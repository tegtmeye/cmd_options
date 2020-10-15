#include "cmd_options.h"

#include <string>
#include <iostream>

#if 0
spar:examples tegtmeye$ ./posix -a -b -c -d arg -e operand
key: a
key: b
key: c
key: d value: arg
key: e value: operand
spar:examples tegtmeye$ ./posix -abc -d arg -e operand
key: a
key: b
key: c
key: d value: arg
key: e value: operand
spar:examples tegtmeye$ ./posix -abcd -e operand
key: a
key: b
key: c
key: d value: implicit
key: e value: operand
spar:examples tegtmeye$ ./posix -abcd -e operand -- -a -b -c -d
key: a
key: b
key: c
key: d value: implicit
key: e value: operand
key: operand_key value: -a
key: operand_key value: -b
key: operand_key value: -c
key: operand_key value: -d
#endif

int main (int argc, char *argv[])
{
  namespace co = cmd_options;

  co::options_group grp{
    // FLAG, accepts -a
    co::make_option(",a","a FLAG"),

    // FLAG, accepts -b
    co::make_option(",b","b FLAG"),

    // FLAG, accepts -c
    co::make_option(",c","c FLAG"),

    // OPTION-ARGUMENT, accepts -e <arg> or -e with default 42
    co::make_option(",d",co::value<std::string>().implicit("implicit"),
      "d OPTION-ARGUMENT"),

    // OPTION-ARGUMENT, accepts -d arg
    co::make_option(",e",co::value<std::string>(),"e OPTION-ARGUMENT"),


    // accepts an operand in any position. The mapped value is a string
    co::make_operand("operand_key",co::value<std::string>())
  };

  // parse the options and put into the variable map. Don't forget that
  // the first argument (argv[0]) is the program name!
  char **res = 0;
  co::variable_map vm;
  std::tie(res,vm) = co::parse_arguments(argv+1,argv+argc,grp);

  if(res != argv+argc) {
    std::cout << "Requested to stop parsing at '"
      << *res << "'\n";
  }

  for(auto &key_val : vm) {
    std::cout << "key: " << key_val.first;
    if(!co::is_empty(key_val.second))
      std::cout << " value: " << co::any_cast<std::string>(key_val.second);
    std::cout << "\n";
  }


  return 0;
}
