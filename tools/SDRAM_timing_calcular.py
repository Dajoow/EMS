#! /usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright © 2023 wirano <git@wirano.me>
#
# Distributed under terms of the MIT license.


import sys
import json
import re
from math import ceil

def main(argv):
    json_file = argv[1]
    json_file = open(json_file, 'r')
    obj = json.load(json_file)

    mcu_sdram_clk = obj['mcu_sdram_clk'].lower()
    mcu_sdram_clk = re.match(r'(\d+\.?\d*)(\w+)', mcu_sdram_clk)
    if mcu_sdram_clk.group(2) == 'mhz':
        mcu_sdram_clk = int(mcu_sdram_clk.group(1)) * 1000 * 1000
    elif mcu_sdram_clk.group(2) == 'khz':
        mcu_sdram_clk = int(mcu_sdram_clk.group(1)) * 1000
    elif mcu_sdram_clk.group(2) == 'hz':
        mcu_sdram_clk = int(mcu_sdram_clk.group(1))

    tCK = 1 / mcu_sdram_clk * 1e9

    sdram = obj['sdram']

    for i in sdram:
        sdram[i] = re.match(r'(\d+\.?\d*)(\w*)', sdram[i])
        if sdram[i].group(2).lower() == 'ns':
            sdram[i] = int(sdram[i].group(1))
        elif sdram[i].group(2).lower() == 'ms':
            sdram[i] = int(sdram[i].group(1)) * 1000
        elif sdram[i].group(2).lower() == 'ck':
            sdram[i] = int(sdram[i].group(1))
        else:
            sdram[i] = int(sdram[i].group(1))

    wr_ck = sdram['tWR']
    t1 = ceil(sdram['tRAS'] / tCK) - ceil(sdram['tRCD'] / tCK)
    t2 = ceil(sdram['tRCD'] / tCK) - ceil(sdram['tRCD'] / tCK) - ceil(sdram['tRP'] / tCK)
    wr_ck = max(t1, t2, wr_ck)

    print("Load mode register to active delay:", sdram['tRSC'])
    print("Exit self-refresh delay:", ceil(sdram['tXSR'] / tCK))
    print("Self-refresh time:", ceil(sdram['tRAS'] / tCK))
    print("SDRAM common row cycle delay", ceil(sdram['tRC'] / tCK))
    print("Write recovery time:", wr_ck)
    print("SDRAM common row precharge delay:", ceil(sdram['tRP'] / tCK))
    print("Row to column delay:", ceil(sdram['tRCD'] / tCK))

    print("\nAuto Refresh Number:", ceil((sdram['tREF'] / 1e6 / sdram['lines']) * mcu_sdram_clk -20))


if __name__ == '__main__':
    main(sys.argv)

