import re
p = r"\s+"
import os
base = os.path.dirname(__file__)
inv_path = os.path.join(base, 'Inventario.txt')
with open(inv_path,'r',encoding='utf-8') as f:
    txt = f.read().strip()
lines = [ln for ln in re.split(r"\r?\n", txt) if ln.strip()]
print('lines_count', len(lines))
for i,ln in enumerate(lines):
    print(i,repr(ln))
# dataLines: skip 1
data = lines[1:]
print('dataLines count', len(data))
names=[]
for ln in data:
    tokens = re.split(p, ln)
    names.append(tokens[0] if tokens else '')
print('parsed names:')
for i,n in enumerate(names,1):
    print(f"{i}. {n}")
