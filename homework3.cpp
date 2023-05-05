#include <iostream>
#include <string>
#include <queue>
#include <map>
#include <fstream>
#include <bitset>
using namespace std;
//定义二叉树结构体
struct BinNode {
    char ch;    //字符
    int freq;   //频率
    BinNode* left;  //左子节点
    BinNode* right; //右子节点
    BinNode(char c = 0, int f = 0, BinNode* l = nullptr, BinNode* r = nullptr) :ch(c), freq(f), left(l), right(r) {};
};
//定义比较函数，用于优先队列排序
class cmp {
public:
    bool operator() (BinNode* a, BinNode* b) {
        return a->freq > b->freq;
    }
};
//定义二叉树类
class BinTree {
public:
    BinNode* root;
    BinTree() :root(nullptr) {};
    BinTree(char c, int f) {
        root = new BinNode(c, f);
    }
    BinTree(BinTree& l, BinTree& r) {
        root = new BinNode(0, l.root->freq + r.root->freq, l.root, r.root);
    }
    ~BinTree() {
        release(root);
    }
    void release(BinNode* node) {
        if (node) {
            release(node->left);
            release(node->right);
            delete node;
        }
    }
};
//定义Huffman编码类
class HuffCode {
public:
    string code;
    int len;
    HuffCode() :code(""), len(0) {};
    void clear() {
        code.clear();
        len = 0;
    }
    void copy(const HuffCode& hc) {
        code = hc.code;
        len = hc.len;
    }
    void operator= (const HuffCode& hc) {
        copy(hc);
    }
    void operator+= (int b) {
        code += (b ? '1' : '0');
        ++len;
    }
    operator string() const {
        return code;
    }
};
//定义Huffman编码树类
class HuffTree {
public:
    BinTree* tree;
    map<char, HuffCode> codes;
    HuffTree() :tree(nullptr) {};
    ~HuffTree() {
        if (tree) {
            delete tree;
        }
    }
    void release() {
        if (tree) {
            delete tree;
            tree = nullptr;
        }
        codes.clear();
    }
    void build(const map<char, int>& freq) {
        release();
        priority_queue<BinNode*, vector<BinNode*>, cmp> pq;
        for (auto it = freq.begin(); it != freq.end(); ++it) {
            pq.push(new BinNode(it->first, it->second));
        }
        while (pq.size() > 1) {
            BinNode* l = pq.top();
            pq.pop();
            BinNode* r = pq.top();
            pq.pop();
            pq.push(new BinNode(0, l->freq + r->freq, l, r));
        }
        tree = new BinTree();
        tree->root = pq.top();
        buildCodes(tree->root, HuffCode());
    }
    void buildCodes(BinNode* node, HuffCode code) {
        if (node->left) {
            HuffCode lcode(code);
            lcode += 0;
            buildCodes(node->left, lcode);
            HuffCode rcode(code);
            rcode += 1;
            buildCodes(node->right, rcode);
        }
        else {
            codes[node->ch] = code;
        }
    }
    void encode(const string& text, string& result) {
        result.clear();
        for (int i = 0; i < text.size(); ++i) {
            result += codes[text[i]];
        }
    }
    void encode(const string& text, ofstream& file) {
        string result;
        encode(text, result);
        int len = result.size();
        file.write((char*)&len, sizeof(int));
        for (int i = 0; i < len; i += 8) {
            bitset<8> bits;
            for (int j = 0; j < 8 && i + j < len; ++j) {
                bits[j] = (result[i + j] == '1');
            }
            char c = (char)bits.to_ulong();
            file.write(&c, sizeof(char));
        }
    }
    void decode(ifstream& file, string& result) {
        result.clear();
        int len;
        file.read((char*)&len, sizeof(int));
        if (len > 0) {
            char c;
            string code;
            while (len > 0) {
                file.read(&c, sizeof(char));
                bitset<8> bits(c);
                code += bits.to_string();
                while (code.size() >= len) {
                    result += decode(code.substr(0, len));
                    code = code.substr(len);
                    len -= 8;
                }
            }
        }
    }
    char decode(const string& code) {
        BinNode* node = tree->root;
        for (int i = 0; i < code.size(); ++i) {
            if (code[i] == '0') {
                node = node->left;
            }
            else {
                node = node->right;
            }
        }
        return node->ch;
    }
};
//主函数
int main() {
    string text = "I have a dream that one day this nation will rise up and live out the true meaning of its creed: We hold these truths to be self-evident, that all men are created equal.";
    map<char, int> freq;
    for (int i = 0; i < text.size(); ++i) {
        ++freq[text[i]];
    }
    HuffTree tree;
    tree.build(freq);
    string code;
    tree.encode(text, code);
    cout << "Huffman编码结果：" << code << endl;
    string decode;
    for (int i = 0; i < code.size(); ++i) {
        decode += tree.decode(code.substr(i, 1));
    }
    cout << "Huffman解码结果：" << decode << endl;
    ofstream file("huffman.bin", ios::binary);
    tree.encode(text, file);
    file.close();
    }
  