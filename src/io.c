#include <stdint.h>
#include "io.h"
#include "ioport.h"

static void basic_print(const char* format, va_list arg);
static void basic_sprint(char * s, size_t n, const char* format, va_list arg);

void printf(const char * format, ...) {
	va_list arg;
	va_start(arg, format);
	basic_print(format, arg);
	va_end(arg);
}

void vprintf(const char * format, va_list arg) {
	basic_print(format, arg);
}

void snprintf(char * s, size_t n, const char * format, ...) {
	va_list arg;
	va_start(arg, format);
	basic_sprint(s, n, format, arg);
	va_end(arg);
}

void vsnprintf(char * s, size_t n, const char * format, va_list arg) {
	basic_sprint(s, n, format, arg);
}

static void convert_n_print(uint64_t n, int base) {
	char symbols[] = "0123456789ABCDEF";
	char digits[30];

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

	if (n < 0) {
		print_char('-');
		n = -n;
	}

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

static void print_str(char *str) {
	while (*str != '\0') {
		print_char(*str);
		str++;
	}
}

static void basic_print(const char* format, va_list arg) {
	const char *cur_char;
	for (cur_char = format; *cur_char != '\0'; cur_char++) {
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
				print_str(va_arg(arg, char*));
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

static void sprint_char(char **s, int64_t *size, char c) {
	if (*size <= 0) {
		return;
	}
	(*s)[0] = c;
	*s = *s + 1;
	*size = *size - 1;
}

static void convert_n_sprint(char **s, int64_t* size, uint64_t n, int base) {
	char symbols[] = "0123456789ABCDEF";
	char digits[20];

	sprint_char(s, size, '0');
	if (base == 16) {
		sprint_char(s, size, 'x');
	}

	int i = 0;
	for (; n != 0; i++) {
		digits[i] = symbols[n % base];
		n = n / base;
	}
	i--;

	for (; i >= 0; i--) {
		sprint_char(s, size, digits[i]);
	}
}

static void sprint_dec(char **s, int64_t* size, int64_t n) {
	char symbols[] = "0123456789";
	char digits[20];

	if (n < 0) {
		sprint_char(s, size, '-');
		n = -n;
	}

	int i = 0;
	for (; n != 0; i++) {
		digits[i] = symbols[n % 10];
		n = n / 10;
	}
	i--;

	for (; i >= 0; i--) {
		sprint_char(s, size, digits[i]);
	}
}

static void sprint_udec(char **s, int64_t* size, uint64_t n) {
	char symbols[] = "0123456789";
	char digits[20];

	int i = 0;
	for (; n != 0; i++) {
		digits[i] = symbols[n % 10];
		n = n / 10;
	}
	i--;

	for (; i >= 0; i--) {
		sprint_char(s, size, digits[i]);
	}
}

static void sprint_str(char **dst, int64_t* size, char * src) {
	while (*src != '\0') {
		sprint_char(dst, size, *src);
		src++;
	}
}


void basic_sprint(char * s, size_t n_, const char* format, va_list arg) {
	if (n_ == 0) {
		s[0] = '\0';
		return;
	}
	int64_t n = n_ - 1;
	printf("my n = %d\n", n);
	const char *cur_char;
	char *dst_char = s;
	for (cur_char = format; *cur_char != '\0' && n >= 0; cur_char++) {
		while (*cur_char != '%' && *cur_char != '\0') {
			sprint_char(&dst_char, &n, *cur_char);
			cur_char++;
		}

		if (*cur_char == '\0') break;

		cur_char++;

		switch (*cur_char) {
			case 'c': {
				sprint_char(&dst_char, &n, va_arg(arg, int));
				break;
			}
			case 'd': case 'i': {
				sprint_dec(&dst_char, &n, va_arg(arg, int));
				break;
			}
			case 'u': {
				sprint_udec(&dst_char, &n, va_arg(arg, uint32_t));
				break;
			}
			case 'o': {
				convert_n_sprint(&dst_char, &n, va_arg(arg, int), 8);
				break;
			}
			case 'x': {
				convert_n_sprint(&dst_char, &n, va_arg(arg, int), 16);
				break;
			}
			case 's': {
				sprint_str(&dst_char, &n, va_arg(arg, char*));
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
							sprint_dec(&dst_char, &n, va_arg(arg, int64_t));
							break;
						}
						case 'u': {
							sprint_udec(&dst_char, &n, va_arg(arg, uint64_t));
							break;
						}
						case 'o': {
							convert_n_sprint(&dst_char, &n, va_arg(arg, uint64_t), 8);
							break;
						}
						case 'x': {
							convert_n_sprint(&dst_char, &n, va_arg(arg, uint64_t), 16);
							break;
						}
					}
				} else {
					switch (*cur_char) {
						case 'i': case 'd': {
							sprint_dec(&dst_char, &n, va_arg(arg, int));
							break;
						}
						case 'u': {
							sprint_udec(&dst_char, &n, va_arg(arg, uint32_t));
							break;
						}
						case 'o': {
							convert_n_sprint(&dst_char, &n, va_arg(arg, uint32_t), 8);
							break;
						}
						case 'x': {
							convert_n_sprint(&dst_char, &n, va_arg(arg, uint32_t), 16);
							break;
						}
					}
				}
			}
		}
	}
	*dst_char = '\0';
}