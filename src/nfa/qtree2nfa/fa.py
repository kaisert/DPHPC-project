import itertools


def _ensure_set(s):
    if not isinstance(s, set):
        return set([s])
    return s


def _label_union(transitions, q):
    q = _ensure_set(q)

    res = set()
    for q_u in q:
        res.update(*transitions[q_u].values())

    return res


def _eclosure(transitions, q, done=None):
    q = _ensure_set(q)
    
    if done is None:
        done = set()

    res = set()
    res.update(q)
    for q_u in q.intersection(set(transitions.keys())):
        for q_v, label in transitions[q_u].items():
            if label is None:
                res.update(_eclosure(transitions, q_v, done.union(set([q_u]))))

    return res


def _delta(transitions, q, t):
    q = _ensure_set(q)
    t = _ensure_set(t)

    res = set()
    for q_u in q: 
        for q_v, label in transitions[q_u].items():
            if t < label:
                res.add(q_v)
    return res


def _unique_out_transitions(transitions, alphabet, q):
    q = _ensure_set(q)

    labels = dict()
    for q_u in q.intersection(set(transitions.keys())):
        for q_v, label in transitions[q_u].items():
            if label is not None:
            #if q_v not in q and label is not None:
                if q_v in labels:
                    labels[q_v].union_update(label)
                else:
                    labels[q_v] = set(label)

    remaining_alphabet = set(alphabet)

    for n in xrange(len(labels), 0, -1):
        state_set = set()
        for comb in itertools.combinations(labels.items(), n):
            (q_list, _labels) = zip(*comb)
            label_intersection = set(remaining_alphabet).intersection(*_labels)
            q_set = set(q_list)
            remaining_alphabet.difference_update(label_intersection)

            if len(label_intersection) > 0:
                yield (q_set, label_intersection)



class FiniteAutomaton(object):
    """
    General class that implements finite automaton.
    """
    def __init__(self, alphabet=None, states=None, default_state_id=None):
        if alphabet is None:
            alphabet = set()

        self.alphabet = set(alphabet)

        if states is None:
            self.states = set()
        else:
            assert isinstance(states, set)

        self.accepting = set()
        self.start_state = None

        self.transitions = {False: dict(), True: dict()}

        self.default_state_id = default_state_id
        if default_state_id is None:
            self.default_state_id = lambda fa: len(fa.states)


    def new_state(self, accepting=False, start_state=False, state_id=None):
        if state_id is None:
            state_id = self.default_state_id(self)

        if state_id in self.states:
            raise Exception("state {0} already exists".format(state_id))

        self.states.add(state_id)

        if accepting:
            self.accepting.add(state_id)

        if start_state:
            self.start_state = state_id

        return state_id


    def add_transition(self, q_u, q_v, label):
        assert q_u in self.states
        assert q_v in self.states
        assert (isinstance(label, set) and label <= self.alphabet) or (label is None)
        assert label is not None or q_u != q_v

        for inv in [False, True]:
            transitions = self.transitions[inv]
            if q_u not in transitions:
                transitions[q_u] = dict()
            transitions[q_u][q_v] = label
            q_u, q_v = q_v, q_u


    def eclosure(self, q, inv=False):
        """
        calculates the epsilon closure of q or {q}, q is a not a set.
        """
        return _eclosure(self.transitions[inv], q)


    def delta(self, q, t, inv=False):
        """
        if q is a set, this function calculates the 
        """
        q == _ensure_set(q)
        t == _ensure_set(t)
        assert t < self.alphabet
        assert q < self.states

        return _delta(self.transitions[inv], q, t)


    def eclosed_delta(self, q, t, inv=False):
        return self.eclosure(self.delta(self.eclosure(q, inv), t))


    def label_union(self, q, inv=False):
        q == _ensure_set(q)
        assert q < self.states
        return _label_union(self.transitions[inv], q)


    def other_label(self, q, inv=False):
        return self.alphabet.difference(self.label_union(q))


    def is_accepting(self, q):
        q = _ensure_set(q)
        return len(q.intersection(self.accepting)) > 0


    def unique_out_transitions(self, q, inv=False):
        return _unique_out_transitions(self.transitions[inv], self.alphabet, q)
    
    
    def __str__(self, inv=False):
        s = "Alphabet:\n"
        for t in self.alphabet:
            s += "    {0}\n".format(t)

        s += "States: \n"
        for q in self.states:
            s_acc, s_start = "", ""
            if q in self.accepting:
                s_acc = "(accepting)"
            if q == self.start_state:
                s_start = "(start)"
            s += "    {0} {1} {2}\n".format(q, s_acc, s_start)

        s += "Transitions: \n"
        transitions = self.transitions[inv]

        for q_u in transitions:
            for q_v, label in transitions[q_u].items():
                _label = label
                if label == self.alphabet:
                    _label = "[all]"
                s += "    {0} -- {1} -> {2}\n".format(q_u, _label, q_v)
        return s


def set2tuple(q):
    q = _ensure_set(q)
    return tuple(sorted(q))


def nfa2dfa(nfa):
    """
    Construct the dfa from the nfa using the power set method.

    TODO: This code is buggy
    """
    assert isinstance(nfa, FiniteAutomaton)

    dfa = FiniteAutomaton(alphabet=set(nfa.alphabet))

    q_start = set2tuple(nfa.eclosure(nfa.start_state))
    dfa.new_state(accepting=nfa.is_accepting(q_start), start_state=True,
            state_id=q_start)

    working_set = set([q_start])
    done_set = set()
    while len(working_set) > 0:
        q = next(iter(working_set))
        working_set.remove(q)
        print 'processing state', q 
        for q_set, label_intersection in nfa.unique_out_transitions(set(q)):
            print '   q_set', q_set
            q_closed = nfa.eclosure(q_set)
            print '   q_closed', q_closed
            print '   label intersection', label_intersection
            accepting = nfa.is_accepting(q_closed)

            q_new = set2tuple(q_closed)
            if q_new not in dfa.states:
                dfa.new_state(accepting, start_state=False,
                        state_id=q_new)

            dfa.add_transition(q, q_new, label_intersection)

            if q_new not in done_set:
                working_set.add(q_new)
        done_set.add(q)

    return dfa
