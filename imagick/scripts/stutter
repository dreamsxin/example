#!/bin/bash
#
# Developed by Fred Weinhaus 5/8/2008 .......... revised 5/8/2008
#
# USAGE: stutter [-s size] [-d direction] infile outfile
# USAGE: stutter [-h or -help]
#
# OPTIONS:
#
# -s      size           stutter offset size; size>0; default=16
# 
# -d      direction      x or h (horizontal), y or v (vertical) offset;
#                        default=x
#
###
#
# NAME: STUTTER 
# 
# PURPOSE: To create a "stuttered" offset-like effect in an image.
# 
# DESCRIPTION: STUTTER creates a a "stuttered" offset-like effect in an image.
# 
# OPTIONS: 
# 
# -s size ... SIZE is the pixelization (block) size. Values are greater 
# than 0. The default is 3.
# 
# -d direction ... DIRECTION is the offset direction. Values may be x or h 
# for horizontal offset and y or v for vertical offset. The default=x.
# 
# CAVEAT: No guarantee that this script will work on all platforms, 
# nor that trapping of inconsistent parameters is complete and 
# foolproof. Use At Your Own Risk. 
# 
######
#

# set default values
size=16
direction="x"

# set directory for temporary files
dir="."    # suggestions are dir="." or dir="/tmp"

# set up functions to report Usage and Usage with Description
PROGNAME=`type $0 | awk '{print $3}'`  # search for executable on path
PROGDIR=`dirname $PROGNAME`            # extract directory of program
PROGNAME=`basename $PROGNAME`          # base name of program
usage1() 
	{
	echo >&2 ""
	echo >&2 "$PROGNAME:" "$@"
	sed >&2 -n '/^###/q;  /^#/!q;  s/^#//;  s/^ //;  4,$p' "$PROGDIR/$PROGNAME"
	}
usage2() 
	{
	echo >&2 ""
	echo >&2 "$PROGNAME:" "$@"
	sed >&2 -n '/^######/q;  /^#/!q;  s/^#*//;  s/^ //;  4,$p' "$PROGDIR/$PROGNAME"
	}


# function to report error messages
errMsg()
	{
	echo ""
	echo $1
	echo ""
	usage1
	exit 1
	}


# function to test for minus at start of value of second part of option 1 or 2
checkMinus()
	{
	test=`echo "$1" | grep -c '^-.*$'`   # returns 1 if match; 0 otherwise
    [ $test -eq 1 ] && errMsg "$errorMsg"
	}

# test for correct number of arguments and get values
if [ $# -eq 0 ]
	then
	# help information
   echo ""
   usage2
   exit 0
elif [ $# -gt 6 ]
	then
	errMsg "--- TOO MANY ARGUMENTS WERE PROVIDED ---"
else
	while [ $# -gt 0 ]
		do
			# get parameter values
			case "$1" in
		  -h|-help)    # help information
					   echo ""
					   usage2
					   exit 0
					   ;;
				-s)    # get size
					   shift  # to get the next parameter - scale
					   # test if parameter starts with minus sign 
					   errorMsg="--- INVALID SIZE SPECIFICATION ---"
					   checkMinus "$1"
					   size=`expr "$1" : '\([0-9]*\)'`
					   [ "$size" = "" ] && errMsg "SIZE=$size MUST BE AN INTEGER"
		   			   sizetest=`echo "$size <= 0" | bc`
					   [ $sizetest -eq 1 ] && errMsg "--- SIZE=$size MUST BE A POSITIVE INTEGER ---"
					   ;;
				-d)    # get  direction
					   shift  # to get the next parameter
					   # test if parameter starts with minus sign 
					   errorMsg="--- INVALID DIRECTION SPECIFICATION ---"
					   checkMinus "$1"
					   direction="$1"
					   case "$1" in 
					   		x) ;;
					   		X) ;;
					   		y) ;;
					   		Y) ;;
					   		h) ;;
					   		H) ;;
					   		v) ;;
					   		V) ;;
					   		horizontal) ;;
					   		vertical) ;;
					   		*) errMsg "--- DIRECTION=$direction IS AN INVALID VALUE ---" 
					   	esac
					   ;;
				 -)    # STDIN and end of arguments
					   break
					   ;;
				-*)    # any other - argument
					   errMsg "--- UNKNOWN OPTION ---"
					   ;;
		     	 *)    # end of arguments
					   break
					   ;;
			esac
			shift   # next option
	done
	#
	# get infile and outfile
	infile=$1
	outfile=$2
fi

# test that infile provided
[ "$infile" = "" ] && errMsg "NO INPUT FILE SPECIFIED"

# test that outfile provided
[ "$outfile" = "" ] && errMsg "NO OUTPUT FILE SPECIFIED"


# test if image an ordinary, readable and non-zero size
if [ -f $infile -a -r $infile -a -s $infile ]
	then
	: 'Do Nothing'
else
	errMsg "--- FILE $infile DOES NOT EXIST OR IS NOT AN ORDINARY FILE, NOT READABLE OR HAS ZERO SIZE ---"
fi

# get parameters
size2=`expr $size \* 2`

# process image
if [ "$direction" = "x" -o "$direction" = "X" -o "$direction" = "h" -o "$direction" = "H" -o "$direction" = "horizontal" ]
	then
	convert $infile -monitor -fx "u.p{i+mod(i,$size2)-$size,j}" $outfile
elif [ "$direction" = "y" -o "$direction" = "Y" -o "$direction" = "v" -o "$direction" = "V" -o "$direction" = "vertical" ]
	then
	convert $infile -monitor -fx "u.p{i,j+mod(j,$size2)-$size}" $outfile
else
	errMsg "--- INVALID DIRECTION ---"
fi

exit 0
