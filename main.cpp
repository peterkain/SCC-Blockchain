#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

using namespace std;

struct Node
{
    Node() {}
    Node(const string& id, const unsigned& hash_rate)
        :   id{id}, hash_rate{hash_rate} {}

    string id;
    unsigned hash_rate;
};
unsigned operator+(const int& l, const Node& r) { return l + r.hash_rate; }

struct PoolInfo
{
    vector<string> ids;
    vector<int> hashing_power;
};

class Pool
{
public:
    void add(const Node& n) { _nodes.push_back(n); }
    void set_id(const string& id) { _id = id; }
    string get_id() const { return _id; }
    void clear() { _nodes.clear(); _id.clear(); }
    int get_hashing_power() const { return accumulate(_nodes.begin(), _nodes.end(), 0); }

private:  
    vector<Node> _nodes;
    string _id;
} p;

class Network
{
public:
    void add(const Node& n) { _nodes.push_back(n); }
    void add(const Pool& p) { _pools.push_back(p); }
    vector<Node> get_nodes() const { return _nodes; }
    int get_hashing_power() const { return accumulate(_nodes.begin(), _nodes.end(), 0); }

    void get_pool_info(PoolInfo& pi) const
    {
        for_each(_pools.begin(), _pools.end(), [&] (const Pool& p) {
            pi.ids.push_back(p.get_id());
            pi.hashing_power.push_back(p.get_hashing_power());
        });
    }

private:
    vector<Node> _nodes;
    vector<Pool> _pools;
};

vector<string> load_words(const string& filename)
{
    string word; vector<string> result;
    while(ifstream{filename} >> word) result.push_back(word);
    return result;
}

Node find_id(const vector<Node>& nodes, const string& id)
{
    for(const auto& n : nodes)
        if(n.id == id) return n;
    return {"NotFound", 0};
}

int main()
{
    ofstream file {"output.txt"};
    for(int files {1}; files++ < 5;)
    {
        Network network;
        vector<string> content = load_words("level1-" + to_string(files) + ".txt");

        int node_count = stoi(content[0]);
        int pool_count = stoi(content[node_count * 2 + 1]);
        bool assigned_id = false;

        for(int i {1}; i < node_count * 2; i += 2)
            network.add({content[i], stoi(content[i + 1])});

        for(int i {node_count * 2 + 2}; i < content.size(); i++)
        {
            if(content[i][0] == 'P' || i + 1 == content.size()) 
            {
                if(assigned_id)
                {
                    network.add(p);
                    p.clear();
                    assigned_id = false; --i;
                }
                else
                {
                    p.set_id(content[i]);
                    assigned_id = true;
                }
            }
            else p.add(find_id(network.get_nodes(), content[i]));
        }

        PoolInfo pi; network.get_pool_info(pi);

        file << network.get_hashing_power() << " ";
        for(int i {0}; i < pi.ids.size(); i++)
            file << pi.ids[i] << " " << pi.hashing_power[i] << " ";
        file << "\n";
    }
    file.close();
}