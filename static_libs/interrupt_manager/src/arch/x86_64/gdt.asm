section .text

gdtr DW 0 ; For limit storage
     DQ 0 ; For base storage

setGdt:
   mov   [gdtr], rdi
   mov   [gdtr+2], rsi
   lgdt  [gdtr]
   ret