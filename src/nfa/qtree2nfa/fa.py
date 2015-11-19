import itertools


def __ensure_set(s):
    if not isinstance(s, set):
        return set([s])


def __label_union(transitions, q):
    q = __ensure_set(q)

    res = set()
    for q_u in q:
        res.update(*transitions[q_u].values())

    return res

def __eclosure(transitions, q, res=None):
    q = __ensure_set(q)
    
    if res is None:
        res = set()

    for q_u in q.difference(res):
        res.add(q_u)
        for q_v, label in transitions[q_u].items():
            if label is None:
                res.update(__eclosure(transitions, q_v, res))

    return res


def __delta(transitions, q, t):
    q = __ensure_set(q)
    t = __ensure_set(t)

    res = set()
    for q_u in q: 
        for q_v, label in transitions[q_u].items():
            if t < label:
                res.add(q_v)
    return res


def __unique_transitions(transitions, alphabet, q):
    q = __ensure_set(q)

    labels = dict()
    for q_u in q:
        for q_v, label in transitions[q_u].items():
            if q_v in labels:
                labels[q_v].union_update(label)
            else:
                labels[q_v] = set(label)

    label_intersection = set(alphabet)
    remaining_alphabet = set(alphabet)

    for n in xrange(len(labels), 0, -1):
        state_set = set()
        for comb in itertools.combinations(labels.items(), n):
            (q_list, labels) = zip(*comb)
            label_intersection = set(remaining_alphabet).intersection(*labels)
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
        self.start_state = (None, None)

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

        print "adding state {0}, len(states) = {1}".format(state_id,
                len(self.states))
        return state_id


    def add_transition(self, q_u, q_v, label):
        assert q_u in self.states
        assert q_v in self.states
        assert (isinstance(label, set) and label <= self.alphabet) or (
                label is None)
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
        return __eclosure(self.transitions[inv], q)


    def delta(self, q, t, inv=False):
        """
        if q is a set, this function calculates the 
        """
        q == __ensure_set(q)
        t == __ensure_set(t)
        assert t < self.alphabet
        assert q < self.states

        return __delta(self.transitions[inv], q, t)


    def label_union(self, q, inv=False):
        q == __ensure_set(q)
        assert q < self.states

        return __label_union(self.transitions[inv], q)


    def other_label(self, q, inv=False):
        return self.alphabet.difference(self.label_union(q))

    
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
