import os
import sys
import time
import string
import ctypes
import datetime

DOns = "1"
SBOns = "2"
DOes = "3"
SBOes = "4"
FAILURE = 1
SUCCESS = 0
libName = (r"pClient.dll")

addcause = ctypes.create_string_buffer(1024)
result = ctypes.create_string_buffer(4096)
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

pClient.xGetDataEx.argtypes = [ctypes.c_int, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
pClient.xGetDataEx.restype = ctypes.c_int

pClient.xSelect.argtypes = [ctypes.c_int, ctypes.c_char_p, ctypes.c_char_p]
pClient.xSelect.restype = ctypes.c_int

pClient.xSelectWithValue.argtypes = [ctypes.c_int, 
                                    ctypes.c_char_p,    
                                    ctypes.c_char_p,    
                                    ctypes.c_char_p,    
                                    ctypes.c_char_p,    
                                    ctypes.c_char_p,    
                                    ctypes.c_char_p,    
                                    ctypes.c_char_p,    
                                    ctypes.c_char_p,    
                                    ctypes.c_char_p,
                                    ctypes.c_int]    
pClient.xSelectWithValue.restype = ctypes.c_int

pClient.xOperate.argtypes = [ctypes.c_int,          
                            ctypes.c_char_p,        
                            ctypes.c_char_p,        
                            ctypes.c_char_p,        
                            ctypes.c_char_p,        
                            ctypes.c_char_p,        
                            ctypes.c_char_p,        
                            ctypes.c_char_p,          
                            ctypes.c_char_p,
                            ctypes.c_int,
                            ctypes.c_int]        
pClient.xOperate.restype = ctypes.c_int

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
        Value = ""
        orCat = ""
        InterlockCheck = ""
        SynchroCheck = ""
        Test = ""
        OperTime = ""
        LDname = ""
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
            elif "Value" == newStr.partition("=")[0]:
                Value = newStr.partition("=")[2]
            elif "orCat" == argvStr.partition ("=") [0]:
                orCat = argvStr.partition ("=") [2]
            elif "InterlockCheck" == argvStr.partition ("=") [0]:
                InterlockCheck = argvStr.partition ("=") [2]
            elif "SynchroCheck" == argvStr.partition ("=") [0]:
                SynchroCheck = argvStr.partition ("=") [2]
            elif "Test" == argvStr.partition ("=") [0]:
                Test = argvStr.partition ("=") [2]
            elif "OperTime" == argvStr.partition ("=") [0]:
                OperTime = argvStr.partition ("=") [2]
                
        if not orCat: 
            orCat = "3"
        else:
            if (orCat == "bay-control"):
                orCat="1"
            elif (orCat == "station-control"):
                orCat="2"
            elif (orCat == "remote-control"):
                orCat="3"
            elif (orCat == "automatic-bay"):
                orCat="4"
            elif (orCat == "automatic-station"):
                orCat="5"
            elif (orCat == "automatic-remote"):
                orCat="6"
            elif (orCat == "maintenance"):
                orCat="7"
            elif (orCat == "process"):
                orCat="8"
            else:
                orCat = "3"
                
        if not InterlockCheck: 
            InterlockCheck = "0"
        else:
            if not (InterlockCheck == "0"):
                InterlockCheck = "1"
           
        if not SynchroCheck: 
            SynchroCheck = "0"
        else:
            if not (SynchroCheck == "0"):
                SynchroCheck = "1"
                
        if not Test: 
            Test = "0"
        else:
            if not (Test == "0"):
                Test = "1"
       
        OperTimeInSec = 0
        if not OperTime:
            OperTimeInSec = 0
        else:
            try:
                OperTimeInSec = int((datetime.datetime.strptime(OperTime, "%Y-%m-%d %H:%M:%S")).timestamp())
            except:
                outFile.write("FAILED\n")
                outFile.write("Input arguments: %s" % newLine)
                outFile.write("Reason: argument \"OperTime\" format should be \"YY:MM:DD HH:MM:SS\" \n")
                continue
               
        if not IPaddr or not IEDname or not DataName:
            outFile.write("FAILED\n")
            outFile.write("Input arguments: %s" % newLine)
            if not IPaddr:
                outFile.write("Reason: \"IPaddr\" missing in input arguments\n")	
            if not IEDname:
                outFile.write("Reason: \"IEDname\" missing in input arguments\n")	
            if not DataName:
                outFile.write("Reason: \"DataName\" missing in input arguments\n")
            if not Value:
                outFile.write("Reason: \"Value\" missing in input arguments\n")
        else:
            fullLeaf = IEDname + DataName
            LDname = DataName.partition("/")[0]
            fullLeaf = fullLeaf.replace ("\\", "/")
            domain = fullLeaf.partition("/")[0]
            leaf = fullLeaf.partition("/")[2].replace (".", "$")

            if not domain or not leaf:
                outFile.write("FAILED\n")
                outFile.write("Input arguments: %s" % newLine)
                outFile.write("Reason: argument \"DataName\" dose not have \"/\" \n")
            else:
                stNode = leaf;
                stNode = stNode.replace ("CO", "ST")
                operLeaf = leaf + "$" + "Oper"
                sboLeaf = leaf + "$" + "SBO"
                sbowLeaf = leaf + "$" + "SBOw"
                ctlModelLeaf = leaf.replace("CO", "CF")
                ctlModelLeaf = ctlModelLeaf + "$" + "ctlModel"
                ret = pClient.xAddIed (ctypes.c_int(id), bytes(IPaddr, encoding='utf8'))
                if (ret == SUCCESS):
                    ret = pClient.xConnect (ctypes.c_int(id))
                    if (ret == SUCCESS):
                        ret = pClient.xGetDataEx (ctypes.c_int(id), bytes(domain, encoding='utf8'), bytes(ctlModelLeaf, encoding='utf8'), result)
                        if (ret == SUCCESS):
                            ctlModel = int (result.value.decode('utf8')) 
                            if ctlModel < 1 or ctlModel > 4:
                                outFile.write("FAILED\n")
                                outFile.write ("Input arguments: %s \n" % sys.argv[1:]) 
                                outFile.write("Reason: ctlModel not in range 1 to 4 \n")
                            else:
                                if (result.value.decode('utf8') == DOns):
                                    ret = pClient.xOperate (ctypes.c_int(id), 
                                                            bytes(domain, encoding='utf8'), 
                                                            bytes(operLeaf, encoding='utf8'), 
                                                            bytes(Value, encoding='utf8'), 
                                                            addcause, 
                                                            bytes(orCat, encoding='utf8'),
                                                            bytes(InterlockCheck, encoding='utf8'),
                                                            bytes(SynchroCheck, encoding='utf8'),
                                                            bytes(Test, encoding='utf8'),
                                                            ctypes.c_int(OperTimeInSec),
                                                            1)
                                    if (ret == SUCCESS):
                                        outFile.write("SUCCESS\n")
                                        outFile.write("Input arguments: %s" % newLine)
                                    else:
                                        outFile.write("FAILED\n")
                                        outFile.write("Input arguments: %s" % newLine)
                                        outFile.write("Reason: operate failed \n")
                                elif (result.value.decode('utf8') == DOes):
                                    ret = pClient.xOperate (ctypes.c_int(id), 
                                                            bytes(domain, encoding='utf8'), 
                                                            bytes(operLeaf, encoding='utf8'), 
                                                            bytes(Value, encoding='utf8'), 
                                                            addcause, 
                                                            bytes(orCat, encoding='utf8'),
                                                            bytes(InterlockCheck, encoding='utf8'),
                                                            bytes(SynchroCheck, encoding='utf8'),
                                                            bytes(Test, encoding='utf8'),
                                                            ctypes.c_int(OperTimeInSec),
                                                            3)
                                    if (ret == SUCCESS):
                                        outFile.write("SUCCESS\n")
                                        outFile.write("Input arguments: %s" % newLine)
                                        if (addcause.value.decode('utf8')):
                                            outFile.write("Info: %s \n" % addcause.value.decode('utf8'))
                                    else:
                                        outFile.write("FAILED\n")
                                        outFile.write("Input arguments: %s" % newLine)
                                        outFile.write("Reason: operate failed \n")
                                        if (addcause.value.decode('utf8')):
                                            outFile.write("Info: %s \n" % addcause.value.decode('utf8'))
                                elif (result.value.decode('utf8') == SBOns):
                                    ret = pClient.xSelect  (ctypes.c_int(id), 
                                                            bytes(domain, encoding='utf8'), 
                                                            bytes(sboLeaf, encoding='utf8'))
                                    if (ret == SUCCESS):
                                        ret = pClient.xOperate (ctypes.c_int(id), 
                                                                bytes(domain, encoding='utf8'), 
                                                                bytes(operLeaf, encoding='utf8'), 
                                                                bytes(Value, encoding='utf8'), 
                                                                addcause, 
                                                                bytes(orCat, encoding='utf8'),
                                                                bytes(InterlockCheck, encoding='utf8'),
                                                                bytes(SynchroCheck, encoding='utf8'),
                                                                bytes(Test, encoding='utf8'),
                                                                ctypes.c_int(OperTimeInSec),
                                                                2)
                                        if (ret == SUCCESS):
                                            outFile.write("SUCCESS\n")
                                        else:
                                            outFile.write("FAILED\n")
                                            outFile.write("Reason: operate failed \n")
                                    else:
                                        outFile.write("FAILED\n")
                                        outFile.write("Input arguments: %s" % newLine)
                                        outFile.write("Reason: select failed \n")
                                elif (result.value.decode('utf8') == SBOes):
                                    ret = pClient.xSelectWithValue (ctypes.c_int(id), 
                                                                    bytes(domain, encoding='utf8'), 
                                                                    bytes(operLeaf, encoding='utf8'), 
                                                                    bytes(sbowLeaf, encoding='utf8'), 
                                                                    bytes(Value, encoding='utf8'), 
                                                                    addcause, 
                                                                    bytes(orCat, encoding='utf8'),
                                                                    bytes(InterlockCheck, encoding='utf8'),
                                                                    bytes(SynchroCheck, encoding='utf8'),
                                                                    bytes(Test, encoding='utf8'),
                                                                    ctypes.c_int(OperTimeInSec))
                                    if (ret == SUCCESS):
                                        ret = pClient.xOperate (ctypes.c_int(id), 
                                                                bytes(domain, encoding='utf8'), 
                                                                bytes(operLeaf, encoding='utf8'), 
                                                                bytes(Value, encoding='utf8'), 
                                                                addcause, 
                                                                bytes(orCat, encoding='utf8'),
                                                                bytes(InterlockCheck, encoding='utf8'),
                                                                bytes(SynchroCheck, encoding='utf8'),
                                                                bytes(Test, encoding='utf8'),
                                                                ctypes.c_int(OperTimeInSec),
                                                                4)
                                        if (ret == SUCCESS):
                                            outFile.write("SUCCESS\n")
                                            if (addcause.value.decode('utf8')):
                                                outFile.write("Info: %s \n" % addcause.value.decode('utf8'))
                                        else:
                                            outFile.write("FAILED\n")
                                            outFile.write("Reason: operate failed \n")
                                            if (addcause.value.decode('utf8')):
                                                outFile.write("Info: %s \n" % addcause.value.decode('utf8'))
                                    else:
                                        outFile.write("FAILED\n")
                                        outFile.write("Input arguments: %s" % newLine)
                                        outFile.write("Reason: select failed \n")
                                        if (addcause.value.decode('utf8')):
                                            outFile.write("Info: %s \n" % addcause.value.decode('utf8'))
                            outFile.write("ST Node\n")
                            ret = pClient.xGetStructData (ctypes.c_int(id), bytes(domain, encoding='utf8'), bytes(stNode, encoding='utf8'), result, size)
                            if (ret == SUCCESS):
                                for line in (result.value.decode('utf8')).splitlines():
                                    outFile.write("IPaddr=%s; IEDname=%s; DataName=%s$%s; Value=%s\n" % (IPaddr, IEDname, LDname, stNode, line))
                            else:
                                outFile.write("Unable to read \n")
                            outFile.write("CO Node\n")
                            ret = pClient.xGetStructData (ctypes.c_int(id), bytes(domain, encoding='utf8'), bytes(operLeaf, encoding='utf8'), result, size)
                            if (ret == SUCCESS):
                                for line in (result.value.decode('utf8')).splitlines():
                                    outFile.write("IPaddr=%s; IEDname=%s; DataName=%s$%s; Value=%s\n" % (IPaddr, IEDname, LDname, operLeaf, line))
                            else:
                                outFile.write("Unable to read \n")
                        else:
                            outFile.write("FAILED\n")
                            outFile.write("Input arguments: %s" % newLine)
                            outFile.write("Reason: read for ctlModel failed\n")
                            outFile.write("Info: check \"IEDname\" or \"DataName\" \n")
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
