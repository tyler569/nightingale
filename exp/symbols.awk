BEGIN       { print "struct sym symbols[] {" }
            { print "        { " $1 ", \"" $NF "\" }," }
END         { print "};" }
