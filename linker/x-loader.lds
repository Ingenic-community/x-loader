/*
 * Ingenic X1000 TCSM size 12KByte
 */
MEMORY
{
	tcsm (rx) : ORIGIN = 0xf4001000, LENGTH = 12K
	dram (rx) : ORIGIN = 0x80000020, LENGTH = 12256
}

OUTPUT_FORMAT("elf32-tradlittlemips", "elf32-tradbigmips", "elf32-tradlittlemips")
OUTPUT_ARCH(mips)
ENTRY(__start)
SECTIONS
{
    . = ORIGIN(dram);

	.text_dram : {
	    */libgcc.a (.text*)
	    */stage2/lib/*/*.o (.text*)
	    */stage2/*.o (.text*)
	} >dram

    . = ALIGN(4);
	.data_dram : {
        */libgcc.a (SORT_BY_ALIGNMENT(.data*))
        */stage2/lib/*/*.o (SORT_BY_ALIGNMENT(.data*))
        */stage2/*.o (SORT_BY_ALIGNMENT(.data*))
    } >dram

    . = ALIGN(4);
    .rodata_dram : {
        */libgcc.a (SORT_BY_ALIGNMENT(.rodata*))
        */stage2/lib/*/*.o (SORT_BY_ALIGNMENT(.rodata*))
        */stage2/*.o (SORT_BY_ALIGNMENT(.rodata*))
    } >dram

    . = ORIGIN(tcsm);

	.text : { *(.text*) } >tcsm

	. = ALIGN(4);
	.rodata : { *(SORT_BY_ALIGNMENT(.rodata*)) } >tcsm

	. = ALIGN(4);
	.data : { *(SORT_BY_ALIGNMENT(.data*)) } >tcsm

	. = ALIGN(4);
	__image_copy_end = .;

	.bss : {
		. = ALIGN(4);
		__bss_start = .;
		*(.sbss.*)
		*(.bss.*)
		*(COMMON)
		. = ALIGN(4);
		__bss_end = .;
	} >tcsm

	/DISCARD/ : {
		*(.dynbss)
		*(.dynstr)
		*(.dynamic)
		*(.interp)
		*(.hash)
		*(.gnu.*)
		*(.note.*)
		*(.plt)
		*(.got.plt)
		*(.rel.plt)
		*(.rel.dyn)
	}
}