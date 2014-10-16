bbcfont
=======
generate X .bdf file out of BBC Micro font contained in /usr/local/share/beebem/roms/bbc/os12.rom

compile
=======
for 16x16 font<br/>
<code>gcc -o createbdf createbdf.c</code>

for 8x8 font<br/>
<code>gcc -DSINGLE_PIXEL -o createbdf createbdf.c</code>

you can define 'user-defined' characters in the extra_chars constant starting at 240.

run
===
<code>./createbdf > bbc16x16.bdf</code><br/>
<code>./createbdf > bbc8x8.bdf</code>

