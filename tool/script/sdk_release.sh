#! /bin/sh

CUR_DIR=`pwd`
EVAL_DIR="prj/Eval/"
TESTBENCH_DIR="prj/testbench/"
BOOTROM_DIR="tool/bootrom/"
MINI_BOOTROM_DIR="tool/mini_bootrom/"
PROGRAM_INFO_PAGE_DIR="tool/program_info_page/"
UPGRADE_DIR="tool/upgrade/"
FANG_CHUAN_HUO_DIR="tool/fang_chuan_huo/"
DOC_SVD_DIR="doc/SVD"
DOC_MISC_DIR="doc/misc"
USER_MANUAL="doc/SDK/*.docx"
RELEASE_NOTES="doc/SDK/*.docx"
BOOTROM_AS="drv/arch/arm/startup_gm660164qp64_bootrom.s"
BOOTROM_MINI_AS="drv/arch/arm/startup_gm660164qp64_bootrom_mini.s"
PROGRAM_AS="drv/arch/arm/startup_gm660164qp64_program.s"
UPGRADE_AS="drv/arch/arm/startup_gm660164qp64_upgrade.s"

# pass by parameter later
BRANCH_NAME=$1
#BRANCH_NAME="SDK1.0_Develop"
#SDK_LABEL="SDK1.0_20161026"
SDK_LABEL=$2

# get the source code
if [ -d $CUR_DIR/$SDK_LABEL ]; then
	rm -rf $CUR_DIR/$SDK_LABEL
fi

mkdir $CUR_DIR/$SDK_LABEL
cd $CUR_DIR/$SDK_LABEL
#git clone git@10.2.32.150:/project/git/gmant
#cd gmant
#git checkout -b $BRANCH_NAME origin/$BRANCH_NAME
git clone ssh://kenny_chang@172.21.45.91:29418/rabbit
cd rabbit
# should get the specify label later
git checkout -b br_$SDK_LABEL $SDK_LABEL

# remove the code that do not release
rm -rf $EVAL_DIR $TESTBENCH_DIR $BOOTROM_DIR $MINI_BOOTROM_DIR $PROGRAM_INFO_PAGE_DIR $UPGRADE_DIR $FANG_CHUAN_HUO_DIR $DOC_SVD_DIR $DOC_MISC_DIR
rm $USER_MANUAL $RELEASE_NOTES
rm -rf .git
find . -name ".gitignore" | xargs rm

# tar the sdk
cd ..
#tar -czf $SDK_LABEL.tar.gz gmant
tar -czf $SDK_LABEL.tar.gz rabbit


