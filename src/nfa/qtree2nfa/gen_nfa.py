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


class EpsNFANode(object):
    """
    There are three types of transitions:
    1. eps-transitions ('.')
    2. asterisk-transitions ('*')
    3. concrete-transitions (element name)
    """

    def __init__(self, _id=-1, accepting=-1):
        self.state_id = accepting
        self.transitions = dict() # t -> list of transitions

    def add_transition(self, t, n):
        if t not in self.transitions:
            self.transitions[t] = set()
        self.transitions[t].add(n)
    
    def eps_closure(self, s=None):
        if s is None:
            s = set()
        if self in s:
            raise EpsCicleException("cyclic eps transitions")
        s.add(self)
        s.update(self.transitions[EPS])
        for q in self.transitions[EPS]:
            q.eps_closure(s)
        return s

    def delta(t):
        res = set()
        if EPS in self.transitions:
            res.update(self.transitions[EPS])
        if t in self.transitions:
            res.update(self.transitions[t])
        return res

    def closed_delta(t):
        start_states = self.eps_closure()
        target_states = set()
        for start_state in start_states:
            target_states.update(start_state.delta(t))
        res = set.union([q.eps_closure() for q in target_states])
        return res

    def visit_rec(self, f):
        """
        the longest possible cycle in this nfa is of length 1, that's why
        we can traverse it in this fashion
        """
        f(self)
        for tag, t_states in self.transitions.items():
            for t_state in filter(lambda c: c != self, t_states):
                t_state.visit_rec(f)

    def __str__(self):
        s = [""]
        def build_str(n):
            s[0] += "State {0}: \n".format(n.state_id)
            for t, t_states in n.transitions.items():
                s[0] += "  Transitions for {0}: {1}\n".format(
                        t, ",".join(map(lambda n: str(n.state_id), t_states)))

        self.visit_rec(build_str)
        return s[0]

def enum_states(nr, no_queries):
    state_id = [no_queries]
    def enum_states_visitor(n):
        if n.state_id == -1:
            n.state_id = state_id[0]
            state_id[0] += 1

    nr.visit_rec(enum_states_visitor)
    return state_id[0]


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
    def qtree2eps_nfa_rec(qn, nn):
        """
        @param qn: current query tree node
        @param nn: current nfa node
        """
        nn.accepting = qn.matching
        for ((typ, tag), child) in qn.children.items():
            new_node = EpsNFANode(accepting=child.matching)
            if typ == '//':
                nn.add_transition(EPS, gen_rec_node(tag, new_node))
            else:
                nn.add_transition(tag, new_node)
            qtree2eps_nfa_rec(child, new_node)

    nr = EpsNFANode()
    qtree2eps_nfa_rec(qr, nr)
    return nr


def gen_delta(qr, no_queries):
    no_accepting_states = [0]
    def count_accepting_states(n):
        if n.state_id >= 0 and n.state_id < no_queries:
            no_accepting_states[0] += 1

    tokens = sorted(list(qtree.extract_token_set(qr)))
    eps_nfa_root = qtree2eps_nfa(qr)

    eps_nfa_root.visit_rec(count_accepting_states)

    no_states = enum_states(eps_nfa_root, no_queries)

    print "no. states: ", no_states
    print "no. accepting states: ", no_accepting_states[0]
    print "no. queries: ", no_queries

    return eps_nfa_root

