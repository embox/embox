#!/usr/bin/env python2

import sys
import re
import md5

def parse(file):
    vmas = dict()
    lens = dict()

    for line in file.readlines():
        m = re.search(r'([0-9a-f]+)\s[A-Z]\s__module_(.*)_text_(vma|len)', line)
        if not m:
            continue

        if m.group(3) == 'vma':
            target = vmas
        elif m.group(3) == 'len':
            target = lens
        else:
            assert False

        k = m.group(2)
        assert not k in target
        target[k] = int(m.group(1), 16)

    assert len(vmas) == len(lens)
    return { k: (vmas[k], lens[k]) for k in vmas }

def main(nmfile, sectfile, vma_offset):
    stat = parse(sys.stdin)
    with open(sectfile, 'rb') as f:
        sectdata = f.read()

    for mod, (start, dlen) in stat.iteritems():
        datastart = start - vma_offset
        m = md5.new()
        m.update(sectdata[datastart : datastart + dlen])
        print 'const char __module_%s_text_md5sum[16] = "%s";' % (mod, ''.join([ '\\x{0:02x}'.format(ord(x)) for x in m.digest() ]))

if __name__ == '__main__':
    main(sys.stdin, sys.argv[1], int(sys.argv[2], 0))
