#include "cmd_options.h"

#include "test_detail.h"

#include <boost/test/unit_test.hpp>

#include <iostream>

/**
  constraints test
 */

BOOST_AUTO_TEST_SUITE( constraints_test_suite )


namespace co = lemma::cmd_options;

/**
  option no restriction given 0
 */
BOOST_AUTO_TEST_CASE( option_no_restrictions_given_0_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "--bar",
  };

  options = co::options_group{
    co::make_option("foo,f","case 2"),
    co::make_option("bar,b","case 2")
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"bar",{}}
    }));
}

/**
  option no restriction given 1
 */
BOOST_AUTO_TEST_CASE( option_no_restrictions_given_1_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "--foo",
  };

  options = co::options_group{
    co::make_option("foo,f","case 2")
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"foo",{}}
    }));
}

/**
  option strict 0 given 0
 */
BOOST_AUTO_TEST_CASE( option_degenerate_occurrences_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "--bar",
  };

  options = co::options_group{
    co::make_option("foo,f","case 2",co::constrain().occurrences(0)),
    co::make_option("bar,b","case 2")
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"bar",{}}
    }));
}

/**
  option strict 0 given 1
 */
BOOST_AUTO_TEST_CASE( option_strict_0_given_1_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "--foo",
  };

  options = co::options_group{
    co::make_option("foo,f","case 2",co::constrain().occurrences(0)),
  };

  BOOST_CHECK_EXCEPTION(co::parse_arguments(argv.size(),argv.data(),options),
    co::occurrence_error, [](const co::occurrence_error &ex) {
      return (ex.mapped_key() == std::string("foo")  && ex.min() == 0
        && ex.max() == 0 && ex.occurrences() == 1);
    }
  );
}

/**
  option Strictly 1 option given 0
 */
BOOST_AUTO_TEST_CASE( option_strict_1_given_0_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
  };

  options = co::options_group{
    co::make_option("foo,f","case 2",co::constrain().occurrences(1))
  };

  BOOST_CHECK_EXCEPTION(co::parse_arguments(argv.size(),argv.data(),options),
    co::occurrence_error, [](const co::occurrence_error &ex) {
      return (ex.mapped_key() == std::string("foo")  && ex.min() == 1
        && ex.max() == 1 && ex.occurrences() == 0);
    }
  );
}

/**
  option Strictly 1 option given 1
 */
BOOST_AUTO_TEST_CASE( option_strict_1_given_1_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "--foo",
  };

  options = co::options_group{
    co::make_option("foo,f","case 2",co::constrain().occurrences(1))
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"foo",{}}
    }));
}

/**
  option Strictly 1 option given 2
 */
BOOST_AUTO_TEST_CASE( option_strict_1_given_2_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "--foo",
    "-f"
  };

  options = co::options_group{
    co::make_option("foo,f","case 2",co::constrain().occurrences(1))
  };

  BOOST_CHECK_EXCEPTION(co::parse_arguments(argv.size(),argv.data(),options),
    co::occurrence_error, [](const co::occurrence_error &ex) {
      return (ex.mapped_key() == std::string("foo")  && ex.min() == 1
        && ex.max() == 1 && ex.occurrences() == 2);
    }
  );
}




/**
  operand no restriction given 0
 */
BOOST_AUTO_TEST_CASE( operand_no_restrictions_given_0_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "--foo",
  };

  options = co::options_group{
    co::make_option("foo,f","case 2"),
    co::make_operand<std::string>("case 14")
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"foo",{}}
    }));
}

/**
  operand no restriction given 1
 */
BOOST_AUTO_TEST_CASE( operand_no_restrictions_given_1_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "--bar",
  };

  options = co::options_group{
    co::make_operand<std::string>("case 14")
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {co::default_operand_key,{std::string("--bar")}}
    }));
}

/**
  operand strict 0 given 0
 */
BOOST_AUTO_TEST_CASE( operand_strict_0_given_0_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "--bar",
  };

  options = co::options_group{
    co::make_operand<std::string>("case 14",co::constrain().occurrences(0)),
    co::make_option("bar,b","case 2")
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"bar",{}}
    }));
}

/**
  operand strict 0 given 1
 */
BOOST_AUTO_TEST_CASE( operand_strict_0_given_1_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "--bar",
  };

  options = co::options_group{
    co::make_operand<std::string>("case 14",co::constrain().occurrences(0))
  };

  BOOST_CHECK_EXCEPTION(co::parse_arguments(argv.size(),argv.data(),options),
    co::occurrence_error, [](const co::occurrence_error &ex) {
      return (ex.mapped_key() == co::default_operand_key  && ex.min() == 0
        && ex.max() == 0 && ex.occurrences() == 1);
    }
  );
}

/**
  operand strict 1 given 0
 */
BOOST_AUTO_TEST_CASE( operand_strict_1_given_0_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
  };

  options = co::options_group{
    co::make_operand<std::string>("case 14",co::constrain().occurrences(1))
  };

  BOOST_CHECK_EXCEPTION(co::parse_arguments(argv.size(),argv.data(),options),
    co::occurrence_error, [](const co::occurrence_error &ex) {
      return (ex.mapped_key() == co::default_operand_key  && ex.min() == 1
        && ex.max() == 1 && ex.occurrences() == 0);
    }
  );
}

/**
  operand strict 1 given 1
 */
BOOST_AUTO_TEST_CASE( operand_strict_1_given_1_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "--bar",
  };

  options = co::options_group{
    co::make_operand<std::string>("case 14",co::constrain().occurrences(1))
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {co::default_operand_key,{std::string("--bar")}}
    }));
}

/**
  operand strict 1 given 2
 */
BOOST_AUTO_TEST_CASE( operand_strict_1_given_2_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "--foo",
    "-f"
  };

  options = co::options_group{
    co::make_operand<std::string>("case 14",co::constrain().occurrences(1))
  };

  BOOST_CHECK_EXCEPTION(co::parse_arguments(argv.size(),argv.data(),options),
    co::occurrence_error, [](const co::occurrence_error &ex) {
      return (ex.mapped_key() == co::default_operand_key  && ex.min() == 1
        && ex.max() == 1 && ex.occurrences() == 2);
    }
  );
}

/**
  operand 0 or 1 given 0
 */
BOOST_AUTO_TEST_CASE( operand_0_1_given_0_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "--foo",
    "-f"
  };

  options = co::options_group{
    co::make_operand<std::string>("case 14",co::constrain().occurrences(0,1)),
    co::make_option("foo,f","case 2")
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"foo",{}},
      {"foo",{}}
    }));
}

/**
  operand 0 or 1 given 1
 */
BOOST_AUTO_TEST_CASE( operand_0_1_given_1_test )
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
    co::make_operand<std::string>("case 14",co::constrain().occurrences(0,1)),
    co::make_option("foo,f","case 2")
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {co::default_operand_key,{std::string("bar")}},
      {"foo",{}},
      {"foo",{}}
    }));
}

/**
  operand 0 or 1 given 2
 */
BOOST_AUTO_TEST_CASE( operand_0_1_given_2_test )
{
  co::variable_map vm;
  co::options_group options;
  std::vector<const char *> argv;

  argv = std::vector<const char *>{
    "--foo",
    "-f",
    "bar1",
    "bar2"
  };

  options = co::options_group{
    co::make_operand<std::string>("case 14",co::constrain().occurrences(0,1)),
    co::make_option("foo,f","case 2")
  };

  BOOST_CHECK_EXCEPTION(co::parse_arguments(argv.size(),argv.data(),options),
    co::occurrence_error, [](const co::occurrence_error &ex) {
      return (ex.mapped_key() == co::default_operand_key  && ex.min() == 0
        && ex.max() == 1 && ex.occurrences() == 2);
    }
  );
}


BOOST_AUTO_TEST_SUITE_END()

