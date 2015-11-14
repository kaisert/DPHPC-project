import qtree
from operator import attrgetter

EPS='.'
ASK='*'

class EpsCicleException(Exception):
    pass

class CicleException(Exception):
    pass

def eps_closure(_q):
    if isinstance(_q, set):
        return set.union(*[eps_closure(q) for q in _q])
    else:
        return q.eps_closure()


def asterisk_label():
    def _asterisk_label(t):
        return True
    _asterisk_label.label = '*'
    return _asterisk_label


def token_label(_t):
    def _token_label(t):
        return t == _t
    _token_label.label = _t
    return _token_label


def other_label(labels):
    _labels = filter(lambda l: l.label != '[o]', labels)
    def _other_label(t):
        return not any(l(t) for l in _labels)
    _other_label.label = '[o]'
    return _other_label


class EpsNFANode(object):
    def __init__(self, _id=-1, accepting=-1):
        self.state_id = accepting
        self.transitions = {True: set(), False: set()}
        self.eps_transitions = {True: set(), False: set()}
        #self.transitions = dict() # t -> list of transitions

    def add_transition(self, t, n, direction=True):
        """
        There are three types of transitions:
        1. eps-transitions ('.')
        2. asterisk-transitions ('*')
        3. concrete-transitions (element name)
        4. other-transitions ('[o]', match all tokens which are not in the
        current label set)
        """
        eps_transitions = self.eps_transitions[direction]
        transitions = self.transitions[direction]

        if t == '.':
            eps_transitions.add(n)
        elif t == '*':
            transitions.add( (asterisk_label(), n) )
        elif t == '[o]':
            transitions.add( (other_label(self.label_set(direction)), n ) )
        else:
            transitions.add( (token_label(t), n) )
        if direction:
            n.add_transition(t, self, not direction)

    def label_set(self, direction=True):
        return set([t[0] for t in self.transitions[direction]])
    
    def eps_closure(self, s=None, direction=True):
        if s is None:
            s = set()
        if self in s:
            raise EpsCicleException("cyclic eps transitions")
        s.add(self)
        for q in self.eps_transitions[direction]:
            q.eps_closure(s, direction)
        return s

    def delta(self, t, direction=True):
        res = set()
        for tr in self.transitions[direction]:
            if tr[0](t):
                res.add(tr[1])
        return res

    def closed_delta(self, t, direction=True):
        start_states = self.eps_closure(direction=direction)
        target_states = set()
        for start_state in start_states:
            target_states.update(start_state.delta(t, direction))
        res = set.union(*([q.eps_closure(direction=direction)
            for q in target_states]+ [set()]))
        return res

    def visit_rec(self, f, s=None, direction=True):
        """
        the longest possible cycle in this nfa is of length 1, that's why
        we can traverse it in this fashion
        """
        if s is None:
            s = set()
        if self not in s:
            s.add(self)
            f(self)
            for (tr, t_state) in self.transitions[direction]:
                if t_state != self:
                    t_state.visit_rec(f, s)
            for q in self.eps_transitions[direction]:
                q.visit_rec(f, s)

    def __str__(self):
        s = [""]
        def build_str(n):
            s[0] += "State {0}: \n".format(n.state_id)
            for (l, q) in n.transitions[True]:
                s[0] += "  Transitions for {0}: {1}\n".format(l.label,
                        q.state_id)
            for q in n.eps_transitions[True]:
                s[0] += "  Eps-Transition: {0}\n".format(q.state_id)

        self.visit_rec(build_str)
        return s[0]

def enum_states(nr, no_queries):
    state_id = [no_queries]
    state_mapping = dict()
    def enum_states_visitor(n):
        if n.state_id == -1:
            n.state_id = state_id[0]
            state_id[0] += 1
        state_mapping[n.state_id] = n

    nr.visit_rec(enum_states_visitor)
    return state_id[0], state_mapping


def gen_rec_node(t, target_node=None):
    """
       -- * --
       |     |
    --( )<---|
       |
       | t
       |
       \/
       ( )
    """
    rec_node = EpsNFANode()
    rec_node.add_transition('*', rec_node)
    if target_node is None:
        target_node = EpsNFANode()
    rec_node.add_transition(t, target_node)
    return rec_node


def qtree2eps_nfa(qr):
    """
    @param qr: root of the query tree
    """
    max_query = [-1]
    def qtree2eps_nfa_rec(qn, nn):
        """
        @param qn: current query tree node
        @param nn: current nfa node
        """
        nn.accepting = qn.matching
        for ((typ, tag), child) in qn.children.items():
            if child.matching > max_query[0]:
                max_query[0] = child.matching
            new_node = EpsNFANode(accepting=child.matching)
            if typ == '//':
                nn.add_transition(EPS, gen_rec_node(tag, new_node))
            else:
                nn.add_transition(tag, new_node)
            qtree2eps_nfa_rec(child, new_node)

    nr = EpsNFANode()
    qtree2eps_nfa_rec(qr, nr)

    no_queries = max_query[0]+1

    no_states, state_mapping = enum_states(nr, no_queries)

    q_N = EpsNFANode()
    q_N.state_id = no_states

    for q in state_mapping.values():
        q.add_transition('[o]', q_N)

    q_N.add_transition('[o]', q_N)
    state_mapping[q_N.state_id] = q_N

    return nr, no_states, no_queries, state_mapping


def gen_delta_push(qr):
    """
    This function returns a function delta: int x string -> set of ints
    
    """
    no_accepting_states = [0]

    no_queries = 0
    def count_accepting_states(n):
        if n.state_id >= 0 and n.state_id < no_queries:
            print n.state_id
            no_accepting_states[0] += 1

    eps_nfa_root, no_states, no_queries, state_mapping = qtree2eps_nfa(qr)
    eps_nfa_root.visit_rec(count_accepting_states) # sanity check

    def delta_push(q, t):
        return set([_q.state_id for _q in state_mapping[q].closed_delta(t)])

    print "no. states: ", no_states
    print "no. accepting states: ", no_accepting_states[0]
    print "no. queries: ", no_queries

    return eps_nfa_root, delta_push, no_states


def print_delta_push(delta_push, no_states, qr):
    tokens = sorted(list(qtree.extract_token_set(qr)))
    for q in xrange(0, no_states+1):
        for t in tokens:
            print "({0}, {1}) -> {2}".format(q, t, str(delta_push(q, t)))
