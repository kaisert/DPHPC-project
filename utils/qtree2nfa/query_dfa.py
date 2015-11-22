import fa

def query2dfa(query, alphabet):
    dfa = FiniteAutomaton(alphabet=alphabet, default_state_id=
            lambda fa: len(fa.states) + 1)

    dfa.new_state(accepting=False, start_state=True)

    for (typ, element) in query:
        
