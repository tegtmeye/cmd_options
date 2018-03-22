#include "cmd_options.h"

#include "test_detail.h"

namespace co = cmd_options;

int main(void)
{
  co::detail::split<detail::check_char_t>(
    _LIT("foo,f,key,"),detail::check_char_t(','));

  return 0;
}
