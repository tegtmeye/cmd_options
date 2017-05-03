#include "cmd_options.h"

#include "test_detail.h"

#include <boost/test/unit_test.hpp>

#include <iostream>

BOOST_AUTO_TEST_SUITE( parse_test_suite )

namespace co = lemma::cmd_options;

template<typename T, typename U, typename VariableMap>
bool contents_equal(const VariableMap &vm,
  const typename VariableMap::key_type &key, std::initializer_list<U> l)
{
  auto && elem = l.begin();
  auto && range = vm.equal_range(key);
  while(range.first != range.second && elem != l.end()) {
    if(co::any_cast<T>(range.first->second) != *elem)
      return false;
    ++range.first;
    ++elem;
  }

  return range.first == range.second && elem == l.end();
}

/**

 */
BOOST_AUTO_TEST_CASE( default_parse_test )
{

  std::vector<const char *> argv{
//     "--foo", "foo_val",
//     "--bar", "bar_val",
//     "-f", "f_val",
//     "--abc=123",
//     "--def=456",
    "-sshort_opt"
  };

  std::size_t end;
  co::variable_map vm = co::parse_arguments(argv.data(),argv.size(),end);

  std::cerr << detail::to_string<std::string>(vm);

  BOOST_REQUIRE(vm.size() == 6);

  BOOST_REQUIRE(contents_equal<std::string>(vm,"foo",{"foo_val"}));

  BOOST_REQUIRE(contents_equal<std::string>(vm,"bar",{"bar_val"}));

  BOOST_REQUIRE(contents_equal<std::string>(vm,"f",{"f_val"}));

  BOOST_REQUIRE(contents_equal<std::string>(vm,"abc",{"123"}));

  BOOST_REQUIRE(contents_equal<std::string>(vm,"def",{"456"}));

  BOOST_REQUIRE(contents_equal<std::string>(vm,"s",{"short_opt"}));
}



BOOST_AUTO_TEST_SUITE_END()

