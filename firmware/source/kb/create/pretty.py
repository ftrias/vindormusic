#!/usr/bin/env python
# 

import models
import parse

notes = []
columns = []
hposition = [2,2,0,0,0,0,3,4,0,4,4,4,1,0]
xposition11 = [2,4,5,8,6,7,9,10,11,12,13]
xposition12 = [3,2,4,5,8,6,7,9,10,11,12,13]
xposition13 = [0,1,2,4,5,8,6,7,9,10,11,12,13]
xposition14 = [3,0,1,2,4,5,8,6,7,9,10,11,12,13]
# lines = [0,0,1,0,0,0,0,0,0,0,0,0,0,0]
code = {"1":"X", "0":"O", "*":"-"}
colors = {"1":"black", "0":"white", "*":"grey"}

bits = 12

def loadTree(fingering):
    strposition = []
    for px in range(0,6):
        s = " " * px + "%s"
        strposition.append(s)

    for item in fingering:
        b = item.split("/")
        b.pop(0)
        notes.append(b.pop())

        res = []

        for key, state in enumerate(b):
            # print(key, state)
            p = hposition[key]
            s = strposition[p] % code[state.strip()]
            res.append(s)

        columns.append(res)

    for n in notes:
        print(n + " " * (8 - len(n)), end = "")
    print()

    for i in range(0, len(hposition)):
        for c in columns:
            print(c[i] + " " * (8 - len(c[i])), end = "")
        print()

def note2line(note):
    p = ord(note[0]) - ord("C")
    if note[0] == "A" or note[0] == "B": p+=7
    if len(note) > 1:
        if note[1] == "0": p-=7
        elif note[1] == "2": p+=7
        elif note[1] == "3": p+=14
        elif note[1] == "4": p+=21
    return p

def notesort(s):
    i = s.split('/')
    n = i[len(i)-1]
    return parse.noteNameToOrder(n)

def wholeNoteAt(x, y, acc):
    svg = '<ellipse cx="{}" cy="{}" rx="5" ry="3" fill="black" stroke="black"/>\n'.format(x, y)
    svg += '<ellipse cx="{}" cy="{}" rx="3" ry="3" fill="white" stroke="black"/>\n'.format(x, y)
    if acc == "#":
        svg += '<text x="{}" y="{}" fill="black" style="transform: scaleX(0.75); font-weight:900; font-size:12pt;">&#x266f;</text>\n'.format(x-15, y+6)
    elif acc == "b":
        svg += '<text x="{}" y="{}" fill="black" style="transform: scaleX(0.75); font-weight:900; font-size:12pt;">&#x266d;</text>\n'.format(x-6, y+4)
    return svg

def noteSVG(width, note):
    p = note2line(note)
    acc = note[len(note)-1]

    height = 140
    x = width/2

    svg = '<svg width="{}" height = "{}">\n'.format(width, height+10)
    y = height - p*5
    # svg += '<ellipse cx="{}" cy="{}" rx="5" ry="3" fill="black" stroke="black"/>\n'.format(x, y)
    # svg += '<ellipse cx="{}" cy="{}" rx="3" ry="3" fill="white" stroke="black"/>\n'.format(x, y)
    if acc == "#":
        y2 = height - (p+1)*5
        svg += wholeNoteAt(x-8, y, '#')
        svg += wholeNoteAt(x+8, y2, 'b')
        if p>12: p+=1
    elif acc == "b":
        y2 = height - (p-1)*5
        svg += wholeNoteAt(x-8, y2, '#')
        svg += wholeNoteAt(x+8, y, 'b')
        if p<0: p-=1
    else:
        acc = ''
        svg += wholeNoteAt(x, y, '')

    if acc == '':
        xwidth = 10
    else:
        xwidth = 20

    if p == 0:
        svg += '<line x1="{}", y1="{}" x2="{}" y2="{}" stroke="black"/>\n'.format(x-xwidth, y, x+xwidth, y)
    elif p >= 12:
        topline = int((p - 10)/2)
        for l in range(0, topline):
            y = height - 60 - l*10
            svg += '<line x1="{}", y1="{}" x2="{}" y2="{}" stroke="black"/>\n'.format(x-xwidth, y, x+xwidth, y)
    elif p < 0:
        bottomline = int(-p/2)
        for l in range(bottomline, 1):
            y = height + l*10
            svg += '<line x1="{}", y1="{}" x2="{}" y2="{}" stroke="black"/>\n'.format(x-xwidth, y, x+xwidth, y)

    for l in range(0, 5):
        y = height - 50 + l*10
        svg += '<line x1="0", y1="{}" x2="{}" y2="{}" stroke="black"/>\n'.format(y, width, y)
    svg += '</svg>\n'
    return svg

def splitKeys(s):
    b = s.split("/")
    b.pop(0)
    note = b.pop()
    if len(b) == 13:
        xp = xposition13
    elif len(b) == 14:
        xp = xposition14
    elif len(b) == 12:
        xp = xposition12
    elif len(b) == 11:
        xp = xposition11
    else:
        xp = xposition12

    # res = ["*"] * 14
    res = ['*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*']
    for i, v in enumerate(b):
        x = xp[i]
        res[x] = v.strip()
    return note, res

def renderSVG(f):

    columnsperline = 10
    column = 0

    fingering = sorted(f.model, key=notesort)

    html = []

    html.append("""<style type="text/css" scoped>
@media print
{
    h1 { page-break-before: always; }
    div.pad { 
        page-break-inside: avoid; 
        display: block;
    }
}
</style>""")

    html.append('<h1>{}</h1>'.format(f.desc))
    html.append('<table cellspacing="0" cellpadding="0">')    
    html.append('<tr>')

    for item in fingering:
        html.append('<td align="center">')        

        note, b = splitKeys(item)

        margin = 20

        width = margin * 2 + 4*5
        height = 14 * 20 + 10

        html.append('<svg width="{}" height="{}"><g>'.format(width, 150+25+height))

        html.append(noteSVG(width, note))
        offset = 150 # from noteSVG height; if changed there, change here

        html.append("""</g><g>
<svg y="{}" width="{}" height="25">
<text x="{}" y="15" fill="black" style="font-weight:normal; font-size:12pt;">{}</text>
</svg></g>""".format(offset, width-10, (width-12)/2, note))
        offset += 25
        # html.append(note)

        svg = '<g><svg y="{}" width="{}" height="{}">\n'.format(offset, width, height)
        cy = 10

        for key, state in enumerate(b):
            # print(key, state)
            p = hposition[key]

            if key == 2:
                cy -= 10
                svg += '<line x1="{}" y1="{}" x2="{}" y2="{}" stroke="grey" />\n'.format(margin/2, cy, width-margin/2, cy+10)
                cy += 20

            color = colors[state.strip()]

            if key == 3: # bis key
                cy -= 5
                cx = margin + 5 * p
                if color == "grey": color = "white"
                svg += '<rect x="{}" y="{}", width="10", height="4" stroke="black" fill="{}"/>\n'.format(cx-5, cy, color)
                cy += 20
            else:
                cx = margin + 5 * p
                if color == "grey":
                    svg += '<circle cx="{}" cy="{}" r="4" stroke="{}" fill="white" stroke-width="4" stroke-dasharray="2 2"/>\n'.format(cx, cy, color)
                else:
                    svg += '<circle cx="{}" cy="{}" r="5" stroke="black" fill="{}" />\n'.format(cx, cy, color)
                cy += 20

        svg += '<line x1="{}" y1="0", x2="{}", y2="{}" stroke="grey"/>\n'.format(margin/2, margin/2, height)
        svg += '<line x1="{}" y1="0", x2="{}", y2="{}" stroke="grey"/>\n'.format(width-margin/2, width-margin/2, height)

        svg += "</svg></g>\n"
        html.append(svg)
        html.append('</svg>')
        html.append('</td>')

        column += 1
        if column >= columnsperline:
            html.append('</tr><tr>')
            column = 0

    html.append('</tr>')
    html.append('</table>')
    html.append("<hr>")
    return "\n".join(html)

def displaySVG(f):
    html = renderSVG(f)
    print(html)

if __name__ == "__main__":
    displaySVG(models.saxFingerings)
    displaySVG(models.clarinet)
    displaySVG(models.recorder)
    displaySVG(models.flute)
    displaySVG(models.trumpet)
    displaySVG(models.oboe)
    displaySVG(models.tuba)
    displaySVG(models.basic)
    displaySVG(models.clarinet2)
    displaySVG(models.frenchhorn)
