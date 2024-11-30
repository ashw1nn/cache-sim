// #include <bits/stdc++.h>
#include <fstream>
#include <iostream>
#include <string>
#include "new_cache_model.h"


using namespace std;

// #define endl '\n'

template <typename T> void print_vector(const vector<T> &v) {for (const auto &elem : v) {cout << elem << '\n';}cout << endl;}



pair<char, string> trace_decoder(const string &input_trace)
{
    string trace = input_trace;
    char mode = trace[0];
    string new_trace = trace.substr(2);

    // Padding address with 0
    while (new_trace.size() != 8) new_trace = '0' + new_trace;

    return make_pair(mode, new_trace);
}


int main(int argc, char** argv)
{
    int l1_size = 0, l1_assoc = 0, l1_blocksize = 0, vc_num_blocks = 0;
    int l2_size = 0, l2_assoc = 0;
    string trace_file_txt = "";

    if (argc != 8)
    {
        cout << "Incorrect number of arguments" << endl;
        return 0;
    }

    // Taking inputs from the command line
    l1_size = stoi(argv[1]);
    l1_assoc = stoi(argv[2]);
    l1_blocksize = stoi(argv[3]);
    vc_num_blocks = stoi(argv[4]);
    l2_size = stoi(argv[5]);
    l2_assoc = stoi(argv[6]);
    trace_file_txt = argv[7];

    // Create the caches based on the given parameters
    Cache L1 = Cache(l1_size, l1_blocksize, l1_assoc, vc_num_blocks, "L1");
    Cache L2 = Cache(l2_size, l1_blocksize, l2_assoc, 0, "L2");

    // cout << "SIBI_LEZZ_GOOWWW" << endl;

    // Take in inputs from the .txt file
    ifstream traceFile(trace_file_txt);
    string line;

    int nos = 1;

    while (getline(traceFile, line))
    {
        // cout << '[' << nos++ << ']' << "Processing: " << line << endl;
        auto [mode, trace] = trace_decoder(line);
        if (mode == 'w') L1.write(trace);
        else if (mode == 'r') L1.read(trace);
        else cout << "Invalid Trace Mode" << endl;
    }
    traceFile.close();
    
    // L1.printTraces();

    if (l2_size)
    {
        for(auto i : L1.getTraceNextLevel())
        {
            auto [mode, trace] = trace_decoder(i);
            if (mode == 'w') L2.write(trace);
            else if (mode == 'r') L2.read(trace);
            else cout << "Invalid Trace Mode" << endl;
        }
    }

    
    cout << "===== Simulator configuration =====" << endl;
    cout << "  l1_size: \t\t" << L1.SIZE << endl;
    cout << "  l1_assoc: \t\t" << L1.ASSOC << endl;
    cout << "  l1_blocksize: \t" << L1.BLOCKSIZE << endl;
    cout << "  vc_num_blocks: \t" << L1.VC_NUM_BLOCKS << endl;
    cout << "  l2_size: \t\t" << L2.SIZE << endl;
    cout << "  l2_assoc: \t\t" << L2.ASSOC << endl;
    cout << "  trace_file: \t\t" << trace_file_txt << endl;
    cout << endl;
    L1.print_cache_state();
    cout << endl;
    if(l2_size)
    {
        L2.print_cache_state();
        cout << endl;
    }
    cout << "===== Simulation results (raw) =====" << endl;
    cout << "  a. number of L1 reads:\t\t\t\t" << L1.reads << endl;
    cout << "  b. number of L1 read misses:\t\t\t\t" << L1.readMisses << endl;
    cout << "  c. number of L1 writes:\t\t\t\t" << L1.writes << endl;
    cout << "  d. number of L1 write misses:\t\t\t\t" << L1.writeMisses << endl;
    cout << "  e. number of swap requests:\t\t\t\t" << L1.swapRequests << endl;
    double srr = (L1.swapRequests*1.0)/(L1.reads+L1.writes);
    printf("  f. swap request rate:\t\t\t\t\t%.4lf\n", srr);
    cout << "  g. number of swaps:\t\t\t\t\t" << L1.swaps << endl;
    double comined_miss_rate = ((L1.readMisses+L1.writeMisses-L1.swaps)*1.0)/(L1.reads+L1.writes);
    printf("  h. combined L1+VC miss rate:\t\t\t\t%.4lf\n", comined_miss_rate);
    cout << "  i. number writebacks from L1/VC:\t\t\t" << L1.writeBacks << endl;
    cout << "  j. number of L2 reads:\t\t\t\t" << L2.reads << endl;
    cout << "  k. number of L2 read misses:\t\t\t\t" << L2.readMisses << endl;
    cout << "  l. number of L2 writes:\t\t\t\t" << L2.writes << endl;
    cout << "  m. number of L2 write misses:\t\t\t\t" << L2.writeMisses << endl;
    double L2_miss_rate = l2_size ? (((L2.readMisses)*(1.0))/(L2.reads)) : 0;
    printf("  n. L2 miss rate:\t\t\t\t\t%.4lf\n", L2_miss_rate);
    cout << "  o. number of writebacks from L2:\t\t\t" << L2.writeBacks << endl;
    if(!l2_size)
    {
        cout << "  p. total memory traffic:\t\t\t\t" << L1.getTraceNextLevel().size() << endl;

    }
    else
        cout << "  p. total memory traffic:\t\t\t\t" << L2.getTraceNextLevel().size() << endl;
    cout << endl;
    cout << "===== Simulation results (performance) =====" << endl;
    cout << "  1. average access time:\t\t\t\t" << endl;
    cout << "  2. energy-delay product:\t\t\t\t" << endl;
    cout << "  3. total area:\t\t\t\t\t" << endl;

    return 0;
}
