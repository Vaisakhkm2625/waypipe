#!/bin/sh

root=`pwd`

program=`which bash`
program=`which weston-terminal`
#program=`which weston-flower`
#program=`which demo.py`

($root/waypipe -d client /tmp/socket-client 2>&1 | sed 's/.*/\x1b[33m&\x1b[0m/') &
# ssh-to-self; should have a local keypair set up
(ssh -R/tmp/socket-server:/tmp/socket-client localhost $root/waypipe server -d /tmp/socket-server -- $program) 2>&1 | sed 's/.*/\x1b[35m&\x1b[0m/'
kill %1
rm -f /tmp/socket-client
rm -f /tmp/socket-server
