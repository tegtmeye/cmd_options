#include "cmd_options.h"

#include "test_detail.h"

#include <boost/test/unit_test.hpp>

#include <iostream>

BOOST_AUTO_TEST_SUITE( parse_test_suite )

namespace co = lemma::cmd_options;

template<typename T, typename VariableMap>
bool contents_equal(const VariableMap &lhs, const VariableMap &rhs)
{
  typename VariableMap::const_iterator lcur = lhs.begin();
  typename VariableMap::const_iterator rcur = rhs.begin();

  while(lcur != lhs.end() && rcur != rhs.end()) {
    if(lcur->first != rcur->first) {
       std::cerr << "lhs vm key '" << lcur->first << "' != rhs vm key '"
         << rcur->first << "'\n";
      return false;
    }

    if(!(co::is_empty(lcur->second) && co::is_empty(rcur->second))) {
      if(co::is_empty(lcur->second) && !co::is_empty(rcur->second)) {
         std::cerr << "lhs vm value for key '" << lcur->first
           << "' is empty but rhs vm value for key '" << rcur->first
           << "' is not\n";
        return false;
      }

      if(!co::is_empty(lcur->second) && co::is_empty(rcur->second)) {
         std::cerr << "lhs vm value for key '" << lcur->first
           << "' is not empty but rhs vm value for key '" << rcur->first
           << "' is\n";
        return false;
      }

      try {
        if(co::any_cast<T>(lcur->second) != co::any_cast<T>(rcur->second))
          return false;
      }
      catch(const co::bad_any_cast &ex) {
        std::cerr << "vm values are not given type T: " << ex.what() << "\n";
        throw;
      }
    }

    ++lcur;
    ++rcur;
  }

  return lcur == lhs.end() && rcur == rhs.end();
}


/**
  case 2, strictly no value
 */
BOOST_AUTO_TEST_CASE( case2_parse_test )
{
  std::vector<const char *> argv_good{
    "--foo",
    "-f"
  };

  std::vector<const char *> argv_bad{
    "--foo=42",
    "-f",
    "42"
  };

  co::options_group options{
    co::make_option("foo,f","line 2"),
  };

  co::variable_map vm =
    co::parse_arguments(argv_good.data(),argv_good.size(),options);

//   std::cerr << detail::to_string<std::string>(vm);

  BOOST_REQUIRE(contents_equal<std::string>(vm,
    co::variable_map{{"foo",{}},{"foo",{}}}));

  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv_bad.data(),argv_bad.size(),options)),
    co::command_option_error);
}

/**
  case 3, hidden strictly no value
 */
BOOST_AUTO_TEST_CASE( case3_parse_test )
{
  std::vector<const char *> argv_good{
    "--foo",
    "-f"
  };

  std::vector<const char *> argv_bad{
    "--foo=42",
    "-f",
    "42"
  };

  co::options_group options{
    co::make_hidden_option("foo,f"),
  };

  co::variable_map vm =
    co::parse_arguments(argv_good.data(),argv_good.size(),options);

  BOOST_REQUIRE(contents_equal<std::string>(vm,
    co::variable_map{{"foo",{}},{"foo",{}}}));

  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv_bad.data(),argv_bad.size(),options)),
    co::command_option_error);
}

/**
  case 4, Raw keys with strictly no value
 */
BOOST_AUTO_TEST_CASE( case4_parse_test )
{
  std::vector<const char *> argv_good{
    "--foo",
    "-f"
  };

  std::vector<const char *> argv_bad{
    "--foo=42",
    "-f",
    "42"
  };

  co::options_group options{
    co::make_option("","line 4"),
  };

  co::variable_map vm =
    co::parse_arguments(argv_good.data(),argv_good.size(),options);

//   std::cerr << detail::to_string<std::string>(vm);

  BOOST_REQUIRE(contents_equal<std::string>(vm,
    co::variable_map{{"foo",{}},{"f",{}}}));

  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv_bad.data(),argv_bad.size(),options)),
    co::command_option_error);
}

/**
  case 5, Hidden raw keys with strictly no value
 */
BOOST_AUTO_TEST_CASE( case5_parse_test )
{
  std::vector<const char *> argv_good{
    "--foo",
    "-f"
  };

  std::vector<const char *> argv_bad{
    "--foo=42",
    "-f",
    "42"
  };

  co::options_group options{
    co::make_hidden_option(""),
  };

  co::variable_map vm =
    co::parse_arguments(argv_good.data(),argv_good.size(),options);

  BOOST_REQUIRE(contents_equal<std::string>(vm,
    co::variable_map{{"foo",{}},{"f",{}}}));

  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv_bad.data(),argv_bad.size(),options)),
    co::command_option_error);
}

/**
  case 6, Keys with strictly required value
 */
BOOST_AUTO_TEST_CASE( case6_parse_test )
{
  std::vector<const char *> argv_good{
    "--foo=bar",
    "-f42"
  };

  std::vector<const char *> argv_bad{
    "--foo"
  };

  std::vector<const char *> argv_bad2{
    "-f"
  };

  co::options_group options{
    co::make_option("foo,f",co::value<std::string>(),"line 6"),
  };

  co::variable_map vm =
    co::parse_arguments(argv_good.data(),argv_good.size(),options);

  BOOST_REQUIRE(contents_equal<std::string>(vm,
    co::variable_map{
      {"foo",{std::string("bar")}},
      {"foo",{std::string("42")}}
    }));

  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv_bad.data(),argv_bad.size(),options)),
    co::command_option_error);

  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv_bad2.data(),argv_bad2.size(),options)),
    co::command_option_error);
}

/**
  case 7, Hidden keys with strictly required value
 */
BOOST_AUTO_TEST_CASE( case7_parse_test )
{
  std::vector<const char *> argv_good{
    "--foo=bar",
    "-f42"
  };

  std::vector<const char *> argv_bad{
    "--foo"
  };

  std::vector<const char *> argv_bad2{
    "-f"
  };

  co::options_group options{
    co::make_hidden_option("foo,f",co::value<std::string>()),
  };

  co::variable_map vm =
    co::parse_arguments(argv_good.data(),argv_good.size(),options);

  BOOST_REQUIRE(contents_equal<std::string>(vm,
    co::variable_map{
      {"foo",{std::string("bar")}},
      {"foo",{std::string("42")}}
    }));

  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv_bad.data(),argv_bad.size(),options)),
    co::command_option_error);

  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv_bad2.data(),argv_bad2.size(),options)),
    co::command_option_error);
}

/**
  case 8, Raw keys with strictly required value
 */
BOOST_AUTO_TEST_CASE( case8_parse_test )
{
  std::vector<const char *> argv_good{
    "--foo=bar",
    "-f42"
  };

  std::vector<const char *> argv_bad{
    "--foo"
  };

  std::vector<const char *> argv_bad2{
    "-f"
  };

  co::options_group options{
    co::make_option("",co::value<std::string>(),"line 8"),
  };

  co::variable_map vm =
    co::parse_arguments(argv_good.data(),argv_good.size(),options);

  BOOST_REQUIRE(contents_equal<std::string>(vm,
    co::variable_map{
      {"foo",{std::string("bar")}},
      {"f",{std::string("42")}}
    }));

  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv_bad.data(),argv_bad.size(),options)),
    co::command_option_error);

  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv_bad2.data(),argv_bad2.size(),options)),
    co::command_option_error);
}

/**
  case 9, Hidden raw keys with strictly required value
 */
BOOST_AUTO_TEST_CASE( case9_parse_test )
{
  std::vector<const char *> argv_good{
    "--foo=bar",
    "-f42"
  };

  std::vector<const char *> argv_bad{
    "--foo"
  };

  std::vector<const char *> argv_bad2{
    "-f"
  };

  co::options_group options{
    co::make_hidden_option("",co::value<std::string>()),
  };

  co::variable_map vm =
    co::parse_arguments(argv_good.data(),argv_good.size(),options);

  BOOST_REQUIRE(contents_equal<std::string>(vm,
    co::variable_map{
      {"foo",{std::string("bar")}},
      {"f",{std::string("42")}}
    }));

  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv_bad.data(),argv_bad.size(),options)),
    co::command_option_error);

  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv_bad2.data(),argv_bad2.size(),options)),
    co::command_option_error);
}

/**
  case 10, Keys with optional value
 */
BOOST_AUTO_TEST_CASE( case10_parse_test )
{
  std::vector<const char *> argv1{
    "--foo=bar",
    "-f42"
  };

  std::vector<const char *> argv2{
    "--foo",
    "-f"
  };

  co::options_group options{
    co::make_option("foo,f",co::value<std::string>("blarp"),"line 10"),
  };

  co::variable_map vm =
    co::parse_arguments(argv1.data(),argv1.size(),options);

  BOOST_REQUIRE(contents_equal<std::string>(vm,
    co::variable_map{
      {"foo",{std::string("bar")}},
      {"foo",{std::string("42")}}
    }));

//   std::cerr << detail::to_string<std::string>(vm);

  vm = co::parse_arguments(argv2.data(),argv2.size(),options);

//   std::cerr << detail::to_string<std::string>(vm);

  BOOST_REQUIRE(contents_equal<std::string>(vm,
    co::variable_map{
      {"foo",{std::string("blarp")}},
      {"foo",{std::string("blarp")}}
    }));
}

/**
  case 11, Hidden keys with optional value
 */
BOOST_AUTO_TEST_CASE( case11_parse_test )
{
  std::vector<const char *> argv1{
    "--foo=bar",
    "-f42"
  };

  std::vector<const char *> argv2{
    "--foo",
    "-f"
  };

  co::options_group options{
    co::make_hidden_option("foo,f",co::value<std::string>("blarp")),
  };

  co::variable_map vm =
    co::parse_arguments(argv1.data(),argv1.size(),options);

  BOOST_REQUIRE(contents_equal<std::string>(vm,
    co::variable_map{
      {"foo",{std::string("bar")}},
      {"foo",{std::string("42")}}
    }));

  vm = co::parse_arguments(argv2.data(),argv2.size(),options);

  BOOST_REQUIRE(contents_equal<std::string>(vm,
    co::variable_map{
      {"foo",{std::string("blarp")}},
      {"foo",{std::string("blarp")}}
    }));
}

/**
  case 12, Raw keys with optional value
 */
BOOST_AUTO_TEST_CASE( case12_parse_test )
{
  std::vector<const char *> argv1{
    "--foo=bar",
    "-f42"
  };

  std::vector<const char *> argv2{
    "--foo",
    "-f"
  };

  co::options_group options{
    co::make_option("",co::value<std::string>("blarp"),"line 10"),
  };

  co::variable_map vm =
    co::parse_arguments(argv1.data(),argv1.size(),options);

  BOOST_REQUIRE(contents_equal<std::string>(vm,
    co::variable_map{
      {"foo",{std::string("bar")}},
      {"f",{std::string("42")}}
    }));

  vm = co::parse_arguments(argv2.data(),argv2.size(),options);

  BOOST_REQUIRE(contents_equal<std::string>(vm,
    co::variable_map{
      {"foo",{std::string("blarp")}},
      {"f",{std::string("blarp")}}
    }));
}

/**
  case 13, Hidden raw keys with optional value
 */
BOOST_AUTO_TEST_CASE( case13_parse_test )
{
  std::vector<const char *> argv1{
    "--foo=bar",
    "-f42"
  };

  std::vector<const char *> argv2{
    "--foo",
    "-f"
  };

  co::options_group options{
    co::make_hidden_option("",co::value<std::string>("blarp")),
  };

  co::variable_map vm =
    co::parse_arguments(argv1.data(),argv1.size(),options);

  BOOST_REQUIRE(contents_equal<std::string>(vm,
    co::variable_map{
      {"foo",{std::string("bar")}},
      {"f",{std::string("42")}}
    }));

  vm = co::parse_arguments(argv2.data(),argv2.size(),options);

  BOOST_REQUIRE(contents_equal<std::string>(vm,
    co::variable_map{
      {"foo",{std::string("blarp")}},
      {"f",{std::string("blarp")}}
    }));
}


/*
  Positional
*/

/**
  case 14,
 */
BOOST_AUTO_TEST_CASE( case14_parse_test )
{
  std::vector<const char *> argv1{
    "--foo=bar14",
    "-f42"
  };

  std::vector<const char *> argv2{
    "--foo",
    "-f"
  };

  co::options_group options{
    co::make_positional<std::string>("line 14"),
  };

  co::variable_map vm =
    co::parse_arguments(argv1.data(),argv1.size(),options);

//   std::cerr << detail::to_string<std::string>(vm);

  BOOST_REQUIRE(contents_equal<std::string>(vm,
    co::variable_map{
      {"",{std::string("--foo=bar14")}},
      {"",{std::string("-f42")}}
    }));

  vm = co::parse_arguments(argv2.data(),argv2.size(),options);

  BOOST_REQUIRE(contents_equal<std::string>(vm,
    co::variable_map{
      {"",{std::string("--foo")}},
      {"",{std::string("-f")}}
    }));
}


BOOST_AUTO_TEST_SUITE_END()

