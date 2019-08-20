path = $(shell if [ $$(uname -s) == "Linux" ] ; then if [[ $$(lsb_release -cs) == "n/a" ]] ; then release=$$(hostname) ; else release=$$(lsb_release -cs) ; fi ; suffix=$$(lsb_release -is)/$$release/ ; fi ; echo var/$$(uname -s)/$$suffix$$(uname -m))
MAKEFILE_DIR=$(dir $(lastword $(MAKEFILE_LIST)))
include $(MAKEFILE_DIR)/$(path)/common.mak
