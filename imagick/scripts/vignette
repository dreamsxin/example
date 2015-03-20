#!/bin/bash
#
# Developed by Fred Weinhaus 9/17/2011 .......... 8/23/2012
#
# USAGE: vignette [-i inner] [-o outer] [-f feather] [-c color] [-a amount] infile outfile
# USAGE: vignette [-help|-h]
#
# OPTIONS:
#
# -i     inner       inner radius of vignette where not darkened;
#                    integer percent of image dimension; default=0
# -o     outer       outer radius of vignette where darkest;
#                    integer percent of image dimension; default=150
# -f     feather     feathering amount for inner radius; float>=0;
#                    default=0
# -c     color       vignette color; any IM opaque color; default=black
# -a     amount      vignette amount; 0<=integer<=0; default=100
#  
#
###
#
# NAME: VIGNETTE 
# 
# PURPOSE: Applies a vignette effect to a picture.
# 
# DESCRIPTION: VIGNETTE applies a vignette effect to a picture. The 
# inner and outer radii of the vignette can be controlled as well as 
# the vignette color and amount. For non-square images the vignette 
# effect will be elliptical to reach the corners.
# 
# 
# OPTIONS: 
# 
# -i inner ... INNER vignette radius as percent of image dimension. The 
# default=0 means the vignette starts near the image center.
# 
# -o outer ... OUTER vignette radius as percent of image dimension. The 
# default=150, which is about the image corners.
# 
# -f feather ... FEATHER is the amount of feathering or smoothing of the 
# transition around the inner radius. Values are floats>0. The default=0
# 
# -c color ... COLOR is the color of vignette. Any valid opaque IM color 
# is allowed. The default=black.
# 
# -a amount ... AMOUNT is the overal coloring amount. Values are integers in 
# the range 0 to 100. A value of zero means no vignette. The default=100 
# means full vignette color. 
# 
# CAVEAT: No guarantee that this script will work on all platforms, 
# nor that trapping of inconsistent parameters is complete and 
# foolproof. Use At Your Own Risk. 
# 
######
#

# set up defaults
inner=50		# inner radius of vignette where not darkened as percent of image dimension
outer=150		# outer radius of vignette where darkest
color="black"	# vignette color
amount=100		# vignette amount
feather=0       # feathering amount for inner radius; 

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
elif [ $# -gt 12 ]
	then
	errMsg "--- TOO MANY ARGUMENTS WERE PROVIDED ---"
else
	while [ $# -gt 0 ]
		do
			# get parameter values
			case "$1" in
		     -help|-h)    # help information
					   echo ""
					   usage2
					   exit 0
					   ;;
				-i)    # get inner
					   shift  # to get the next parameter
					   # test if parameter starts with minus sign 
					   errorMsg="--- INVALID INNER SPECIFICATION ---"
					   checkMinus "$1"
					   inner=`expr "$1" : '\([0-9]*\)'`
					   [ "$inner" = "" ] && errMsg "--- INNER=$inner MUST BE A NON-NEGATIVE INTEGER (with no sign) ---"
					   ;;
				-o)    # get outer
					   shift  # to get the next parameter
					   # test if parameter starts with minus sign 
					   errorMsg="--- INVALID OUTER SPECIFICATION ---"
					   checkMinus "$1"
					   outer=`expr "$1" : '\([0-9]*\)'`
					   [ "$outer" = "" ] && errMsg "--- OUTER=$outer MUST BE A NON-NEGATIVE INTEGER (with no sign) ---"
					   ;;
				-f)    # get feather
					   shift  # to get the next parameter
					   # test if parameter starts with minus sign 
					   errorMsg="--- INVALID FEATHER SPECIFICATION ---"
					   checkMinus "$1"
					   feather=`expr "$1" : '\([0-9]*\)'`
					   [ "$feather" = "" ] && errMsg "--- FEATHER=$feather MUST BE A NON-NEGATIVE INTEGER (with no sign) ---"
					   ;;
				-c)    # get  color
					   shift  # to get the next parameter
					   # test if parameter starts with minus sign 
					   errorMsg="--- INVALID COLOR SPECIFICATION ---"
					   checkMinus "$1"
					   color=`echo "$1"`
					   ;;
				-a)    # get amount
					   shift  # to get the next parameter
					   # test if parameter starts with minus sign 
					   errorMsg="--- INVALID AMOUNT SPECIFICATION ---"
					   checkMinus "$1"
					   amount=`expr "$1" : '\([0-9]*\)'`
					   [ "$amount" = "" ] && errMsg "--- AMOUNT=$amount MUST BE AN INTEGER ---"
					   test1=`echo "$amount < 0" | bc`
					   test2=`echo "$amount > 100" | bc`
					   [ $test1 -eq 1 -o $test2 -eq 1 ] && errMsg "--- AMOUNT=$amount MUST BE AN INTEGER BETWEEN 0 AND 100 ---"
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


# setup temp files
tmpA1="$dir/vignette_1_$$.mpc"
tmpB1="$dir/vignette_1_$$.cache"
trap "rm -f $tmpA1 $tmpB1; exit 0" 0
trap "rm -f $tmpA1 $tmpB1; exit 1" 1 2 3 15

# get im_version
im_version=`convert -list configure | \
	sed '/^LIB_VERSION_NUMBER /!d; s//,/;  s/,/,0/g;  s/,0*\([0-9][0-9]\)/\1/g' | head -n 1`

# colorspace RGB and sRGB swapped between 6.7.5.5 and 6.7.6.7 
# though probably not resolved until the latter
# then -colorspace gray changed to linear between 6.7.6.7 and 6.7.8.2 
# then -separate converted to linear gray channels between 6.7.6.7 and 6.7.8.2,
# though probably not resolved until the latter
# so -colorspace HSL/HSB -separate and -colorspace gray became linear
# but we need to use -set colorspace RGB before using them at appropriate times
# so that results stay as in original script
# The following was determined from various version tests using vignette
# with IM 6.7.4.10, 6.7.6.10, 6.7.9.1
if [ "$im_version" -lt "06070607" -o "$im_version" -gt "06070707" ]; then
	setcspace="-set colorspace RGB"
else
	setcspace=""
fi

# test input image
convert -quiet -regard-warnings "$infile" +repage "$tmpA1" ||
	errMsg "--- FILE $infile DOES NOT EXIST OR IS NOT AN ORDINARY FILE, NOT READABLE OR HAS ZERO SIZE ---"


# setup vignette
ww=`convert $tmpA1 -format "%w" info:`
hh=`convert $tmpA1 -format "%h" info:`
wwo=`convert xc: -format "%[fx:$outer*$ww/100]" info:`
hho=`convert xc: -format "%[fx:$outer*$hh/100]" info:`
mwh=`convert xc: -format "%[fx:$outer*min($ww,$hh)/100]" info:`
if [ "$inner" = "0" ]; then
	mlevel=""
else
	inner=$((100-inner))
	mlevel="-level 0x$inner%"
fi
if [ "$amount" = "100" ]; then
	plevel=""
else
	amount=$((100-amount))
	plevel="+level ${amount}x100%"
fi
#echo "ww=$ww; hh=$hh; wwo=$wwo; hho=$hho; mwh=$mwh; mlevel=$mlevel; plevel=$plevel"

# setup feathering
if [ "$feather" = "0" ]; then
	feathering=""
else
	feathering="-blur ${feather}x65000"
fi
#echo "feathering=$feathering"


convert -background $color $tmpA1 \
	\( -size ${mwh}x${mwh} radial-gradient: -resize ${wwo}x${hho}! \
		-gravity center -crop ${ww}x${hh}+0+0 +repage $mlevel $plevel $feathering \) \
	$setcspace -alpha off -compose copy_opacity -composite \
	-compose over -flatten  $outfile


exit 0