## Makefile
## Default:
##    <some-directory>/hdl
##    <some-directory>/no-OS

comma := ,
empty :=
space := $(empty) $(empty)

M_ROOT_DIR := $(abspath $(lastword $(MAKEFILE_LIST)))
M_ROOT_DIR := $(subst /no-OS, no-OS,$(M_ROOT_DIR))
M_ROOT_DIR := $(subst /Makefile,,$(M_ROOT_DIR))
M_ROOT_DIR := $(lastword $(M_ROOT_DIR))
M_ROOT_DIR := $(subst /, ,$(M_ROOT_DIR))
M_ROOT_DIR := $(patsubst %,..,$(M_ROOT_DIR))
M_ROOT_DIR := $(subst $(space),/,$(M_ROOT_DIR))

## To change root directory, use make ROOT-DIR=<my-directory>
## Note - both hdl & no-OS must be present at the root.

ifeq ($(ROOT-DIR),)
  ROOT-DIR := $(M_ROOT_DIR)
endif

## To change individually (i.e. separate directories for no-OS and hdl),
## use make NOOS-DIR=<my-noos-directory> HDL-DIR=<my-hdl-directory>

ifeq ($(NOOS-DIR),)
  NOOS-DIR := $(M_ROOT_DIR)/no-OS
endif

ifeq ($(HDL-DIR),)
  HDL-DIR := $(M_ROOT_DIR)/hdl
endif

## defines (& re-defines) from command line
## simple diff and decide if we need to rebuild

$(shell touch mkdefines.log)

REBUILD := 0
PREV_DEFINE := $(shell cat mkdefines.log)
ifeq ($(DEFINE),)
  DEFINE := $(PREV_DEFINE)
endif
ifneq ($(PREV_DEFINE),$(DEFINE))
  REBUILD := 1
endif

$(shell echo $(DEFINE) > mkdefines.log)

