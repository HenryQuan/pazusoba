"""
The time taken to read in a board from command line or a board file
"""
import time
import os, subprocess

# mac or win
platform="win"
if platform == "mac":
    program = "../automation/pazusoba.out"
else:
    program = "../automation/pazusoba.exe"

# update boards here
board = "RHBDDRRGHDGBHGBGHHRLLRGBBHHRLL"

overall = time.time()
time_taken = 0
count = 20
divider = count
crash_count = 0

while count > 0:
    # run the program
    pazusoba = subprocess.Popen([program, board, '3', '50', '5000'], stdout=subprocess.DEVNULL)
    start = time.time()
    pazusoba.wait()

    output_file = "path.pazusoba"
    if os.path.exists(output_file):
        count -= 1
        curr_time = time.time() - start
        print("{}s".format(curr_time))
        time_taken += curr_time
    else:
        crash_count += 1

print("It took {}s, on average {}s. crashed {} times".format(time.time() - overall, time_taken / divider, crash_count))