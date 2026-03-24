ENTRY(entry0)

STACK_SIZE      = 0x1000;
SHARED_MEM_SIZE = 0x400;

SECTIONS
{
    . = 0x80000;

    .text : {
        *(.text*)
    }

    .rodata : {
        *(.rodata*)
    }

    .data : {
        *(.data*)
    }

    .bss : {
        __bss_start = .;
        *(.bss*)
        . = ALIGN(8);
        __bss_end = .;
    }

    .stack (NOLOAD) : {
        . = ALIGN(16);
        stack0_bottom = .;
        . += STACK_SIZE;
        stack0_top = .;

        . = ALIGN(16);
        stack1_bottom = .;
        . += STACK_SIZE;
        stack1_top = .;

        . = ALIGN(16);
        stack2_bottom = .;
        . += STACK_SIZE;
        stack2_top = .;

        . = ALIGN(16);
        stack3_bottom = .;
        . += STACK_SIZE;
        stack3_top = .;
    }

    .shared (NOLOAD) : {
        . = ALIGN(16);
        cpu_shared_mem = .;
        . += SHARED_MEM_SIZE;
    }

    kernel_end = .;
}
