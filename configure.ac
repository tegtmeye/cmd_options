#  Copyright (c) 2012-2017, Mike Tegtmeyer
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

AC_PREREQ(2.59)
AC_INIT(cmd_options, 0.0.1)
AC_CONFIG_SRCDIR([Makefile.am])
AM_CONFIG_HEADER(config.h)
AC_CONFIG_AUX_DIR(config)
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE([foreign tar-pax])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.

AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LIBTOOL

# We need boost
AX_BOOST_BASE([1.61])
AX_BOOST_UNIT_TEST_FRAMEWORK
AX_BOOST_PROGRAM_OPTIONS

# Use SYSTEM_BOOST_CPPFLAGS to add includes with -isystem instead of -I
# to suppress boost compiler warnings
AC_SUBST([SYSTEM_BOOST_CPPFLAGS], [`echo $BOOST_CPPFLAGS | sed -e 's|^-I|-isystem |g' -e 's|[[:space:]]\{1,\}-I| -isystem |g'`])

# Check for single testsuite
AC_ARG_WITH([examples],
[AS_HELP_STRING([--with-examples],
        [build examples @<:@default=yes@:>@])],
[case "${withval}" in
  yes) ;;
  no) ;;
  *) AC_MSG_ERROR([bad value ${withval} for --with-examples]) ;;
esac],[with_examples=yes])
AM_CONDITIONAL([EXAMPLES], [test "x$with_examples" == xyes])

# Check for C++11 support
AX_CXX_COMPILE_STDCXX([11],[noext],[mandatory])

AC_CONFIG_FILES( \
		 cmd_options.pc \
		 cmd_options/Makefile \
		 tests/Makefile \
		 examples/Makefile \
		 Makefile)

AC_OUTPUT
