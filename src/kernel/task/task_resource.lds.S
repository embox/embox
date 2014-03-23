SECTIONS {
	.bss.ktask : {
		*(.bss.ktask)

		_ktask_resource_start = .;
		*(.bss.ktask.resource)
		_ktask_resource_end = .;
	}
}
