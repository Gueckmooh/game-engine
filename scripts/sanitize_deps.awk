BEGIN {
    nl=0
}

/.*/ {
    if (nl == 0) {
        print $0
        nl = 1
    } else {
        system("echo -n $(realpath " $1 ")")
        print " " $2
    }
}
