.code64
.text
.global idle

idle:
    rep; nop
    jmp idle
