#!/bin/sh

OS=`uname -s`
LIBTOOLIZE=libtoolize

if [ "x$OS" = "xDarwin" ]; then
	LIBTOOLIZE=glibtoolize
fi

echo "Generating build system..."

$LIBTOOLIZE --install --copy || exit 1
aclocal -I m4 || exit 1
autoheader || exit 1
autoconf || exit 1
automake --add-missing --copy || exit 1
