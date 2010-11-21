#
#   Date: Nov 21, 2010
# Author: Eldar Abusalimov
#

ifndef __util_log_mk
__util_log_mk := 1

__log_counter :=
__log_counter_get = \
  $(words $(__log_counter))
__log_counter_inc_and_get = \
  ${eval __log_counter += x}$(__log_counter_get)

log = $(call __log_record,$(__log_tag))
log_id_last = $(__log_counter_get)

__log_record = \
  ${eval $(__log_counter_inc_and_get:%=$(__log_id_by_tag_pattern)) := $$2)

__log_tag_by_id_pattern = __log_record_$(__log_id)_%
__log_id_by_tag_pattern = __log_record_%_$(__log_tag)

__log_id = $(word $1,)$1
__log_tag = \
  $(call assert,$(filter 0 1,$(words $(value 1))),Invalid log record tag: \
           [$(value 1)])$(or $(strip $(value 1)),__log_generic))

log_ids_by_tag = \
  $(sort $(call __log_filter_patsubst,$(__log_id_by_tag_pattern)))
log_tag_by_id  = $(call __log_filter_patsubst,$(__log_tag_by_id_pattern))

__log_filter          = $(filter $1,$(.VARIABLES))
__log_filter_patsubst = $(patsubst $1,%,$(__log_filter))

log_report_by_tag = $(call __log_report,$(__log_id_by_tag_pattern))
log_report_by_id  = $(call __log_report,$(__log_tag_by_id_pattern))

__log_report = $(foreach e,$(__log_filter),$(info $e))

log_get_by_id  = $(call __log_report,$(__log_tag_by_id_pattern))

# Make-style error and warning strings.

# The most general way to get error/warning string.
# First argument should contain the location to print (directory and file).
error_str_file   = $1:1: error:
warning_str_file = $1:1: warning:

# Print location using the first argument as directory
# and 'Makefile' as file within the directory.
error_str_dir    = $(call error_str_file,$1/Makefile)
warning_str_dir  = $(call warning_str_file,$1/Makefile)

# Generates error/warning string in $(dir)/Makefile.
error_str        = $(call error_str_dir,$(dir))
warning_str      = $(call warning_str_dir,$(dir))

endef # __util_log_mk
