
FILE=$1
MOD=${2:-ng}

find . -name '*.[hc]' | xargs sed -i"" "s/#include \"$FILE\"/#include <$MOD\/$FILE>/"
find . -name '*.[hc]' | xargs sed -i"" "s/#include <$FILE>/#include <$MOD\/$FILE>/"
