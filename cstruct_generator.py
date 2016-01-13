#!/usr/bin/env python3

import sys, os, ntpath, re

def translate_to_c(filename):
    with open(sys.argv[1], 'r') as src:
        data = src.read()
        while len(data) > 0:
            try:
                s = re.search("struct\s*\w*\s*\{", data)
                if s == None: break
                counter = 0
                for i in range(s.start(), len(data)):
                    last_counter = counter
                    if data[i] == "{":
                        counter += 1
                    if data[i] == "}":
                        counter -= 1
                    if last_counter > 0 and counter == 0:
                        print(data[s.start():i+1])
                        data = data[i+1:]
                        break;
            except:
                break;



if __name__ == "__main__":
    #_zz_test_translate()
    if len(sys.argv) > 1:
        translate_to_c(sys.argv[1])
    else:
        print("Please provide a filename as argument")
