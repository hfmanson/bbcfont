#include <stdio.h>

/*
http://en.wikipedia.org/wiki/Glyph_Bitmap_Distribution_Format
http://www.math.utah.edu/~beebe/fonts/X-Window-System-fonts.html
http://damieng.com/blog/2011/02/20/typography-in-8-bits-system-fonts

*/

#define NR_CHARS 0x60
#define NR_EXTRA_CHARS (sizeof(extra_chars) / sizeof(extra_chars[0]))
#define NR_BYTES (NR_CHARS * 8)

typedef unsigned char  uchar;
typedef unsigned short ushort;

static char os12rom[] = "/usr/local/share/beebem/roms/bbc/os12.rom";

static uchar extra_chars[][8] = 
{
    { (uchar) 0x00, (uchar) 0x00, (uchar) 0x00, (uchar) 0x00, (uchar) 0x00, (uchar) 0x00, (uchar) 0x00, (uchar) 0x00 },
    { (uchar) 0x00, (uchar) 0x00, (uchar) 0x00, (uchar) 0x18, (uchar) 0x18, (uchar) 0x00, (uchar) 0x00, (uchar) 0x00 },
    { (uchar) 0x3C, (uchar) 0x7E, (uchar) 0x99, (uchar) 0x99, (uchar) 0xFF, (uchar) 0x81, (uchar) 0x42, (uchar) 0x3C },
    { (uchar) 0x3C, (uchar) 0x7E, (uchar) 0x99, (uchar) 0xBD, (uchar) 0xFF, (uchar) 0xBD, (uchar) 0x42, (uchar) 0x3C },
    { (uchar) 0x00, (uchar) 0x7E, (uchar) 0x42, (uchar) 0x42, (uchar) 0x42, (uchar) 0x42, (uchar) 0x7E, (uchar) 0x00 },
    { (uchar) 0x00, (uchar) 0x7E, (uchar) 0x42, (uchar) 0x5A, (uchar) 0x5A, (uchar) 0x42, (uchar) 0x7E, (uchar) 0x00 },
    { (uchar) 0xFF, (uchar) 0xD5, (uchar) 0xAB, (uchar) 0xD5, (uchar) 0xAB, (uchar) 0xD5, (uchar) 0xAB, (uchar) 0xFF }
};

void bdf_header(char *fontname, int nr_chars, int char_width, int char_height)
{
    printf("STARTFONT 2.1\n");
    printf("FONT %s\n", fontname);
    printf("SIZE 16 75 75\n");
    printf("FONTBOUNDINGBOX %d %d 0 0\n", char_width, char_height);
    printf("STARTPROPERTIES 2\n");
    printf("FONT_ASCENT %d\n", char_height);
    printf("FONT_DESCENT 0\n");
    printf("ENDPROPERTIES\n");
    printf("CHARS %d\n", nr_chars);
}

void bdf_char(int ch, int char_width, int char_height, uchar *bitmap)
{
    int i;

    printf("STARTCHAR U+%04X\n", ch);
    printf("ENCODING %d\n", ch);
    printf("SWIDTH 500 0\n");
    printf("DWIDTH 8 0\n");
    printf("BBX %d %d 0 0\n", char_width, char_height);
    printf("BITMAP\n");
    for (i = 0; i < 8; i++) {
        printf("%02X\n", *bitmap++);
    }
    printf("ENDCHAR\n");
}

void bdf_chars(int nr_chars, int ch, int char_width, int char_height, uchar *bitmap)
{
    int i;

    for (i = 0; i < nr_chars; i++) {
        bdf_char(ch, char_width, char_height, bitmap);
        bitmap += 8;
        ch++;
    }
}

ushort double_bits(uchar bitmap)
{
    ushort ch16   = 0;
    uchar  mask8  = 1;
    ushort mask16 = 1;
    int    i;

    for (i = 0; i < 8; i++) {
        if (bitmap & mask8) {
            ch16 |= mask16;
            mask16 <<= 1;
            ch16 |= mask16;
            mask16 <<= 1;
        } else {
            mask16 <<= 2;
        }
        mask8 <<= 1;
    }
    return ch16;
}

void bdf_char16x16(int ch, int char_width, int char_height, uchar *bitmap)
{
    int i;

    printf("STARTCHAR U+%04X\n", ch);
    printf("ENCODING %d\n", ch);
    printf("SWIDTH 500 0\n");
    printf("DWIDTH 16 0\n");
    printf("BBX %d %d 0 0\n", char_width, char_height);
    printf("BITMAP\n");
    for (i = 0; i < 8; i++) {
        ushort ch16 = double_bits(*bitmap++);
        printf("%04X\n%04X\n", ch16, ch16);
    }
    printf("ENDCHAR\n");
}

void bdf_chars16x16(int nr_chars, int ch, int char_width, int char_height, uchar *bitmap)
{
    int i;

    for (i = 0; i < nr_chars; i++) {
        bdf_char16x16(ch, char_width, char_height, bitmap);
        bitmap += 8;
        ch++;
    }
}

void bdf_footer()
{
    printf("ENDFONT\n");
}

void bbc8x8(uchar *buf, int nr_chars)
{
    bdf_header("-bbc-unifont-medium-r-normal--16-160-75-75-c-80-iso10646-1", nr_chars, 8, 8);
    bdf_chars(NR_CHARS, ' ', 8, 8, buf);
    bdf_chars(NR_EXTRA_CHARS, 240, 8, 8, extra_chars[0]);
    bdf_footer();
}

void bbc16x16(uchar *buf, int nr_chars)
{
    bdf_header("-bbc-unifont-medium-r-normal--16-160-75-75-c-80-iso10646-1", nr_chars, 16, 16);
    bdf_chars16x16(NR_CHARS, ' ', 16, 16, buf);
    bdf_chars16x16(NR_EXTRA_CHARS, 240, 16, 16, extra_chars[0]);
    bdf_footer();
}

int main(int argc, char *argv[])
{
    int   nr_chars = NR_CHARS + NR_EXTRA_CHARS;
    FILE  *f;    
    uchar buf[NR_BYTES];

    if ((f = fopen(os12rom, "r")) == NULL) {
        fprintf(stderr, "%s not found\n", os12rom);
        return 1;
    }
    fread(buf, 1, NR_BYTES, f);
    fclose(f);

#ifdef SINGLE_PIXEL
    bbc8x8(buf, nr_chars);
#else
    bbc16x16(buf, nr_chars);
#endif
    return 0;
}
