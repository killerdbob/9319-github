#include <iostream>
#include <cstring>
#include <fstream>
#include <cstdio>
#include <string>
#include <unistd.h>
#include <sstream>
#include <vector>

using namespace std;

class BWT
{
public:
    char delimiter;
    string tmpfile;
    FILE *fi;
    FILE *fo;
    FILE *findex;
    string content;
    string bwt_content;
    int content_length;
    int character_number[128];
    int character_index[128];
    int delimeter_length;

    BWT()
    {};

    ~BWT()
    {};

    void setdelimiter(char a)
    {
        delimiter=a;
    }

    void open_indexfile(string a)
    {
        findex=fopen(string(a+".hw").c_str(),"wb+");
    }

    void close_indexfile()
    {
        fclose(findex);
    }

    void open_inputfile(char *input)
    {
        fi=fopen(input,"rb");
    }

    void close_inputfile()
    {
        fclose(fi);
    }

    void open_outputfile(char *output)
    {
        fo=fopen(output,"ab+");
    }

    void close_outputfile()
    {
        fclose(fo);
    }

    void input2content(){
        char *buffer;
        while(!feof(fi))
        {
            buffer = (char*)new char[100000];
            fread(buffer,1,100000,fi);
            content+=string(buffer);
            delete []buffer;
        }
    }

    int get_inputfilelength(){
        fseek(fi,0,SEEK_END);
        content_length=ftell(fi);
        fseek(fi,0,SEEK_SET);
        return content_length;
    }

    void int2str(const int &i_tmp,string &s_tmp){
        stringstream s;
        s<<i_tmp;
        s_tmp=s.str();
    }

    void clean_tmpbuckets(){
        for(int i=0; i<128; i++)
        {
            string a;
            int2str(i,a);
            string tmpbuckets=string(tmpfile)+'/'+a;
            if(access(tmpbuckets.c_str(),F_OK)==0)
            {
                remove(tmpbuckets.c_str());
            }
        }
    }

    void SuffixArray(){
        for(int i=0; i<128; i++)
        {
            character_number[i]=0;
            character_index[i]=0;
        }
        for(int i=0; i<content_length; i++)
        {
            int ind=int(content[i]);
            character_number[ind]++;
        }
        for(int i=0; i<128; i++)
        {
            if(character_number[i]>0)
            {
                int *perbucket;
                perbucket = new int[character_number[i]];
                for(int j=0; j<content_length; j++){
                    if(int(content[j])==i){
                        perbucket[character_index[i]]=j;
                        character_index[i]++;
                    }
                }
                string a;
                int2str(i,a);
                bucket_sort(perbucket,character_number[i],0);
                write2output(perbucket,character_number[i]);
                delete[] perbucket;
            }
        }
        fwrite(bwt_content.c_str(),sizeof(char),content_length,fo);
    }

    void bucket_sort(int *bk, int bk_length, int level){
        int *bk_number;
        int *bk_ind;
        int *bk_copy;
        bk_number = new int[128]();
        bk_ind = new int[128]();
        bk_copy = new int[bk_length]();
        int accum=0;
        for(int i=0; i<bk_length; i++)
        {
            if(bk[i]+level >= content_length )
            {
                bk_copy[0]=bk[i];
                accum++;
            }
            else
            {
                int cur_a=int(content[bk[i]+level]);
                bk_number[cur_a]++;
            }
        }
        for(int i=0; i<128; i++)
        {
            if(bk_number[i]>0)
            {
                bk_ind[i]=accum;
                accum+=bk_number[i];
            }
        }
        for(int i=0; i<bk_length; i++)
        {
            if(bk[i]+level < content_length )
            {
                int cur_a=int(content[bk[i]+level]);
                int ind=bk_ind[cur_a];
                bk_copy[ind]=bk[i];
                bk_ind[cur_a]++;
            }
        }
        for(int i=0; i<bk_length; i++)
        {
            bk[i]=bk_copy[i];
        }
        delete[] bk_copy;
        for(int i=1; i<128; i++)
        {
            if(bk_number[i]>1 && i!=delimiter)
            {
                int ind=bk_ind[i]-bk_number[i];
                bucket_sort(bk+ind,bk_number[i],level+1);
            }
        }
        delete[] bk_ind;
        delete[] bk_number;
    }

    void write2output(int *suffix,int length){
        for(int i=0; i<length; i++)
        {
            int bwt=(suffix[i]==0)?content_length-1:suffix[i]-1;
            char a=content[bwt];
            bwt_content+=a;
        }
        if(content[suffix[0]]==delimiter){
            delimeter_length=length;
        }
    }

    void stage1_release_memory(){
        content=string("");
    }

////////////////////////////////stage 2///////////////////////////////////

    int **checkpoint;
    int checkpoint_number;
    int checkpoint_length;
    int position[128][2];

    void setchkpntlength(int i){
        checkpoint_length=i;
    }

    void open_stage2_input(string a){
        fi=fopen(a.c_str(),"rb+");
    }

    void close_stage2_input(){
        fclose(fi);
    }

    void stage2_release_memory(){
        for(int i=0;i<checkpoint_number;i++){
            delete[] checkpoint[i];
        }
    }

    void First_array(){
        int checkpoint_index=0;
        checkpoint_number = (content_length/checkpoint_length)+1;
        checkpoint = new int* [checkpoint_number];
        for(int i=0;i<checkpoint_number;i++){
            checkpoint[i]=new int[128];
        }

        for(int i=0; i<128; i++)
        {
            position[i][0]=0;
            position[i][1]=0;
        }
        for(unsigned int i=0; i<bwt_content.length(); i++)
        {
            position[int(bwt_content[i])][0]++;
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
        position[127][1]=content_length+1;
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
            int number_of_rest=i-(i/checkpoint_length)*checkpoint_length;
            int start=(i/checkpoint_length)*checkpoint_length;
            for(int j=start;j<start+number_of_rest;j++){
                if(bwt_content[j]==c){
                    result++;
                }
            }
        }
        return result;
    }

    int write2index(){
        int *buffer=new int[delimeter_length];
        fwrite(buffer,sizeof(int),delimeter_length,findex);
        delete[] buffer;


        for(int i=0;i<delimeter_length;i++){
            int delimeter_index = i;
            char c=bwt_content[position[delimiter][0]+delimeter_index];
            int c_position=position[delimiter][0]+delimeter_index;
            while(c!=delimiter){
                c_position = position[c][0]+Occurance(c,c_position);
                c=bwt_content[c_position];
            }
            int *buffer=new int;
            *buffer=i;
            int O_c=Occurance(delimiter,c_position);
            fseek(findex,O_c*sizeof(int),SEEK_SET);
            fwrite(buffer,sizeof(int),1,findex);
            delete buffer;
        }
    }
};

int main(int argc, char* argv[])
{

    static BWT bwt;
    if(argc != 5)
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
        bwt.open_inputfile(argv[3]);
        bwt.open_outputfile(argv[4]);
        bwt.input2content();
        bwt.get_inputfilelength();
        bwt.SuffixArray();
        bwt.close_inputfile();
        bwt.close_outputfile();

        bwt.open_indexfile(argv[4]);
        bwt.open_stage2_input(argv[4]);
        bwt.stage1_release_memory();
        bwt.setchkpntlength(500);
        bwt.First_array();
        bwt.write2index();
        bwt.stage2_release_memory();
        bwt.close_stage2_input();
        bwt.close_indexfile();
    }
}
