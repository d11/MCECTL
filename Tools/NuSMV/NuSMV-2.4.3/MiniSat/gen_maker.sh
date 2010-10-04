## Copyright (c) 2007 by ITC-irst
## Author: Roberto Cavada
##
## This file is intended to be used by other builders
## A set of variables are intended to be already set when included
##
## This is the set of variables that ust be defined
##  tool
##  tool_dir
##  tool_zip
##  tool_patch
##  tool_link
##  tool_targets
## ----------------------------------------------------------------------

# Command line programs
MAKE="make"
if [ `uname -s` = SunOS ]; then
  MAKE="gnumake"
fi

UNZIP=unzip
TAR=tar
GZIP=gzip
BZIP2=bzip2
PATCH=patch
TOUCH=touch
LN="ln -s"

CURDIR=`pwd`


# ----------------------------------------------------------------------
if [ ! -d ${tool_dir} ] ; then 
   if [ ! -f ${tool_zip} ] ; then
      echo Error: ${tool_zip} not found
      exit 1
   fi

   echo "Uncompressing the archive <${tool_zip}>..."
   type=`file $tool_zip | cut -d \  -f 2`

   if [ "${type}x" = "Zipx" ]; then
     ${UNZIP} $tool_zip
   elif [ "${type}x" = "gzipx" ]; then  
     ${GZIP} -cd $tool_zip | ${TAR} xf -
   elif [ "${type}x" = "bzip2x" ]; then  
     ${BZIP2} -cd $tool_zip | ${TAR} xf -
   else
     echo "Error: do not know how to decompress the archive $tool_zip"
     echo "Decompress it manually, then re-run build.sh"
   fi    
fi

if [ ! -d ${tool_dir} ] ; then
    echo Error: After decompression of ${tool_zip} directory
    echo ${tool_dir} was expected to exist.
    exit 1
fi


# applies the patch if needed
if [ ! -f ${patched_tag} ]; then
    ${PATCH} -p0 -N < ${tool_patch} && ${TOUCH} ${patched_tag}
fi

if [ ! -h ${tool_link} ] ; then             
   ${LN} ${CURDIR}/${tool_dir} ${tool_link}
fi

# ----------------------------------------------------------------------
# Make invokation:
echo
echo "Building ${tool} using <$MAKE $tool_targets $*> ..."
cd ${tool_dir} && $MAKE $tool_targets $*
echo
# ----------------------------------------------------------------------
