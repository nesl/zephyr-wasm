import sys
import glob
import os
import subprocess
import textwrap

arg = str(sys.argv[1])
files = []

if arg == '-a':
    files += glob.glob("*.c")
elif ".c" in arg:
    files.append(arg)
else:
    print("Invalid argument!")

for file in files:
    wasm_file = file[0:-1]+"wasm"
    bash_args = ['/bin/bash', 'build.sh']
    bash_args.append(wasm_file)
    bash_args.append(file)
    subprocess.check_call(bash_args)

    print("File name: " + wasm_file)
    f = open(wasm_file,"rb")
    num_list = list(f.read())
    results = []

    for num in num_list:
        results.append(hex(num))

    aggregated_results = ', '.join(results)
    formatted_results = textwrap.wrap(aggregated_results, 80, break_long_words=False)
    for formatted in formatted_results:
        print(formatted)
    print("\n")
    f.close()
