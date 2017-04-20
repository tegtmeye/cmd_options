#include "cmd_options.h"

#include <iostream>

namespace co = lemma::cmd_options;

int main(int argc, char *argv[])
{
  co::options_group main_options;

#if 0
  co::option_description foo_desc = {
    [](const std::string &s) {
      return (s=="foo"?s:std::string());
    },
    {},
    [](const std::string &s) {
      return (s=="f"?std::string("foo"):std::string());
    },
    {},
    {},
    {},
    [](const std::string &key, const co::variable_map &)
      {
        return boost::any(std::string("foo implicit"));
      },
    {},
    [](const std::string &key, const std::string &val, const co::variable_map &)
      {
        if(val.empty())
          return boost::any(std::string("foo missing"));
        return boost::any(std::string("foo ")+val);
      }
  };
  co::option_description blah_desc = {
    [](const std::string &s) {
      return (s=="blah"?s:std::string());
    },
    {},
    [](const std::string &s) {
      return (s=="l"?std::string("blah"):std::string());
    },
    {},
    {},
    {},
    [](const std::string &key, const std::string &val, const co::variable_map &)
      {
        if(val.empty())
          return boost::any(std::string("blah missing"));
        return boost::any(std::string("blah ")+val);
      }
  };

  co::option_description bar_desc = {
    [](const std::string &s) {
      return (s=="bar"?s:std::string());
    },
    {},
    [](const std::string &s) {
      return (s=="b"?std::string("bar"):std::string());
    },
  };
  co::option_description pos_desc = {
    {},
    {},
    {},
    {},
    [](std::size_t n) {
      return std::string("positional");
    },
    {},
    [](const std::string &key, const std::string &val, const co::variable_map &)
      {
        return boost::any(std::string("positional ")+val);
      }
  };

  main_options.push_back(foo_desc);
#endif

//   main_options.push_back(bar_desc);
//   main_options.push_back(blah_desc);
//   main_options.push_back(pos_desc);


  main_options.push_back(co::make_option("foo",std::string("z"),
    "Some foo desc Some foo desc Some foo desc Some foo desc Some foo desc"
    "Some foo desc Some foo desc Some foo desc Some foo desc Some foo desc",
    co::value<std::string>().occurrences(2)));

  main_options.push_back(co::make_option("bar",std::string("b"),
    std::string("Some bar desc"),
      co::value<std::string>().occurrences(1).implicit_value("_bar_")));

  main_options.push_back(co::make_hidden_option("shoe",std::string("s"),
    co::value<std::string>()));

  std::cerr << "\n\n" << to_string(main_options) << "\n\n";

//   co::make_woption(L"foo",std::wstring(L"f"),
//     std::wstring(L"Some foo desc"),co::value<std::wstring>().occurrences(2)));
  //co::value<int>().implicit_value(42);

  co::variable_map vm = co::parse_arguments(argv+1,argc-1,main_options);

  for(auto &pair : vm) {
    std::cerr << "key: [" << pair.first << "] value: ";
    if(!pair.second.empty())
      std::cerr << "[" << boost::any_cast<std::string>(pair.second) << "]\n";
    else
      std::cerr << "[empty]\n";
  }
  return 0;
}
