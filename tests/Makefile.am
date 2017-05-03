#  Copyright (c) 2017, Mike Tegtmeyer
#  All rights reserved.
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
# 
#      * Redistributions of source code must retain the above copyright
#        notice, this list of conditions and the following disclaimer.
#      * Redistributions in binary form must reproduce the above copyright
#        notice, this list of conditions and the following disclaimer in the
#        documentation and/or other materials provided with the distribution.
#      * Neither the name of the author nor the names of its contributors may
#        be used to endorse or promote products derived from this software
#        without specific prior written permission.
# 
#  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND ANY
#  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
#  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#  DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
#  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
#  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
#  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
#  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
#  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

master_suite=$(top_srcdir)/tests/master_suite.cc

additional_cppflags= \
	-pedantic -Wall -I.. \
	$(SYSTEM_BOOST_CPPFLAGS)

additional_cflags=

additional_libs= \
	$(BOOST_UNIT_TEST_FRAMEWORK_LIBS) \
	$(BOOST_SYSTEM_LIBS) \
	$(BOOST_FILESYSTEM_LIBS)

additional_ldflags= \
        $(BOOST_LDFLAGS) \
        $(BOOST_UNIT_TEST_FRAMEWORK_LDFLAGS) \
        $(BOOST_SYSTEM_LDFLAGS) \
        $(BOOST_FILESYSTEM_LDFLAGS)


check_PROGRAMS=\
	basic_test \
	parse_test

basic_test_SOURCES=$(master_suite) \
	test_detail.h \
	basic_test.cc 
basic_test_CPPFLAGS=$(additional_cppflags)
basic_test_LDFLAGS=$(additional_ldflags)
basic_test_LDADD=$(additional_libs)

parse_test_SOURCES=$(master_suite) \
	test_detail.h \
	parse_test.cc 
parse_test_CPPFLAGS=$(additional_cppflags)
parse_test_LDFLAGS=$(additional_ldflags)
parse_test_LDADD=$(additional_libs)

TESTS=\
	basic_test \
	parse_test



