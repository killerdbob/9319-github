#include <iostream>
#include <cstring>
#include <fstream>
#include <cstdio>
#include <map>
#include <list>

using namespace std;

struct node
{
    bool node_type;//true means leaves, false means nodes.
    int a;
    long num;
    node *left;
    node *right;
    bool operator<(node a)
    {
        return num<a.num;
    }
};

class tree
{
public:
    node *head;
    node *go_cur;
    tree()
    {
        head            = new node;
        head->left      = NULL;
        head->right     = NULL;
        head->node_type = false;
        head->num       = 0;
    };

    //merger two tree
    void mer(node *left,node *right)
    {
        head->left  = left;
        head->right = right;
        head->num   = left->num+right->num;

    }

    //true go left, false go right
    bool go(bool i)
    {
        if(i)
        {
            go_cur = go_cur->right;
        }
        else
        {
            go_cur = go_cur->left;
        }
        return go_cur->node_type;
    }

    void go2head()
    {
        go_cur=head;
    }

    char go2a()
    {
        return go_cur->a;
    }
    //release the tree
    void release(node *p)
    {
        if(p->left)
        {
            if(p->left->left || p->left->right)
            {
                release(p->left);
            }
            if(!p->left->left && !p->left->right)
            {
                delete p->left;
                p->left=NULL;
            }
        }
        if(p->right)
        {
            if(p->right->left || p->right->right)
            {
                release(p->right);
            }
            if(!p->right->left && !p->right->right)
            {
                delete p->right;
                p->right=NULL;
            }
        }
    }

    //release the tree
    ~tree()
    {
        release(head);
        delete head;
    };

};

class bitwise
{
public:
    int bitctrl;
    int curindex;

    bitwise()
    {
        bitctrl  = -1;
        curindex = -1;
    };
    ~bitwise() {};

    void setbyte(int c)
    {
        curindex = 7;
        bitctrl  = c;
    }

    int getbyte()
    {
        if(curindex==-1)
        {
            return -1;
        }
        else
        {
            int result = (bitctrl&(1<<curindex))>0?1:0;
            curindex--;
            return result;
        }
    }

    void setind(int c=8)
    {
        curindex=c;
    }

    int getind()
    {
        return curindex;
    }

    //create a mask fillter out other irrelavant bits e.g.00001101,3-->00000101
    int mask(int a,int b)
    {
        int mask=0;
        for(int i=0; i<b; i++)
        {
            mask=mask<<1;
            mask++;
        }
        return a&mask;
    }

    //return ith value of int
    bool index_mask(int data,int index)
    {
        return ((data&(1<<index))>0)?1:0;
    }
};

class huffman
{
public :
    FILE *fi        = NULL;
    FILE *fo        = NULL;
    int header;
    string curcode;
    int sta[256];
    map<int, string> huffcode;
    int huffcode_length;
    tree h2tree;
    bitwise bitw;
    int *failure;
    long filesz;
    long bitlong;
    char *buffer;
    int cnt;

    huffman() {};

    ~huffman() {};

    void init()
    {
        header  = 0;
        curcode = "";
    }
    /*
        file system operation
    */
    void setinputfile(char *input)
    {
        fi=fopen(input,"rb");
    }

    void setoutputfile(char *output)
    {
        fo=fopen(output,"wb+");
    }

    void closeinputfile()
    {
        if(fi)
        {
            fclose(fi);
        }
    }

    void closeoutputfile()
    {
        if(fo)
        {
            fclose(fo);
        }
    }

    void write2file(char w)
    {
        fputc(w,fo);
        header++;
    }

    /*
       binary operation
    */
    void endbits2file(){
        fseek(fo,1023,SEEK_SET);
        fputc(huffcode_length,fo);
    }

    void str2bin2file()
    {
        std::string bin="";//to store binary code
        int c;//get the charater
        int a;//to store binary code
        fseek(fi,0,SEEK_SET);
        while((c=getc(fi))!=EOF)
        {
            bin += huffcode[c];
            while(bin.length()>=8)
            {
                a=0;
                for(unsigned int i=0; i<8; i++)
                {
                    if(bin[i]=='1'){
                        a=a<<1;
                        a++;
                    }else{
                        a=a<<1;
                    }
                }
                write2file(a);
                bin.erase(0,8);
            }
        }
        huffcode_length = (8-bin.length())%8;
        if(bin.length()>0){
            a=0;
            for(unsigned int i=0; i<bin.length(); i++){
                if(bin[i]=='1'){
                    a=a<<1;
                    a++;
                }else{
                    a=a<<1;
                }
            }
            a=a<<(8-bin.length());
            write2file(a);
        }
    }

    //this part is for decoder

    void get_info(bool e=true){//e=true encoded file, e=false original file.
        filesz  = 0;
        bitlong = 0;
        fseek(fi,0,SEEK_END);
        if(e){
            filesz  = ftell(fi)-1024;
        }else{
            filesz  = ftell(fi);
        }
        bitlong = filesz*8-get_endbits();
        fseek(fi,0,SEEK_SET);
    }

    void bin2str2file()
    {
        buffer = (char*)new char[1000000];
        cnt=0;
        h2tree.go2head();
        if(fseek(fi,1024,SEEK_SET)!=-1)
        {
            for(long i=0; i<bitlong; i++ )
            {
                int j = -1;
                if((j=bitw.getbyte())==-1)
                {
                    bitw.setbyte(my_getc());
                    j=bitw.getbyte();
                }
                if(h2tree.go(j))
                {
                    write2file(h2tree.go2a());
                    h2tree.go2head();
                }
            }
        }
    }
    /*
    *      generate header:
    *              0 as start of char, 1 as SEP, 2 as EOF, use left first tree to store.
    */

    void flushheader()
    {
        if(header<=1024)
        {
            while(header<1024)
            {
                write2file(2);
            }
            header = 0;
        }
    }

    //write out the important part of the header.(tree)
    void writeheader(node *p)
    {
        if(p->left)
        {
            curcode += "1";
            writeheader(p->left);
            curcode.erase(curcode.end()-1);
        }
        if(p->right)
        {
            curcode += "0";
            writeheader(p->right);
            curcode.erase(curcode.end()-1);
        }
        if(p->node_type)
        {
            huffcode[p->a]=curcode;
            write2file(0);
            write2file(p->a);
        }
        else
        {
            write2file(1);//write SEP as Node
        }
    }

    //for decode part
    int get_endbits()
    {
        if(! fseek(fi,1023,SEEK_SET))
        {
            return fgetc(fi);
        }
        return -1;
    }

    void header2tree()
    {
        int cur_pos=0;
        list<node> tree_list;
        node *left  = NULL;
        node *right = NULL;
        node *temp  = NULL;
        if(fi)
        {
            while(cur_pos<1024)
            {
                int c=getc(fi);
                cur_pos++;
                if( c==2 ) //encounter stop symbol
                {

                    h2tree.head->a         = tree_list.front().a;
                    h2tree.head->left      = tree_list.front().left;
                    h2tree.head->right     = tree_list.front().right;
                    h2tree.head->node_type = false;

                    tree_list.pop_front();
                    break;
                }
                else if( c==0 )  //encounter a chararater
                {
                    c=getc(fi);
                    temp             = new node;
                    temp->a          = c;
                    temp->node_type  = true;
                    temp->left       = NULL;
                    temp->right      = NULL;
                    tree_list.push_front(*temp);
                }
                else if( c==1 )   //encounter a node
                {
                    temp             = new node;
                    temp->a          = '\0';
                    temp->node_type  = false;

                    left             = new node;
                    left->a          = tree_list.front().a;
                    left->left       = tree_list.front().left;
                    left->right      = tree_list.front().right;
                    left->node_type  = tree_list.front().node_type;
                    tree_list.pop_front();
                    temp->left       = left;

                    if(tree_list.size()>0)
                    {
                        right            = new node;
                        right->a         = tree_list.front().a;
                        right->left      = tree_list.front().left;
                        right->right     = tree_list.front().right;
                        right->node_type = tree_list.front().node_type;
                        tree_list.pop_front();
                        temp->right      = right;
                    }
                    else
                    {
                        temp->right      = NULL;
                    }
                    tree_list.push_front(*temp);
                }
            }
        }
    }

    void statistic()
    {
        int cnt=0;
        buffer = (char*)new char[1000000];
        for(int i=0;i<256;i++){
            sta[i]=0;
        }
        while(cnt<filesz){
            fread(buffer,1,1000000,fi);
            for(int i=0;i<1000000;i++){
                if(cnt==filesz){
                    break;
                }else{
                    cnt++;
                    unsigned int j=buffer[i];
                    sta[j%256]++;
                }
            }
        }
        delete []buffer;
    }

    void char2tree()
    {
        std::list<node> my_list;
        for(int i=0;i<256;i++)
        {
            if(sta[i]){
                node *p         = new node;
                p->num          = sta[i];
                p->node_type    = true;
                p->a            = i;
                p->left         = NULL;
                p->right        = NULL;
                my_list.push_front(*p);
            }
        }
        while(my_list.size()>1){
            my_list.sort();
            node *left       = new node;
            node *right      = new node;
            left->a          = my_list.front().a;
            left->right      = my_list.front().right;
            left->left       = my_list.front().left;
            left->num        = my_list.front().num;
            left->node_type  = my_list.front().node_type;
            my_list.pop_front();

            right->a         = my_list.front().a;
            right->right     = my_list.front().right;
            right->left      = my_list.front().left;
            right->num       = my_list.front().num;
            right->node_type = my_list.front().node_type;
            my_list.pop_front();

            node temp;
            temp.left        = left;
            temp.right       = right;
            temp.num         = left->num + right->num;
            temp.a           = ' ';
            temp.node_type   = false;
            my_list.push_front(temp);
        }
        if(my_list.front().node_type == false)
        {
            h2tree.head->a            = my_list.front().a;
            h2tree.head->right        = my_list.front().right;
            h2tree.head->left         = my_list.front().left;
            h2tree.head->num          = my_list.front().num;
            h2tree.head->node_type    = my_list.front().node_type;
            my_list.pop_front();
        }
        else{
            node *temp             = new node;
            temp->a                = my_list.front().a;
            temp->right            = my_list.front().right;
            temp->left             = my_list.front().left;
            temp->num              = my_list.front().num;
            temp->node_type        = my_list.front().node_type;
            my_list.pop_front();
            h2tree.head->right     = temp;
            h2tree.head->num       = temp->num;
            h2tree.head->node_type = false;
        }
        //h2tree.transverse(h2tree.head);
    }

    /*
            main operation:encode
    */
    void encode()
    {
        get_info(false);
        if(filesz>0){
            statistic();//get the statistic of the charater
            char2tree();
            writeheader(h2tree.head);
        }
        flushheader();
        str2bin2file();
        endbits2file();
    };

    void decode()
    {
        get_info();
        if(filesz>0){
            header2tree();
            bin2str2file();
        }
    };

    void buildfailure(string query){
        failure = new int[query.length()];
        failure[0]=-1;
        for(unsigned int i=1; i<query.length();i++){
            std::string temp  = query.substr(0,i);
            int maxmatch = 0;
            for(unsigned int j=0;j<temp.length()-1;j++){
                if(temp.substr(0,j+1)
                    .compare(temp.substr(temp.length()-j-1,j+1))
                        ==0){
                    maxmatch=j+1;
                }
            }
            failure[i] = maxmatch;
        }
    }

    void releasefailure(){
        delete[] failure;
    }

    int my_getc(){
        if(cnt==1000000){
            cnt = 0;
        }
        if(cnt==0){
            fread(buffer,1,1000000,fi);
        }
        unsigned int j = buffer[cnt];
        int result     = j%256;
        cnt++;
        return result;
    }

    int KMP(string s){
        int found=0;
        int cur_index=0;//pointer of ther query string
        h2tree.go2head();
        fseek(fi,1024,SEEK_SET);
        buffer = (char*)new char[1000000];
        cnt=0;
        for(long long i=0;i<bitlong;i++){
            int lor = bitw.getbyte();
            int cur_a = -1;//current encount character
            if(lor ==-1){
                bitw.setbyte(my_getc());
                lor = bitw.getbyte();
            }
            if(h2tree.go(lor)){//if get a new character
                cur_a = h2tree.go2a();
                h2tree.go2head();
                if(cur_a==s[cur_index]){
                    cur_index++;
                    if(cur_index==s.length()){
                        found++;
                        cur_index = failure[cur_index-1];
                        while(cur_index!=-1 && s[cur_index]!=cur_a ){
                            cur_index = failure[cur_index];
                        }
                        cur_index++;
                    }
                }else{
                    cur_index = failure[cur_index];
                    while(cur_index!=-1 && s[cur_index]!=cur_a ){
                        cur_index = failure[cur_index];
                    }
                    cur_index++;
                }
            }
        }
        delete[] buffer;
        return found;
    }

    void search_word(string s)
    {
        get_info();
        header2tree();
        buildfailure(s);
        cout<<KMP(s)<<endl;
        releasefailure();

    };

};

int main(int argc, char* argv[])
{
    huffman huff;

    if(argc == 1)
    {
        return 1;
    }
    else if( strcmp(argv[1],"-e")==0 )
    {
        huff.setinputfile(argv[2]);
        huff.setoutputfile(argv[3]);
        huff.init();
        huff.encode();
        huff.closeinputfile();
        huff.closeoutputfile();
    }
    else if( strcmp(argv[1],"-d")==0 )
    {
        huff.setinputfile(argv[2]);
        huff.setoutputfile(argv[3]);
        huff.init();
        huff.decode();
        huff.closeinputfile();
        huff.closeoutputfile();
    }
    else if( strcmp(argv[1],"-s")==0 )
    {
        huff.setinputfile(argv[3]);
        huff.init();
        huff.search_word(argv[2]);
        huff.closeinputfile();
    }
    return 0;
}
