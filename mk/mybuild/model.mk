#
# Base model framework.
#
#   Date: Dec 7, 2011
# Author: Eldar Abusalimov
#

ifndef __mybuild_model_mk
__mybuild_model_mk := 1

include mk/core/define.mk
include mk/core/object.mk

include mk/mybuild/scoping.mk

define class-node

	$(field container : node)

	# 1. Field of this.
	# 2. What to add.
	$(method add_to,
		$(set+ $1,
			$(foreach e,$(filter-out $(get $1),$2),$e
				$(invoke e->inverse_add_to,$(basename $e),$1$(this))
			)
		)
	)

	# 1. Field of this.
	# 2. What to remove.
	$(method remove_from,
		$(set- $1,
			$(foreach e,$(filter $2,$(get $1)),$e
				$(invoke e->inverse_remove_from,$(basename $e),$1$(this))
			)
		)
	)

	# 1. Field of this.
	$(method clear_refs,
		$(silent-foreach e,$(get $1),
			$(invoke e->inverse_remove_from,$(basename $e),$1$(this))
		)
		$(set $1,)
	)

	# 1. Field of this.
	# 2. New references.
	$(method replace_refs,
		$(invoke clear_refs,$1)
		$(invoke add_to,$1,$2)
	)

	# 1. Field of this.
	# 2. What is being added.
	$(method inverse_add_to,
		$(assert $(singleword $2),'$0' handles a single reference at once)

		$(if $1,
			$(set+ $1,$2),

			$(assert $(not $(multiword $(get container))))
			$(invoke clear_refs,container)
			$(set container,$2)
		)
	)

	# 1. Field of this.
	# 2. What is being removed.
	$(method inverse_remove_from,
		$(assert $(singleword $2),'$0' handles a single reference at once)

		$(if $1,
			$(set- $1,$2),

			$(assert $(eq $2,$(get container)))
			$(set container,)
		)
	)

	$(method get_container,
		$(suffix $(get container)))

	# 1. New container.
	$(method set_container,
		$(assert $(not $(multiword $1)),
			A node can't have more than one container)
		$(assert $(not $(multiword $(get container))))

		$(if $(not $(eq $1,$(get container))),
			$(invoke clear_refs,container)
			$(invoke add_to,container,$1)
		)
	)

	$(method serialize,
		$(if $(filter $(this),$1),,
			$(this) := $($(this))$(\n)
			$(foreach f,$(call field_name,$($($(this)).fields)),
				$(this).$f := $($(__this).$f)$(\n))))

	$(method deserialize,
		${eval \
			$*
		})


endef

# Constructor args:
#   1. Name.
define class-named
	$(field name,$(value 1))

	$(method get_name,$(get name))
	$(method set_name,$(set name,$1))
endef

define class-resource
	$(field nodes : node)
	$(field issues)
endef

# Constructor args:
#   1. Name representing the crosslink.
define class-abstract_ref
	$(super node)

	$(field src : *)
	$(field dst : *)

	$(field link_name,$1)

	$(method get_link_name,$(get link_name))

	$(method get_link_type)
endef

$(def_all)

endif # __mybuild_model_mk
