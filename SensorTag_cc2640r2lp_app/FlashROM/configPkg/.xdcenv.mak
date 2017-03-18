#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = C:/ti/simplelink_cc2640r2_sdk_1_00_00_22/source;C:/ti/simplelink_cc2640r2_sdk_1_00_00_22/kernel/tirtos/packages;C:/ti/ccsv7/ccs_base
override XDCROOT = C:/ti/xdctools_3_32_01_22_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = C:/ti/simplelink_cc2640r2_sdk_1_00_00_22/source;C:/ti/simplelink_cc2640r2_sdk_1_00_00_22/kernel/tirtos/packages;C:/ti/ccsv7/ccs_base;C:/ti/xdctools_3_32_01_22_core/packages;..
HOSTOS = Windows
endif
