import os
import sys
import time
import string
import ctypes
import multiprocessing
 
FAILURE = 1
SUCCESS = 0
libName = (r"D:\Doble\Doble.pClient.Script\Python\batchRead\pClient.dll")
 
result = ctypes.create_string_buffer (1024)
size = 4096
OutputFile = ""
InputFile = ""
ret = FAILURE
id = 13
argvStr = ""
 

 
def worker(libName,newLine,logQueue,fileLock):
    processLog=[]
 
    pClient = ctypes.CDLL(libName)
    if not pClient:
        processLog.append("FAILED\n")
        processLog.append ("Input arguments: %s \n" % sys.argv[1:])
        processLog.append("Reason: unable to load DLL\n")  
        sys.exit ("1")
 
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
        IPaddr = ""
        IEDname = ""
        DataName = ""
        
        for word in newLine.split():
            newStr = word.replace(";", "")
            if "IPaddr" == newStr.partition("=")[0]:
                IPaddr = newStr.partition("=")[2]
            elif "IEDname" == newStr.partition("=")[0]:
                IEDname = newStr.partition("=")[2]
            elif "DataName" == newStr.partition("=")[0]:
                DataName = newStr.partition("=")[2]
                
        if not IPaddr or not IEDname or not DataName:
            processLog.append("FAILED\n")
            processLog.append("Input arguments: %s" % newLine)
            if not IPaddr:
                processLog.append("Reason: \"IPaddr\" missing in input arguments\n")    
            if not IEDname:
                processLog.append("Reason: \"IEDname\" missing in input arguments\n")  
            if not DataName:
                processLog.append("Reason: \"DataName\" missing in input arguments\n")
        else:
            fullLeaf = IEDname + DataName
            fullLeaf = fullLeaf.replace ("\\", "/")
            domain = fullLeaf.partition("/")[0]
            leaf = fullLeaf.partition("/")[2].replace (".", "$")

            if not domain or not leaf:
                processLog.append("FAILED\n")
                processLog.append("Input arguments: %s" % newLine)
                processLog.append("Reason: argument \"DataName\" dose not have \"/\" \n")
            else:
                ret = pClient.xAddIed (ctypes.c_int(id), bytes(IPaddr, encoding='utf8'))
                if (ret == SUCCESS):
                    print("[Log]Connecting to IP {}\n".format(IPaddr))
                    ret = pClient.xConnect (ctypes.c_int(id))
                    if (ret == SUCCESS):
                        print("[Log]Reading Value for {}".format(IPaddr))

                        ret = pClient.xGetStructData (ctypes.c_int(id), bytes(domain, encoding='utf8'), bytes(leaf, encoding='utf8'), result, size)
                        print("[Log]Issuing Read Command for {}".format(IPaddr))
                        if (ret == SUCCESS):
                            processLog.append("SUCCESS\n")
                            processLog.append("Input arguments: %s" % newLine)
                            for line in (result.value.decode('utf8')).splitlines():
                                processLog.append("IPaddr=%s; IEDname=%s; DataName=%s; Value=%s\n" % (IPaddr, IEDname, DataName, line))
                        else:
                            processLog.append("FAILED\n")
                            processLog.append("Input arguments: %s" % newLine)
                            processLog.append("Reason: read function failed, check \"IEDname\" and \"DataName\" \n")
                        pClient.xDisconnect (ctypes.c_int(id))
                    else:
                        processLog.append("FAILED\n")
                        processLog.append("Input arguments: %s" % newLine)
                        processLog.append("Reason: connect to IED failed\n")
                    pClient.xRemoveIed (ctypes.c_int(id))
                else:
                    processLog.append("FAILED\n")
                    processLog.append("Input arguments: %s" % newLine)
                    processLog.append("Reason: unable to allocate for IED\n")
           
        pClient.xDllPost ()
    else:
        processLog.append("FAILED\n")
        processLog.append ("Input arguments: %s \n" % sys.argv[1:])
        processLog.append("Reason: unable to initialize DLL\n")

    with fileLock:
        logQueue.put(processLog)

    del pClient


 
if __name__=='__main__':
    flag=0
    manager=multiprocessing.Manager()
    logQueue = multiprocessing.Queue()
    fileLock=manager.Lock()

    numArgv = len (sys.argv)
    if not numArgv:
        sys.exit ("1")
    
    for num in range(1, numArgv):
        argvStr = sys.argv[num].replace (";", "")
        if "InputFile" == argvStr.partition ("=") [0]:
            InputFile = argvStr.partition ("=") [2]
        elif "OutputFile" == argvStr.partition ("=") [0]:
            OutputFile = argvStr.partition ("=") [2]
    
    if not OutputFile:
        OutputFile = "out.txt"
    
  
    
    outFile = open (OutputFile, "w")
    if not outFile:
        sys.exit ("1")
    
    if not InputFile:
        print("FAILED\n")
        print("Input arguments: %s \n" % sys.argv[1:])
        print("Reason: \"InputFile\" missing in input arguments\n")
        sys.exit ("1")
    
    try:
        inFile = open (InputFile, "r")
        if not inFile:
            print("FAILED\n")
            print("Input arguments: %s \n" % sys.argv[1:])
            print("Reason: Unable to open input file\n")    
            sys.exit ("1")
    except:
        print("FAILED\n")
        print ("Input arguments: %s \n" % sys.argv[1:])
        print("Reason: Unable to open input file\n")    
        sys.exit ("1")

    processes=[]

    newLine=inFile.readline()
    while(newLine):
        process=multiprocessing.Process(target=worker,args=(libName,newLine, logQueue, fileLock))
        processes.append(process)
        process.start()
        newLine=inFile.readline()

    for process in processes:
        process.join()
    
   
    cnt=0

    with open(OutputFile,'w') as file:
        while not logQueue.empty():
            logMessage=logQueue.get()
            cnt+=1
            file.write(f'{cnt}\n')
            for item in logMessage:
                lines = item.split('\n')
                for line in lines:
                    if line:  
                        if line=="FAILED":
                            flag=1                     
                        file.write(f'{line}\n')
    if flag==1:
        sys.exit("1")
    else:
        sys.exit("0")