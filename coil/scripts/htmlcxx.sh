#!/bin/sh

dir=$1

ver=0.85
file=htmlcxx-$ver.tar.gz
url=http://sourceforge.net/projects/htmlcxx/files/htmlcxx/$ver/$file/download

if ! test -d $dir; then
    wget --quiet $url -O $file || exit
    mkdir -p $dir || exit
    tar --strip-components=1 -xf $file -C $dir || exit
    cd $dir || exit
    ./configure --prefix=$HOME && make --no-print-directory && make --no-print-directory install || exit
fi