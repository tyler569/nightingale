
BEFORE=$1
AFTER=$2

find . -name '*.[hc]' | xargs sed -i"" "s/#include \"$BEFORE\"/#include <$AFTER>/"
find . -name '*.[hc]' | xargs sed -i"" "s/#include <$BEFORE>/#include <$AFTER>/"

