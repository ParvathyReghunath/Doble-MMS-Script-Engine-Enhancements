import os
import sys
import time
import string
import ctypes

FAILURE = 1
SUCCESS = 0
libName = (r"pClient.dll")

result = ctypes.create_string_buffer (1024)
size = 4096
OutputFile = ""
InputFile = ""
ret = FAILURE
id = 13
argvStr = ""

numArgv = len (sys.argv)
if not numArgv:
    sys.exit ()

for num in range(1, numArgv):
    argvStr = sys.argv[num].replace (";", "")
    if "InputFile" == argvStr.partition ("=") [0]:
        InputFile = argvStr.partition ("=") [2]
    elif "OutputFile" == argvStr.partition ("=") [0]:
        OutputFile = argvStr.partition ("=") [2]

if not OutputFile: 
    OutputFile = "out.txt"

if not OutputFile.endswith (".txt"):
    sys.exit ()

outFile = open (OutputFile, "w")
if not outFile:
    sys.exit ()

if not InputFile:
    outFile.write("FAILED\n")
    outFile.write ("Input arguments: %s \n" % sys.argv[1:]) 
    outFile.write("Reason: \"InputFile\" missing in input arguments\n")	
    outFile.close ()
    sys.exit ()
    
try:
    inFile = open (InputFile, "r")
    if not inFile:
        outFile.write("FAILED\n")
        outFile.write ("Input arguments: %s \n" % sys.argv[1:]) 
        outFile.write("Reason: Unable to open input file\n")	
        sys.exit ()
except:
    outFile.write("FAILED\n")
    outFile.write ("Input arguments: %s \n" % sys.argv[1:]) 
    outFile.write("Reason: Unable to open input file\n")	
    sys.exit ()

pClient = ctypes.CDLL(libName)
if not pClient:
    outFile.write("FAILED\n")
    outFile.write ("Input arguments: %s \n" % sys.argv[1:]) 
    outFile.write("Reason: unable to load DLL\n")	
    outFile.close ()
    sys.exit ()

pClient.xDllInit.argtypes = []
pClient.xDllInit.restype = ctypes.c_int

pClient.xDllPost.argtypes = []
pClient.xDllPost.restype = ctypes.c_int

pClient.xAddIed.argtypes = [ctypes.c_int, ctypes.c_char_p]
pClient.xAddIed.restype = ctypes.c_int

pClient.xRemoveIed.argtypes = [ctypes.c_int]
pClient.xRemoveIed.restype = ctypes.c_int

pClient.xConnect.argtypes = [ctypes.c_int]
pClient.xConnect.restype = ctypes.c_int

pClient.xDisconnect.argtypes = [ctypes.c_int]
pClient.xDisconnect.restype = ctypes.c_int

pClient.xGetStructData.argtypes = [ctypes.c_int, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_int]
pClient.xGetStructData.restype = ctypes.c_int

ret = pClient.xDllInit ()
if (ret == SUCCESS):
    outFile.write ("Input arguments: %s \n" % sys.argv[1:]) 
    cnt = 0
    newLine = inFile.readline()
    while newLine:
        IPaddr = ""
        IEDname = ""
        DataName = ""
        cnt = cnt + 1
        outFile.write("%d\n" % cnt)
        for word in newLine.split():
            newStr = word.replace(";", "")
            if "IPaddr" == newStr.partition("=")[0]:
                IPaddr = newStr.partition("=")[2]
            elif "IEDname" == newStr.partition("=")[0]:
                IEDname = newStr.partition("=")[2]
            elif "DataName" == newStr.partition("=")[0]:
                DataName = newStr.partition("=")[2]
                
        if not IPaddr or not IEDname or not DataName:
            outFile.write("FAILED\n")
            outFile.write("Input arguments: %s" % newLine)
            if not IPaddr:
                outFile.write("Reason: \"IPaddr\" missing in input arguments\n")	
            if not IEDname:
                outFile.write("Reason: \"IEDname\" missing in input arguments\n")	
            if not DataName:
                outFile.write("Reason: \"DataName\" missing in input arguments\n")
        else:
            fullLeaf = IEDname + DataName
            fullLeaf = fullLeaf.replace ("\\", "/")
            domain = fullLeaf.partition("/")[0]
            leaf = fullLeaf.partition("/")[2].replace (".", "$")

            if not domain or not leaf:
                outFile.write("FAILED\n")
                outFile.write("Input arguments: %s" % newLine)
                outFile.write("Reason: argument \"DataName\" dose not have \"/\" \n")
            else:
                ret = pClient.xAddIed (ctypes.c_int(id), bytes(IPaddr, encoding='utf8'))
                if (ret == SUCCESS):
                    ret = pClient.xConnect (ctypes.c_int(id))
                    if (ret == SUCCESS):
                        ret = pClient.xGetStructData (ctypes.c_int(id), bytes(domain, encoding='utf8'), bytes(leaf, encoding='utf8'), result, size)
                        if (ret == SUCCESS):
                            outFile.write("SUCCESS\n")
                            outFile.write("Input arguments: %s" % newLine)
                            for line in (result.value.decode('utf8')).splitlines():
                                outFile.write("IPaddr=%s; IEDname=%s; DataName=%s; Value=%s\n" % (IPaddr, IEDname, DataName, line))
                        else:
                            outFile.write("FAILED\n")
                            outFile.write("Input arguments: %s" % newLine)
                            outFile.write("Reason: read function failed, check \"IEDname\" and \"DataName\" \n")
                        pClient.xDisconnect (ctypes.c_int(id))
                    else:
                        outFile.write("FAILED\n")
                        outFile.write("Input arguments: %s" % newLine)
                        outFile.write("Reason: connect to IED failed\n")
                    pClient.xRemoveIed (ctypes.c_int(id))
                else:
                    outFile.write("FAILED\n")
                    outFile.write("Input arguments: %s" % newLine)
                    outFile.write("Reason: unable to allocate for IED\n")
        newLine = inFile.readline()
    pClient.xDllPost ()
else:
    outFile.write("FAILED\n")
    outFile.write ("Input arguments: %s \n" % sys.argv[1:]) 
    outFile.write("Reason: unable to initialize DLL\n")

del pClient
outFile.close()	