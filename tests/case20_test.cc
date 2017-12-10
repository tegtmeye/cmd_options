#include "cmd_options.h"

#include "test_detail.h"

#include <boost/test/unit_test.hpp>

#include <iostream>

/**
  case 20, Degenerate positional with implicit key
 */

BOOST_AUTO_TEST_SUITE( case18_test_suite )


namespace co = lemma::cmd_options;

/**
  All keys values
 */
BOOST_AUTO_TEST_CASE( all_key_value_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "-f",
    "-f42",
    "--bar",
    "--bar=43"
  };

  options = co::options_group{
    co::make_operand("positional_key","case 20")
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"positional_key",{}},
      {"positional_key",{}},
      {"positional_key",{}},
      {"positional_key",{}}
    }));
}


/**
  keys values with embedded positional
 */
BOOST_AUTO_TEST_CASE( key_value_with_embedded_positional_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "-f",
    "f_arg",
    "positional1",
    "-f42",
    "positional2",
    "positional3",
    "--bar",
    "bar_arg",
    "positional4",
    "--bar=43",
    "positional5",
  };

  options = co::options_group{
    co::make_option("",co::value<std::string>("floo"),"case 18"),
    co::make_operand("positional_key","case 20")
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

//   std::cerr << detail::to_string<std::string>(vm);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"positional_key",{}},
      {"positional_key",{}},
      {"positional_key",{}},
      {"positional_key",{}},
      {"positional_key",{}},
      {"f",{std::string("f_arg")}},
      {"f",{std::string("42")}},
      {"bar",{std::string("bar_arg")}},
      {"bar",{std::string("43")}}
    }));
}



BOOST_AUTO_TEST_SUITE_END()

