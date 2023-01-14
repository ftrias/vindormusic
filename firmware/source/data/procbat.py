def toNum(s):
    n = [x[1] for x in s.split() if x[0]!='F']
    return ''.join(n)

with open("battery.txt") as f:
    for line in f:
        if 'F0' not in line: continue
        a = line.split("F0")
        b = a[1].split("11")
        if len(b) < 2: continue
        min = toNum(b[0])
        volt = toNum(b[1])
        print(f"{min}\t{volt}")