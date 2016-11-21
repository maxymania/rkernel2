/* http://wiki.osdev.org/Meaty_Skeleton#kernel.2Farch.2Fi386.2Ftty.c */
#include <machine/types.h>
#include <sysplatform/console.h>
#include "vga.h"


static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
//static u_int16_t* const VGA_MEMORY = (u_int16_t*) 0xB8000;
static u_int16_t* const VGA_MEMORY = (u_int16_t*) 0xC0BFF000;

static size_t terminal_row;
static size_t terminal_column;
static u_int8_t terminal_color;
static u_int16_t* terminal_buffer;

void console_init() {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = VGA_MEMORY;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

static
void terminal_setcolor(u_int8_t color) {
	terminal_color = color;
}

static
void terminal_putentryat(unsigned char c, u_int8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

size_t console_get_col(){
	return terminal_column;
}

void console_carriage_return() {
	terminal_column = 0;
};
void console_newline() {
	if (++terminal_row == VGA_HEIGHT)
		terminal_row = 0;
};

void console_putchar(char c) {
	unsigned char uc = c;
	terminal_putentryat(uc, terminal_color, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT)
			terminal_row = 0;
	}
}

void console_write_text(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		console_putchar(data[i]);
}

#if 0
void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}
#endif

