SECTIONS {
        . = 0x07000000;
        .text : { * (.text); }

        . = 0x07040000;
        .data : { * (.data); }
}
