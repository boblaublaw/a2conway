#!/usr/bin/env python
import sys
from collections import defaultdict

page1 = [
    0x0400, # triad 0
    0x0480,
    0x0500,
    0x0580,
    0x0600,
    0x0680,
    0x0700,
    0x0780,
    0x0428, # triad 1
    0x04A8,
    0x0528,
    0x05A8,
    0x0628,
    0x06A8,
    0x0728,
    0x07A8,
    0x0450, # triad 2
    0x04D0,
    0x0550,
    0x05D0,
    0x0650,
    0x06D0,
    0x0750,
    0x07D0 ]


def process_input(handle):
    metavars = {}
    lines = []
    for line in handle:
        if line[0] == '#':
            continue
        line=line.strip()
        if line[0] == 'x':
            parts = line.split(',')
            for part in parts:
                key , val = part.split('=')
                metavars[key.strip()] = val.strip()
            continue       
        count = "1" # deliberately a string
        default = True
        line = line.strip()
        totalLines = int(metavars['y'])
        if len(lines) == 0:
            for x in xrange(totalLines):
                lines.append("")
            currline = 0
        while(len(line)):
            currchar = line[0]
            line = line[1:]
            if currchar == '!':
                return lines
            elif currchar == '$':
                n = int(count)
                currline = currline + n
                count = "1"
                default = True
            elif currchar == 'o':
                n = int(count)
                buf = "*" * n
                lines[currline] = lines[currline] + buf
                count = "1"
                default = True
            elif currchar == 'b':
                n = int(count)
                buf = " " * n
                lines[currline] = lines[currline] + buf
                count = "1"
                default = True
            elif currchar.isdigit():
                if default:
                    count = currchar
                    default = False
                else:
                    count = count + currchar
            else:
                print "WTF is this"
                sys.exit(-1)
    return lines
                

if len(sys.argv) == 1:
    l = process_input(sys.stdin)
elif len(sys.argv) == 2:
    l = process_input(open(sys.argv[1], 'r'))
for line in l:
    print line
