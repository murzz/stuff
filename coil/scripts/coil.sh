#!/bin/sh

# init
url="http://www.hacker.org/coil/index.php"
name="murzzz"
pass="mAmuZ8fzgVYjavZ"
log="coil.log"
doc="coil.html"
flashvars="FlashVars=\""
level_line_begin="<br><br><div style=\"clear:both\"><form action=index.php method=get><input type=\"text\" name=\"gotolevel\" size=\"3\" value=\""
level_line_end="\" \/>"

# construct url
name="name=$name"
pass="password=$pass"
url="$url?$name&$pass"

# wget parameters
params="$params --output-document=$doc"
params="$params --append-output=$log"
params="$params $url"

# download
wget $params

# is downloaded
wgetreturn=$?
if test $wgetreturn -ne 0; then
   echo "failed to get level"
   exit $wgetreturn
fi

# parse flash vars string
fwstring=$(grep $flashvars <$doc)
# echo $fwstring

# strip flash vars string
fwstring=$(echo $fwstring | sed -e "s/$flashvars//g")
#fwstring=$(echo $fwstring | tr -d "$flashvars")

# remove trailing quote
#fwstring=$(echo $fwstring | tr -d "\"")
fwstring=$(echo $fwstring | sed -e "s/\"//g")

# separate parameters with space instead of ampersand
#fwstring=$(echo $fwstring | tr -d "\"")
#fwstring=$(echo $fwstring | sed -e "s/&/ /g")

# parse level
level=$(grep "$level_line_begin" <$doc)
level=$(echo $level | sed -e "s/$level_line_begin//g" | sed -e "s/$level_line_end//g")
#echo $level

# return result
echo "$fwstring" | tr "&" "\n">$level

# run coil
./coil --file=$level

