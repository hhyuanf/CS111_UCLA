#! /bin/sh

# UCLA CS 111 Lab 1 - Test that valid syntax is processed correctly.

tmp=$0-$$.tmp
mkdir "$tmp" || exit

(
cd "$tmp" || exit

cat >test.sh <<'EOF'
#! /bin/bash

sleep 2 && echo wakeup

ls | grep test | grep p

ls > b

(echo hello && ls) > b

sort < b | grep test

(echo hi && false; echo hi2) && echo end

EOF

echo running test in normal mode 

../timetrash  test.sh >>resultB.txt 2>test.err || exit

rm test.sh 

cat >test.sh <<'EOF'
#! /bin/bash

sleep 2 && echo wakeup

ls | grep test | grep p

ls > b

(echo hello && ls) > b

sort < b | grep test

(echo hi && false; echo hi2) && echo end

EOF

chmod 777 test.sh

echo running test in bash shell

./test.sh >>resultB.exp 2>test.err || exit

rm test.sh 

echo comparing Output......

diff -u resultB.exp resultB.txt || exit
test ! -s test.err || {
  cat test.err
  exit 1
}

) || exit

echo No differences. Test for part B done.
rm -fr "$tmp"
