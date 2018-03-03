
#
# Nightingale only has one root makefile - so you have to be in the root folder to
# run `make` on it.
#
# This is inconvenient and leads to working out of there and typing full file paths
# into my editor command line.  I made this alias to auto search for the Makefile,
# run make for me, then return to where I am.
#

rmake() {
    local ORIGINAL_DIR=$PWD

    while ! [ -f Makefile ]; do
        if [ "$PWD" = '/' ]; then
            echo "Error: reached root without finding Makefile!"
            cd $ORIGINAL_DIR
            return
        fi

        cd ..
    done

    make "$@"

    cd $ORIGINAL_DIR
}
