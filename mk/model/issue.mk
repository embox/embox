#
#   Date: Mar 27, 2012
# Author: Anton Kozlov
#

ifndef __model_issue_mk
__model_issue_mk := 1

define class-IssueReceiver
	$(property-field issues... : Issue)

	$(method addIssues,
		$(set+ issues,$1))

	$(method getIssues,
		$(get issues))

	$(method printIssues,
		$(for issue <- $(get issues),
			$(invoke issue->report)))

endef

# Constructor args:
#   1. Resource.
define class-Issue
	$(property resource : Resource)

	$(property severity)
	$(property location)
	$(property message)

	$(property isError)
	$(getter isError,
		$(filter error,$(get severity)))

	$(property isWarning)
	$(getter isWarning,
		$(filter warning,$(get severity)))

	$(method report,
		$(with \
			$(get $(get resource).fileName):$(get location): \
			$(get severity): $(get message).,

			$(shell env echo '$(subst ','"'"',$1)' >&2)))

endef

# Constructor args:
#   1. Resource.
#   2. (optional) severity.
#   3. (optional) location.
#   4. (optional) message.
define class-BaseIssue
	$(super Issue)

	$(property-field resource : Resource,$1)

	$(property-field severity,$(value 2))
	$(property-field location,$(value 3))
	$(property-field message,$(value 4))

endef

# Constructor args:
#   1. Link.
define class-UnresolvedLinkIssue
	$(super Issue)

	$(property-field link : ELink,$1)

	$(property resource : Resource)
	$(getter resource,
		$(get $(get link).eResource))

	$(getter severity,
		error)
	$(getter location,
		$(get $(get link).origin))
	$(getter message,
		$(for link     <- $(get link),
			targetType <- $(get $(get link->eMetaReference).eReferenceType),

			Couldn't resolve reference to $(get targetType->name) \
				'$(get link->name)'))

endef

$(def_all)

endif # __model_issue_mk
