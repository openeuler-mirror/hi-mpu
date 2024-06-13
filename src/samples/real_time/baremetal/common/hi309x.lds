
OUTPUT_ARCH(aarch64)
ENTRY(app_startup)
SECTIONS
{
	. = 0x8f200000;
	__image_copy_start =.;
	. = ALIGN(4);
	.text	:
	{
        __text_start = .;
		KEEP(*(.mcsboot.text))
		KEEP(*(.boot.text))
		*(.init.text)
        __init_end = .;
        ASSERT(((__init_end - __text_start) < 0x800), "init sections too big!");
		*(.vectortbl.text)
		*(.text*)
	}
	__text_end = .;

	. = ALIGN(4);
	.image : { *(.image*) }

	. = ALIGN(4);
	.rodata : { *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.rodata*))) }

	. = ALIGN(4);
	.data : { *(.data*) }

	. = ALIGN(4);
	.got : { *(.got*) }

	. = ALIGN(4);
	__image_copy_end =.;
	__bss_start = .;
	.bss : { *(.bss*) }
	__bss_end = .;

	.xfer_segment	:
	{
		. = ALIGN(8);
		__xfer_segment_start = .;
		*(.xfer_segment*)
		__xfer_segment_end = .;
		ASSERT(((__xfer_segment_end - __xfer_segment_start) < 0x10000), "xfer sections too big!");
	}

	_end = .;
}
