import sys
import struct

import qtree
import nfa

XPATH_EXT=".xpath"
FWD_EXT=".dfwd"
BK_EXT=".dbk"
TOKENS_EXT=".tokens"

def usage():
    print "python generate_delta_table.py <xpath-file>"


def write_table(fname, t):
    f = open(fname, "wb")
    f.write(struct.pack("I"*len(t), *t))
    f.close()


def read_table(fname):
    f = open(fname, "rb")
    buf = f.read()
    delta_table = list(struct.unpack("I"*(len(buf)/4), buf))
    f.close()
    return delta_table


def write_token_file(fname, ts):
    f = open(fname, "w")
    for t in ts:
        f.write(t)
        f.write('\n')
    f.close()

def main():
    if len(sys.argv) < 0:
        usage()
        sys.exit(1)
    
    fname_xpath = sys.argv[1]
    bare_name = fname_xpath[:-len(XPATH_EXT)]
    delta_fwd_fname = bare_name + FWD_EXT;
    delta_bk_fname = bare_name + BK_EXT;
    token_fname = bare_name + TOKENS_EXT;

    print fname_xpath
    qr = qtree.generate_query_tree(fname_xpath)
    nr, delta, no_states = nfa.gen_nfa_delta(qr)
    delta_fwd_table = nfa.gen_delta_table(nr, delta, no_states, qr)
    delta_bk_table = nfa.gen_delta_table(nr, delta, no_states, qr, False)

    write_table(delta_fwd_fname, delta_fwd_table)
    write_table(delta_bk_fname, delta_bk_table)
    write_token_file(token_fname, qtree.extract_token_list(qr))

if __name__=="__main__":
    main()
