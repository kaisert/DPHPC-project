import itertools


def __ensure_set(s):
    if not isinstance(s, set):
        return set([s])


def __eclosure(transitions, q):
    q = __ensure_set(q)
    
    res = set()
    res.update(q)
    for q_u in q:
        for q_v, label in transitions[q_u].items():
            if label is None:
                res.add(q_v)
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
            label_intersection =
                set(remaining_alphabet).intersection(*labels)
            q_set = set(q_list)
            remaining_alphabet.difference_update(label_intersection)

            if len(label_intersection) > 0:
                yield (q_set, label_intersection)


class FiniteAutomaton(object):
    def __init__(self, alphabet=None, default_state_id=None):
        if alphabet is None:
            alphabet = set()

        self.alphabet = set(alphabet)
        self.states = set()
        self.accepting = dict()
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

        if accepting:
            self.accepting.add(state_id)

        if start_state:
            self.start_state = state_id

        return state_id


    def add_transition(q_u, q_v, label):
        pass


    def eclosure(self, q, inv=False):
        """
        calculates the epsilon closure of q or {q}, q is a not a set.
        """
        pass


    def delta(self, q, t, inv=False):
        """
        if q is a set, this function calculates the 
        """
        pass
