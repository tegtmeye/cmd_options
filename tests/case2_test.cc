#include "cmd_options.h"

#include "test_detail.h"

#include <boost/test/unit_test.hpp>

#include <iostream>

/**
  case 2, strictly no value
 */

BOOST_AUTO_TEST_SUITE( case2_test_suite )


namespace co = cmd_options;

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
    co::make_option(_LIT("foo,f"),_LIT("case 2"))
  };

  vm =  co::parse_arguments(argv.data(),argv.data()+argv.size(),options);

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
    co::make_option(_LIT("foo,f"),_LIT("case 2")),
    co::make_operand(_LIT("operand_key"),
      co::basic_value<string_type,detail::check_char_t>())
  };

  vm =  co::parse_arguments(argv.data(),argv.data()+argv.size(),options);

//   stream_select::cerr << detail::to_string(vm,co::basic_value<string_type,detail::check_char_t>());

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {_LIT("operand_key"),string_type(_LIT("--bar"))},
      {_LIT("operand_key"),string_type(_LIT("blah"))},
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
    co::make_option(_LIT("foo,f"),_LIT("case 2")),
    co::make_option(_LIT(",a"),_LIT("case 2")),
    co::make_option(_LIT(",b"),_LIT("case 2")),
    co::make_option(_LIT(",c"),_LIT("case 2")),
    co::make_option(_LIT(",d"),_LIT("case 2"))
  };

  vm =  co::parse_arguments(argv.data(),argv.data()+argv.size(),options);

//   stream_select::cerr << detail::to_string(vm,co::basic_value<string_type,detail::check_char_t>());

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
    co::make_option(_LIT("foo,f"),_LIT("case 2")),
    co::make_option(_LIT(",a"),_LIT("case 2")),
    co::make_option(_LIT(",b"),_LIT("case 2")),
    co::make_option(_LIT(",c"),_LIT("case 2")),
    co::make_option(_LIT(",d"),_LIT("case 2")),
    co::make_operand(_LIT("operand_key"),
      co::basic_value<string_type,detail::check_char_t>())
  };

  vm =  co::parse_arguments(argv.data(),argv.data()+argv.size(),options);

//   stream_select::cerr << detail::to_string(vm,co::basic_value<string_type,detail::check_char_t>());

  BOOST_REQUIRE(detail::contents_equal<string_type>(vm,
    variable_map_type{
      {_LIT("operand_key"),string_type(_LIT("--bar"))},
      {_LIT("operand_key"),string_type(_LIT("blah"))},
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
    co::make_option(_LIT("foo,f"),_LIT("case 2"))
  };

//   stream_select::cerr << detail::to_string(options.front()) << _LIT("\n");

  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv.data(),argv.data()+argv.size(),options)),
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
    co::make_option(_LIT("foo,f"),_LIT("case 2"))
  };

  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv.data(),argv.data()+argv.size(),options)),
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
    co::make_option(_LIT("foo,f"),_LIT("case 2"))
  };

  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv.data(),argv.data()+argv.size(),options)),
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
    co::make_option(_LIT("foo,f"),_LIT("case 2")),
    co::make_option(_LIT(",a"),_LIT("case 2"))
  };

  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv.data(),argv.data()+argv.size(),options)),
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
    co::make_option(_LIT("foo,f"),_LIT("case 2")),
    co::make_option(_LIT(",a"),_LIT("case 2"))
  };

  BOOST_REQUIRE_THROW(
    (vm = co::parse_arguments(argv.data(),argv.data()+argv.size(),options)),
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
    co::make_option(_LIT("foo,f"),_LIT("case 2")),
    co::make_option(_LIT(",a"),_LIT("case 2")),
    co::make_option(_LIT(",b"),_LIT("case 2")),
    co::make_option(_LIT(",c"),_LIT("case 2")),
    co::make_option(_LIT(",d"),_LIT("case 2")),
    co::make_option(_LIT(",e"),_LIT("case 2"))
  };

  vm =  co::parse_arguments(argv.data(),argv.data()+argv.size(),options);

//   stream_select::cerr << detail::to_string(vm,co::basic_value<string_type,detail::check_char_t>());

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

