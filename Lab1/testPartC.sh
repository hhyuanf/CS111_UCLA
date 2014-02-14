#! /bin/sh

# UCLA CS 111 Lab 1 - Test that valid syntax is processed correctly.

tmp=$0-$$.tmp
mkdir "$tmp" || exit

(
cd "$tmp" || exit

cat >test.sh <<'EOF'

sleep 1 && ls ..| cat >result1.txt

cat<result1.txt>result2.txt

sleep 1 && ls ..| cat >result3.txt

cat<result3.txt>result4.txt

sleep 1 && ls ..| cat >result5.txt

cat<result5.txt>result6.txt

sleep 1 && ls ..| cat >result7.txt

cat<result7.txt>result8.txt

(cat <result2.txt|echo) && (cat <result4.txt|echo) && (cat <result6.txt|echo) && (cat <result8.txt|echo)

EOF

echo Running time for Timetrash in parallel mode is 
time ../timetrash -t test.sh >resultCC.txt
rm result?.txt test.sh

cat >test.sh <<'EOF'
#! /bin/bash

sleep 1 && ls ..| cat >result1.txt

cat<result1.txt>result2.txt

sleep 1 && ls ..| cat >result3.txt

cat<result3.txt>result4.txt

sleep 1 && ls ..| cat >result5.txt

cat<result5.txt>result6.txt

sleep 1 && ls ..| cat >result7.txt

cat<result7.txt>result8.txt

(cat <result2.txt|echo) && (cat <result4.txt|echo) && (cat <result6.txt|echo) && (cat <result8.txt|echo)

EOF

chmod 777 test.sh

echo Running time for Bash Shell is

time ./test.sh > resultCC.exp

echo Comparing Output......

diff -u resultCC.exp resultCC.txt || exit
test ! -s test.err || {
  cat test.err
  exit 1
}

) || exit

echo No Difference. Test for part C done. 

rm -fr "$tmp"
