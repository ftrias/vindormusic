logo = """                                  
OOO   OOO    OOO   OOO                                                   
OOO   OOO   OOO   OOO                      OOO                            OOOOOOOOOOO       OOOOOO          OO
OOO   OOO  OOO   OOO                       OOO                            OOOOOOOOOOO     OOOOOOOOOO       OOO
OOO   OOO OOO   OOO                        OOO                            OOO            OOO      OOO     OOOO
OOO   OOOOOO   OOO                         OOO                            OOO           OOO      OOO     OOOOO
OOO   OOOO   OOO                           OOO                            OOO           OOO                OOO
 OOO  OOO   OOO  O  OOO  OOOOO      OOOOOOOOOO   OOOOOOOO   OO  OOOO      OOO            OOO               OOO
 OOO  OO   OOO  OO  OOOOOOOOOOOO   OOOOOOOOOOO  OOOOOOOOOO  OOOOOOOOOO    OOOOOOOO        OOOO             OOO
 OOO  O  OOO   OOO  OOO      OOO  OOO      OOO OOO      OOO OOO     OO    OOOOOOOO           OOOO          OOO
 OOO  O  OOO   OOO  OOO      OOO  OOO      OOO OOO      OOO OOO           OOO                   OOOO       OOO
 OOO    OOO    OOO  OOO      OOO  OOO      OOO OOO      OOO OOO           OOO                      OOO     OOO
 OOO   OOO     OOO  OOO      OOO  OOO      OOO OOO      OOO OOO           OOO                       OOO    OOO
 OOO   OOO     OOO  OOO      OOO  OOO      OOO OOO      OOO OOO           OOO            OOO        OOO    OOO
  OOO OOO      OOO  OOO      OOO  OOO      OOO OOO      OOO OOO           OOO           OOO       OOOO     OOO
  OOOOO        OOO  OOO      OOO  OOOOOOOOOOOO  OOOOOOOOOO  OOO           OOOOOOOOOOO    OOOOOOOOOOOO    OOOOOOO
  OOOO         OOO  OOO      OOO   OOOOOOOOOO    OOOOOOOO   OOO           OOOOOOOOOOO      OOOOOOOO      OOOOOOO
"""

lines = logo.split("\n")
maxlen = 128

for line in lines:
    if len(line) < maxlen:
        line += " " * (maxlen - len(line))
    # print ("A",line,"Z")
    i = 0
    out = ""
    for char in line:
        if i == 0:
            out += "B"
        elif i % 8 == 0:
            out += ",B"
        if char == " ":
            out += "0"
        else:
            out += "1"
        i += 1
    out += ","
    print(out)
print(len(lines), maxlen)