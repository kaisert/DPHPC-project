#!/usr/bin/env python

import matplotlib.pyplot as plt
import numpy as np

xvals = (1, 2, 4, 8, 16, 32)
yvals1 = (1.918499, 1.580462, 1.813013, 1.808579, 1.729736, 1.729384,)
yvals2 =  (0.618573, 0.545513, 0.574069, 0.459962, 0.567306, 0.519556,)

yprops = dict(rotation=0, y=1.05, horizontalalignment='left')

plt.subplot(111,axisbg='#BBBBBB',alpha=0.1)
plt.grid(color='white', alpha=0.5, linewidth=2, linestyle='-', axis='y')

for spine_name in ['top', 'left', 'right']:
    plt.gca().spines[spine_name].set_color('none')
    
plt.ylabel('Performance [flops/cycle]', **yprops)
plt.xlabel('N [doubles]')

plt.gca().tick_params(direction='out', length=0, color='k')

plt.plot(xvals, yvals1, 'bo-', linewidth=2)
plt.plot(xvals, yvals2, 'go-', linewidth=2)
plt.gca().set_axisbelow(True)

plt.savefig('test.png', dpi=300)

plt.show()

