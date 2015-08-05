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
    metavars={}
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
        #print line
        count="1" # deliberately a string
        default=True
        line=line.strip()
        while(len(line)):
            curr=line[0]
            line=line[1:]
            if curr == '!':
                print
                return
            elif curr.isspace():
                sys.exit(-1)
            elif curr == '$':
                sys.stdout.write(int(count) * "\n")
                count="1"
                default=True
            elif curr == 'o':
                sys.stdout.write(int(count) * "*")
                count="1"
                default=True
            elif curr == 'b':
                sys.stdout.write(int(count) * " ")
                count="1"
                default=True
            elif curr.isdigit():
                # greedily grab up numbers
                #print "curr is a number: ", curr
                if default:
                    count=curr
                    default=False
                else:
                    count=count + curr
            else:
                print "WTF is this"
                sys.exit(-1)
                

if len(sys.argv) == 1:
    process_input(sys.stdin)
elif len(sys.argv) == 2:
    process_input(open(sys.argv[1], 'r'))
