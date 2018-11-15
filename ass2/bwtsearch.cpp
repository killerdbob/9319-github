#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>
#include <list>
#include <set>
#include <stdlib.h>

using namespace std;
class BWTsearch
{
public:
    char delimiter;
    FILE *fi;
    FILE *findex;
    int position[128][2];
    string tmpfile;
    string action;
    string search_string;
    int filesize;

    BWTsearch() {};
    ~BWTsearch() {};

    void setdelimiter(char a)
    {
        delimiter=a;
    }


    void open_inputfile(char *input)
    {
        fi=fopen(input,"rb");
        fseek(fi,0,SEEK_END);
        filesize=ftell(fi);
        fseek(fi,0,SEEK_SET);
    }

    void close_inputfile()
    {
        fclose(fi);
    }

    int delimeter_length;
    void open_indexfile(string a)
    {
        int indexfilesz;
        findex=fopen(string(a+".hw").c_str(),"rb");
        fseek(findex,0,SEEK_END);
        indexfilesz=ftell(findex);
        fseek(findex,0,SEEK_SET);
        delimeter_length = indexfilesz/sizeof(int);
    }

    void close_indexfile()
    {
        fclose(findex);
    }

    void setaction(string a){
        action=a;
    }

    void setsearch(string a){
        search_string=a;
    }

    int **checkpoint;
    int checkpoint_number;
    int checkpoint_length;

    void setchkpntlength(int i){
        checkpoint_length=i;
    }

    void release_memory(){
        for(int i=0;i<checkpoint_number;i++){
            delete[] checkpoint[i];
        }
    }

    void First_array(){
        char *buffer;

        int checkpoint_index=0;
        checkpoint_number = (filesize/checkpoint_length)+1;
        checkpoint = new int* [checkpoint_number];
        for(int i=0;i<checkpoint_number;i++){
            checkpoint[i]=new int[128];
        }

        for(int i=0; i<128; i++)
        {
            position[i][0]=0;
            position[i][1]=0;
        }

        for(unsigned int i=0; i<filesize; i++)
        {
            char a=getc(fi);
            position[int(a)][0]++;
            if((i+1)%checkpoint_length==0){
                for(int j=0;j<128;j++){
                    checkpoint[checkpoint_index][j]=position[j][0];
                }
                checkpoint_index++;
            }
        }
        for(int j=0;j<128;j++){
            checkpoint[checkpoint_index][j]=position[j][0];
        }

        int tmp=0;
        for(int i=0; i<128; i++)
        {
            int t=position[i][0];
            position[i][0]=tmp;
            tmp+=t;
        }
        int flag=position[127][0];
        position[127][1]=filesize+1;
        for(int i=126; 0<=i; i--)
        {
            if(flag!=position[i][0]){
                position[i][1]=position[i+1][0];
                flag=position[i][0];
            }else{
                position[i][1]=position[i+1][1];
            }
        }
    }

    int Occurance(char c,int i){
        int result;
        if(i/checkpoint_length>0){
            result = checkpoint[(i/checkpoint_length) - 1][c];
        }else{
            result=0;
        }
        if(i%checkpoint_length!=0){
            fseek(fi,(i/checkpoint_length)*checkpoint_length,SEEK_SET);
            int number_of_rest=i-(i/checkpoint_length)*checkpoint_length;
            char *next_chars=new char[number_of_rest];
            fread(next_chars,sizeof(char),number_of_rest,fi);
            for(int j=0;j<number_of_rest;j++){
                if(next_chars[j]==c){
                    result++;
                }
            }
            delete[] next_chars;
        }
        return result;
    }

    void a_search(){
        set<int> result;
        int c_index = search_string.length()-1;
        char c_0    = search_string[c_index--];
        int First   = position[c_0][0];
        int Last    = position[c_0][1];
        while((Last>First) && c_index >= 0){
            c_0     = search_string[c_index--];
            First = position[c_0][0] + Occurance(c_0,First);
            Last  = position[c_0][0] + Occurance(c_0,Last);
        }

        if(Last<=First){
            return;
        }
        char *c = new char[2];
        c[1]='\0';
        int *delimeter_index = new int;
        for(int i=First;i<Last;i++){
            fseek(fi,i,SEEK_SET);
            fread(c,sizeof(char),1,fi);
            int c_position=i;
            while(c[0]!=delimiter){
                c_position=position[c[0]][0]+Occurance(c[0],c_position);
                fseek(fi,c_position,SEEK_SET);
                fread(c,sizeof(char),1,fi);
            }

            int O_c=Occurance(delimiter,c_position);
            fseek(findex,O_c*sizeof(int),SEEK_SET);
            fread(delimeter_index,sizeof(int),1,findex);
            result.insert(*delimeter_index+1);
        }
        delete delimeter_index;
        delete[] c;
        for(set<int>::iterator it=result.begin();it!=result.end();it++){
            cout<<*it<<endl;
        }
    }

    void n_search(){
        set<int> result;
        string res;
        int c_index = search_string.length()-1;
        char c_0    = search_string[c_index--];
        int First   = position[c_0][0];
        int Last    = position[c_0][1];
        while((Last>First) && c_index >= 0){
            c_0     = search_string[c_index--];
            First = position[c_0][0] + Occurance(c_0,First);
            Last  = position[c_0][0] + Occurance(c_0,Last);
        }

        if(Last<=First){
            return;
        }

        char *c = new char[2];
        c[1]='\0';
        for(int i=First;i<Last;i++){
            res=search_string;
            fseek(fi,i,SEEK_SET);
            fread(c,sizeof(char),1,fi);
            int c_position=i;
            while(c[0]!=delimiter){
                c_position=position[c[0]][0]+Occurance(c[0],c_position);
                res = string(c)+res;
                fseek(fi,c_position,SEEK_SET);
                fread(c,sizeof(char),1,fi);
            }
            //cout<<"---"<<res<<endl;
            result.insert(Occurance(delimiter,c_position));
        }
        delete[] c;
        cout<<result.size()<<endl;
    }

    void m_search(){
        int c_index = search_string.length()-1;
        int c       = search_string[c_index--];
        int First   = position[c][0];
        int Last    = position[c][1];
        while((Last > First) && (c_index >= 0)){
            c     = search_string[c_index--];
            First = position[c][0] + Occurance(c,First);
            Last  = position[c][0] + Occurance(c,Last);
        }
        cout<<Last-First<<endl;
    }

    void i_search(){
        size_t space_p = search_string.find(" ");
        string First_str  = search_string.substr(0,space_p);
        string Last_str   = search_string.substr(space_p+1);
        int First = atoi( First_str.c_str());
        int Last = atoi( Last_str.c_str());
        string result;
        char *c=new char[2];
        c[1]='\0';
        for(int i=First-1;i<Last;i++){
            result="";
            int delimeter_index = i;

            fseek(fi,position[delimiter][0]+(delimeter_index),SEEK_SET);
            fread(c,sizeof(char),1,fi);
            int c_position=position[delimiter][0]+(delimeter_index);
            while(c[0]!=delimiter){
                c_position = position[c[0]][0]+Occurance(c[0],c_position);
                result = string(c)+result;
                fseek(fi,c_position,SEEK_SET);
                fread(c,sizeof(char),1,fi);
            }
            cout<<result<<endl;
        }
        delete c;
    }

};


int main(int argc, char* argv[])
{
    static BWTsearch bwt;
    if(argc != 6)
    {
        return 1;
    }
    else
    {
        string action=string(argv[1]);

        if(action.length()==2 && action[0]=='\\' && action[1]=='n'){
            bwt.setdelimiter(char(10));
        }else{
            bwt.setdelimiter(argv[1][0]);
        }
        bwt.open_inputfile(argv[2]);
        bwt.open_indexfile(argv[2]);
        bwt.setaction(argv[4]);
        bwt.setsearch(argv[5]);
        bwt.setchkpntlength(8000);
        bwt.First_array();

        //cout<<bwt.Occurance('r',23)<<endl;
        if(argv[4][1]=='a'){//output unique sorted record
            bwt.a_search();
        }else if(argv[4][1]=='m'){//output total number of substring contains the argv[5]
            bwt.m_search();
        }else if(argv[4][1]=='n'){//output not duplicate total number of
            bwt.n_search();
        }else if(argv[4][1]=='i'){//output all the record between
            bwt.i_search();
        }
        bwt.release_memory();
        bwt.close_indexfile();
        bwt.close_inputfile();
    }
}













