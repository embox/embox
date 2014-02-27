SECTIONS {
	.bss..reserve.ktask : {
		*(.bss..reserve.ktask)

		_ktask_resource_start = .;
		*(.bss..reserve.ktask.resource)
		_ktask_resource_end = .;
	}
}
