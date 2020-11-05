#include <stdlib.h>

char instructions[8] = {
    '>', // incr PC
    '<', // decr PC
    '+', // incr val at PC
    '-', // decr val at PC
    '.', // output byte at PC
    ',', // input byte, write to PC
    '[', // jeq to matching ']'
    ']'  // jne to matching '['
    }
