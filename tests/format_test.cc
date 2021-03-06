#include "cmd_options.h"

#include "test_detail.h"

#include <boost/test/unit_test.hpp>

#include <iostream>

/**
  case 4, Raw keys with strictly no value
 */

BOOST_AUTO_TEST_SUITE( format_test_suite )


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
  std::locale::global(std::locale("en_US.UTF-8"));

  options_group_type options = options_group_type{
    co::make_option(_LIT("a-really-long-option-name,b"),
      co::basic_value<string_type,detail::check_char_t>()
        .implicit(_LIT("implicit")),detail::BMP_celebs),
//     co::make_option(_LIT("foo,f"),
//       co::basic_value<string_type,detail::check_char_t>(),detail::ipsum_par)
  };

  stream_select::cout.imbue(std::locale());
  stream_select::cout << co::to_string(options);
}

/*

--bar,-b =<path>[/usr/local]

--bar,-b <arg>[/usr/local]

--bar,-b [=arg<implicit>]

--bar,-b [arg=<implicit>]

--bar,-b arg

*/

BOOST_AUTO_TEST_SUITE_END()

