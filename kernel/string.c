#include <stdarg.h>
#include "include/types.h"
#include "include/printf.h"

void*
memset(void *dst, int c, uint n)
{
  char *cdst = (char *) dst;
  int i;
  for(i = 0; i < n; i++){
    cdst[i] = c;
  }
  return dst;
}

int
memcmp(const void *v1, const void *v2, uint n)
{
  const uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  while(n-- > 0){
    if(*s1 != *s2)
      return *s1 - *s2;
    s1++, s2++;
  }

  return 0;
}

void*
memmove(void *dst, const void *src, uint n)
{
  const char *s;
  char *d;

  s = src;
  d = dst;
  if(s < d && s + n > d){
    s += n;
    d += n;
    while(n-- > 0)
      *--d = *--s;
  } else
    while(n-- > 0)
      *d++ = *s++;

  return dst;
}

// memcpy exists to placate GCC.  Use memmove.
void*
memcpy(void *dst, const void *src, uint n)
{
  return memmove(dst, src, n);
}

int
strncmp(const char *p, const char *q, uint n)
{
  while(n > 0 && *p && *p == *q)
    n--, p++, q++;
  if(n == 0)
    return 0;
  return (uchar)*p - (uchar)*q;
}

char*
strncpy(char *s, const char *t, int n)
{
  char *os;

  os = s;
  while(n-- > 0 && (*s++ = *t++) != 0)
    ;
  while(n-- > 0)
    *s++ = 0;
  return os;
}

// Like strncpy but guaranteed to NUL-terminate.
char*
safestrcpy(char *s, const char *t, int n)
{
  char *os;

  os = s;
  if(n <= 0)
    return os;
  while(--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  return os;
}

int
strlen(const char *s)
{
  int n;

  for(n = 0; s[n]; n++)
    ;
  return n;
}

// convert uchar string into wide char string 
void wnstr(wchar *dst, char const *src, int len) {
  while (len -- && *src) {
    *(uchar*)dst = *src++;
    dst ++;
  }

  *dst = 0;
}

// convert wide char string into uchar string 
void snstr(char *dst, wchar const *src, int len) {
  while (len -- && *src) {
    *dst++ = (uchar)(*src & 0xff);
    src ++;
  }
  while(len-- > 0)
    *dst++ = 0;
}

int wcsncmp(wchar const *s1, wchar const *s2, int len) {
  int ret = 0;

  while (len-- && *s1) {
    ret = (int)(*s1++ - *s2++);
    if (ret) break;
  }

  return ret;
}

char*
strchr(const char *s, char c)
{
  for(; *s; s++)
    if(*s == c)
      return (char*)s;
  return 0;
}

// Converts an integer `value` to a null-terminated
// string and stores the result in the array given
// by `str`
char *itoa(int value, char *str) {
  char *p = str;
  if (value == 0x80000000) {
    strncpy(p, "-2147483648", 12);
    return str;
  }

  if (value == 0) {
    strncpy(p, "0", 2);
    return str;
  }

  if (value < 0) {
    *p++ = '-';
    value = -value;
  }

  char digits[11];
  int len = 0;

  while (value) {
    digits[len++] = (value % 10) + '0';
    value /= 10;
  }

  while (len--) {
    *p++ = digits[len];
  }

  *p = '\0';

  return str;
}

int
atoi(const char *s)
{
  int n;
  int neg = 1;
  if (*s == '-') {
    s++;
    neg = -1;
  }
  n = 0;
  while('0' <= *s && *s <= '9')
    n = n*10 + *s++ - '0';
  return n * neg;
}

static char digits[] = "0123456789abcdef";

static char*
sprintint(char *str, int xx, int base, int sign)
{
  char buf[16];
  int i;
  uint x;

  if(sign && (sign = xx < 0))
    x = -xx;
  else
    x = xx;

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } while((x /= base) != 0);

  if(sign)
    buf[i++] = '-';

  while(--i >= 0)
    *str++ = buf[i];

  return str;
}

void
sprintf(char *str, char *fmt, ...)
{
  va_list ap;
  int i, c;
  char *s;

  if (fmt == 0)
    panic("null fmt");

  va_start(ap, fmt);
  for(i = 0; (c = fmt[i] & 0xff) != 0; i++){
    if(c != '%'){
      *str++ = c;
      continue;
    }
    c = fmt[++i] & 0xff;
    if(c == 0)
      break;
    switch(c){
    case 'c':
      *str++ = (char)va_arg(ap, int);
      break;
    case 'd':
      str = sprintint(str, va_arg(ap, int), 10, 1);
      break;
    case 'x':
      str = sprintint(str, va_arg(ap, int), 16, 1);
      break;
    case 'p': {
      int i;
      uint64 x = va_arg(ap, uint64);
      *str++ = '0';
      *str++ = 'x';
      for (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
        *str++ = (digits[x >> (sizeof(uint64) * 8 - 4)]);
    } break;
    case 's':
      if((s = va_arg(ap, char*)) == 0)
        s = "(null)";
      for(; *s; s++)
        *str++ = *s;
      break;
    case '%':
      *str++ = '%';
      break;
    default:
      // Print unknown % sequence to draw attention.
      *str++ = '%';
      *str++ = c;
      break;
    }
  }
  *str++ = '\0';
}