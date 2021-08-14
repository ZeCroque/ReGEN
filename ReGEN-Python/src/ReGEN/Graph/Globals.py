import sys

def initGlobals():
    global maxRamUsage
    maxRamUsage = 0

    global minRamUsage
    minRamUsage = sys.maxint

    global sumRamUsage
    sumRamUsage = 0

    global maxVirtualRamUsage
    maxVirtualRamUsage = 0

    global minVirtualRamUsage
    minVirtualRamUsage = sys.maxint

    global sumVirtualRamUsage
    sumVirtualRamUsage = 0

    global maxCPU
    maxCPU = 0

    global minCPU
    minCPU = sys.maxint

    global sumCPU
    sumCPU = 0

    global measureCount 
    measureCount = 0