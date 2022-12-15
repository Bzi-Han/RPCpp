DL = '#define _DL{}({}){}'
NL = '#define _NL{}{}'

def genDL(i):
    if 0 == i:
        return DL.format(i, '', '')

    n1 = ''
    n2 = ''
    for i in range(i):
        if '' == n1:
            n1 = 't{}'.format(i + 1)
        else:
            n1 = '{}, t{}'.format(n1, i + 1)

        if '' == n2:
            n2 = ' t{} data{:0>2d}'.format(i + 1, i + 1)
        else:
            n2 = '{}, t{} data{:0>2d}'.format(n2, i + 1, i + 1)

    result = DL.format(i + 1, n1, n2)
    return result

def genNL(i):
    if 0 == i:
        return NL.format(i, '')

    n1 = ''
    for i in range(i):
        if '' == n1:
            n1 = ' data{:0>2d}'.format(i + 1)
        else:
            n1 = '{}, data{:0>2d}'.format(n1, i + 1)

    result = NL.format(i + 1, n1)
    return result

maxParams = int(input('max params:')) + 1

DLS = ''
NLS = ''
for i in range(maxParams):
    DLS += genDL(i) + '\n'
    NLS += genNL(i) + '\n'

print(DLS)
print()
print(NLS)