#include "cmd_options.h"

#include "test_detail.h"

#include <boost/test/unit_test.hpp>

#include <iostream>

/**
  constraints test
 */

BOOST_AUTO_TEST_SUITE( constraints_test_suite )


namespace co = lemma::cmd_options;

typedef std::basic_string<detail::check_char_t> string_type;
typedef co::basic_option_pack<detail::check_char_t> option_pack_type;
typedef co::basic_option_description<detail::check_char_t>
  option_description_type;
typedef co::basic_options_group<detail::check_char_t> options_group_type;
typedef co::basic_variable_map<detail::check_char_t> variable_map_type;
typedef detail::std_stream_select<detail::check_char_t> stream_select;

/**
  option no restriction given 0
 */
BOOST_AUTO_TEST_CASE( option_no_restrictions_given_0_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--bar"),
  };

  options = options_group_type{
    co::make_option(_LIT("foo,f"),_LIT("case 2")),
    co::make_option(_LIT("bar,b"),_LIT("case 2"))
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {_LIT("bar"),{}}
    }));
}

/**
  option no restriction given 1
 */
BOOST_AUTO_TEST_CASE( option_no_restrictions_given_1_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo"),
  };

  options = options_group_type{
    co::make_option(_LIT("foo,f"),_LIT("case 2"))
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {_LIT("foo"),{}}
    }));
}

/**
  option strict 0 given 0
 */
BOOST_AUTO_TEST_CASE( option_degenerate_occurrences_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--bar"),
  };

  options = options_group_type{
    co::make_option(_LIT("foo,f"),
      _LIT("case 2"),co::basic_constraint<detail::check_char_t>().occurrences(0)),
    co::make_option(_LIT("bar,b"),_LIT("case 2"))
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {_LIT("bar"),{}}
    }));
}

/**
  option strict 0 given 1
 */
BOOST_AUTO_TEST_CASE( option_strict_0_given_1_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo"),
  };

  options = options_group_type{
    co::make_option(_LIT("foo,f"),_LIT("case 2"),
      co::basic_constraint<detail::check_char_t>().occurrences(0)),
  };

  BOOST_CHECK_EXCEPTION(co::parse_arguments(argv.size(),argv.data(),options),
    co::occurrence_error, [](const co::occurrence_error &ex) {
      return (ex.mapped_key() == co::asUTF8(string_type(_LIT("foo")))
        && ex.min() == 0 && ex.max() == 0 && ex.occurrences() == 1);
    }
  );
}

/**
  option Strictly 1 option given 0
 */
BOOST_AUTO_TEST_CASE( option_strict_1_given_0_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
  };

  options = options_group_type{
    co::make_option(_LIT("foo,f"),_LIT("case 2"),
      co::basic_constraint<detail::check_char_t>().occurrences(1))
  };

  BOOST_CHECK_EXCEPTION(co::parse_arguments(argv.size(),argv.data(),options),
    co::occurrence_error, [](const co::occurrence_error &ex) {
      return (ex.mapped_key() == co::asUTF8(string_type(_LIT("foo")))
        && ex.min() == 1 && ex.max() == 1 && ex.occurrences() == 0);
    }
  );
}

/**
  option Strictly 1 option given 1
 */
BOOST_AUTO_TEST_CASE( option_strict_1_given_1_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo"),
  };

  options = options_group_type{
    co::make_option(_LIT("foo,f"),_LIT("case 2"),
      co::basic_constraint<detail::check_char_t>().occurrences(1))
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {_LIT("foo"),{}}
    }));
}

/**
  option Strictly 1 option given 2
 */
BOOST_AUTO_TEST_CASE( option_strict_1_given_2_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo"),
    _LIT("-f")
  };

  options = options_group_type{
    co::make_option(_LIT("foo,f"),_LIT("case 2"),
      co::basic_constraint<detail::check_char_t>().occurrences(1))
  };

  BOOST_CHECK_EXCEPTION(co::parse_arguments(argv.size(),argv.data(),options),
    co::occurrence_error, [](const co::occurrence_error &ex) {
      return (ex.mapped_key() == co::asUTF8(string_type(_LIT("foo")))
        && ex.min() == 1 && ex.max() == 1 && ex.occurrences() == 2);
    }
  );
}




/**
  operand no restriction given 0
 */
BOOST_AUTO_TEST_CASE( operand_no_restrictions_given_0_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo"),
  };

  options = options_group_type{
    co::make_option(_LIT("foo,f"),_LIT("case 2")),
    co::make_operand(_LIT("case 14"),co::value<string_type>())
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {_LIT("foo"),{}}
    }));
}

/**
  operand no restriction given 1
 */
BOOST_AUTO_TEST_CASE( operand_no_restrictions_given_1_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--bar"),
  };

  options = options_group_type{
    co::make_operand(_LIT("case 14"),co::value<string_type>())
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {co::default_operand_key<detail::check_char_t>(),
        {string_type(_LIT("--bar"))}}
    }));
}

/**
  operand strict 0 given 0
 */
BOOST_AUTO_TEST_CASE( operand_strict_0_given_0_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--bar"),
  };

  options = options_group_type{
    co::make_operand(_LIT("case 14"),co::value<string_type>(),
      co::basic_constraint<detail::check_char_t>().occurrences(0)),
    co::make_option(_LIT("bar,b"),_LIT("case 2"))
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {_LIT("bar"),{}}
    }));
}

/**
  operand strict 0 given 1
 */
BOOST_AUTO_TEST_CASE( operand_strict_0_given_1_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--bar"),
  };

  options = options_group_type{
    co::make_operand(_LIT("case 14"),co::value<string_type>(),
      co::basic_constraint<detail::check_char_t>().occurrences(0))
  };

  BOOST_CHECK_EXCEPTION(co::parse_arguments(argv.size(),argv.data(),options),
    co::occurrence_error, [](const co::occurrence_error &ex) {
      return
        (ex.mapped_key() ==
          co::asUTF8(co::default_operand_key<detail::check_char_t>())
        && ex.min() == 0 && ex.max() == 0 && ex.occurrences() == 1);
    }
  );
}

/**
  operand strict 1 given 0
 */
BOOST_AUTO_TEST_CASE( operand_strict_1_given_0_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
  };

  options = options_group_type{
    co::make_operand(_LIT("case 14"),co::value<string_type>(),
      co::basic_constraint<detail::check_char_t>().occurrences(1))
  };

  BOOST_CHECK_EXCEPTION(co::parse_arguments(argv.size(),argv.data(),options),
    co::occurrence_error, [](const co::occurrence_error &ex) {
      return
        (ex.mapped_key() ==
          co::asUTF8(co::default_operand_key<detail::check_char_t>())
        && ex.min() == 1 && ex.max() == 1 && ex.occurrences() == 0);
    }
  );
}

/**
  operand strict 1 given 1
 */
BOOST_AUTO_TEST_CASE( operand_strict_1_given_1_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--bar"),
  };

  options = options_group_type{
    co::make_operand(_LIT("case 14"),co::value<string_type>(),
      co::basic_constraint<detail::check_char_t>().occurrences(1))
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {co::default_operand_key<detail::check_char_t>(),
        {string_type(_LIT("--bar"))}}
    }));
}

/**
  operand strict 1 given 2
 */
BOOST_AUTO_TEST_CASE( operand_strict_1_given_2_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo"),
    _LIT("-f")
  };

  options = options_group_type{
    co::make_operand(_LIT("case 14"),co::value<string_type>(),
      co::basic_constraint<detail::check_char_t>().occurrences(1))
  };

  BOOST_CHECK_EXCEPTION(co::parse_arguments(argv.size(),argv.data(),options),
    co::occurrence_error, [](const co::occurrence_error &ex) {
      return
        (ex.mapped_key() ==
          co::asUTF8(co::default_operand_key<detail::check_char_t>())
        && ex.min() == 1 && ex.max() == 1 && ex.occurrences() == 2);
    }
  );
}

/**
  operand 0 or 1 given 0
 */
BOOST_AUTO_TEST_CASE( operand_0_1_given_0_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo"),
    _LIT("-f")
  };

  options = options_group_type{
    co::make_operand(_LIT("case 14"),co::value<string_type>(),
      co::basic_constraint<detail::check_char_t>().occurrences(0,1)),
    co::make_option(_LIT("foo,f"),_LIT("case 2"))
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {_LIT("foo"),{}},
      {_LIT("foo"),{}}
    }));
}

/**
  operand 0 or 1 given 1
 */
BOOST_AUTO_TEST_CASE( operand_0_1_given_1_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo"),
    _LIT("-f"),
    _LIT("bar")
  };

  options = options_group_type{
    co::make_operand(_LIT("case 14"),co::value<string_type>(),
      co::basic_constraint<detail::check_char_t>().occurrences(0,1)),
    co::make_option(_LIT("foo,f"),_LIT("case 2"))
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {co::default_operand_key<detail::check_char_t>(),
        {string_type(_LIT("bar"))}},
      {_LIT("foo"),{}},
      {_LIT("foo"),{}}
    }));
}

/**
  operand 0 or 1 given 2
 */
BOOST_AUTO_TEST_CASE( operand_0_1_given_2_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo"),
    _LIT("-f"),
    _LIT("bar1"),
    _LIT("bar2")
  };

  options = options_group_type{
    co::make_operand(_LIT("case 14"),co::value<string_type>(),
      co::basic_constraint<detail::check_char_t>().occurrences(0,1)),
    co::make_option(_LIT("foo,f"),_LIT("case 2"))
  };

  BOOST_CHECK_EXCEPTION(co::parse_arguments(argv.size(),argv.data(),options),
    co::occurrence_error, [](const co::occurrence_error &ex) {
      return
        (ex.mapped_key() ==
          co::asUTF8(co::default_operand_key<detail::check_char_t>())
        && ex.min() == 0 && ex.max() == 1 && ex.occurrences() == 2);
    }
  );
}


/*
  Position and argument checks
*/

/**
  unconstrained operand argument
 */
BOOST_AUTO_TEST_CASE( operand_unconstrained_argument )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("bar1"),
    _LIT("--foo"),
    _LIT("bar2"),
    _LIT("-f"),
    _LIT("bar3")
  };

  options = options_group_type{
    co::make_operand(_LIT("case 14"),co::value<string_type>(),
      co::basic_constraint<detail::check_char_t>().at_argument(-1)),
    co::make_option(_LIT("foo,f"),_LIT("case 2"))
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

//   stream_select::cerr << detail::to_string(vm,co::value<string_type>());

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {co::default_operand_key<detail::check_char_t>(),
        {string_type(_LIT("bar1"))}},
      {co::default_operand_key<detail::check_char_t>(),
        {string_type(_LIT("bar2"))}},
      {co::default_operand_key<detail::check_char_t>(),
        {string_type(_LIT("bar3"))}},
      {_LIT("foo"),{}},
      {_LIT("foo"),{}}
    }));
}

/**
  operand strictly at argument 0
 */
BOOST_AUTO_TEST_CASE( operand_argument_0_given_0 )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("bar"),
    _LIT("--foo")
  };

  options = options_group_type{
    co::make_operand(_LIT("case 14"),co::value<string_type>(),
      co::basic_constraint<detail::check_char_t>().at_argument(0)),
    co::make_option(_LIT("foo,f"),_LIT("case 2"))
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {co::default_operand_key<detail::check_char_t>(),
        {string_type(_LIT("bar"))}},
      {_LIT("foo"),{}}
    }));
}

/**
  operand strictly at argument 0 given 1
 */
BOOST_AUTO_TEST_CASE( operand_argument_0_given_1 )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo"),
    _LIT("bar")
  };

  options = options_group_type{
    co::make_operand(_LIT("case 14"),co::value<string_type>(),
      co::basic_constraint<detail::check_char_t>().at_argument(0)),
    co::make_option(_LIT("foo,f"),_LIT("case 2"))
  };

  BOOST_CHECK_EXCEPTION(co::parse_arguments(argv.size(),argv.data(),options),
    co::unexpected_operand_error, [](const co::unexpected_operand_error &ex) {
      return (ex.position() == 0 && ex.argument() == 1);
    }
  );
}


/**
  unconstrained operand position
 */
BOOST_AUTO_TEST_CASE( operand_unconstrained_position )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("bar1"),
    _LIT("--foo"),
    _LIT("bar2"),
    _LIT("-f"),
    _LIT("bar3")
  };

  options = options_group_type{
    co::make_operand(_LIT("case 14"),co::value<string_type>(),
      co::basic_constraint<detail::check_char_t>().at_position(-1)),
    co::make_option(_LIT("foo,f"),_LIT("case 2"))
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

//   stream_select::cerr << detail::to_string(vm,co::value<string_type>());

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {co::default_operand_key<detail::check_char_t>(),
        {string_type(_LIT("bar1"))}},
      {co::default_operand_key<detail::check_char_t>(),
        {string_type(_LIT("bar2"))}},
      {co::default_operand_key<detail::check_char_t>(),
        {string_type(_LIT("bar3"))}},
      {_LIT("foo"),{}},
      {_LIT("foo"),{}}
    }));
}

/**
  operand strictly at position 0
 */
BOOST_AUTO_TEST_CASE( operand_position_0_given_0 )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo"),
    _LIT("bar")
  };

  options = options_group_type{
    co::make_operand(_LIT("case 14"),co::value<string_type>(),
      co::basic_constraint<detail::check_char_t>().at_position(0)),
    co::make_option(_LIT("foo,f"),_LIT("case 2"))
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {co::default_operand_key<detail::check_char_t>(),
        {string_type(_LIT("bar"))}},
      {_LIT("foo"),{}}
    }));
}

/**
  operand strictly at position 0 given 1
 */
BOOST_AUTO_TEST_CASE( operand_position_0_given_1 )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo"),
    _LIT("bar0"),
    _LIT("bar1")
  };

  options = options_group_type{
    co::make_operand(_LIT("case 14"),co::value<string_type>(),
      co::basic_constraint<detail::check_char_t>().at_position(0)),
    co::make_option(_LIT("foo,f"),_LIT("case 2"))
  };

  BOOST_CHECK_EXCEPTION(co::parse_arguments(argv.size(),argv.data(),options),
    co::unexpected_operand_error, [](const co::unexpected_operand_error &ex) {
      return (ex.position() == 1 && ex.argument() == 2);
    }
  );
}

/**
  option (non) mutual exclusion
*/
BOOST_AUTO_TEST_CASE( option_non_mutual_exclusion_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo")
  };

  options = options_group_type{
    co::make_option(_LIT("foo,f"),_LIT("case 2"),
      co::basic_constraint<detail::check_char_t>().mutually_exclusive(
        {_LIT("bar"),_LIT("baz"),_LIT("foobar")})),
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

//   stream_select::cerr << detail::to_string(vm,co::value<string_type>());

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {_LIT("foo"),{}}
    }));
}

/**
  option mutual exclusion
*/
BOOST_AUTO_TEST_CASE( option_mutual_exclusion_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo"),
    _LIT("--bar")
  };

  options = options_group_type{
    co::make_option(_LIT("bar,b"),_LIT("case 2")),
    co::make_option(_LIT("foo,f"),_LIT("case 2"),
      co::basic_constraint<detail::check_char_t>().mutually_exclusive(
        {_LIT("bar")})),
  };

  BOOST_CHECK_EXCEPTION(co::parse_arguments(argv.size(),argv.data(),options),
    co::mutually_exclusive_error, [](const co::mutually_exclusive_error &ex) {
      return (ex.mapped_key() == co::asUTF8(string_type(_LIT("foo")))
        && ex.exclusive_mapped_key() == co::asUTF8(string_type(_LIT("bar"))));
    }
  );
}

/**
  option (non) mutual inclusion
*/
BOOST_AUTO_TEST_CASE( option_non_mutual_inclusion_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo"),
    _LIT("--bar")
  };

  options = options_group_type{
    co::make_option(_LIT("bar,b"),_LIT("case 2")),
    co::make_option(_LIT("foo,f"),_LIT("case 2"),
      co::basic_constraint<detail::check_char_t>().mutually_inclusive(
        {_LIT("bar")})),
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

//   stream_select::cerr << detail::to_string(vm,co::value<string_type>());

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {_LIT("bar"),{}},
      {_LIT("foo"),{}}
    }));
}

/**
  option mutual inclusion (actual)
*/
BOOST_AUTO_TEST_CASE( option_mutual_inclusion_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo")
  };

  options = options_group_type{
    co::make_option(_LIT("bar,b"),_LIT("case 2")),
    co::make_option(_LIT("foo,f"),_LIT("case 2"),
      co::basic_constraint<detail::check_char_t>().mutually_inclusive(
        {_LIT("bar")})),
  };

  BOOST_CHECK_EXCEPTION(co::parse_arguments(argv.size(),argv.data(),options),
    co::mutually_inclusive_error, [](const co::mutually_inclusive_error &ex) {
      return (ex.mapped_key() == co::asUTF8(string_type(_LIT("foo")))
        && ex.inclusive_mapped_key() == co::asUTF8(string_type(_LIT("bar"))));
    }
  );
}

BOOST_AUTO_TEST_SUITE_END()

