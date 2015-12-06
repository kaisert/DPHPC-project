import re
from operator import itemgetter

path_element_rx = re.compile('^(//?)([a-zA-Z][_\-A-Za-z0-9]*|\*)(/?.*)$')


def split_query(line):
    remainder = line
    m = path_element_rx.match(remainder)
    while m is not None:
        (typ, element, remainder) = m.groups()
        m = path_element_rx.match(remainder)
        yield (typ, element)


def print_qtree_rec(node, depth=0):
    s = ""
    for transition, child in node.children.items():
        s = s + depth*"    " + str(transition) + ' ' + str(child.matching) + '\n'
        s = s + print_qtree_rec(child, depth+1)
    return s


class QTreeNode(object):
    def __init__(self, matching=-1):
        self.matching = matching
        self.children = dict()

    def add_transition(self, transistion, node):
        assert isinstance(node, QTreeNode)
        self.children[transition] = node

    def rec_add_query(self, query, q_id):
        try:
            elem = query.pop(0)
            if elem not in self.children:
                self.children[elem] = QTreeNode()
            self.children[elem].rec_add_query(query, q_id)
        except IndexError:
            self.matching = q_id

    def __str__(self, depth=0):
        s = ""
        depth = depth
        for transition, child in self.children.items():
            s = "{0}{1}{2} {3}\n".format(s, depth*"    ", str(transition),
                    str(child.matching))
            s = s + child.__str__(depth=depth+1)
        return s


def extract_token_set(qr, exclude_ast=True):
    """
    @param qr: root node of the query tree
    """
    def extract_token_set_rec(qn, s):
        """
        """
        s.update(map(itemgetter(1), qn.children.keys()))
        for child in qn.children.values():
            extract_token_set_rec(child, s)
        return s
    s = extract_token_set_rec(qr, set())
    if exclude_ast:
        if '*' in s:
            s.remove('*')
    return s


def extract_token_list(qr, exclude_ast=True):
    return sorted(extract_token_set(qr))


def query_list(fname):
    f = open(fname, 'r')
    for line in f:
        query = list(split_query(line.rstrip()))
        yield query
    f.close()


def generate_query_tree(query_list):
    r = QTreeNode()
    no_queries = 0
    for query in query_list:
        if len(query) > 0:
            r.rec_add_query(list(query), no_queries)
            no_queries += 1
    return r
