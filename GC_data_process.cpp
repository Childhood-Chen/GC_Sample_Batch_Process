#include <io.h>
#include <fstream>
#include <string.h>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <sstream>
#include <stdio.h>
#include <cmath>

using namespace std;

struct peak
{
    string RetentionTime="";
    string Area="";
    string BeginTime="";
    string EndTime="";
    int sampleid=0;
};

vector<peak> Column1;
vector<peak> Column2;
int sampleid = 0;

void GetAllFormatFiles( string path,vector<string>& files,string format)//get all formatting file from folder
{
    long hFile = 0;
    struct _finddata_t fileinfo;
    string p;
    if((hFile = _findfirst(p.assign(path).append("\\*" + format).c_str(),&fileinfo)) != -1)
    {
        do
        {
            if((fileinfo.attrib &  _A_SUBDIR))
            {
                if(strcmp(fileinfo.name,".") != 0  &&  strcmp(fileinfo.name,"..") != 0)
                {
                    GetAllFormatFiles( p.assign(path).append("\\").append(fileinfo.name),files,format);
                }
            }
            else
            {
                files.push_back(p.assign(path).append("\\").append(fileinfo.name));
            }
        }
        while(_findnext(hFile, &fileinfo)  == 0);
        _findclose(hFile);
    }
}

string Process(string str,char s)//delete all tab from string
{
    for (;;)
    {
        int l = str.length()-1;
        str=str.substr(1,l);
        if(str[0]!=s)
            break;
    }
    return str;
}

bool cmp(peak t1,peak t2)
{
    if(abs(strtod(t1.RetentionTime.c_str(),NULL)-strtod(t2.RetentionTime.c_str(),NULL))>0.002)
        return t1.RetentionTime<t2.RetentionTime;
    else
        return t1.sampleid<t2.sampleid;
}

string GetNumber(string str) // Get Number from string
{
    int s=0,e=1;
    for(; str[s]!='\"'; s++)
        e++;
    for(; str[e]!='\"'; e++);
    if(str[s+2]=='.'&e>s+6)
    {
        return str.substr(s+1,6);
    }
    else
    {
        return str.substr(s+1,e-s-1);
    }
}

void ReadDocument(string path)
{
    ifstream infile;
    string str1;
    infile.open(path.c_str());
    getline(infile,str1);//unknown bug
    getline(infile,str1);
    int col = 0;
    while(infile.get()!=EOF)//EOF check
    {
        getline(infile,str1);
        char s = str1[0];
        str1=Process(str1,s);
        if (str1.substr(0,13)== "<SignalResult")
        {
            if(col==0)//The first column
            {
                sampleid++;
                for(;;)
                {
                    getline(infile,str1);
                    str1=Process(str1,s);
                    //cout<<str1<<endl;
                    if (str1.substr(0,5)== "<Peak")
                    {
                        //cout<<str1<<endl;
                        peak temp;
                        for(;;)
                        {
                            if(str1.substr(0,7)=="<Retent")
                                temp.RetentionTime=GetNumber(str1);
                            if(str1.substr(0,7)=="<Area v")
                                temp.Area=GetNumber(str1);
                            if(str1.substr(0,7)=="<BeginT")
                                temp.BeginTime=GetNumber(str1);
                            if(str1.substr(0,7)=="<EndTim")
                                temp.EndTime=GetNumber(str1);
                            if(str1.substr(0,6)=="</Peak")
                                break;
                            getline(infile,str1);
                            str1=Process(str1,s);
                        }
                        //cout<<"peak"<<endl;
                        temp.sampleid=sampleid;
                        Column1.push_back(temp);
                        //cout<<temp.RetentionTime<<"!!"<<temp.Area<<endl;
                    }

                    if(str1.substr(0,14)=="</SignalResult")
                        break;
                }
                col++;
            }
            else
            {
                for(;;)
                {
                    getline(infile,str1);
                    str1=Process(str1,s);
                    //cout<<str1<<endl;
                    if (str1.substr(0,5)== "<Peak")
                    {
                        peak t;
                        //cout<<str1<<endl;
                        for(;;)
                        {
                            //cout<<str1<<endl;
                            if(str1.substr(0,7)=="<Retent")
                                t.RetentionTime=GetNumber(str1);
                            if(str1.substr(0,7)=="<Area v")
                                t.Area=GetNumber(str1);
                            if(str1.substr(0,7)=="<BeginT")
                                t.BeginTime=GetNumber(str1);
                            if(str1.substr(0,7)=="<EndTim")
                                t.EndTime=GetNumber(str1);
                            if(str1.substr(0,6)=="</Peak")
                                break;
                            getline(infile,str1);
                            str1=Process(str1,s);
                        }
                        //cout<<"peak"<<endl;
                        t.sampleid=sampleid;
                        Column2.push_back(t);
                    }

                    if(str1.substr(0,14)=="</SignalResult")
                        break;
                }
            }
        }
    }
    infile.close();
    //cout<<Column1.size()<<' '<<Column2.size()<<endl;
}


int main()
{
    for(;;)
    {
        vector<peak>().swap(Column1);
        vector<peak>().swap(Column2);
        sampleid=0;
        string filepath;
        cout << "Please input file path. Input Q to exit.\n";
        getline(cin,filepath);
        if (filepath=="Q"|filepath=="q")
            break;
        vector<string> files;
        string distAll = filepath+"\\Data.txt";
        string format = ".acam_";
        GetAllFormatFiles(filepath,files,format);
        ofstream ofn(distAll.c_str());
        //int size = files.size();
        //ofn<<size<<endl;
        for (int i=0; i<files.size(); i++)
        {
            ReadDocument(files[i]);
            //ofn<<files[i]<<endl;
            cout<< files[i] << endl;
            //cout<<Column1.size()<<' '<<Column2.size()<<endl;
        }
        //end of read
        sort(Column1.begin(),Column1.end(),cmp);
        sort(Column2.begin(),Column2.end(),cmp);
        //sort
        ofn<<"Column1, MS5A"<<endl;
        ofn<<"Ret_Time"<<"  "<<"Area"<<"  "<<"Begin"<<"  "<<"End"<<"   "<<"ID"<<endl;
        for (int i=0; i<Column1.size(); i++)
        {
            ofn<<Column1[i].RetentionTime<<"   "<<Column1[i].Area<<"   "<<Column1[i].BeginTime<<"   "<<Column1[i].EndTime<<"   "<<Column1[i].sampleid<<endl;
        }
        ofn<<endl<<"Column2, PPU"<<endl;
        ofn<<"Ret_Time"<<"  "<<"Area"<<"  "<<"Begin"<<"  "<<"End"<<"   "<<"ID"<<endl;
        for (int i=0; i<Column2.size(); i++)
        {
            ofn<<Column2[i].RetentionTime<<"   "<<Column2[i].Area<<"   "<<Column2[i].BeginTime<<"   "<<Column2[i].EndTime<<"   "<<Column2[i].sampleid<<endl;
        }
        ofn.close();
    }

    return 0;
}
