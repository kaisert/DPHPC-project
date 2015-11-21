import sys

import fa
import qtree
import qtree_nfa

UNKNOWN_TOKEN = '[u]'

fname_xpath = sys.argv[1]
fname_alpha = fname_xpath[:-5] + 'tokens'
fname_multi_dfa = fname_xpath[:-5] + 'mdfa'

query_list = list(qtree.query_list(fname_xpath))
qr = qtree.generate_query_tree(query_list)
nr = qtree_nfa.qtree2nfa(qr)
alphabet = nr.alphabet

out_list = [len(query_list), len(alphabet)]

sorted_alphabet = sorted(alphabet.difference(set([UNKNOWN_TOKEN])))
#alphabet_relabeling = dict(zip(sorted_alphabet, xrange(len(sorted_alphabet))))
#alphabet_relabeling[UNKNOWN_TOKEN] = len(sorted_alphabet)
sorted_alphabet.append(UNKNOWN_TOKEN)

f_alpha = open(fname_alpha, 'w')
for letter in sorted_alphabet:
    f_alpha.write('{0}\n'.format(letter))
f_alpha.close()

for query in query_list:
    print '## Processing query', query
    query_qr = qtree.generate_query_tree([query])
    query_nr = qtree_nfa.qtree2nfa(query_qr, alphabet=alphabet)
    #print(nr)

    dfa = fa.nfa2dfa(query_nr)
    #print(dfa)

    for q in dfa.states:
        if not dfa.are_labels_unique(q):
            print 'ERROR: state {0} has non_unqiue transitions'.format(q)

    # relabel states
    sorted_states = zip(sorted([(q in dfa.accepting, q) for q in dfa.states]),
            xrange(len(dfa.states)))
    state_relabeling = dict( ((q[0][1],
        (((-2)*q[0][0])+1)*q[1]-q[0][0]) for q in sorted_states))

    sorted_states = [q[0][1] for q in sorted_states]

    out_list.append(len(dfa.states))
    for q in sorted_states:
        for t in sorted_alphabet:
            q_t = dfa.delta(q, t)
            out_list.append(state_relabeling[next(iter(q_t))])

f_multi_dfa = open(fname_multi_dfa, 'w')
for i in out_list:
    f_multi_dfa.write('{0}\n'.format(i))
f_multi_dfa.close()
