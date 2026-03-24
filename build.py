import os

#CC = "arm-none-eabi" # 32b
CC = "aarch64-none-elf" #64b

INCLUDE = "include"
LDC = "linker/linker.ld"
BUILDDIR = "build"

#IMAGE = "kernel7" #32b
IMAGE = "kernel8" #64b

#ASFLAGS = ("-mcpu=cortex-a53") #32b
ASFLAGS = ("-march=armv8-a") #64b

CFLAGS  = ("-Iinclude/ "
           "-ffreestanding "
           "-O2 "
           "-nostdlib "
           "-Wall "
           "-Wextra")

LDFLAGS = (f"-T {LDC} "
           "-ffreestanding "
           "-O2 "
           "-nostdlib "
           "-Wall "
           "-Wextra")

OBJFLAGS = ("-O binary")

SRC = ["kernel/start.S",
       "kernel/kernel.c",
       "src/driver/board.c",
       "src/driver/emmc.c",
       "src/driver/firmware.c",
       "src/driver/gpio.c",
       "src/driver/lfb.c",
       "src/driver/mailbox.c",
       "src/driver/spi.c",
       "src/driver/temp.c",
       "src/driver/timer.c",
       "src/fs/fs.c",
       "src/mem.c",
       "src/string.c",
       "src/video.c"]

for src in SRC:
    if src.endswith(".c") or src.endswith(".S"):
        os.system(f"{CC}-gcc {CFLAGS} -c {src} -o {BUILDDIR}/{src.replace("/", "_")}.o")
    elif (src.endswith(".s")):
        os.system(f"{CC}.s {ASFLAGS} {ASFLAGS} {src} -o {BUILDDIR}/{src.replace("/", "_")}.o")
    else: continue
    print(f"{src} -> {BUILDDIR}/{src.replace("/", "_")}.o")

OUT = ""
for src in SRC: OUT += f"{BUILDDIR}/{src.replace("/", "_")}.o "

os.system(f"{CC}-gcc {LDFLAGS} -o {BUILDDIR}/{IMAGE}.elf {OUT}")
print(f"{BUILDDIR}/* -> {BUILDDIR}/{IMAGE}.elf")

os.system(f"{CC}-objcopy {BUILDDIR}/{IMAGE}.elf {OBJFLAGS} {IMAGE}.img")
print(f"{BUILDDIR}/{IMAGE}.elf -> {IMAGE}.img")