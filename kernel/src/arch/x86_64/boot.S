.global _start

_start:
    cli
    call kernel_main
    
#it should never reach here, as kernel_main is a diverging function
hang:
    hlt
    jmp hang