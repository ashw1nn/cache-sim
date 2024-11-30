#include <bits/stdc++.h>
#include <fstream>
#include <iostream>
#include <string>
#include "cache_model.h"


using namespace std;

#define endl '\n'

template <typename T> void print_vector(const vector<T> &v) {for (const auto &elem : v) {cout << elem << '\n';}cout << endl;}



pair<char, string> trace_decoder(const string &input_trace)
{
    string trace = input_trace;
    char mode = trace[0];
    string new_trace = trace.substr(2);

    // Padding address with 0
    while (trace.size() != 8) new_trace = '0' + new_trace;

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

    
    while (getline(traceFile, line))
    {
        pair<char, string> result  = trace_decoder(line);
        char mode = result.first;
        string key = result.second;
        // cout << "Mode outside:" << mode << endl;
        // cout << "Key outside:" << key << endl;
        if (mode == 'w') L1.write(key);
        else if (mode == 'r') L1.read(key);
        // else cout << "RIP_NOONE" << endl;
    }
    traceFile.close();
    
    // vector<string> next_trace = L1.getTraceNextLevel();
    // ofstream myfile2("nxt_level_trace.txt");
    // for(auto i : next_trace)
    // {
    //     myfile2 << i << endl;
    // }
    // myfile2.close();

    // vector<string> L1_out = L1.getTraceNextLevel();
    // cout << L1_out[0] << endl;
    // print_vector(L1.getTraceNextLevel());

    if (l2_size)
    {
        for(auto i : L1.getTraceNextLevel())
        {
            auto [mode, key] = trace_decoder(i);
            if (mode == 'w') L2.write(key);
            else if (mode == 'r') L2.read(key);
            // else cout << "RIP_NOTWO" << endl;
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
    cout << "  a. number of L1 reads:\t\t\t\t" << L1.reads_cache << endl;
    cout << "  b. number of L1 read misses:\t\t\t\t" << L1.read_cache_miss << endl;
    cout << "  c. number of L1 writes:\t\t\t\t" << L1.writes_cache << endl;
    cout << "  d. number of L1 write misses:\t\t\t\t" << L1.write_cache_miss << endl;
    cout << "  e. number of swap requests:\t\t\t\t" << L1.number_of_swap_requests << endl;
    double srr = (L1.number_of_swap_requests*1.0)/(L1.reads_cache+L1.writes_cache);
    printf("  f. swap request rate:\t\t\t\t\t%.4lf\n", srr);
    cout << "  g. number of swaps:\t\t\t\t\t" << L1.number_of_swaps << endl;
    double comined_miss_rate = ((L1.read_cache_miss+L1.write_cache_miss-L1.number_of_swaps)*1.0)/(L1.reads_cache+L1.writes_cache);
    printf("  h. combined L1+VC miss rate:\t\t\t\t%.4lf\n", comined_miss_rate);
    cout << "  i. number of writebacks from L1/VC:\t\t\t" << L1.write_back << endl;
    cout << "  j. number of L2 reads:\t\t\t\t" << L2.reads_cache << endl;
    cout << "  k. number of L2 read misses:\t\t\t\t" << L2.read_cache_miss << endl;
    cout << "  l. number of L2 writes:\t\t\t\t" << L2.writes_cache << endl;
    cout << "  m. number of L2 write misses:\t\t\t\t" << L2.write_cache_miss << endl;
    double L2_miss_rate = l2_size ? (((L2.read_cache_miss)*(1.0))/(L2.reads_cache)) : 0;
    printf("  n. L2 miss rate:\t\t\t\t\t%.4lf\n", L2_miss_rate);
    cout << "  o. number of writebacks from L2:\t\t\t" << L2.write_back << endl;
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
