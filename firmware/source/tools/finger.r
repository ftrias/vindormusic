# install.packages("R.utils")
library(R.utils)
library("foreach")

extendwild <- function(str) {
    while(TRUE) {
        i=grep("*", str, fixed=TRUE)
        if (length(i) > 0) {
            v0 = sub("*", "0", str[i], fixed=TRUE)
            v1 = sub("*", "1", str[i], fixed=TRUE)
            str = c(str[-i], v0, v1)
        }
        else {
            break
        }
    }
    str
}

finddups <- function(str) {
    s2=gsub("/[A-Z].*","",str)
    sdup = duplicated(s2)
    if (max(sdup)==1) {
        print("DUPLICATES")
        print(str[sdup])
        return(TRUE)
    }
    return(FALSE)
}

addbutton <- function(x, model) {
    i = 1
    # x[x=="*"]="0"
    for (it in x) {
        if (i >= model$nextitem) model$nextitem <- i+1
        # print(sprintf("nextitem=%d",model$nextitem))
        # print(i)
        if (it=="*") {
            model$out[i] <- "0"
            i =  i + 1
            # print("star")
        }
        else if (it=="0") {
            i = i + 1
            # print(sprintf("next %d",i))
        }
        else if (it=="1") {
            if (is.na(model$out[i])) {
                print ("NA!!!!")
                model$out[i] = "0"
            }
            if (model$out[i] == "0") {
                model$out[i] <- as.character(model$nextitem)
                i = model$nextitem
                # print(sprintf("set jump to %d", i))
            }
            else {
                i = as.integer(model$out[i])
            }
            # print("jump")
        }
        else {
            model$out[i] <- it
            # print(sprintf("terminal max=%d i=%d", model$nextitem, i))
            model$nextitem <- max(model$nextitem, i + 1)
        }
    }
    model
}

findb <- function(x, model) {
    steps = 0
    i = 1
    if (length(x)==1) {
        if (is.numeric(x)) {
            x = substring(intToBin(2^model$bits+x),2)
            # x = sapply(as.numeric(intToBits(x)),toString)
        }
        x = substring(x ,first = c(1:nchar(x)), last=c(1:nchar(x)))
    }
    print(x)
    for (it in x) {
        print(it)
        steps = steps + 1
        if (model$out[i]=="0") {
            print("irrelevant, so go to next one")
            i = i + 1
            print(i)
        }
        else if (it=="0") {
            i = i + 1
            print("jump to next")
            print(i)
        }
        else if (it=="1") {
            ix = as.integer(model$out[i])
            if (is.na(ix)) {
                print("terminal")
                break            
            }
            i=ix
            print("jump to item")
            print(i)
        }
        print(model$out[i])
        if (is.na(as.integer(model$out[i]))) break
    }
    print("the end")
    print(steps)            
    print(i)            
    print(model$out[i])   
    model$out[i]
}

buildmodel <- function(ins, simplify=TRUE) {
    # paths=extendwild(ins$pathString)
    paths=ins$pathString
    finddups(paths)
    model=list(bits=ins$bits, out=rep("0",255), nextitem=1,bout=list())
    # print(model
    # print(paths)
    if (substring(paths[1],1,2)=="b/")
        pth=sort(substring(paths,3))
    else 
        pth=sort(paths)
    pth = gsub(" ", "", pth, fixed = TRUE)
    # print(pth)
    if (simplify) {
        bcomb = strsplit(pth,"/")
        # model$bout = list()
        for (bi in 1:length(bcomb)) {
            bx = bcomb[[bi]]
            if (bi == 1) {
                by = bcomb[[bi+1]]    
                dx = min(which(bx!=by))
            }
            else if (bi == length(bcomb)) {
                by = bcomb[[bi-1]]
                dx = min(which(bx!=by))
            }
            else {
                by = bcomb[[bi+1]]    
                dx1 = min(which(bx!=by))
                by2 = bcomb[[bi-1]]    
                dx2 = min(which(bx!=by2))
                dx = max(dx1,dx2)
            }
            bz = c(bx[1:dx], tail(bx,n=1))
            model$bout[[bi]] = bz
        }
    }
    else {
        model$bout = strsplit(pth,"/")
    }
    for(bz in model$bout) {
        # print(bz)
        model = addbutton(bz, model)
        # print(model$out)
    }
    print(model$out)
    model$out=model$out[1:max(which(model$out!=0))]
    model
}

tocpp <- function(model) {
    scale=c("B0",
        "C","C#","D","D#","E","F","F#","G","G#","A","A#","B",
        "C2","C2#","D2","D2#","E2","F2","F2#","G2","G2#","A2","A2#","B2",
        "C3","C3#","D3","D3#","E3","F3","F3#","G3","G3#","A3","A3#","B3",
        "C4","C4#","D4","D4#","E4","F4","F4#","G4","G4#","A4","A4#","B4",
        "C5"
        )
    model$out[model$out=="Bb"]="A#"
    model$out[model$out=="Eb"]="F#"
    m=as.numeric(model$out)-1
    m[m==-1]=0
    m2=as.numeric(sapply(model$out, function(x) -(which(scale==x)+1)))
    m[is.na(m)]=m2[is.na(m)]
    print(is.na(m))
    print(model$out[is.na(m)])
    m[is.na(m)] = 0
    paste(m,collapse=', ')
}

tocx <- function(model) {
    scale=c("B0",
        "C","C#","D","D#","E","F","F#","G","G#","A","A#","B",
        "C2","C2#","D2","D2#","E2","F2","F2#","G2","G2#","A2","A2#","B2",
        "C3","C3#","D3","D3#","E3","F3","F3#","G3","G3#","A3","A3#","B3",
        "C4","C4#","D4","D4#","E4","F4","F4#","G4","G4#","A4","A4#","B4",
        "C5"
        )
    res=sapply(seq(0,2047), function(x) findb(x, model2))
    res[res=="Bb"]="A#"
    res[res=="Eb"]="F#"
    m2=as.numeric(sapply(res, function(x) which(scale==x)-1))
    paste(m2, collapse=",")
    m3=foreach(v=c(0:(2047/16))) %do% paste(m2[(1:16)+(16*v)],collapse=",")
    m4=c("start", m3, "end")
    cat(paste(m4, collapse=",\n"))
}

# model2=buildmodel(saxmodel)
# tocpp(model2)
# res=sapply(seq(0,2047), function(x) findb(x, model2))