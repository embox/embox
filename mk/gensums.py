#!/usr/bin/env python

import sys
import re

try:
    # Python2
    import md5
    md5factory = md5.new
    md5dig = lambda m: [ ord(x) for x in m.digest() ]
except ImportError:
    # Python3
    import hashlib
    md5factory = hashlib.md5
    md5dig = lambda m: list(m.digest())

def parse(file, sectname):
    vmas = dict()
    lens = dict()
    target_d = { 'vma' : vmas, 'len' : lens }

    for line in file.readlines():
        m = re.search(r'([0-9a-f]+)\s[A-Z]\s__module_(.*)_%s_(vma|len)' % (sectname,), line)
        if not m:
            continue

        target = target_d[m.group(3)]

        k = m.group(2)
        assert not k in target
        target[k] = int(m.group(1), 16)

    assert len(vmas) == len(lens)
    return dict( (k, (vmas[k], lens[k])) for k in vmas )

def main(nmfile, sectname, sectfile, vma_offset):
    stat = parse(sys.stdin, sectname)
    with open(sectfile, 'rb') as f:
        sectdata = f.read()

    for mod, (start, dlen) in stat.items():
        datastart = start - vma_offset
        m = md5factory()
        m.update(bytes(sectdata[datastart : datastart + dlen]))
        print('const char __module_%s_%s_md5sum[16] = "%s";' %
                (mod, sectname, ''.join([ '\\x{0:02x}'.format(x) for x in md5dig(m) ])))

if __name__ == '__main__':
    main(sys.stdin, sys.argv[1], sys.argv[2], int(sys.argv[3], 0))
