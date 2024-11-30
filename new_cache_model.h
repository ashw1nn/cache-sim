#ifndef NEW_CACHE_MODEL_H
#define NEW_CACHE_MODEL_H

#include <bits/stdc++.h>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;
#define endl '\n'


string decimalToBinary(int decimal) {
    string binary = bitset<32>(decimal).to_string();
    
    size_t pos = binary.find_first_not_of('0');
    if (pos == string::npos) {
        return "0";
    }

    return binary.substr(pos);
}

string hexToBinary(const string &hex)
{
    // Lookup table for hexadecimal to binary conversion
    unordered_map<char, string> hexToBinDict = {
        {'0', "0000"}, {'1', "0001"}, {'2', "0010"}, {'3', "0011"},
        {'4', "0100"}, {'5', "0101"}, {'6', "0110"}, {'7', "0111"},
        {'8', "1000"}, {'9', "1001"}, {'A', "1010"}, {'B', "1011"},
        {'C', "1100"}, {'D', "1101"}, {'E', "1110"}, {'F', "1111"},
        {'a', "1010"}, {'b', "1011"}, {'c', "1100"}, {'d', "1101"},
        {'e', "1110"}, {'f', "1111"}
    };

    string binary = "";
    for (char hexDigit : hex) binary += hexToBinDict[hexDigit];

    return binary;
}

string binaryToHex(const string &binary)
{
    // Lookup table for binary to hexadecimal conversion
    unordered_map<string, char> binToHexMap = {
        {"0000", '0'}, {"0001", '1'}, {"0010", '2'}, {"0011", '3'},
        {"0100", '4'}, {"0101", '5'}, {"0110", '6'}, {"0111", '7'},
        {"1000", '8'}, {"1001", '9'}, {"1010", 'a'}, {"1011", 'b'},
        {"1100", 'c'}, {"1101", 'd'}, {"1110", 'e'}, {"1111", 'f'}
    };

    string hex = "";
    string binaryPadded = binary;
    
    // If the length of the binary string is not a multiple of 4, pad it with leading zeros
    while (binaryPadded.size() % 4 != 0) binaryPadded = "0" + binaryPadded;

    // Convert every 4 bits to a hexadecimal digit
    for (size_t i = 0; i < binaryPadded.size(); i += 4)
    {
        string fourBits = binaryPadded.substr(i, 4);  // Get 4-bit chunk
        hex += binToHexMap[fourBits];  // Convert to hex using lookup table
    }

    return hex;
}


class VictimCache
{
private:
    int VC_NUM_BLOCKS = 0;
    int VC_BLOCKSIZE = 0;
    int TAG_LEN = 0; int BLOCK_LEN = 0;
    string LABEL = "";
    list<pair<string, int>> vcachemap_; // [tag, tag, tag, tag, ...] 


    void update_next_level_trace(char mode, const string &trace, vector<string> &traceNextLevel)
    {
        string traceToUpdate = (mode + (" " + trace));
        traceNextLevel.push_back(traceToUpdate);
        return;
    }

    string getTag(const string &trace)
    {
        if(trace.size() != 8)
        {
            cout << "ERROR: Obtained trace inside VC: " << trace << endl;
            cout << "ERROR: Trace size is not 8 inside VC" << endl;
            print_vc_state();
            exit(1);
        }
        string tag;
        string binTrace = hexToBinary(trace);
        string binTag = binTrace.substr(0, TAG_LEN);
        tag = binaryToHex(binTag);

        return tag;
    }


    void allocate_vc_block(const string &tag, int dirty_bit, vector<string> &traceNextLevel, int &writeBacks)
    {
        if (vcachemap_.size() != VC_NUM_BLOCKS) // Not FULL
        {
            vcachemap_.push_front({tag, dirty_bit}); ////////marked by me for review///////
            return;                                  // Should I writeback before allocation---of that sort ////
        }
        else // FULL --- LRU policy
        {
            if (vcachemap_.back().second == 1)
            {
                writeBacks++;
                string tagInVC = vcachemap_.back().first;
                string evicted_block_trace = hexToBinary(tagInVC);
                for (int i = 0; i < BLOCK_LEN; i++) evicted_block_trace += '0';
                update_next_level_trace('w', evicted_block_trace, traceNextLevel);
            }
            vcachemap_.pop_back();
            vcachemap_.push_front({tag, dirty_bit});
            return;
        }
    }

    list<pair<string, int>>::iterator findTagInSet(const string& tag)
    {
        for (auto it = vcachemap_.begin(); it != vcachemap_.end(); it++)
        {
            if(it->first == tag) return it;
        }
        
        return vcachemap_.end();
    }


public:
    VictimCache(){}
    VictimCache(int vc_num_blocks, int blocksize, string label)
    {
        VC_NUM_BLOCKS = vc_num_blocks;
        VC_BLOCKSIZE = blocksize;
        LABEL = label;
        BLOCK_LEN = (int)log2(VC_BLOCKSIZE); // In bits
        TAG_LEN = 32 - BLOCK_LEN;            // In bits

        vcachemap_.resize(VC_NUM_BLOCKS);
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

    pair<bool, list<pair<string, int>>::iterator> findInVC(const string &trace)
    {
        // cout << "Finding in VC" << endl;
        string tag = getTag(trace);
        auto tagIter = findTagInSet(tag);
        if (tagIter == vcachemap_.end()) return make_pair(false, tagIter);
        return make_pair(true, tagIter);
    }


    void insertIntoVC(const string &trace, int dirty_bit, vector<string> &traceNextLevel, int &writeBacks)
    {
        // cout << "Inserting in VC" << endl;
        string tag = getTag(trace);
        allocate_vc_block(tag, dirty_bit, traceNextLevel, writeBacks);
    }

    void swapWithVC(list<pair<string, int>>::iterator &iterToVC, const string &cacheTrace, int dirty_bit)
    {
        // cout << "Swapping in VC" << endl;
        string tag = getTag(cacheTrace);
        *iterToVC = {tag, dirty_bit};
        vcachemap_.splice(vcachemap_.begin(), vcachemap_, iterToVC);
        return;
    }    

};



class Cache
{
private:
    vector<list<pair<string, int>>> cachemap_;
    // {idx: [{tag, 0}, {tag, 1}, {tag, 0}, {tag, 0},...]}}
    bool victim_cache = false;
    vector<string> traceNextLevel;
    string LABEL;
    VictimCache vc;

    int TAG_LEN = 0;     // In bits
    int INDEX_LEN = 0;   // In bits
    int OFFSET_LEN = 0;  // In bits



    pair<string, int> getTagIndex(const string &trace)
    {
        if(trace.size() != 8)
        {
            cout << "ERROR: Trace size is not 8" << endl;
            exit(1);
        }
        string tag; int index;
        string binTrace = hexToBinary(trace);
        string binTag = binTrace.substr(0, TAG_LEN);
        string binIndex = binTrace.substr(TAG_LEN, INDEX_LEN);
        tag = binaryToHex(binTag);
        index = stoi(binIndex, nullptr, 2);

        return make_pair(tag, index);
    }

    list<pair<string, int>>::iterator findTagInCacheLine(int index, const string &tag)
    {
        for (auto it = cachemap_[index].begin(); it != cachemap_[index].end(); it++)
        {
            if(it->first == tag) return it;
        }
        return cachemap_[index].end();
    }


    string get_evicted_block_trace(const int index)
    {
        string evictTag = hexToBinary(cachemap_[index].back().first);
        string binEvictTag = evictTag.substr(evictTag.size()-TAG_LEN, TAG_LEN);
        string binEvictIndex = decimalToBinary(index);
        string binEvictTrace = binEvictTag + binEvictIndex;
        while (binEvictTrace.size() < 32) binEvictTrace = binEvictTrace + '0';
        
        return binaryToHex(binEvictTrace);
    }


    // Returns trace of evicted block... if nothing is evicted, returns "-1"
    string allocate_block(const int index, const string &tag, bool write_to_next_lvl=1, int dirty_bit_to_set=0)
    {
        if (cachemap_[index].size() != ASSOC) // Not FULL
        {
            cachemap_[index].push_front({tag, dirty_bit_to_set});
            return "-1";
        }
        else
        {
            string evicted_block_trace = get_evicted_block_trace(index);
            if(cachemap_[index].back().second == 1 && write_to_next_lvl)
            {
                updateNextLevelTrace('w', evicted_block_trace);
                writeBacks++;
            }
            cachemap_[index].pop_back();
            cachemap_[index].push_front({tag, dirty_bit_to_set});
            
            return evicted_block_trace;
        }
    }


public:
    int SIZE, BLOCKSIZE, ASSOC, NUM_BLOCKS=1;
    int VC_NUM_BLOCKS = 0;

    int reads = 0;
    int writes = 0;
    int readMisses = 0;
    int writeMisses = 0;
    int writeBacks = 0;
    int swapRequests = 0;
    int swaps = 0;
    
    Cache(int size, int blocksize, int assoc, int vc_num_blocks, string label)
    {
        SIZE = size;
        BLOCKSIZE = blocksize;
        ASSOC = assoc;
        NUM_BLOCKS = (assoc != 0) ? SIZE/(BLOCKSIZE*ASSOC) : 1;
        LABEL = label;
        
        OFFSET_LEN = (int)log2(BLOCKSIZE);
        INDEX_LEN = (int)log2(NUM_BLOCKS);
        TAG_LEN = 32 - OFFSET_LEN - INDEX_LEN;

        cachemap_.resize(NUM_BLOCKS);

        VC_NUM_BLOCKS = vc_num_blocks;
        if(vc_num_blocks > 0)
        {
            victim_cache = true;
            vc = VictimCache(vc_num_blocks, blocksize, "VC");
        }
    }

    vector<string> getTraceNextLevel() {return traceNextLevel;}

    void print_cache_state()
    {
        cout << "===== " << LABEL << " contents =====" << endl; 
        
        for (int i = 0; i < cachemap_.size(); i++)
        {
            cout << "  set\t" << i << ":\t";
            for (auto iter = cachemap_[i].begin(); iter != cachemap_[i].end(); ++iter)
            {
                if(iter->second == 0) cout << iter->first << " " << " " << "\t";
                if(iter->second == 1) cout << iter->first << " " << "D" << "\t";
            }
            cout << endl;
        }
    }

    void updateNextLevelTrace(char mode,const string &trace)
    {
        string nxtTrace = (mode + (" " + trace));
        // cout << "[UPDATE] Next Trace: " << nxtTrace << endl;
        traceNextLevel.push_back(nxtTrace);
        return;
    }

    void printTraces() {
        for (const auto &t : traceNextLevel) {
            cout << t << endl;
        }
    }


    void read(const string &trace) // Returns 1 for hits and 0 for misses
    {
        // cout << "[READ] " << trace << endl;
        reads++;
        
        auto [tag, index] = getTagIndex(trace); // Tag is a Hex String, Index is a decimal number
        if (index >= NUM_BLOCKS)
        {
            cout << "ERROR: Index out of bounds" << endl;
            exit(1);
        }

        auto tagIter = findTagInCacheLine(index, tag);

        if (tagIter == cachemap_[index].end()) // Read Miss
        {
            readMisses++;

            if(victim_cache)
            {
                if(cachemap_[index].size() == ASSOC) // Only if cache full, check VC
                {
                    swapRequests++;

                    auto [foundInVC, vcIter] = vc.findInVC(trace);

                    if(foundInVC)
                    {
                        swaps++;

                        // Operations to update VC with the LRU of cache and vice-versa
                        int vc_dirty_bit = vcIter->second;
                        int cache_dirty_bit = cachemap_[index].back().second;

                        string evicted_block_trace = allocate_block(index, tag, 0, vc_dirty_bit);
                        vc.swapWithVC(vcIter, evicted_block_trace, cache_dirty_bit);
                    }
                    else
                    {
                        // Assume block came from next lvl and update everything
                        int cahce_dirty_bit = cachemap_[index].back().second;
                        string evicted_block_trace = allocate_block(index, tag, 0, 0);
                        updateNextLevelTrace('r', trace);
                        
                        vc.insertIntoVC(evicted_block_trace, cahce_dirty_bit, traceNextLevel, writeBacks);
                        return;
                    }
                }
            }

            // No VC or VC not full, just get block from nxt lvl and proceed
            allocate_block(index, tag);
            updateNextLevelTrace('r', trace);

            return;
        }
        else // Read Hit
        {
            cachemap_[index].splice(cachemap_[index].begin(), cachemap_[index], tagIter);
            return;
        }
    }

    void write(const string &trace)
    {
        writes++;
        
        auto [tag, index] = getTagIndex(trace); // Tag is a Hex String, Index is a decimal number
        if (index >= NUM_BLOCKS)
        {
            cout << "ERROR: Index out of bounds" << endl;
            exit(1);
        }

        auto tagIter = findTagInCacheLine(index, tag);

        if (tagIter == cachemap_[index].end()) // Write Miss
        {
            writeMisses++;

            if(victim_cache)
            {
                if(cachemap_[index].size() == ASSOC) // Only if cache full, check VC
                {
                    swapRequests++;

                    auto [foundInVC, vcIter] = vc.findInVC(trace);

                    if(foundInVC)
                    {
                        swaps++;

                        // Operations to update VC with the LRU of cache and vice-versa
                        int vc_dirty_bit = vcIter->second;
                        int cache_dirty_bit = cachemap_[index].back().second;

                        string evicted_block_trace = allocate_block(index, tag, 0);

                        cachemap_[index].front().second = 1; // Set dirty bit
                        vc.swapWithVC(vcIter, evicted_block_trace, cache_dirty_bit);
                        return;
                    }
                    else
                    {
                        // Assume block came from next lvl and update everything
                        int cahce_dirty_bit = cachemap_[index].back().second;
                        string evicted_block_trace = allocate_block(index, tag, 0);
                        cachemap_[index].front().second = 1; // Set dirty bit
                        updateNextLevelTrace('r', trace);
                        
                        vc.insertIntoVC(evicted_block_trace, cahce_dirty_bit, traceNextLevel, writeBacks);
                        return;
                    }

                }
            }

            // No VC or VC not full, just get block from nxt lvl and proceed            
            allocate_block(index, tag);
            updateNextLevelTrace('r', trace);
            cachemap_[index].front().second = 1; // Set dirty bit
            
            return;
        }
        else // Write Hit
        {
            cachemap_[index].splice(cachemap_[index].begin(), cachemap_[index], tagIter);
            cachemap_[index].front().second = 1; // Set dirty bit

            return;
        }
    }

};

#endif
