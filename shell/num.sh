#!/bin/bash
usage(){
cat <<EOF
USEAGE:sh $0 args1 args2
exit 1
EOF
}
checkInt(){
        expr $1 + 0 &>/dev/null
        [ $? -ne 0 ] && { echo "Args must be integer!";exit 1; }
}
checkInt1(){
        tmp=`echo $1 |sed 's/[0-9]//g'`
        [ -n "${tmp}" ]&& { echo "Args must be integer!";exit 1; }
}
[ $# -ne 2 ]&&usage
args1=$1
args2=$2
checkInt $args1
checkInt1 $args2
if [ $args1 -gt $args2 ];then
        echo "yes,$args1 greate than $args2"
else
        echo "no,$args1 less than $args2"
fi
