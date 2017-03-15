============
vmod-dateformat
============

SYNOPSIS
========

import dateformat;

DESCRIPTION
===========

Dateformat Varnish vmod convert date into time accordingly to format.

It extends basic capability of varnish std module by specifing a date format.

FUNCTIONS
=========

date2time
-----

Prototype
        ::

                date2time(STRING format, STRING str, TIME fallback)
Return value
	TIME
Description
	Convert a STRING str into a TIME accordingly to date format (see strftime.3)
	Returns a TIME fallback if conversion failed.
        ::
                if (dateformat.date2time("%Y:%m:%d:%T", req.http.CustomDate, now) >= now) {
                        return (synth(400, "Invalid date"));
                }



time2date
-----

Prototype
        ::

                time2date(STRING format, TIME t)
Return value
	STRING
Description
	Convert a TIME t into a STRING accordingly to date format (see strftime.3)
	Returns an empty string if conversion failed.
        ::
                set resp.http.foodate = dateformat.time2date("%a, %d %b %Y %T GMT", now + 8h);

INSTALLATION
============

The source tree is based on autotools to configure the building, and
does also have the necessary bits in place to do functional unit tests
using the ``varnishtest`` tool.

Building requires the Varnish header files and uses pkg-config to find
the necessary paths.

Usage::

 ./autogen.sh
 ./configure

If you have installed Varnish to a non-standard directory, call
``autogen.sh`` and ``configure`` with ``PKG_CONFIG_PATH`` pointing to
the appropriate path. For instance, when varnishd configure was called
with ``--prefix=$PREFIX``, use

::

 export PKG_CONFIG_PATH=${PREFIX}/lib/pkgconfig
 export ACLOCAL_PATH=${PREFIX}/share/aclocal

The module will inherit its prefix from Varnish, unless you specify a
different ``--prefix`` when running the ``configure`` script for this
module.

Make targets:

* make - builds the vmod.
* make install - installs your vmod.
* make check - runs the unit tests in ``src/tests/*.vtc``.
* make distcheck - run check and prepare a tarball of the vmod.

If you build a dist tarball, you don't need any of the autotools or
pkg-config. You can build the module simply by running::

 ./configure
 make

Installation directories
------------------------

By default, the vmod ``configure`` script installs the built vmod in the
directory relevant to the prefix. The vmod installation directory can be
overridden by passing the ``vmoddir`` variable to ``make install``.


COMMON PROBLEMS
===============

* configure: error: Need varnish.m4 -- see README.rst

  Check whether ``PKG_CONFIG_PATH`` and ``ACLOCAL_PATH`` were set correctly
  before calling ``autogen.sh`` and ``configure``

* Incompatibilities with different Varnish Cache versions

  Make sure you build this vmod against its correspondent Varnish Cache version.
  For instance, to build against Varnish Cache 4.1, this vmod must be built from
  branch 4.1.
