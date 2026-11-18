########################################################################
#
# The run of a program and its status as the scripts of tests/ show
# it.  A program runs under a time limit of 10 seconds, as
# timeout -k 1 10, and timeout(1) ends with 124 when SIGTERM ended it
# at the limit and 137 when SIGKILL did; either is the limit's only
# where the run lasted the limit, and shows as "timeout".  The shell
# reports a run that signal n killed as 128 + n, and timeout(1) dies of
# the signal its program died of, so any other status above 128 is a
# signal's and shows as the signal's name, SEGV for 139, KILL for 137
# before the limit.  tests/oracle and tests/probes read this file with
# ".", from the tree's root, once they have checked that they run
# there.
#
########################################################################

##########
# global constants

# the shell's status for a run that a signal killed: this plus the
# signal's number
signal_base=128

# the seconds a program may run, the seconds after SIGTERM until
# SIGKILL, the statuses timeout(1) gives for a program it stops with
# each, and the status shown for either
time_limit=10
kill_after=1
timeout_status=124
killed_status=137
timed_out=timeout


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


##########
# run_under_limit(): runs the program that follows with no arguments
# and no standard input, through the calling script's logged(), under
# the time limit when the script's limited is 1, and prints its status
# as the scripts show it: "timeout" when the limit stopped it, and
# shown_status() of its exit status otherwise; 124 or 137 is the
# limit's only when the run lasted the limit, and one that ended
# sooner is the program's own 124, or a SIGKILL from elsewhere, KILL
run_under_limit()
{
    program=$1

    if [ "$limited" -eq 0 ]; then
        logged "$program" < /dev/null
        shown_status $?
        return 0
    fi

    started=$(date +%s)
    logged timeout -k "$kill_after" "$time_limit" "$program" < /dev/null
    run_status=$?
    ended=$(date +%s)

    if { [ "$run_status" -eq "$timeout_status" ] ||
         [ "$run_status" -eq "$killed_status" ]; } &&
       [ $((ended - started)) -ge "$time_limit" ]; then
        echo "$timed_out"
    else
        shown_status "$run_status"
    fi
}
