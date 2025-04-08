
#include "black_box.h"
void init_adc() {
    ADFM = 1; //right justification

    /*conversion time*/
    ADCS2 = 0;
    ADCS1 = 1;
    ADCS0 = 0;

    /*Acq time*/
    ACQT2 = 1;
    ACQT1 = 0;
    ACQT0 = 0;

    ADRESH = 0;
    ADRESL = 0;
    GO = 0;
    ADON = 1;
}
int read_adc(char channel) 
{
    unsigned int digital_val=0;
    ADCON0 = ADCON0 & 0XC3 | (channel << 2);
    GO = 1;
    while (GO);
    digital_val = ADRESL | ADRESH << 8;
    return digital_val;
}

