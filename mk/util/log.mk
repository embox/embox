#
#   Date: Nov 21, 2010
# Author: Eldar Abusalimov
#

ifndef __util_log_mk
__util_log_mk := 1

log = $(and $(log_entry),)

log_entry = $(call __log_entry,$(__log_tag),$2)

log_error   = $(and $(log_error_entry),)
log_warning = $(and $(log_warning_entry),)

log_error_entry   = $(call log_entry,$1,$(call   error_str_file,$2)$3)
log_warning_entry = $(call log_entry,$1,$(call warning_str_file,$2)$3)

log_id_last = $(__log_counter_get)

__log_entry = $(call __log_entry_record \
  ,$(__log_counter_inc_and_get:%=$(__log_id_by_tag_pattern)),$2)

__log_entry_record = ${eval $$1 := $$2}$1

__log_tag_by_id_pattern = __log-$(__log_id)-%
__log_id_by_tag_pattern = __log-%-$(__log_tag)

__log_id = $(word $1,)$1
__log_tag = \
  $(call assert,$(call list_single,$1),Invalid log tag: [$1])$(strip $1)

log_ids_by_tag = \
  $(sort $(call __log_filter_patsubst,$(__log_id_by_tag_pattern)))
log_tag_by_id  = $(call __log_filter_patsubst,$(__log_tag_by_id_pattern))

__log_filter          = $(filter $1,$(.VARIABLES))
__log_filter_patsubst = $(patsubst $1,%,$(__log_filter))

log_report_entries = $(call __log_report,$1)
log_report_by_tag  = $(call __log_report,$(__log_id_by_tag_pattern))
log_report_by_id   = $(call __log_report,$(__log_tag_by_id_pattern))

__log_report = $(foreach e,$(__log_filter),$(info $($e)))

log_get_by_id  = $(call __log_report,$(__log_tag_by_id_pattern))

__log_counter :=
__log_counter_get = $(words $(__log_counter))
__log_counter_inc_and_get = \
  ${eval __log_counter += x}$(__log_counter_get)

# Make-style error and warning strings.

# The most general way to get error/warning string.
# First argument should contain the location to print (directory and file).
error_str_file   = $1:0: error:
warning_str_file = $1:0: warning:

# Print location using the first argument as directory
# and 'Makefile' as file within the directory.
error_str_dir    = $(call error_str_file,$1/Makefile)
warning_str_dir  = $(call warning_str_file,$1/Makefile)

# Generates error/warning string in $(dir)/Makefile.
error_str        = $(call error_str_dir,$(dir))
warning_str      = $(call warning_str_dir,$(dir))

endif # __util_log_mk
