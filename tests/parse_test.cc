#include "cmd_options.h"

#include "test_detail.h"

#include <boost/test/unit_test.hpp>

#include <iostream>

BOOST_AUTO_TEST_SUITE( parse_test_suite )

namespace co = lemma::cmd_options;

typedef std::basic_string<char> string_type;
typedef co::basic_option_pack<char> option_pack;
typedef co::basic_option_description<char> option_description;
typedef co::basic_variable_map<char> variable_map;


option_description nested1{
  [](const string_type &option) -> option_pack {
    if(option == "-foo")
      return option_pack{true,false,"-","foo",{"-a","-bar","-c"}};
    return option_pack();
  },
  [](const string_type &raw_key, std::size_t, std::size_t, const variable_map &)
  {
    return std::make_pair(true,raw_key);
  },
  [](void) -> string_type {
    return "test nested";
  }
};

option_description nested2{
  [](const string_type &option) -> option_pack {
    if(option == "-bar")
      return option_pack{true,false,"-","bar",{"-d","-e","-f"}};
    return option_pack();
  },
  [](const string_type &raw_key, std::size_t, std::size_t, const variable_map &)
  {
    return std::make_pair(true,raw_key);
  },
  [](void) -> string_type {
    return "test nested2";
  }
};

option_description nested3{
  [](const string_type &option) -> option_pack {
    if(option == "-bar")
      return option_pack{true,false,"-","bar",{"-d","pos","-f"}};
    return option_pack();
  },
  [](const string_type &raw_key, std::size_t, std::size_t, const variable_map &)
  {
    return std::make_pair(true,raw_key);
  },
  [](void) -> string_type {
    return "test nested3";
  }
};

option_description all_operands{
  {},{},{},{},{},{},
  [](const string_type &mapped_key, const string_type &value,
    const variable_map &) -> co::any
  {
    return co::any(value);
  }
};



/**
  Nested option test
 */
BOOST_AUTO_TEST_CASE( parse_nested_option_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "-foo"
  };

  options = co::options_group{
    nested1,
    nested2,
    co::make_option(",a","case 2"),
    co::make_option(",b","case 2"),
    co::make_option(",c","case 2"),
    co::make_option(",d","case 2"),
    co::make_option(",e","case 2"),
    co::make_option(",f","case 2")
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  std::cerr << detail::to_string<std::string>(vm);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"a",{}},
      {"bar",{}},
      {"c",{}},
      {"d",{}},
      {"e",{}},
      {"f",{}},
      {"foo",{}}
    }));
}

/**
  Nested operand_test
 */
BOOST_AUTO_TEST_CASE( parse_nested_operand_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "-foo"
  };

  options = co::options_group{
    nested1,
    nested3,
    co::make_option(",a","case 2"),
    co::make_option(",b","case 2"),
    co::make_option(",c","case 2"),
    co::make_option(",d","case 2"),
    co::make_option(",f","case 2"),
    all_operands
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  std::cerr << detail::to_string<std::string>(vm);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"alt_key",{string_type("pos")}},
      {"a",{}},
      {"bar",{}},
      {"c",{}},
      {"d",{}},
      {"f",{}},
      {"foo",{}},
    }));
}


BOOST_AUTO_TEST_SUITE_END()

