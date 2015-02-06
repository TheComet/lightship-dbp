#!/usr/bin/env python
import sys

def process_file(file_name):
    with open(file_name, 'r+') as f:
        new_lines = ''.join([x.strip('\n').replace('//', '/*', 1) + ' */\n' if not x.find('//') == -1 else x for x in f])
        f.seek(0, 0)
        f.write(new_lines)

if __name__ == '__main__':
    
    # no arguments
    if len(sys.argv) == 1:
        print('usage: ' + sys.argv[0] + ' <file1> [file2] [file3]...')
        sys.exit(0)

    # process files
    for file_name in sys.argv[1:]:
        process_file(file_name)
    
    sys.exit(0)
