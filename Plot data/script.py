import itertools
import subprocess
import pandas as pd
import numpy as np

# Define your possible configurations for l1_size and l1_assoc
l1_sizes = [pow(2, i) for i in range(10, 16)]  # Example values for L1 size in B
l1_assocs = [2, 4]
vc_num_blocks = 0

# l1_assocs = []
# for ele in l1_sizes:
#     l1_assocs.append(int(ele/32))     # Example values for L1 associativity

# Fixed parameters (you can change these according to your configuration)
l2_size = 262144
l1_blocksize = 32
l2_blocksize = 32
l2_assoc = 8              # L2 associativity
trace_file = 'gcc_trace.txt'  # Trace file name

# Function to run the executable and extract the miss rate
def get_area(l1_size, l1_assoc):
    # Command to execute the a.out with the given arguments
    command = f'./a.out {l1_size} {l1_assoc} {l1_blocksize} {vc_num_blocks} {l2_size} {l2_assoc} {trace_file}'

    # Run the command and capture the output
    result = subprocess.run(command.split(), capture_output=True, text=True)

    # Assuming the output contains the miss rate in a line like: "Miss Rate: <value>"
    for line in result.stdout.splitlines():
        if "total area:" in line:
            # Extract and return          
            total_area = line.split(":")[1].strip()
            print("total_area inside", total_area)
            return total_area
    return None

def get_aat(l1_size, l1_assoc):
    # Command to execute the a.out with the given arguments
    command = f'./a.out {l1_size} {l1_assoc} {l1_blocksize} {vc_num_blocks} {l2_size} {l2_assoc} {trace_file}'

    # Run the command and capture the output
    result = subprocess.run(command.split(), capture_output=True, text=True)

    # Assuming the output contains the miss rate in a line like: "Miss Rate: <value>"
    for line in result.stdout.splitlines():
        if "average access time:" in line:
            # Extract and return          
            aat = line.split(":")[1].strip()
            print("aat inside", aat)
            return aat
    return None

# List to store the results
results = {"l1_size":[], "l1_assoc":[], "aat":[], "area":[]}

# Iterate over all combinations of l1_size and l1_assoc
for l1_assoc in l1_assocs:
    # temp = {"LOG2_Blocksize":[], "miss_rate":[]}
    for l1_size in l1_sizes:
        print(f"Running with l1_size={l1_size}, l1_assoc={l1_assoc}")
        aat = get_aat(l1_size, l1_assoc)
        area  = get_area(l1_size, l1_assoc)
        # print(f"miss_rate = {miss_rate}")
        print(f"aat = {aat}")
        print(f"area = {area}")
        results["l1_size"].append(np.log2(l1_size))
        results["l1_assoc"].append(l1_assoc)
        results["area"].append(area)
        results["aat"].append(aat)
        # temp.update({"LOG2_L1_Size": np.log2(l1_size), "miss_rate": miss_rate})
    # results.append(temp)

# for i in range(0, len(l1_assocs)):
#     l1_assoc = l1_assocs[i]
#     l1_size  = l1_sizes[i]
#     print(f"Running with l1_size={l1_size}, l1_assoc={l1_assoc}")
#     miss_rate = get_aat(l1_size, l1_assoc)
#     results.append({"LOG2_L1_Size": np.log2(l1_size), "miss_rate": miss_rate})


# for i in range(0, len(results)):
#     # Convert the results into a pandas DataFrame
#     df = pd.DataFrame(results[i])

#     # Save DataFrame to CSV (if needed)
#     df.to_csv(f"plot4_miss_rates_blocksize{i}.csv", index=False)

df = pd.DataFrame(results)
df.to_csv("plot7_aats_area_last2.csv", index=False)
