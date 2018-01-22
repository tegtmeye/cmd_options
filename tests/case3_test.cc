#include "cmd_options.h"

#include "test_detail.h"

#include <boost/test/unit_test.hpp>

#include <iostream>

/**
  case 3, strictly no value
 */

BOOST_AUTO_TEST_SUITE( case3_test_suite )


namespace co = lemma::cmd_options;

typedef std::basic_string<detail::check_char_t> string_type;
typedef co::basic_option_pack<detail::check_char_t> option_pack_type;
typedef co::basic_option_description<detail::check_char_t>
  option_description_type;
typedef co::basic_options_group<detail::check_char_t> options_group_type;
typedef co::basic_variable_map<detail::check_char_t> variable_map_type;
typedef detail::std_stream_select<detail::check_char_t> stream_select;

/**
  All flags
 */
BOOST_AUTO_TEST_CASE( all_flags_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo"),
    _LIT("-f")
  };

  options = options_group_type{
    co::make_hidden_option(_LIT("foo,f"))
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {_LIT("foo"),{}},
      {_LIT("foo"),{}}
    }));
}


/**
  All flags embedded cease
 */
BOOST_AUTO_TEST_CASE( all_flags_cease_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo"),
    _LIT("-f"),
    _LIT("--"),
    _LIT("--bar"),
    _LIT("blah")
  };

  options = options_group_type{
    co::make_hidden_option(_LIT("foo,f")),
    co::make_operand(_LIT("accept all operands"),co::value<string_type>())
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

//   stream_select::cerr << detail::to_string(vm,co::value<string_type>());

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {co::default_operand_key<detail::check_char_t>(),
        string_type(_LIT("--bar"))},
      {co::default_operand_key<detail::check_char_t>(),
        string_type(_LIT("blah"))},
      {_LIT("foo"),{}},
      {_LIT("foo"),{}}
    }));
}

/**
  Packed flags
 */
BOOST_AUTO_TEST_CASE( packed_flags_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo"),
    _LIT("-f"),
    _LIT("-abcd")
  };

  options = options_group_type{
    co::make_hidden_option(_LIT("foo,f")),
    co::make_hidden_option(_LIT(",a")),
    co::make_hidden_option(_LIT(",b")),
    co::make_hidden_option(_LIT(",c")),
    co::make_hidden_option(_LIT(",d"))
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

//   stream_select::cerr << detail::to_string(vm,co::value<string_type>());

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {_LIT("foo"),{}},
      {_LIT("foo"),{}},
      {_LIT("a"),{}},
      {_LIT("b"),{}},
      {_LIT("c"),{}},
      {_LIT("d"),{}}
    }));
}

/**
  Packed flags embedded cease
 */
BOOST_AUTO_TEST_CASE( packed_flags_cease_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo"),
    _LIT("-f"),
    _LIT("-abcd"),
    _LIT("--"),
    _LIT("--bar"),
    _LIT("blah")
  };

  options = options_group_type{
    co::make_hidden_option(_LIT("foo,f")),
    co::make_hidden_option(_LIT(",a")),
    co::make_hidden_option(_LIT(",b")),
    co::make_hidden_option(_LIT(",c")),
    co::make_hidden_option(_LIT(",d")),
    co::make_operand(_LIT("accept all operands"),co::value<string_type>())
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

//   stream_select::cerr << detail::to_string(vm,co::value<string_type>());

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {co::default_operand_key<detail::check_char_t>(),
        string_type(_LIT("--bar"))},
      {co::default_operand_key<detail::check_char_t>(),
        string_type(_LIT("blah"))},
      {_LIT("foo"),{}},
      {_LIT("foo"),{}},
      {_LIT("a"),{}},
      {_LIT("b"),{}},
      {_LIT("c"),{}},
      {_LIT("d"),{}}
    }));
}

/**
  Flags w/invalid packed argument
 */
BOOST_AUTO_TEST_CASE( flags_packed_arg_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo=bar"),
    _LIT("-f"),
    _LIT("-abcd")
  };

  options = options_group_type{
    co::make_hidden_option(_LIT("foo,f"))
  };

//   stream_select::cerr << detail::to_string(vm,co::value<string_type>());

  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv.size(),argv.data(),options)),
      co::unexpected_argument_error);
}

/**
  Flags w/operand as invalid separate argument
 */
BOOST_AUTO_TEST_CASE( flags_invalid_posarg_test )
{
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo"),
    _LIT("-f"),
    _LIT("bar")
  };

  options = options_group_type{
    co::make_hidden_option(_LIT("foo,f"))
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
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo"),
    _LIT("-f"),
    _LIT("-a")
  };

  options = options_group_type{
    co::make_hidden_option(_LIT("foo,f"))
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
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo"),
    _LIT("-f"),
    _LIT("-abcd")
  };

  options = options_group_type{
    co::make_hidden_option(_LIT("foo,f")),
    co::make_hidden_option(_LIT("),a"))
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
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo"),
    _LIT("-f"),
    _LIT("-a--")
  };

  options = options_group_type{
    co::make_hidden_option(_LIT("foo,f")),
    co::make_hidden_option(_LIT(",a"))
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
  variable_map_type vm;
  options_group_type options;
  std::vector<const detail::check_char_t *> argv{
    _LIT("--foo"),
    _LIT("-abc"),
    _LIT("-def")
  };

  options = options_group_type{
    co::make_hidden_option(_LIT("foo,f")),
    co::make_hidden_option(_LIT(",a")),
    co::make_hidden_option(_LIT(",b")),
    co::make_hidden_option(_LIT(",c")),
    co::make_hidden_option(_LIT(",d")),
    co::make_hidden_option(_LIT(",e"))
  };

  vm =  co::parse_arguments(argv.size(),argv.data(),options);

//   stream_select::cerr << detail::to_string(vm,co::value<string_type>());

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {_LIT("a"),{}},
      {_LIT("b"),{}},
      {_LIT("c"),{}},
      {_LIT("d"),{}},
      {_LIT("e"),{}},
      {_LIT("foo"),{}},
      {_LIT("foo"),{}}
    }));
}






BOOST_AUTO_TEST_SUITE_END()

