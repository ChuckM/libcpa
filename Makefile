# Makefile to build the CPA library of utility functions.
# I seem to use these in all my programs so they are being
# collected here.

LIBNAME	= libcpa

FP_FLAGS	?= -mfloat-abi=hard -mfpu=fpv4-sp-d16
PREFIX		?= arm-none-eabi
CC			= $(PREFIX)-gcc
AR			= $(PREFIX)-ar
DESTDIR	= /home/cmcmanis/gcc-arm-none-eabi-4_7-2013q3/$(PREFIX)

CFLAGS	= -Os -g \
          -Wall -Wextra -Wimplicit-function-declaration \
          -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes \
          -Wundef -Wshadow \
          -I../../../include -fno-common \
          -mcpu=cortex-m4 -mthumb $(FP_FLAGS) \
          -Wstrict-prototypes \
          -ffunction-sections -fdata-sections -MD -DSTM32F4

# ARFLAGS   = rcsv
ARFLAGS     = rcs

SRCLIBDIR = .

OBJS	= clock.o term.o uart.o sdio.o debug.o dump.o gfx.o gpio.o

include Makefile.include
