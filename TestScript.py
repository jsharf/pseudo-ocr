import os
import sys
import subprocess

tests = os.listdir("tests")

testnum = 1

while (True):
    try:
        if (str(testnum)+".txt" in tests):
            testfile = "./tests/"+str(testnum)+".txt"
            ansfile = "./tests/"+str(testnum)+".ans"
            test = open(testfile)
            ans = open(ansfile)
            
            testproc = subprocess.Popen("./readpass", stdin=subprocess.PIPE,
                    stdout=subprocess.PIPE)
            testproc.stdin.write(test.read())
            result = testproc.stdout.read()
            answer = ans.read()
            if (result == answer):
                print("Test " + str(testnum) + " Success")
                print("Answer: " + str(result))
            else:
                print("Test " + str(testnum) + " Failure")
                print("Program Output: " + str(result))
                print("Expected Output: " + str(answer))
            testnum += 1
        else:
            break
    except OSError as ose:
        print "OS Error: " + ose.strerror
        raise
    except:
        print "Unexpected error:", sys.exc_info()[0]
        raise

