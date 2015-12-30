import libxml2
import sys
import re
import random

#
# This script finds some unique leaf matches in file 'test.xml' and prints 
# the corresponding xpath queries to the console.
#
# optional parameters:
# 1: number of queries, defaults to 240
# 2: stop when N*numQueries queries have been seen, defaults to 1000
#
#
# Side remark: Huge XMark files have only 434 unique leaves
#

if len(sys.argv) > 1:
    numQueries = int(sys.argv[1])
else:
    numQueries = 240

if len(sys.argv) > 2:
    stopEarly = int(sys.argv[2])
else:
    stopEarly = 1000

inputFilename = 'test.xml'
rawxml = open(inputFilename).read()


xmlTags = re.findall('<([a-zA-Z/][^>]*)>',rawxml)
tagnameRegex = re.compile('[/]*([a-zA-Z]*)[ /]*')

stack = []
cur = ''
lastTagClosing = False; 
leaves = {}
realCount = 0

for t in xmlTags:
    if t == '':
        continue
    
    tagname = tagnameRegex.findall(t)[0];

    if t[0] != '/':
        stack.append(cur)
        cur = cur+'/'+tagname
        lastTagClosing = False


    if t[0] == '/' or t[-1] == '/':
        if (lastTagClosing == False):
            #leaf found
            if not cur in leaves:
                leaves[cur] = cur
                realCount = realCount + 1

                #reduce memory footprint
                if len(leaves) > 2*numQueries:
                    tmpleaves = random.sample(leaves,numQueries)
                    leaves = {}
                    for k in tmpleaves:
                        leaves[k] = k;

                    tmpleaves = None

        cur = stack[-1]
        stack = stack[0:-1]
        lastTagClosing = True


#pseudo random selection of leaf queries:
if len(leaves) > numQueries:
    leaves = random.sample(leaves,numQueries)

for leaf in leaves:
    print leaf
