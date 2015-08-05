#!/usr/bin/env python
import sys
from collections import defaultdict

page1 = [
    0x0000, # triad 0
    0x0080,
    0x0100,
    0x0180,
    0x0200,
    0x0280,
    0x0300,
    0x0380,
    0x0028, # triad 1
    0x00A8,
    0x0128,
    0x01A8,
    0x0228,
    0x02A8,
    0x0328,
    0x03A8,
    0x0050, # triad 2
    0x00D0,
    0x0150,
    0x01D0,
    0x0250,
    0x02D0,
    0x0350,
    0x03D0 ]


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
    lines = process_input(sys.stdin)
elif len(sys.argv) == 2:
    lines = process_input(open(sys.argv[1], 'r'))

print '\t/*'
print '\t/* shape to create:'
for line in lines:
    print '\t * \t',line
print '\t */'

lineNum=0
plot=True
varname="page"
for row in xrange(len(lines)):
    for col in xrange(len(lines[row])):
        if lines[row][col] == '*':
            if plot:
                print '\tlo_plot(' + varname + ', ' + str(row) + ', ' + str(col) + ', 0xf);'
                continue
            if row % 2:
                print "\tbaseaddr[", hex(page1[row/2] + col),"] |= 0xF0;"
            else:
                print "\tbaseaddr[", hex(page1[row/2] + col),"] |= 0x0F;"
