SECTIONS {
	.bss.embox.ktask : {
		*(.bss.embox.ktask)

		_ktask_resource_start = .;
		*(.bss.embox.ktask.resource)
		_ktask_resource_end = .;
	}
}
