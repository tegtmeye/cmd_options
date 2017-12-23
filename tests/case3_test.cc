#include "cmd_options.h"

#include "test_detail.h"

#include <boost/test/unit_test.hpp>

#include <iostream>

/**
  case 3, strictly no value
 */

BOOST_AUTO_TEST_SUITE( case3_test_suite )


namespace co = lemma::cmd_options;

/**
  All flags
 */
BOOST_AUTO_TEST_CASE( all_flags_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "--foo",
    "-f"
  };

  options = co::options_group{
    co::make_hidden_option("foo,f")
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"foo",{}},
      {"foo",{}}
    }));
}


/**
  All flags embedded cease
 */
BOOST_AUTO_TEST_CASE( all_flags_cease_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "--foo",
    "-f",
    "--",
    "--bar",
    "blah"
  };

  options = co::options_group{
    co::make_hidden_option("foo,f"),
    co::make_operand<std::string>("accept all operands")
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  //std::cerr << detail::to_string<std::string>(vm);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"",std::string("--bar")},
      {"",std::string("blah")},
      {"foo",{}},
      {"foo",{}}
    }));
}

/**
  Packed flags
 */
BOOST_AUTO_TEST_CASE( packed_flags_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "--foo",
    "-f",
    "-abcd"
  };

  options = co::options_group{
    co::make_hidden_option("foo,f"),
    co::make_hidden_option(",a"),
    co::make_hidden_option(",b"),
    co::make_hidden_option(",c"),
    co::make_hidden_option(",d")
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

// std::cerr << detail::to_string<std::string>(vm);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"foo",{}},
      {"foo",{}},
      {"a",{}},
      {"b",{}},
      {"c",{}},
      {"d",{}}
    }));
}

/**
  Packed flags embedded cease
 */
BOOST_AUTO_TEST_CASE( packed_flags_cease_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "--foo",
    "-f",
    "-abcd",
    "--",
    "--bar",
    "blah"
  };

  options = co::options_group{
    co::make_hidden_option("foo,f"),
    co::make_hidden_option(",a"),
    co::make_hidden_option(",b"),
    co::make_hidden_option(",c"),
    co::make_hidden_option(",d"),
    co::make_operand<std::string>("accept all operands")
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

// std::cerr << detail::to_string<std::string>(vm);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"",std::string("--bar")},
      {"",std::string("blah")},
      {"foo",{}},
      {"foo",{}},
      {"a",{}},
      {"b",{}},
      {"c",{}},
      {"d",{}}
    }));
}

/**
  Flags w/invalid packed argument
 */
BOOST_AUTO_TEST_CASE( flags_packed_arg_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "--foo=bar",
    "-f",
    "-abcd"
  };

  options = co::options_group{
    co::make_hidden_option("foo,f")
  };

  std::cerr << detail::to_string(options.front()) << "\n";

  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv.size(),argv.data(),options)),
      co::unexpected_argument_error);
}

/**
  Flags w/operand as invalid separate argument
 */
BOOST_AUTO_TEST_CASE( flags_invalid_posarg_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "--foo",
    "-f",
    "bar"
  };

  options = co::options_group{
    co::make_hidden_option("foo,f")
  };

  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv.size(),argv.data(),options)),
      co::unexpected_operand_error);
}

/**
  Flags w/invalid option
 */
BOOST_AUTO_TEST_CASE( flags_invalid_option_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "--foo",
    "-f",
    "-a"
  };

  options = co::options_group{
    co::make_hidden_option("foo,f")
  };

  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv.size(),argv.data(),options)),
      co::unknown_option_error);
}

/**
  Flags w/invalid packed option
 */
BOOST_AUTO_TEST_CASE( flags_invalid_packed_option_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "--foo",
    "-f",
    "-abcd"
  };

  options = co::options_group{
    co::make_hidden_option("foo,f"),
    co::make_hidden_option(",a")
  };

  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv.size(),argv.data(),options)),
      co::unknown_option_error);
}

/**
  Flags w/invalid packed cease
 */
BOOST_AUTO_TEST_CASE( flags_invalid_packed_cease_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "--foo",
    "-f",
    "-a--"
  };

  options = co::options_group{
    co::make_hidden_option("foo,f"),
    co::make_hidden_option(",a")
  };

  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv.size(),argv.data(),options)),
      co::unknown_option_error);
}

/**
  Multi packed flags
 */
BOOST_AUTO_TEST_CASE( multi_packed_flags_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "--foo",
    "-abc",
    "-def"
  };

  options = co::options_group{
    co::make_hidden_option("foo,f"),
    co::make_hidden_option(",a"),
    co::make_hidden_option(",b"),
    co::make_hidden_option(",c"),
    co::make_hidden_option(",d"),
    co::make_hidden_option(",e")
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  std::cerr << detail::to_string<std::string>(vm);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"a",{}},
      {"b",{}},
      {"c",{}},
      {"d",{}},
      {"e",{}},
      {"foo",{}},
      {"foo",{}}
    }));
}






BOOST_AUTO_TEST_SUITE_END()

