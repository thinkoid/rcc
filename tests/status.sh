########################################################################
#
# The status of a run as the scripts of tests/ show it.  The shell
# reports a run that signal n killed as 128 + n, and timeout(1) dies of
# the signal its program died of, so a status above 128 is a signal's
# and shows as the signal's name, SEGV for 139.  tests/oracle and
# tests/probes read this file with ".", from the tree's root, once
# they have checked that they run there.
#
########################################################################

##########
# global constants

# the shell's status for a run that a signal killed: this plus the
# signal's number
signal_base=128


##########
# shown_status(): prints an exit status as the scripts show it: a
# number up to 128 as it is, and one above as the name kill -l gives
# its signal, SEGV for 139; the number when kill -l has no name for
# that signal, which bash prints as nothing and dash as its number or
# an error
shown_status()
{
    code=$1

    if [ "$code" -le "$signal_base" ]; then
        echo "$code"
        return 0
    fi

    signal_name=$(kill -l $((code - signal_base)) 2> /dev/null)

    case "$signal_name" in
        "" | [0-9]* | *[!A-Z0-9+-]*)  # no name for the signal
            echo "$code"
            ;;

        *)  # the signal's name, without SIG
            echo "$signal_name"
            ;;
    esac
}
