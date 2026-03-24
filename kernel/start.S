.section .text

.extern kernel
.extern slavecore

.extern __bss_start
.extern __bss_end

.global entry0
.extern stack0_top

entry0:
    mov     x1, #0
    b       el_check
entry0_start:
    ldr     x0, =stack0_top
    mov     sp, x0

    bl      zero_bss
    bl      kernel
    b       hold

.global entry1
.extern stack1_top

entry1:
    mov     x1, #1
    b       el_check
entry1_start:
    ldr     x0, =stack1_top
    mov     sp, x0

    mov     x0, x1
    bl      slavecore
    b       hold

.global entry2
.extern stack2_top

entry2:
    mov     x1, #2
    b       el_check
entry2_start:
    ldr     x0, =stack2_top
    mov     sp, x0

    mov     x0, x1
    bl      slavecore
    b       hold

.global entry3
.extern stack3_top

entry3:
    mov     x1, #3
    b       el_check
entry3_start:
    ldr     x0, =stack3_top
    mov     sp, x0
    
    mov     x0, x1
    bl      slavecore
    b       hold

hold:
    wfi
    b       hold

zero_bss:
    ldr     x0, =__bss_start
    ldr     x1, =__bss_end
zero_bss_clr:
    cmp     x0, x1
    bge     zero_bss_done
    str     xzr, [x0], #8
    b       zero_bss_clr
zero_bss_done:
    ret

el_check:
    mrs     x0, currentel
    lsr     x0, x0, #2
    and     x0, x0, #3

    ldr     x2, =boot_el
    str     x0, [x2]

    cmp     x0, #2
    beq     el_check_el2
    cmp     x0, #1
    beq     el_check_el1
    b       hold

el_check_el2:
    mov     x0, #(1 << 31)
    msr     hcr_el2, x0

    mrs     x0, cnthctl_el2
    orr     x0, x0, #3
    msr     cnthctl_el2, x0
    msr     cntvoff_el2, xzr

    msr     cptr_el2, xzr

    mov     x0, #0x3c5
    msr     spsr_el2, x0

    adr     x0, el_check_el1
    msr     elr_el2, x0

    isb
    eret
    
el_check_el1:
    cmp     x1, #0
    beq     entry0_start
    cmp     x1, #1
    beq     entry1_start
    cmp     x1, #2
    beq     entry2_start
    cmp     x1, #3
    beq     entry3_start
    b       hold

.section .data

.global boot_el
boot_el:
    .quad 0
