#!/bin/bash
#
# Developed by Fred Weinhaus 5/26/2008 .......... revised 8/23/2012
#
# USAGE: zoomblur [-z zoom] [-e] infile outfile
# USAGE: zoomblur [-h or -help]
#
# OPTIONS:
#
# -a      amount        amount of zoom blur; float; amount>=1; default=1.2
# -e                    expand the output image size by the amount of zoom
#
###
#
# NAME: ZOOMBLUR 
# 
# PURPOSE: To apply a radial or zoom blur to an image.
# 
# DESCRIPTION: ZOOMBLUR applies a radial blur to an image by a 
# process of iterative zooming and blending.
# 
# OPTIONS: 
# 
# -a amount ... AMOUNT is the zoom factor. Values are floats  
# greater than or equal to 1. A value of 1 produces no effect. Larger 
# values increase the radial blur. Typical values are between 1 and 1.5. 
# The default is 1.2. Note that the large the amount, the more iterations 
# it will take.
#
# -e ... Expands the output image size by the amount of the zoom.
# 
# CAVEAT: No guarantee that this script will work on all platforms, 
# nor that trapping of inconsistent parameters is complete and 
# foolproof. Use At Your Own Risk. 
# 
######
#

# set default values
amount=1.2
expand="no"

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
elif [ $# -gt 5 ]
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
				-a)    # get amount
					   shift  # to get the next parameter - amount
					   # test if parameter starts with minus sign 
					   errorMsg="--- INVALID AMOUNT SPECIFICATION ---"
					   checkMinus "$1"
					   amount=`expr "$1" : '\([.0-9]*\)'`
					   [ "$amount" = "" ] && errMsg "AMOUNT=$amount MUST BE A FLOAT"
		   			   amounttest=`echo "$amount < 1" | bc`
					   [ $amounttest -eq 1 ] && errMsg "--- AMOUNT=$amount MUST BE A FLOAT GREATER THAN OR EQUAL TO ONE ---"
					   ;;
				-e)    # set the expand mode
					   expand="yes"
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


# set temporary files
tmpA="$dir/zoomblur_1_$$.mpc"
tmpB="$dir/zoomblur_1_$$.cache"
tmpC="$dir/zoomblur_2_$$.mpc"
tmpD="$dir/zoomblur_2_$$.cache"
trap "rm -f $tmpA $tmpB $tmpC $tmpD; exit 0" 0
trap "rm -f $tmpA $tmpB $tmpC $tmpD; exit 1" 1 2 3 15

if convert -quiet -regard-warnings "$infile" +repage "$tmpA"
	then
	: ' Do Nothing '
else
	errMsg "--- FILE $infile DOES NOT EXIST OR IS NOT AN ORDINARY FILE, NOT READABLE OR HAS ZERO SIZE ---"
fi

# get im_version
im_version=`convert -list configure | \
	sed '/^LIB_VERSION_NUMBER /!d; s//,/;  s/,/,0/g;  s/,0*\([0-9][0-9]\)/\1/g' | head -n 1`

rad=`convert $infile -format "%[fx:hypot(w/2,h/2)]" info:`
zoomfrac=`convert xc: -format "%[fx:($amount-1)]" info:`
iter=`convert xc: -format "%[fx:int($zoomfrac*$rad)]" info:`

ww=`convert $infile -format "%w" info:`
hh=`convert $infile -format "%h" info:`
if [ "$expand" = "yes" ]
	then
	ww=`convert $infile -format "%[fx:floor(w*$amount)]" info:`
	hh=`convert $infile -format "%[fx:floor(h*$amount)]" info:`
fi	

echo ""
echo "Number Of Interations = $iter"
echo ""

#process image
i=1
while [ $i -le $iter ]
	do
	echo "iteration=$i"
	j=`expr $i + 1`
	#compute running average percentages
	new=`convert xc: -format "%[fx:100/$j]" info:`
	old=`convert xc: -format "%[fx:100-$new]" info:`
	s=`convert xc: -format "%[fx:100*(1 + $zoomfrac*$i/$iter)]" info:`
	convert $tmpA -resize $s% -gravity center -crop ${ww}x${hh}+0+0 +repage $tmpC
	if [ "$im_version" -lt "06050304" ]; then
		composite -blend $old%x$new% -gravity center $tmpA $tmpC $tmpA
	else
		convert $tmpC $tmpA -gravity center -define compose:args=$old%x$new% \
			-compose blend -composite $tmpA
	fi
	i=`expr $i + 1`
done
convert $tmpA $outfile
exit 0
