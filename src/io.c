#include <stdarg.h>
#include <stdint.h>
#include "ioport.h"

static void convert_n_print(uint64_t n, int base) {
	char symbols[] = "0123456789ABCDEF";
	char digits[20];

	print_char('0');
	if (base == 16) {
		print_char('x');
	}

	int i = 0;
	for (; n != 0; i++) {
		digits[i] = symbols[n % base];
		n = n / base;
	}
	i--;

	for (; i >= 0; i--) {
		print_char(digits[i]);
	}
}

static void print_dec(int64_t n) {
	char symbols[] = "0123456789";
	char digits[20];

	int i = 0;
	for (; n != 0; i++) {
		digits[i] = symbols[n % 10];
		n = n / 10;
	}
	i--;

	for (; i >= 0; i--) {
		print_char(digits[i]);
	}
}

static void print_udec(uint64_t n) {
	char symbols[] = "0123456789";
	char digits[20];

	int i = 0;
	for (; n != 0; i++) {
		digits[i] = symbols[n % 10];
		n = n / 10;
	}
	i--;

	for (; i >= 0; i--) {
		print_char(digits[i]);
	}
}

static void print_s(char *str) {
	while (*str != '\0') {
		print_char(*str);
		str++;
	}
}

void printf(const char * format, ...) {
	char *cur_char;

	va_list arg;
	va_start(arg, format);

	for (cur_char = (char *)format; *cur_char != '\0'; cur_char++) {
		while (*cur_char != '%' && *cur_char != '\0') {
			print_char(*cur_char);
			cur_char++;
		}

		if (*cur_char == '\0') break;

		cur_char++;

		switch (*cur_char) {
			case 'c': {
				print_char(va_arg(arg, int));
				break;
			}
			case 'd': case 'i': {
				print_dec(va_arg(arg, int));
				break;
			}
			case 'u':{
				print_udec(va_arg(arg, uint32_t));
				break;
			}
			case 'o': {
				convert_n_print(va_arg(arg, int), 8);
				break;
			}
			case 'x': {
				convert_n_print(va_arg(arg, int), 16);
				break;
			}
			case 's': {
				print_s(va_arg(arg, char*));
				break;
			}
			case 'h': case 'l': {
				cur_char++;
				if (*cur_char == 'h') {
					cur_char++;
				}
				if (*cur_char == 'l') {
					cur_char++;
					switch (*cur_char) {
						case 'i': case 'd': {
							print_dec(va_arg(arg, int64_t));
							break;
						}
						case 'u': {
							print_udec(va_arg(arg, uint64_t));
							break;
						}
						case 'o': {
							convert_n_print(va_arg(arg, uint64_t), 8);
							break;
						}
						case 'x': {
							convert_n_print(va_arg(arg, uint64_t), 16);
							break;
						}
					}
				} else {
					switch (*cur_char) {
						case 'i': case 'd': {
							print_dec(va_arg(arg, int));
							break;
						}
						case 'u': {
							print_udec(va_arg(arg, uint32_t));
							break;
						}
						case 'o': {
							convert_n_print(va_arg(arg, uint32_t), 8);
							break;
						}
						case 'x': {
							convert_n_print(va_arg(arg, uint32_t), 16);
							break;
						}
					}
				}
			}
		}
	}
}