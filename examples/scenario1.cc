#include "cmd_options.h"

#include <string>
#include <array>

int main (int argc, char *argv[])
{
  namespace co = cmd_options;

  co::options_group grp{
    co::make_option(",a",co::value<std::string>(),
      "Description of the 'a' flag"),
    co::make_option(",b",co::value<std::string>(),
      "Description of the 'b' flag"),
    co::make_option(",c",co::value<std::string>(),
      "Description of the 'c' flag"),
    co::make_option("pi",co::value<double>(),
      "Value of Pi"),
    co::make_option("path",co::value<std::string>(),
      "Description of the path flag"),
    co::make_option("foo",co::value<std::string>().implicit("bar"),
      "Description of the foo flag")
  };

  /*
    Assume these arguments came from a application-wide config file somewhere
  */
  std::vector<std::string> app_config{
    "-a","a app value","-b","a app value"
  };
  co::variable_map vm = co::parse_arguments(app_config.begin(),
    app_config.end(),grp);

  /*
    Assume these arguments came from a user config file somewhere
  */
  std::vector<std::string> usr_config{
    "-b","b usr value","-c","c usr value"
  };
  vm = co::parse_arguments(usr_config.begin(),usr_config.end(),grp,vm);

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

  for(auto &pr : env_map) {
    const char *res = std::getenv(pr.first.c_str());
    if(res) {
      std::array<std::string,2> opt{{pr.second}};
      if(*res)
        opt[1] = res;
      try {
        vm = co::parse_arguments(opt.begin(),opt.end(),grp,vm);
      }
      catch(const std::runtime_error &ex) {
        std::cerr << ex.what() << ": " << pr.first << ", received: "
          << res << "\n";
      }
    }
  }

  vm = co::parse_arguments(argv+1,argv+argc,grp,vm);

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
