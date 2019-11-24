TEXT_START = 0x0fff00;

ENTRY(trampoline)

PHDRS {
    text PT_LOAD FLAGS(5);
}

SECTIONS {
    . = TEXT_START;

    .text :
    {
        *(.text)
    } :text

    . = ALIGN(0x1000);
}
