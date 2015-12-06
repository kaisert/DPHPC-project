import qtree
import fa

def find_max_query(qt):
    return max([qt.matching] + [find_max_query(child) for
        child in qt.children.values()])

def tag2alphabet(nfa, tag):
    if tag == '*':
        return set(nfa.alphabet)
    return set([tag])

def qtree2nfa(qt, alphabet=None):
    if alphabet is None:
        alphabet = qtree.extract_token_set(qt)
        alphabet.add('[u]') # the unknown token

    no_queries = find_max_query(qt) + 1
    
    nfa = fa.FiniteAutomaton(alphabet=alphabet, states=None,
            default_state_id=(lambda fa: no_queries + len(fa.states)-
                len(fa.accepting)))

    def qtree2eps_nfa_rec(qtn, q_parent=None, label=None):
        state_id, accepting = None, False
        if qtn.matching >= 0:
            state_id, accepting = qtn.matching, True

        if q_parent is not None:
            q = nfa.new_state(accepting, start_state=False, state_id=state_id)
            nfa.add_transition(q_parent, q, label)
        else:
            q = nfa.new_state(accepting, start_state=True, state_id=state_id)

        for ((typ, tag), child) in qtn.children.items():
            if typ == '//':
                q_rec = nfa.new_state()
                nfa.add_transition(q, q_rec, None)
                nfa.add_transition(q_rec, q_rec, set(nfa.alphabet))
                qtree2eps_nfa_rec(child, q_rec, tag2alphabet(nfa, tag))
            else:
                qtree2eps_nfa_rec(child, q, tag2alphabet(nfa, tag))

    qtree2eps_nfa_rec(qt)

    return nfa
