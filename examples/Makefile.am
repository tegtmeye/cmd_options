#  Copyright (c) 2016-2018, Mike Tegtmeyer
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

additional_cppflags= \
	-pedantic -Wall -I$(top_srcdir) \
	$(SYSTEM_BOOST_CPPFLAGS)

additional_cflags=

additional_libs=

additional_ldflags= \
        $(BOOST_LDFLAGS)


noinst_PROGRAMS=\
	impatient \
	posix \
	parsing \
	scenario1 \
	scenario2 \
	scenario3 \
	co_first \
	co_option_groups \
	co_multiple_sources \
	co_options_description \
	co_regex \
	co_real \
	co_custom_syntax

impatient_SOURCES=$(top_srcdir)/cmd_options.h impatient.cc
impatient_CPPFLAGS=$(additional_cppflags)
impatient_LDFLAGS=$(additional_ldflags)
impatient_LDADD=$(additional_libs)

posix_SOURCES=$(top_srcdir)/cmd_options.h posix.cc
posix_CPPFLAGS=$(additional_cppflags)
posix_LDFLAGS=$(additional_ldflags)
posix_LDADD=$(additional_libs)

parsing_SOURCES=$(top_srcdir)/cmd_options.h parsing.cc
parsing_CPPFLAGS=$(additional_cppflags)
parsing_LDFLAGS=$(additional_ldflags)
parsing_LDADD=$(additional_libs)

scenario1_SOURCES=$(top_srcdir)/cmd_options.h scenario1.cc
scenario1_CPPFLAGS=$(additional_cppflags)
scenario1_LDFLAGS=$(additional_ldflags)
scenario1_LDADD=$(additional_libs)

scenario2_SOURCES=$(top_srcdir)/cmd_options.h scenario2.cc
scenario2_CPPFLAGS=$(additional_cppflags)
scenario2_LDFLAGS=$(additional_ldflags)
scenario2_LDADD=$(additional_libs)

scenario3_SOURCES=$(top_srcdir)/cmd_options.h scenario3.cc
scenario3_CPPFLAGS=$(additional_cppflags)
scenario3_LDFLAGS=$(additional_ldflags)
scenario3_LDADD=$(additional_libs)

co_first_SOURCES=$(top_srcdir)/cmd_options.h co_first.cc
co_first_CPPFLAGS=$(additional_cppflags)
co_first_LDFLAGS=$(additional_ldflags)
co_first_LDADD=$(additional_libs)

co_option_groups_SOURCES=$(top_srcdir)/cmd_options.h co_option_groups.cc
co_option_groups_CPPFLAGS=$(additional_cppflags)
co_option_groups_LDFLAGS=$(additional_ldflags)
co_option_groups_LDADD=$(additional_libs)

co_multiple_sources_SOURCES=$(top_srcdir)/cmd_options.h co_multiple_sources.cc
co_multiple_sources_CPPFLAGS=$(additional_cppflags)
co_multiple_sources_LDFLAGS=$(additional_ldflags)
co_multiple_sources_LDADD=$(additional_libs)

co_options_description_SOURCES=$(top_srcdir)/cmd_options.h co_options_description.cc
co_options_description_CPPFLAGS=$(additional_cppflags)
co_options_description_LDFLAGS=$(additional_ldflags)
co_options_description_LDADD=$(additional_libs)

co_regex_SOURCES=$(top_srcdir)/cmd_options.h co_regex.cc
co_regex_CPPFLAGS=$(additional_cppflags)
co_regex_LDFLAGS=$(additional_ldflags)
co_regex_LDADD=$(additional_libs)

co_real_SOURCES=$(top_srcdir)/cmd_options.h co_real.cc
co_real_CPPFLAGS=$(additional_cppflags)
co_real_LDFLAGS=$(additional_ldflags)
co_real_LDADD=$(additional_libs)

co_custom_syntax_SOURCES=$(top_srcdir)/cmd_options.h co_custom_syntax.cc
co_custom_syntax_CPPFLAGS=$(additional_cppflags)
co_custom_syntax_LDFLAGS=$(additional_ldflags)
co_custom_syntax_LDADD=$(additional_libs)
