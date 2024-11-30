#include <bits/stdc++.h>
#include <fstream>
#include <iostream>
#include <string>
#include "parse.h"

using namespace std;

#define endl '\n'

template <typename T> void print_vector(const vector<T> &v) {for (const auto &elem : v) {cout << elem << '\n';}cout << endl;}


string hexToBinary(string hex)
{
    string binary = "";
    unordered_map<char, string> hexToBinaryDict = {
        {'0', "0000"}, {'1', "0001"}, {'2', "0010"}, {'3', "0011"},
        {'4', "0100"}, {'5', "0101"}, {'6', "0110"}, {'7', "0111"},
        {'8', "1000"}, {'9', "1001"}, {'a', "1010"}, {'b', "1011"},
        {'c', "1100"}, {'d', "1101"}, {'e', "1110"}, {'f', "1111"}
    };
    for (auto it: hex) binary += hexToBinaryDict[it];

    return binary;
}

string binaryToHex(string binary)
{
    string hex = "";
    unordered_map<string, char> binaryToHexDict = {
        {"0000", '0'}, {"0001", '1'}, {"0010", '2'}, {"0011", '3'},
        {"0100", '4'}, {"0101", '5'}, {"0110", '6'}, {"0111", '7'},
        {"1000", '8'}, {"1001", '9'}, {"1010", 'a'}, {"1011", 'b'},
        {"1100", 'c'}, {"1101", 'd'}, {"1110", 'e'}, {"1111", 'f'}
    };

    for (int i = 0; i < binary.size(); i += 4) hex += binaryToHexDict[binary.substr(i, 4)];
    return hex;
}

pair<char, string> trace_decoder(const string &input_trace)
{
    string trace = input_trace;
    char mode = trace[0];
    string new_trace = trace.substr(2);

    // Padding address with 0
    if(new_trace.size() < 8)
    {
        int def = 8 - new_trace.size();
        string prefix = "";
        for (int i = 0; i < def; i++) new_trace = '0' + new_trace;
    }
    return make_pair(mode, new_trace);
}

class VictimCache 
{
private:
    int VC_NUM_BLOCKS = 0;
    int VC_BLOCKSIZE = 0;
    string LABEL = "";
    list<pair<string, int>> vcachemap_; // [tag, tag, tag, tag, ...] 

    string convertPaddedTrace(const string &padded_trace)
    {
        string padded_trace_in_bits = hexToBinary(padded_trace);
        string trace = padded_trace_in_bits.substr(padded_trace_in_bits.size() - 32);
        string converted_trace = binaryToHex(trace);
        return converted_trace;
    }

    string getTag(const string &key)
    {
        // cout << "Key inside get Tag: " << key << endl;
        // Get the tag
        int tagLenBits = 32 - (int)log2(VC_BLOCKSIZE);
        string keyInBin = hexToBinary(key);           
        string tagInBin = keyInBin.substr(0, tagLenBits);
        while(tagInBin.size()%4 != 0) tagInBin = '0' + tagInBin;
        string tag = binaryToHex(tagInBin);
        
        return tag;
    }

    list<pair<string, int>>::iterator findTagInSet(const string& tag) 
    {
        // Iterate through the list of pairs in the specific set
        for (auto iter = vcachemap_.begin(); iter != vcachemap_.end(); ++iter)
        {
            if (iter->first == tag) return iter;
        }
        return vcachemap_.end();
    }  

    void update_next_level_trace(const char mode, const string key, vector<string> &traceNextLevel)
    {
        string trace = (mode + (" " + key));
        traceNextLevel.push_back(trace);
        return;
    }


    void allocate_block_in_vc(const string &tag, const int dirty_bit, vector<string> &traceNextLevel, int &write_back)
    {
        if (vcachemap_.size() != VC_NUM_BLOCKS) // Not FULL
        {
            vcachemap_.push_front({tag, dirty_bit});
            return;
        }
        else // FULL --- LRU policy
        {
            if (vcachemap_.back().second == 1) 
            {
                write_back++;
                string tagInVC = vcachemap_.back().first;
                // cerr << "Tag in VC: " << tagInVC << endl;
                string evicted_block_trace = tagInVC;
                string bin_evicted_block_trace = hexToBinary(evicted_block_trace);
                for (int i = 0; i < int(log2(VC_BLOCKSIZE)); i++)
                    bin_evicted_block_trace += '0';
                evicted_block_trace = binaryToHex(bin_evicted_block_trace);
                // string key = convertPaddedTrace(evicted_block_trace);
                // cerr << "Key in VC: " <<  evicted_block_trace << endl;
                update_next_level_trace('w', evicted_block_trace, traceNextLevel);
            }
            vcachemap_.pop_back();
            vcachemap_.push_front({tag, dirty_bit});
            return;
        }
    }


public:
    VictimCache(){}
    VictimCache(int vc_num_blocks, int blocksize, string label)
    {
        VC_NUM_BLOCKS = vc_num_blocks;
        VC_BLOCKSIZE = blocksize;
        LABEL = label;
    }

    pair<bool , list<pair<string, int>>::iterator> find(const string &key)
    {
        string tag = getTag(key);
        auto tagIter = findTagInSet(tag);
        if (tagIter == vcachemap_.end()) return make_pair(false, tagIter);
        else return make_pair(true, tagIter);
    }

    void insert(const string &padded_trace, const int &dirty_bit, vector<string> &traceNextLevel, int &write_back)
    {
        string trace = convertPaddedTrace(padded_trace);
        // cout << "Trace obtained in insert: " << trace << endl;
        string tag = getTag(trace);
        // cout << "TAG IN VC : " << tag << endl;
        allocate_block_in_vc(tag, dirty_bit, traceNextLevel, write_back);
        return;
    }

    void swap(list<pair<string, int>>::iterator &iter, const string &padded_trace, const int &dirty_bit)
    {
        string trace = convertPaddedTrace(padded_trace);
        string tag = getTag(trace);
        *iter = {tag, dirty_bit};
        vcachemap_.splice(vcachemap_.begin(), vcachemap_, iter);
        return;
    }

    void print_vc_state()
    {
        cout << "===== VC contents =====" << endl;
        cout << "  set\t0:\t";
        for (auto iter = vcachemap_.begin(); iter != vcachemap_.end(); ++iter)
        {
            if(iter->second == 0) cout << iter->first << " " << " " << "\t";
            if(iter->second == 1) cout << iter->first << " " << "D" << "\t";
        }
        cout << endl;
        return;
    }

};

class Cache
{
private:
    unordered_map<string, list<pair<string, int>>> cachemap_; // {idx: [{tag, 0}, {tag, 1}, {tag, 0}, {tag, 0},...]}} 
    bool victim_cache = false;
    vector<string> traceNextLevel;
    string LABEL;
    VictimCache vc;

    int TAG_LEN_BITS = 0;
    int INDEX_LEN_BITS = 0;

    void update_next_level_trace(const char mode, const string &key)
    {
        string trace = (mode + (" " + key));
        traceNextLevel.push_back(trace);
        return;
    }


    pair<string, string> getTagIndex(const string &key)
    {
        // Get the tag, index, and offset
        string tag; string index;
        int offsetLenBits = (int)log2(BLOCKSIZE);
        int indexLenBits = (int)log2(NUM_BLOCKS);
        int tagLenBits = 32 - offsetLenBits - indexLenBits;
        TAG_LEN_BITS = tagLenBits;
        INDEX_LEN_BITS = indexLenBits;
        // cout << "Tag Len in bits: " << tagLenBits << endl;
        string keyInBin = hexToBinary(key);           
        string tagInBin = keyInBin.substr(0, tagLenBits);
        string indexInBin = keyInBin.substr(tagLenBits, indexLenBits);
        while(tagInBin.size()%4 != 0) tagInBin = '0' + tagInBin;
        while(indexInBin.size()%4 != 0) indexInBin = '0' + indexInBin;
        tag = binaryToHex(tagInBin);
        index = binaryToHex(indexInBin);
        
        return make_pair(tag, index);
    }


    list<pair<string, int>>::iterator findTagInSet(unordered_map<string, list<pair<string, int>>> &cachemap_, const string& idx, const string& tag) 
    {
        // Iterate through the list of pairs in the specific set
        for (auto iter = cachemap_[idx].begin(); iter != cachemap_[idx].end(); ++iter)
        {
            if (iter->first == tag) return iter;
        }
        return cachemap_[idx].end();
    }       

    // Returns trace of evicted block... if nothing is evicted, returns "-1"
    string allocate_block(const string &tag, const string &idx, bool write_to_nxt_lvl = 1, int dirty_bit_to_set = 0)
    {
        if (cachemap_[idx].size() != ASSOC) // Not FULL
        {
            cachemap_[idx].push_front({tag, dirty_bit_to_set});
            return "-1";
        }
        else // FULL --- LRU policy
        {
            auto evicted_block_tag = cachemap_[idx].back().first;
            string evicted_block_trace =  get_evicted_block_trace(evicted_block_tag, idx);
            if ((cachemap_[idx].back().second == 1) && (write_to_nxt_lvl)) 
            {
                update_next_level_trace('w', evicted_block_trace);
                write_back++;
            }
            cachemap_[idx].pop_back();
            cachemap_[idx].push_front({tag, dirty_bit_to_set});
            return evicted_block_trace;
        }
    }

    string get_evicted_block_trace(const string &evicted_block_tag, const string &idx)
    {
        string evicted_block_tag_bin = hexToBinary(evicted_block_tag); // Get tag in Bin
        string copy_evicted_block_tag = evicted_block_tag_bin.substr(evicted_block_tag_bin.size() - TAG_LEN_BITS, TAG_LEN_BITS);
        string idx_bin = hexToBinary(idx);
        string idx_to_join = idx_bin.substr(idx_bin.size() - INDEX_LEN_BITS, INDEX_LEN_BITS);
        string evicted_block_trace = copy_evicted_block_tag + idx_to_join;

        for (int i = 0; i < int(log2(BLOCKSIZE)); i++) // Add dummy block
        {
            evicted_block_trace = evicted_block_trace + '0';
        }
        string correct_trace = binaryToHex(evicted_block_trace); // Get back hex trace(key)
        return correct_trace;
    }

    string removeLeadingZeros(const string& paddedHex)
    {
        string result = paddedHex;
        
        // Find the first non-zero character
        size_t pos = result.find_first_not_of('0');
        
        // If the string consists of all zeros, return "0"
        if (pos == string::npos) {
            return "0";
        }
        
        // Erase the leading zeros by returning a substring starting at the first non-zero character
        result = result.substr(pos);
        
        return result;
    }

public:
    int SIZE, BLOCKSIZE, ASSOC;
    int NUM_BLOCKS = 1;
    int VC_NUM_BLOCKS;
    int read_cache_miss = 0;
    int reads_cache = 0;
    int write_cache_miss = 0;
    int writes_cache = 0;
    int write_back = 0;
    int number_of_swap_requests = 0;
    int number_of_swaps = 0;

    Cache(int size, int blocksize, int assoc, int vc_num_blocks, string label)
    {
        SIZE = size;
        BLOCKSIZE = blocksize;
        ASSOC = assoc;
        NUM_BLOCKS = (assoc != 0) ? SIZE/(BLOCKSIZE*ASSOC) : 1;
        LABEL = label;
        VC_NUM_BLOCKS = vc_num_blocks;
        if(vc_num_blocks > 0)
        {
            victim_cache = true;
            vc = VictimCache(vc_num_blocks, blocksize, "VC");
        }
    }


    void print_cache_state()
    {
        cout << "===== " << LABEL << " contents =====" << endl; 
        int set_no = 0;

        vector<string> cacheItems(cachemap_.size());
        for (const auto &pair: cachemap_)
        {
            const string &key = pair.first;
            int key_deci = stoi(key, nullptr, 16);
            string printItem = "";
            for (auto &iter2 : pair.second)
            {
                printItem = printItem + removeLeadingZeros(iter2.first) + " "; 
                if(iter2.second == 0) printItem = printItem + " " + "\t";
                if(iter2.second == 1) printItem = printItem + "D" + "\t";
            }
            cacheItems[key_deci] = printItem;
        }

        for (int i = 0; i < cacheItems.size(); i++)
        {
            cout << "  set\t" << i << ":\t";
            cout << cacheItems[i] << endl;
        }
        
        if(victim_cache){
            cout << endl;
            vc.print_vc_state();
        }
    }

    vector<string> getTraceNextLevel() {return traceNextLevel;}


    void write(const string &key)
    {
        writes_cache++;
        auto [tag, idx] = getTagIndex(key);
        auto tagIter = findTagInSet(cachemap_, idx, tag);
        if(tagIter == cachemap_[idx].end()) // Write Miss
        {
            write_cache_miss++;

            // Check if VC is present
            if(victim_cache)
            {
                if(cachemap_[idx].size() == ASSOC) // Only if cache set is full, Check in VC
                {
                    number_of_swap_requests++;
                    // Search in victim cache
                    auto [isInVC, vcIter] = vc.find(key);

                    // If present in vc, LRU of main is swapped with the found block in vc
                    if(isInVC)
                    {
                        number_of_swaps++;
                        // Assusme block came from VC
                        int evicted_block_dirty_bit = cachemap_[idx].back().second;
                        string evicted_block_trace = allocate_block(tag, idx, 0);
                        
                        cachemap_[idx].front().second = 1; // Set dirty bit
                        vc.swap(vcIter, evicted_block_trace, evicted_block_dirty_bit);
                        return;
                    }
                    // If not present in vc, LRU of main goes to vc and new block to main cache
                    else
                    {
                        // Assume block came from next level
                        int evicted_block_dirty_bit = cachemap_[idx].back().second;
                        string evicted_block_trace = allocate_block(tag, idx, 0);
                        cachemap_[idx].front().second = 1; // Set dirty bit
                        update_next_level_trace('r', key);
                        // Put the evicted block from L1 into VC
                        vc.insert(evicted_block_trace, evicted_block_dirty_bit, traceNextLevel, write_back);
                        return;
                    }
                }
            }

            // No VC or VC not full,just get block from next level and proceed
            allocate_block(tag, idx);
            update_next_level_trace('r', key);
            cachemap_[idx].front().second = 1; // Set dirty bit
            return;
        }
        else // Write Hit
        {
            cachemap_[idx].splice(cachemap_[idx].begin(), cachemap_[idx], tagIter);
            cachemap_[idx].front().second = 1; // Set dirty bit
            return;
        }
    }


    void read(const string &key)
    {
        // cout << "Read Began: " << endl;
        reads_cache++;
        auto [tag, idx] = getTagIndex(key);
        // cout << "Obtained Tag and Index" << endl;
        auto tagIter = findTagInSet(cachemap_, idx, tag);
        // cout << "Found tag in set:(Hit/Miss)" << endl;
        if(tagIter == cachemap_[idx].end())// Read Miss
        {
            // cout << "Read Miss" << endl;
            read_cache_miss++;

            // Check if VC is present
            if(victim_cache)
            {
                if(cachemap_[idx].size() == ASSOC) // Only if cache is full, Check in VC
                {
                    // cout << "Checking VC" << endl;
                    number_of_swap_requests++;
                    // Search in victim cache
                    auto [isInVC, vcIter] = vc.find(key);

                    // If present in vc, LRU of main is swapped with the found block in vc
                    if(isInVC)
                    {
                        // cout << "Key found in VC" << endl;
                        number_of_swaps++;

                        // Assusme block came from VC
                        int vc_block_dirty_bit = vcIter->second;
                        int evicted_block_dirty_bit = cachemap_[idx].back().second;
                        string evicted_block_trace = allocate_block(tag, idx, 0, vc_block_dirty_bit);
                        // cout << "Obtained evicted block trace: " << evicted_block_trace << endl;
                        
                        vc.swap(vcIter, evicted_block_trace, evicted_block_dirty_bit);
                        // cout << "Swap successfull" << endl;
                        return;
                    }
                    // If not present in vc, LRU of main goes to vc and new block to main cache
                    else
                    {
                        // cout << "Block isnt in VC, obtained it from next level" << endl;
                        // Assume block came from next level
                        int evicted_block_dirty_bit = cachemap_[idx].back().second;
                        string evicted_block_trace = allocate_block(tag, idx, 0);
                        update_next_level_trace('r', key);
                        // cout << "Obtained evicted block trace: " << evicted_block_trace << endl;

                        // Put the evicted block from L1 into VC
                        vc.insert(evicted_block_trace, evicted_block_dirty_bit, traceNextLevel, write_back);
                        // cout << "Insert successfull" << endl;
                        return;
                    }
                }
            }
            // No VC or VC not full,just get block from next level and proceed
            // cout << "No VC or VC not full,just get block from next level and proceed" << endl;
            allocate_block(tag, idx);
            update_next_level_trace('r', key);
            // cout << "Allocated successfully!" << endl;
            return;
        }
        else // Read Hit
        {
            // cout << "Read Hit" << endl;
            cachemap_[idx].splice(cachemap_[idx].begin(), cachemap_[idx], tagIter);
            // cout << "Hit serviced" << endl;
            return;
        }
    }

};

double calculate_miss_rate(Cache &C)
{
    double missrate =  ((C.read_cache_miss)*(1.0))/(C.reads_cache);
    return missrate;
}


int main(int argc, char** argv)
{
    int L1_SIZE = 0, L1_ASSOC = 0, L1_BLOCKSIZE = 0;
    int VC_NUM_BLOCKS = 0, L2_SIZE = 0, L2_ASSOC = 0;
    string trace_file = "";

    if (argc != 8)
    {
        cout << "Incorrect number of arguments" << endl;
        return 0;
    }

    // Taking inputs from the command line
    L1_SIZE = stoi(argv[1]);
    L1_ASSOC = stoi(argv[2]);
    L1_BLOCKSIZE = stoi(argv[3]);
    VC_NUM_BLOCKS = stoi(argv[4]);
    L2_SIZE = stoi(argv[5]);
    L2_ASSOC = stoi(argv[6]);
    trace_file = argv[7];

    // Create the caches based on the given parameters
    Cache L1 = Cache(L1_SIZE, L1_BLOCKSIZE, L1_ASSOC, VC_NUM_BLOCKS, "L1");
    Cache L2 = Cache(L2_SIZE, L1_BLOCKSIZE, L2_ASSOC, 0, "L2");


    // Take in inputs from the .txt file
    ifstream myfile(trace_file);
    string line;
    
    while (getline(myfile, line))
    {
        pair<char, string> result  = trace_decoder(line);
        char mode = result.first;
        string key = result.second;
        // cout << "Mode outside:" << mode << endl;
        // cout << "Key outside:" << key << endl;
        if (mode == 'w') L1.write(key);
        else if (mode == 'r') L1.read(key);
        else exit(1);
    }
    myfile.close();
    

    if (L2_SIZE)
    {
    // print_vector(L1.getTraceNextLevel());
        for(auto i : L1.getTraceNextLevel())
        {
            auto [mode, key] = trace_decoder(i);
            if (mode == 'w') L2.write(key);
            else if (mode == 'r') L2.read(key);
            else exit(1);
        }
    }

    
    cout << "===== Simulator configuration =====" << endl;
    cout << "  L1_SIZE: \t\t" << L1.SIZE << endl;
    cout << "  L1_ASSOC: \t\t" << L1.ASSOC << endl;
    cout << "  L1_BLOCKSIZE: \t" << L1.BLOCKSIZE << endl;
    cout << "  VC_NUM_BLOCKS: \t" << L1.VC_NUM_BLOCKS << endl;
    cout << "  L2_SIZE: \t\t" << L2.SIZE << endl;
    cout << "  L2_ASSOC: \t\t" << L2.ASSOC << endl;
    cout << "  trace_file: \t\t" << trace_file << endl;
    cout << endl;
    L1.print_cache_state();
    cout << endl;
    if(L2_SIZE)
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
    cout << "  i. number writebacks from L1/VC:\t\t\t" << L1.write_back << endl;
    cout << "  j. number of L2 reads:\t\t\t\t" << L2.reads_cache << endl;
    cout << "  k. number of L2 read misses:\t\t\t\t" << L2.read_cache_miss << endl;
    cout << "  l. number of L2 writes:\t\t\t\t" << L2.writes_cache << endl;
    cout << "  m. number of L2 write misses:\t\t\t\t" << L2.write_cache_miss << endl;
    double L2_miss_rate = L2_SIZE ? calculate_miss_rate(L2) : 0;
    printf("  n. L2 miss rate:\t\t\t\t\t%.4lf\n", L2_miss_rate);
    cout << "  o. number of writebacks from L2:\t\t\t" << L2.write_back << endl;
    if(!L2_SIZE)
    {
        // cout << "  p. total memory traffic:\t\t\t\t" << L1.getTraceNextLevel().size() << endl;
        cout << "  p. total memory traffic:\t\t\t\t" << L1.read_cache_miss + L1.write_cache_miss - L1.number_of_swaps + L1.write_back << endl;

    }
    else
        // cout << "  p. total memory traffic:\t\t\t\t" << L2.getTraceNextLevel().size() << endl;
        cout << "  p. total memory traffic:\t\t\t\t" << L2.read_cache_miss + L2.write_cache_miss + L2.write_back << endl;
    cout << endl;
    cout << "===== Simulation results (performance) =====" << endl;

    // AAT Calculation
    float average_access_time = 0; double energy_delay_product = 0; float total_area = 0;
    float L1_ht = 0; float L2_ht = 0; float VC_ht = 0; float mem_miss_penalty = 20; // ns
    float peak_bandwidth = 16; // GB/s 

    // Energy Calculation
    float L1_E = 0; float L2_E = 0; float VC_E = 0; float mem_E = 0.05; // nJ per access
    
    // Area Calculation
    float L1_Ar = 0; float L2_Ar = 0; float VC_Ar = 0;
    

    int L1_results_invalid = get_cacti_results(L1.SIZE, L1.BLOCKSIZE, L1.ASSOC, &L1_ht, &L1_E, &L1_Ar);
    if(L1_results_invalid) cout << "Cacti failed for L1" << endl;
    if(VC_NUM_BLOCKS)
    {
        int VC_results_invalid = get_cacti_results(VC_NUM_BLOCKS*L1.BLOCKSIZE, L1.BLOCKSIZE, 0, &VC_ht, &VC_E, &VC_Ar);
        // cout << "VC_ht: " << VC_ht << endl;
        // cout << "VC_E: " << VC_E << endl;
        // cout << "VC_Ar: " << VC_Ar << endl;
        if(VC_results_invalid) cout << "Cacti failed for VC" << endl;
    }
    if(L2_SIZE)
    {
        int L2_results_invalid = get_cacti_results(L2_SIZE, L2.BLOCKSIZE, L2.ASSOC, &L2_ht, &L2_E, &L2_Ar);
        if(L2_results_invalid) cout << "Cacti failed for L2" << endl;
    }

    float aat_next_lvl = mem_miss_penalty + L1.BLOCKSIZE/peak_bandwidth;
    if(L2_SIZE) // If L2 present
    {
        float miss_penalty_for_L2 = aat_next_lvl;
        float aat_l2 = L2_ht + L2_miss_rate * miss_penalty_for_L2;
        aat_next_lvl = aat_l2;
    }

    float aat = 0;
    if(L1.number_of_swap_requests) // If VC present
    {
        aat = L1_ht + comined_miss_rate * ((L1.number_of_swaps/L1.number_of_swap_requests) * VC_ht + (1 - (L1.number_of_swaps/L1.number_of_swap_requests)) * aat_next_lvl);
    }
    else
    {
        // cout << "L1 miss rate = " << comined_miss_rate << endl; 
        // cout << "L1_ht = " << L1_ht << endl;
        // cout << "aat_next_lvl = " << aat_next_lvl << endl;
        aat = L1_ht + comined_miss_rate* aat_next_lvl;
    }

    total_area = L1_Ar + L2_Ar + VC_Ar;
    if(!L2_SIZE)energy_delay_product = (L1.reads_cache + L1.writes_cache + L1.read_cache_miss + L1.write_cache_miss)*L1_E + 2*(L1.number_of_swap_requests)*VC_E + (L1.read_cache_miss + L1.write_cache_miss - L1.number_of_swaps + L1.write_back)*mem_E;
    else energy_delay_product = (L1.reads_cache + L1.writes_cache + L1.read_cache_miss + L1.write_cache_miss)*L1_E + 2*(L1.number_of_swap_requests)*VC_E + (L2.reads_cache+L2.writes_cache + L2.read_cache_miss + L2.write_cache_miss)*L2_E + (L2.read_cache_miss+L2.write_cache_miss + L2.write_back)*mem_E;
    energy_delay_product *= (L1.reads_cache+L1.writes_cache)*aat;

    printf("  1. average access time:\t\t\t\t%.4f\n", aat);
    // cout << "  1. average access time:\t\t\t\t" << aat << endl;
    printf("  2. energy-delay product:\t\t\t\t%.4f\n", energy_delay_product);
    // cout << "  2. energy-delay product:\t\t\t\t" << << endl;
    printf("  3. total area:\t\t\t\t\t%.4f\n", total_area);
    // cout << "  3. total area:\t\t\t\t\t" << endl;

    return 0;
}
