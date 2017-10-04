#include "cmd_options.h"

#include "test_detail.h"

#include <boost/test/unit_test.hpp>

#include <iostream>

BOOST_AUTO_TEST_SUITE( parse_test_suite )

namespace co = lemma::cmd_options;


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

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
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

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
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

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
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

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
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

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
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

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
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

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
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

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
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

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"foo",{std::string("bar")}},
      {"foo",{std::string("42")}}
    }));

//   std::cerr << detail::to_string<std::string>(vm);

  vm = co::parse_arguments(argv2.data(),argv2.size(),options);

//   std::cerr << detail::to_string<std::string>(vm);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
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

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"foo",{std::string("bar")}},
      {"foo",{std::string("42")}}
    }));

  vm = co::parse_arguments(argv2.data(),argv2.size(),options);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
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

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"foo",{std::string("bar")}},
      {"f",{std::string("42")}}
    }));

  vm = co::parse_arguments(argv2.data(),argv2.size(),options);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
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

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"foo",{std::string("bar")}},
      {"f",{std::string("42")}}
    }));

  vm = co::parse_arguments(argv2.data(),argv2.size(),options);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
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

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"",{std::string("--foo=bar14")}},
      {"",{std::string("-f42")}}
    }));

  vm = co::parse_arguments(argv2.data(),argv2.size(),options);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"",{std::string("--foo")}},
      {"",{std::string("-f")}}
    }));
}

/**
  case 14.2,
 */
BOOST_AUTO_TEST_CASE( case14_2_parse_test )
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
    co::make_options_error()
  };

  co::variable_map vm;

  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv1.data(),argv1.size(),options)),
    co::command_option_error);

//   std::cerr << detail::to_string<std::string>(vm);

  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv2.data(),argv2.size(),options)),
    co::command_option_error);
}

/**
  case 15,
 */
BOOST_AUTO_TEST_CASE( case15_parse_test )
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
    co::make_hidden_positional<std::string>(),
  };

  co::variable_map vm =
    co::parse_arguments(argv1.data(),argv1.size(),options);

//   std::cerr << detail::to_string<std::string>(vm);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"",{std::string("--foo=bar14")}},
      {"",{std::string("-f42")}}
    }));

  vm = co::parse_arguments(argv2.data(),argv2.size(),options);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"",{std::string("--foo")}},
      {"",{std::string("-f")}}
    }));
}

/**
  case 16,
 */
BOOST_AUTO_TEST_CASE( case16_parse_test )
{
  std::vector<const char *> argv{
    "--foo=bar14",
    "-f42",
    "--foo",
    "-f"
  };

  co::options_group options{
    co::make_positional<std::string>("key1","line 16")
  };

  co::variable_map vm =
    co::parse_arguments(argv.data(),argv.size(),options);

//   std::cerr << detail::to_string<std::string>(vm);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"key1",{std::string("--foo=bar14")}},
      {"key1",{std::string("-f42")}},
      {"key1",{std::string("--foo")}},
      {"key1",{std::string("-f")}}
    }));


  co::options_group options2{
    co::make_positional<std::string>("key1","line 16",0),
    co::make_positional<std::string>("key2","line 16",1)
  };

  vm =
    co::parse_arguments(argv.data(),2,options2);

  // std::cerr << detail::to_string<std::string>(vm);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"key1",{std::string("--foo=bar14")}},
      {"key2",{std::string("-f42")}}
    }));

  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv.data(),argv.size(),options2)),
    co::command_option_error);

  co::options_group options3{
    co::make_positional<std::string>("key1","line 16",0),
    co::make_positional<std::string>("key2","line 16",1),
    co::make_positional<std::string>("default","line 16",-1)
  };

  vm = co::parse_arguments(argv.data(),argv.size(),options3);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"key1",{std::string("--foo=bar14")}},
      {"key2",{std::string("-f42")}},
      {"default",{std::string("--foo")}},
      {"default",{std::string("-f")}}
      }));
}

/**
  case 17,
 */
BOOST_AUTO_TEST_CASE( case17_parse_test )
{
  std::vector<const char *> argv{
    "--foo=bar14",
    "-f42",
    "--foo",
    "-f"
  };

  co::options_group options{
    co::make_hidden_positional<std::string>("key1")
  };

  co::variable_map vm =
    co::parse_arguments(argv.data(),argv.size(),options);

//   std::cerr << detail::to_string<std::string>(vm);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"key1",{std::string("--foo=bar14")}},
      {"key1",{std::string("-f42")}},
      {"key1",{std::string("--foo")}},
      {"key1",{std::string("-f")}}
    }));


  co::options_group options2{
    co::make_hidden_positional<std::string>("key1",0),
    co::make_hidden_positional<std::string>("key2",1)
  };

  vm =
    co::parse_arguments(argv.data(),2,options2);

  // std::cerr << detail::to_string<std::string>(vm);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"key1",{std::string("--foo=bar14")}},
      {"key2",{std::string("-f42")}}
    }));

  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv.data(),argv.size(),options2)),
    co::command_option_error);

  co::options_group options3{
    co::make_hidden_positional<std::string>("key1",0),
    co::make_hidden_positional<std::string>("key2",1),
    co::make_hidden_positional<std::string>("default",-1)
  };

  vm = co::parse_arguments(argv.data(),argv.size(),options3);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"key1",{std::string("--foo=bar14")}},
      {"key2",{std::string("-f42")}},
      {"default",{std::string("--foo")}},
      {"default",{std::string("-f")}}
      }));
}

/**
  case 18,
 */
BOOST_AUTO_TEST_CASE( case18_parse_test )
{
  std::vector<const char *> argv{
    "--foo=bar14",
    "-f42",
    "--foo",
    "-f"
  };

  co::options_group options{
    co::make_positional("line 18"),
  };

  co::variable_map vm =
    co::parse_arguments(argv.data(),argv.size(),options);

//   std::cerr << detail::to_string<std::string>(vm);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"",{}},
      {"",{}},
      {"",{}},
      {"",{}}
    }));
}

/**
  case 19,
 */
BOOST_AUTO_TEST_CASE( case19_parse_test )
{
  std::vector<const char *> argv{
    "--foo=bar14",
    "-f42",
    "--foo",
    "-f"
  };

  co::options_group options{
    co::make_hidden_positional(),
  };

  co::variable_map vm =
    co::parse_arguments(argv.data(),argv.size(),options);

//   std::cerr << detail::to_string<std::string>(vm);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"",{}},
      {"",{}},
      {"",{}},
      {"",{}}
    }));
}

/**
  case 20,
 */
BOOST_AUTO_TEST_CASE( case20_parse_test )
{
  std::vector<const char *> argv{
    "--foo=bar14",
    "-f42",
    "--foo",
    "-f"
  };

  co::options_group options{
    co::make_positional("key1","line 20",0),
    co::make_positional("key2","line 20",1),
    co::make_positional("key3","line 20",2),
    co::make_positional("key4","line 20",3)
  };

  co::variable_map vm =
    co::parse_arguments(argv.data(),argv.size(),options);


  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"key1",{}},
      {"key2",{}},
      {"key3",{}},
      {"key4",{}}
    }));

  co::options_group options2{
    co::make_positional("key1","line 20",0),
    co::make_positional("key2","line 20",1),
  };

//   std::cerr << detail::to_string<std::string>(vm);

  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv.data(),argv.size(),options2)),
    co::command_option_error);

  co::options_group options3{
    co::make_positional("key1","line 20",0),
    co::make_positional("key2","line 20",1),
    co::make_positional("default","line 20",-1)
  };

  vm = co::parse_arguments(argv.data(),argv.size(),options3);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"key1",{}},
      {"key2",{}},
      {"default",{}},
      {"default",{}}
      }));
}

/**
  case 21,
 */
BOOST_AUTO_TEST_CASE( case21_parse_test )
{
  std::vector<const char *> argv{
    "--foo=bar14",
    "-f42",
    "--foo",
    "-f"
  };

  co::options_group options{
    co::make_hidden_positional("key1",0),
    co::make_hidden_positional("key2",1),
    co::make_hidden_positional("key3",2),
    co::make_hidden_positional("key4",3)
  };

  co::variable_map vm =
    co::parse_arguments(argv.data(),argv.size(),options);


  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"key1",{}},
      {"key2",{}},
      {"key3",{}},
      {"key4",{}}
    }));

  co::options_group options2{
    co::make_hidden_positional("key1",0),
    co::make_hidden_positional("key2",1),
  };

//   std::cerr << detail::to_string<std::string>(vm);

  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv.data(),argv.size(),options2)),
    co::command_option_error);

  co::options_group options3{
    co::make_hidden_positional("key1",0),
    co::make_hidden_positional("key2",1),
    co::make_hidden_positional("default",-1)
  };

  vm = co::parse_arguments(argv.data(),argv.size(),options3);

  BOOST_REQUIRE(detail::contents_equal<std::string>(vm,
    co::variable_map{
      {"key1",{}},
      {"key2",{}},
      {"default",{}},
      {"default",{}}
      }));
}


BOOST_AUTO_TEST_SUITE_END()

