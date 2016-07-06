KIWI_DIR=`pwd`

select_parm()
{
    while :
    do
        i=0;
		echo "Please select input number:"
		for parm in "$@"
		do
			i=$(( $i + 1))
			echo "$i:$parm"
		done
		
		read input_number
		if [ ! $input_number ];then #must have input
			echo "your input is invalid, please input again"
			continue
		fi
		
		expr $input_number + 0 &>/dev/null #the input must be a number
		if [ $? -ne 0 ];then
			echo "your input is invalid, please input again"
			continue
		fi
		
		if [ $(($input_number)) -gt $(($i)) ];then
			echo "your input is invalid, please input again"
		else
			eval select_option='$'$input_number
			echo "your input is $input_number, then begin build $select_option"
			break
		fi
    done
}


select_parm Project_7620 Project_7628

case $select_option in
Project_7620)
	echo "7620"
	echo $KIWI_DIR
	#TODO:
	;;
Project_7628)
	echo "7628"
	#TODO:
	;;
esac




