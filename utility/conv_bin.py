# Happily copy pasted from https://gist.github.com/tedyapo/3588813459d3fd7ca5fd4962d7242c0c :)
# Thanks random internet guy

import sys
import os

def bin2logisim(in_filename, out_filename):
    """ convert binary file to RLE-compressed logisim data file """
    infile = open(in_filename, 'rb')
    outfile = open(out_filename, 'wt')
    outfile.write('v2.0 raw\n')
    count = 0
    byte = infile.read(1)
    old_byte = byte
    while byte != b'':
        if byte != old_byte:
            if count == 1:
                outfile.write('{:x}\n'.
                          format(int.from_bytes(old_byte, byteorder='little')))
            else:
                outfile.write('{:d}*{:x}\n'.
                          format(count,
                                 int.from_bytes(old_byte, byteorder='little')))
            count = 1
        else:
            count += 1
        old_byte = byte
        byte = infile.read(1)
    if count == 1:
        outfile.write('{:x}\n'.
                      format(int.from_bytes(old_byte, byteorder='little')))
    else:
        outfile.write('{:d}*{:x}\n'.
                      format(count,
                             int.from_bytes(old_byte, byteorder='little')))
    outfile.close()
    infile.close()