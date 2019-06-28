#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <string>
#include <queue>
#include <math.h>
#include <sys/time.h>
#include <vector>
#include <set>
#include <cfloat>
#include <utility>
#include <map>
#include <algorithm>
#include <boost/algorithm/string.hpp> 
#define MIN_SLACK 0
#define NUM_VT 3
#define PI_SLEW 1.312//1.772627//0.854977 //0.231266//0.066965//0.022726;//0.066965//0.121472
int parity_on=1;
//different ip slew to try for 0.022726,0.066965,0.121472,0.231266,0.451283,0.854976,1.772639
using namespace std;
    struct VertexProperty
{
    vector<int> successors;
    vector<int> predecessors;
    string type;
    double leakage[3];
    double capacitance;

    double delay[6];
    double arrival[2];
    double cumulativeslack;
    double slew[2];
    int level;
    int num_replacements;
    int parity;
    bool state;
   
};

bool mysortfunction(const pair<int,double> &i,const pair<int,double> &j)
{
    return i.second>j.second;
}
double interpolate(double xleft,double xright,double yleft,double yright,double xval)
{
    cout<<"Inside Interpolate "<<xleft<<" "<<xright<<" "<<yleft<<" "<<yright<<" "<<xval<<"---------"<<endl; 
    double y=yleft+(((xval-xleft)*(yright-yleft))/(xright-xleft));
    return y;
}
typedef boost::adjacency_list<boost::vecS,boost::vecS,boost::bidirectionalS,VertexProperty> Graph;
int gate_level(int element,Graph &g)
{
    cout<<"Printing the elements"<<element<<"Get type of element"<<g[element].type<<endl;
    vector<int>::iterator intit;
    int i,l,lmax=-1;
    int tempnode;

    if(g[element].level==-1)
    {
        lmax=-1;
        for(intit=g[element].predecessors.begin();intit<g[element].predecessors.end();intit++)
        {
            tempnode=*intit;
            l=gate_level(tempnode,g);
            if(l>lmax)
                lmax=l;
        }
        g[element].level=lmax+1;
        return(g[element].level);
    }
    else
        return (g[element].level);
}

void slew_calculation( vector<double> index_1,vector<double> index_2,map<string,vector<double> > &risetransitionmap,map<string,vector<double> > &falltransitionmap, int val,int levels,int size,Graph &g,int vt,map<string,string> gate_fp_map_lvt,map<string,string> gate_fp_map_hvt)
{

    int count=0;
    cout<<"iNSIDE sLEW Calculation"<<endl;
    vector<int>::iterator intit;
    g[size-1].slew[val]=0; // val 0 rise val 1 fall ? check.
    double slew=DBL_MIN;
    int tempnode;

       int x1,x2,x3,x4;
    double xleft,xright,yleft,yright,xval,minslew,maxslew;
    for (int i=0;i<=levels;i++)
    {
        cout<<"inside 1st for -- "<<i<<endl;
        for(int f=0;f<size;f++)
        {
            cout<<"inside 2nd for -- "<<f<<endl;
            count=0;
           if(g[f].level==i && i==levels)
            {
                        cout<<"when first g[f].level==i && i==levels"<<endl;
                        int parity=g[f].parity;
                        while(count<2)
                        {
                            cout<<"inside while count val -- "<<count<<endl;
                                     slew=DBL_MIN;
                                     for(intit=g[f].predecessors.begin();intit!=g[f].predecessors.end();intit++)
                            {
                                cout<<"inside the for "<<endl;
                                tempnode=*intit;
                                if(parity_on==1){
                                     if(g[tempnode].slew[val^parity]>slew)//removed ^parity
                                         slew=g[tempnode].slew[val^parity];//removed ^parity
    
                                } else{

                                      if(g[tempnode].slew[val]>slew)//removed ^parity
                                    slew=g[tempnode].slew[val];//removed ^parity  
                                    
                                }
                                
                                
                            }

                                cout<<"The slew value is ---- "<<slew<<endl;
                                g[f].slew[val]=slew;
                                val=~val;
                                count++;
                        }

                            //cout<<g[f].slew[val]<<endl;
                            cout<<"exited while count val -- "<<count<<endl;
                    
            }
            if(g[f].level==i && i==0)
            {
                            cout<<"when g[f].level==i && i==0"<<endl;    
                            g[f].slew[0]=PI_SLEW;
                            g[f].slew[1]=PI_SLEW;
                            //cout<<g[f].slew[val]<<endl;
                            
        
        
                        
            }

            cout<<"The slew value is ---- "<<slew<<endl;
            if(g[f].level==i && i!=levels && i >0 && g[f].type=="PO")
            {
                int parity=g[f].parity;
                cout<<"when g[f].level==i && i!=levels && i >0 && g[f].type==PO"<<endl;
                while(count<2)
                {
                            cout<<"inside while count val -- "<<count<<endl;
                            slew=DBL_MIN;

                            for(intit=g[f].predecessors.begin();intit!=g[f].predecessors.end();intit++)
                            {
                                cout<<"inside the for "<<endl;
                                tempnode=*intit;
                                //if(g[tempnode].slew[val^parity]>slew)
                                // if(g[tempnode].slew[val^parity]>slew)
                                //     slew=g[tempnode].slew[val^parity];//removed ^parity

                                if(parity_on==1){
                                     if(g[tempnode].slew[val^parity]>slew)//removed ^parity
                                         slew=g[tempnode].slew[val^parity];//removed ^parity
    
                                } else{

                                      if(g[tempnode].slew[val]>slew)//removed ^parity
                                    slew=g[tempnode].slew[val];//removed ^parity  
                                    
                                }
                            }
                                g[f].slew[val]=slew;

                            cout<<"g[f].slew[val]"<<g[f].slew[val]<<endl;
                            val=~val;
                            count++;
                }

                            
                    cout<<"exited while count val -- "<<count<<endl;
        
            }

            
            if(g[f].level==i && i>0 && i!=levels && g[f].type!="PO")
            {
                        slew=DBL_MIN;
                        int parity=g[f].parity;
                        cout<<"when g[f].level==i && i>0 && i!=levels && g[f].type!=PO"<<endl;
                                               
                           //cout<<"f:"<<f<<"TEMPVAL"<<tempval<<"VAL"<<val<<endl; 
                        while(count<2)
                        {
                            cout<<"inside while count val -- "<<count<<endl;
                            for(intit=g[f].predecessors.begin();intit<g[f].predecessors.end();intit++)
                            {
                                cout<<"inside the first for "<<endl;
                                tempnode=*intit;
                                if(parity_on==1){
                                     if(g[tempnode].slew[val^parity]>slew)//removed ^parity
                                         slew=g[tempnode].slew[val^parity];//removed ^parity
    
                                } else{

                                      if(g[tempnode].slew[val]>slew)//removed ^parity
                                    slew=g[tempnode].slew[val];//removed ^parity  
                                    
                                }
                            }
                            //cout<<"max slew here: "<<slew<<endl;
                            for (int t=0;t<index_1.size();t++)
                            {
                                cout<<"inside the second for "<<endl;
                                if(slew<index_1[i])
                                {
                                    x1=t-1;
                                    x2=t;
                                }
                                else if(t==index_1.size()-1)
                                {
                                    x1=t-1;
                                    x2=t;
                                }

                            }
                           cout<<"X1:"<<x1<<"X2:"<<x2<<endl;
                            for(int t=0;t<index_2.size();t++)
                            {
                                cout<<"inside the third for "<<endl;
                                 if(g[f].capacitance<index_2[t])
                                 {
                                     x3=t-1;
                                     x4=t;
                                 }
                                 else if(t==index_1.size()-1)
                                 {
                                     x3=t-1;
                                     x4=t;
                                 }

                            }
                           cout<<"X3:"<<x3<<"X4:"<<x4<<endl;
                            string searchby="";

                            if(vt == 2){
                                //assuming that the netlist was given in lvt
                                string temp=gate_fp_map_lvt[g[f].type];
                                string fm=temp.substr(0,temp.length()-3);
                                searchby=fm+"hvt";    
                            } else
                            //if(vt_type==1){
                                searchby=gate_fp_map_lvt[g[f].type];
                            //} else{}
                            if(val==0)
                            {
                                cout<<"when val==0"<<endl;
                                // cout<<g[f].type<<"-g[f].type "<<x1<<" -x1 "<<x2<<" -x2 "<<x3<<" -x3 "<<x4<<" -x4 "<<endl;
                                
                                


                                if(x1!=-1)
                                {
                                    xleft=index_2[x3];
                                    xright=index_2[x4];
                             //       cout<<"xleft:"<<xleft<<" xright: "<<xright<<"num elements: "<<risetransitionmap[g[f].type].size()<<endl;
                                    // rt_size=risetransitionmap.size();
                                    // for(int i=0,i<rt_size;i++)
                                    //     cout<<
                                    // string searchby="";
                                    // if(vt_type==1){
                                    //         searchby=gate_fp_map[g[f].type];



                                    // }    


                                    //yleft=risetransitionmap[g[f].type][6*x1+x3];
                                    yleft=risetransitionmap[searchby][6*x1+x3];
                                    //yright=risetransitionmap[g[f].type][6*x1+x4];   
                                    yright=risetransitionmap[searchby][6*x1+x4];   
                               //     cout<<"xleft:"<<xleft<<" xright: "<<xright<<" yleft: "<<yleft<<" yright: "<<yright<<endl;
                                }
                                else if(x1==-1)
                                {

                                    xleft=0;
                                    yleft=0;
                                    xright=index_2[x4];
                                    // string searchby="";
                                    // if(vt_type==1){
                                    //         searchby=gate_fp_map[g[f].type];

                                    // }
                                    // yright=risetransitionmap[g[f].type][x4];
                                    yright=risetransitionmap[searchby][x4];
                                }

                                cout<<"start"<<endl;
                                minslew=interpolate(xleft,xright,yleft,yright,(double) g[f].capacitance);
                                    xleft=index_2[x3];
                                    xright=index_2[x4];
                                    // yleft=risetransitionmap[g[f].type][6*x2+x3];
                                    yleft=risetransitionmap[searchby][6*x2+x3];
                                    // yright=risetransitionmap[g[f].type][6*x2+x4];   
                                    yright=risetransitionmap[searchby][6*x2+x4];   
                                    maxslew=interpolate(xleft,xright,yleft,yright,g[f].capacitance);
                                    
                                        g[f].slew[val]=interpolate(index_1[x1],index_1[x2],minslew,maxslew,slew);
                                        cout<<"SLEW:"<<g[f].slew[val]<<endl;
                                cout<<"end"<<endl;            
                            }

                            else if(val==1)
                            {
                                cout<<"when val==1"<<endl;
                                if(x1!=-1)
                                {
                                    xleft=index_2[x3];
                                    xright=index_2[x4];
                                   // cout<<"xleft:"<<xleft<<" xright: "<<xright<<"num elements: "<<falltransitionmap[g[f].type].size()<<endl;

                                    // yleft=falltransitionmap[g[f].type][6*x1+x3];
                                    yleft=falltransitionmap[searchby][6*x1+x3];
                                    // yright=falltransitionmap[g[f].type][6*x1+x4];   
                                    yright=falltransitionmap[searchby][6*x1+x4];   
                                    //cout<<"xleft:"<<xleft<<" xright: "<<xright<<" yleft: "<<yleft<<" yright: "<<yright<<endl;

                                }
                                else if(x1==-1)
                                {
                                    xleft=0;
                                    yleft=0;
                                    xright=index_2[x4];
                                    // yright=falltransitionmap[g[f].type][x4];
                                    yright=falltransitionmap[searchby][x4];
                                }

                                cout<<"start"<<endl;
                                minslew=interpolate(xleft,xright,yleft,yright,g[f].capacitance);
                                    xleft=index_2[x3];
                                    xright=index_2[x4];
                                    // yleft=falltransitionmap[g[f].type][6*x2+x3];
                                    yleft=falltransitionmap[searchby][6*x2+x3];
                                    // yright=falltransitionmap[g[f].type][6*x2+x4];   
                                    yright=falltransitionmap[searchby][6*x2+x4];   
                                    maxslew=interpolate(xleft,xright,yleft,yright,(double) g[f].capacitance);
                                    if(x1!=-1){
                                        cout<<" X1 not equals - 1"<<endl;
                                        g[f].slew[val]=interpolate(index_1[x1],index_1[x2],minslew,maxslew, (double) slew);
                                        cout<<"g[f].slew[val]"<<g[f].slew[val];
                                    }

                                cout<<"end"<<endl;    
                            }
                            val=~val;
                            // if val == 0 :
                            //     val = 1;
                            count++;
                        }

                        cout<<"outside while count val -- "<<count<<endl;

            }
            }
        cout<<"outside 2nd for -- "<<endl;
    } 

    cout<<"outside 1st for -- "<<endl;
}


void delay_calculation(vector<double> index_1,vector<double> index_2,map<string,vector<double> >risemap,map<string,vector<double> >fallmap, int tempval,int levels,int size,int position,Graph &g,int vt,map<string,string> gate_fp_map_lvt,map<string,string> gate_fp_map_hvt)
{
    cout<<"Inside delay calculation"<<endl;

       double tempdelay,mindelay,maxdelay,minslew,maxslew,slew;
       int x1,x2,x3,x4;
       double xleft,xright,yleft,yright,xval;
       for (int f=0;f<size-1;f++)
       {

            cout<<"Inside the first for - Delay"<<endl;
           if(g[f].type!="PI" && g[f].type!="PO")
           {

                cout<<"when g[f].type!=PI && g[f].type!=PO"<<endl;
                 for (int t=0;t<index_1.size();t++)
                            {
                                cout<<"Inside the second for 1 - Delay"<<endl;
                                if(g[f].slew[tempval]<index_1[t])
                                {
                                    x1=t-1;
                                    x2=t;
                                }
                                else if(t==index_1.size()-1)
                                {
                                    x1=t-1;
                                    x2=t;
                                }

                            }
                    cout<<"X1: "<<x1<<" X2: "<<x2<<endl;
                    for(int t=0;t<index_2.size();t++)
                             {
                                cout<<"Inside the second for 2 - Delay"<<endl;
                                 if(g[f].capacitance<index_2[t])
                                 {
                                     x3=t-1;
                                     x4=t;
                                 }
                                 else if(t==index_1.size()-1)
                                 {
                                     x3=t-1;
                                     x4=t;
                                 }

                             }
                    cout<<"X3: "<<x3<<" X4: "<<x4<<endl;

                    string searchby="";
                            //if(vt_type==1){
                    cout<<g[f].type<<endl;
                    // 
                    // string searchby="";

                    if(vt == 2){
                                //assuming that the netlist was given in lvt
                        string temp=gate_fp_map_lvt[g[f].type];
                        string fm=temp.substr(0,temp.length()-3);
                        searchby=fm+"hvt";    
                    } else
                            //if(vt_type==1){
                        searchby=gate_fp_map_lvt[g[f].type];




                    if(tempval==0)
                    {
                        cout<<"Inside tempval==0"<<endl;
                        cout<<searchby<<"-g[f].type "<<x1<<" -x1 "<<x2<<" -x2 "<<x3<<" -x3 "<<x4<<" -x4 "<<endl;
                        // cout<<risemap["OR2KHD"][1]<<endl;
                        //map  xcd       
                                if(x1!=-1)
                                {
                                    xleft=index_2[x3];
                                    xright=index_2[x4];
                                //    cout<<"XLEFT: "<<xleft<<"XRIGHT: "<<xright<<endl;
                                    // cout<<risemap[g[f].type]<<endl;
                                    // yleft=risemap[g[f].type][6*x1+x3];
                                    yleft=risemap[searchby][6*x1+x3];
                                    // yright=risemap[g[f].type][6*x1+x4];   
                                    yright=risemap[searchby][6*x1+x4];   
                                //    cout<<"YLEFT: "<<yleft<<"YRIGHT: "<<yright<<endl;
                                }
                                else if(x1==-1)
                                {
                                    xleft=0;
                                    yleft=0;
                                    xright=index_2[x4];
                                    // yright=risemap[g[f].type][x4];
                                    yright=risemap[searchby][x4];

                                }
                                mindelay=interpolate(xleft,xright,yleft,yright,g[f].capacitance);
                                    xleft=index_2[x3];
                                    xright=index_2[x4];
                                    
                                    // yleft=risemap[g[f].type][6*x2+x3];
                                    yleft=risemap[searchby][6*x2+x3];
                                    // yright=risemap[g[f].type][6*x2+x4];   
                                    yright=risemap[searchby][6*x2+x4];   
                                    maxdelay=interpolate(xleft,xright,yleft,yright,g[f].capacitance);
                                  //  cout<<"MINDELAY :"<<mindelay<<" MAXDELAY "<<maxdelay<<"NUM ELTS: "<<risemap[g[f].type].size()<<endl;
                                    //cout<<g[f].slew[tempval]<<endl;
                                    if(x1!=-1)
                                        g[f].delay[position]=interpolate(index_1[x1],index_1[x2],mindelay,maxdelay,g[f].slew[tempval]);
                                    
                                    else
                                        g[f].delay[position]=interpolate(0,index_1[x2],mindelay,maxdelay,g[f].slew[tempval]);
                                //cout<<g[f].delay[0]<<endl;
                    }
                    else if(tempval==1)
                    {
                        cout<<"Inside tempval==1"<<endl;
                                if(x1!=-1)
                                {
                                    xleft=index_2[x3];
                                    xright=index_2[x4];
                                    // yleft=fallmap[g[f].type][6*x1+x3];
                                    yleft=fallmap[searchby][6*x1+x3];
                                    // yright=fallmap[g[f].type][6*x1+x4];   
                                    yright=fallmap[searchby][6*x1+x4];   
                                }
                                else if(x1==-1)
                                {
                                    xleft=0;
                                    yleft=0;
                                    xright=index_2[x4];
                                    // yright=fallmap[g[f].type][x4];
                                    yright=fallmap[searchby][x4];
                                }
                                mindelay=interpolate(xleft,xright,yleft,yright,g[f].capacitance);
                                    xleft=index_2[x3];
                                    xright=index_2[x4];
                                    // yleft=fallmap[g[f].type][6*x2+x3];
                                    yleft=fallmap[searchby][6*x2+x3];
                                    // yright=fallmap[g[f].type][6*x2+x4];   
                                    yright=fallmap[searchby][6*x2+x4];   
                                    maxdelay=interpolate(xleft,xright,yleft,yright,g[f].capacitance);
                                    if(x1!=-1)
                                        g[f].delay[position]=interpolate(index_1[x1],index_1[x2],mindelay,maxdelay,g[f].slew[tempval]);
                                    else
                                        g[f].delay[position]=interpolate(0,index_1[x2],mindelay,maxdelay,g[f].slew[tempval]);
                    }



               
        }
       }

}
    

int main(int argc,char *argv[])
{

    string line;
    int num,num1;
    if(argc!=15)
       // cout<<"usage:./a.out hgrfile typefile lvtrise lvtfall lvtrisetransition lvtfalltransition rvtrise rvtfall rvtrisetransition rvtfalltransition   hvtrise hvtfall hvtrisetransition hvtfalltransition capacitance parityfile "<<endl;
        cout<<"usage:./a.out hgrfile typefile lvt_gate_fp_map hvt_gate_fp_map lvtrise lvtfall lvtrisetransition lvtfalltransition hvtrise hvtfall hvtrisetransition hvtfalltransition capacitance parityfile"<<endl; 
    
    else
    {
        Graph g; 


        ifstream hgrfile(argv[1]);
        if(hgrfile.is_open())
        {
            getline(hgrfile,line);
            cout<<line<<endl;
            std::istringstream ss(line);
            ss >> num1;
            num1++;
            cout<<"total number of nodes in graph="<<num1<<endl;
            cout<<"created graph"<<endl;
            int count1=0;
            while(hgrfile.good())
            {
               // string line;
                getline(hgrfile,line);
                cout<<line<<endl;
                if(!line.empty())
                {
                    istringstream ss(line);
                    while(ss >> num)
                    {
                        boost::add_edge(count1,num,g);
            //            cout<<num<<"\t";
                    }
                }
                count1++;
//                cout<<count1<<endl;
            }
        }
            num=num1-1;
            typedef boost::graph_traits<Graph>::vertex_iterator VItr;
            VItr vitr,vend;
            boost::tie(vitr,vend)=boost::vertices(g);
            ifstream propertyfile(argv[2]);
            cout<<"reading types"<<endl;
            for(;vitr!=vend;++vitr) // we assign the gates their types pi po or cell
            {
                if(propertyfile.is_open())
                    getline(propertyfile,line);
                g[*vitr].type=line;
                cout<<*vitr<<" "<<g[*vitr].type<<endl;
            }
            boost::tie(vitr,vend)=boost::vertices(g);
            int j,k;
            boost::tie(vitr,vend)=boost::vertices(g);
            VItr vitr1,vend1;
            boost::tie(vitr1,vend1)=boost::vertices(g);
            cout<<num<<endl;
            cout<<"identifying fanins/ fanouts"<<endl;
            for(int i=0;i<num;i++)
            {
                cout<<"Fan in i started - "<<i<<endl;
                vector<int>successors;
                for(int j=0;j<num;j++)
                    if(boost::edge(i,j,g).second)
                        successors.push_back(j);
                g[i].successors=successors;
                cout<<"Fan in i done - "<<i<<endl;
            }
            cout<<"fanins done"<<endl;
            for(int i=0;i<num;i++)
            {
                cout<<"Fan out i started - "<<i<<endl;
                vector<int>predecessors;
                for(int j=0;j<num;j++)
                    if(boost::edge(j,i,g).second )
                        predecessors.push_back(j);
                g[i].predecessors=predecessors;
                cout<<"Fan out i done - "<<i<<endl;
            }
            cout<<"fanouts done"<<endl;

            //new gate mapping code inserted by PBR on 22nd June 2019
            map<string,string > lvt_gate_fp_map;
            map<string,string > lvt_fp_gate_map;
            ifstream lvtmapfile(argv[3]);
            // while(lvtmapfile.good())
            //     {

                    while(getline(lvtmapfile,line)){    
                    vector<string> result; 
                    boost::split(result, line, boost::is_any_of(":"));
                    // std::string s1;
                    string s1 = result[1];
                    int len=s1.length();
                    string s=s1.substr(0,len);
                    // std::replace( s.begin(), s.end(), '\n', ' ');
                    cout<<s<<endl;
                    cout<<"before inserting"<<endl;   
                    lvt_gate_fp_map.insert(std::pair<string,string>(result[0],s)); 
                    lvt_gate_fp_map.insert(std::pair<string,string>(s,result[0])); 
                    cout<<"after inserting"<<endl;       
                }

            map<string,string > hvt_gate_fp_map;
            map<string,string > hvt_fp_gate_map;
            ifstream hvtmapfile(argv[4]);
            //while(hvtmapfile.good())
                //{
                    

                    while(getline(hvtmapfile,line)){    
                    cout<<"In hvt map file"<<endl;
                    vector<string> result; 
                    boost::split(result, line, boost::is_any_of(":"));
                    // std::string s1;
                    string s1 = result[1];
                    int len=s1.length();
                    string s=s1.substr(0,len-2);
                    //std::replace( s.begin(), s.end(), '\n', '');

                    hvt_gate_fp_map.insert(pair<string,string>(result[0],s));
                    hvt_gate_fp_map.insert(pair<string,string>(s,result[0])); 

                }    

            ////////////////////////////////////////////////////////////////////////////    
            int pi;
            map<string,vector<double> > lvt_risemap;
            map<string,vector<double> > lvt_fallmap;
            //map<string,vector<double>> rvt_risemap;
            //map<string,vector<double>> rvt_fallmap;
            map<string,vector<double> > hvt_risemap;
            map<string,vector<double> > hvt_fallmap;
            map<string,double> capacitancemap;
            map<string,vector<double> > lvt_risetransitionmap;
            map<string,vector<double> > lvt_falltransitionmap;
            //map<string,vector<double>> rvt_risetransitionmap;
            //map<string,vector<double>> rvt_falltransitionmap;
            map<string,vector<double> > hvt_risetransitionmap;
            map<string,vector<double> > hvt_falltransitionmap;
            string tempword;
            double tempnum;

            // additions made on 15-08-2013
            vector<int>::iterator vecit;
            int linecount=0;
            ifstream risefile(argv[5]);
                while(risefile.good())
                {
                    if(linecount==0)
                    {
                        linecount++;
                        getline(risefile,line);
                        stringstream ss(line);
                        ss>>tempword;

                    }
                    else if(linecount<=7)
                    {
                        linecount++;
                    
                        getline(risefile,line);
                        stringstream ss(line);

                        while(ss>>tempnum)
                            lvt_risemap[tempword].push_back(tempnum);
                        if(linecount==8)
                            linecount=0;
                   //     cout <<line<<endl;
                    }
//                    linecount++;
                   // cout<<linecount<<endl;
                        
                }
            
            linecount=0;
            ifstream fallfile(argv[6]);
                while(fallfile.good())
                {
                   // cout<<"file opened"<<endl;
                    if(linecount==0)
                    {
                        linecount++;
                        getline(fallfile,line);
                        stringstream ss(line);
                        ss>>tempword;
                     //   cout<<tempword<<endl;
                    }
                    else if(linecount<=7)
                    {

                        linecount++;
                        getline(fallfile,line);
                        stringstream ss(line);

                        while(ss>>tempnum)
                            lvt_fallmap[tempword].push_back(tempnum);
                        if(linecount==8)
                            linecount=0;
                   //     cout <<line<<endl;
                    }
                    
                  //  cout<<linecount<<endl;
                        
                }            
                linecount=0;
            ifstream risetransitionfile(argv[7]);
                while(risetransitionfile.good())
                {

                    if(linecount==0)
                    {
                        linecount++;
                        getline(risetransitionfile,line);
                        stringstream ss(line);
                        ss>>tempword;
                        cout<<tempword<<endl;

                    }
                    else if(linecount<=7)
                    {
                        linecount++; 
                        getline(risetransitionfile,line);
                       stringstream ss(line);

                        while(ss>>tempnum)
                            lvt_risetransitionmap[tempword].push_back(tempnum);
                        if(linecount==8)
                            linecount=0;
                    }
                 
                }
                        
                 linecount=0;
            ifstream falltransitionfile(argv[8]);
                while(falltransitionfile.good())
                {
                    if(linecount==0)
                    {
                        linecount++;

                        getline(falltransitionfile,line);
                        stringstream ss(line);
                        ss>>tempword;
                        cout<<tempword<<endl;
                    }
                    else if(linecount<=7)
                    {
                        linecount++;

                    
                        getline(falltransitionfile,line);
                        stringstream ss(line);

                        while(ss>>tempnum)
                            lvt_falltransitionmap[tempword].push_back(tempnum);
                        if(linecount==8)
                            linecount=0;
                        cout <<line<<endl;
                    }
//                    linecount++;
                 
                }           
                cout<<"lvt done"<<endl;
        /*    linecount=0;
            risefile.close();
            fallfile.close();
            risetransitionfile.close();
            falltransitionfile.close();
             risefile.open(argv[7]);
                while(risefile.good())
                {
                    if(linecount==0)
                    {
                        linecount++;
                        getline(risefile,line);
                        stringstream ss(line);
                        ss>>tempword;

                    }
                    else if(linecount<=7)
                    {
                        linecount++;
                    
                        getline(risefile,line);
                        stringstream ss(line);

                        while(ss>>tempnum)
                           rvt_risemap[tempword].push_back(tempnum);
                        if(linecount==8)
                            linecount=0;
                   //     cout <<line<<endl;
                    }
//                    linecount++;
                   // cout<<linecount<<endl;
                        
                }
            

            linecount=0;
            fallfile.open(argv[8]);
                while(fallfile.good())
                {
                   // cout<<"file opened"<<endl;
                    if(linecount==0)
                    {
                        linecount++;
                        getline(fallfile,line);
                        stringstream ss(line);
                        ss>>tempword;
                     //   cout<<tempword<<endl;
                    }
                    else if(linecount<=7)
                    {

                        linecount++;
                        getline(fallfile,line);
                        stringstream ss(line);

                        while(ss>>tempnum)
                            rvt_fallmap[tempword].push_back(tempnum);
                        if(linecount==8)
                            linecount=0;
                   //     cout <<line<<endl;
                    }
                    
                  //  cout<<linecount<<endl;
                        
                }            
                linecount=0;
            risetransitionfile.open(argv[9]);
                while(risetransitionfile.good())
                {

                    if(linecount==0)
                    {
                        linecount++;
                        getline(risetransitionfile,line);
                        stringstream ss(line);
                        ss>>tempword;
                     //   cout<<tempword<<endl;

                    }
                    else if(linecount<=7)
                    {
                        linecount++; 
                        getline(risetransitionfile,line);
                       stringstream ss(line);

                        while(ss>>tempnum)
                            rvt_risetransitionmap[tempword].push_back(tempnum);
                        if(linecount==8)
                            linecount=0;
                    }
                 
                }
                        
                 linecount=0;
            falltransitionfile.open(argv[10]);
                while(falltransitionfile.good())
                {
                    if(linecount==0)
                    {
                        linecount++;

                        getline(falltransitionfile,line);
                        stringstream ss(line);
                        ss>>tempword;
                       // cout<<tempword<<endl;
                    }
                    else if(linecount<=7)
                    {
                        linecount++;

                    
                        getline(falltransitionfile,line);
                        stringstream ss(line);

                        while(ss>>tempnum)
                            rvt_falltransitionmap[tempword].push_back(tempnum);
                        if(linecount==8)
                            linecount=0;
                   //     cout <<line<<endl;
                    }
//                    linecount++;
                 
                }*/           
            linecount=0;
            risefile.close();
            fallfile.close();
            risetransitionfile.close();
            falltransitionfile.close();
                // cout<<"rvt done"<<endl;

            cout<<"starting with the hvt files"<<endl;    
            risefile.open(argv[9]);
                while(risefile.good())
                {
                    if(linecount==0)
                    {
                        linecount++;
                        getline(risefile,line);
                        stringstream ss(line);
                        ss>>tempword;

                    }
                    else if(linecount<=7)
                    {
                        linecount++;
                    
                        getline(risefile,line);
                        stringstream ss(line);

                        while(ss>>tempnum)
                            hvt_risemap[tempword].push_back(tempnum);
                        if(linecount==8)
                            linecount=0;
                       cout <<line<<endl;
                    }
//                    linecount++;
                    cout<<linecount<<endl;
                        
                }
            
            linecount=0;
            fallfile.open(argv[10]);
                while(fallfile.good())
                {
                   // cout<<"file opened"<<endl;
                    if(linecount==0)
                    {
                        linecount++;
                        getline(fallfile,line);
                        stringstream ss(line);
                        ss>>tempword;
                        cout<<tempword<<endl;
                    }
                    else if(linecount<=7)
                    {

                        linecount++;
                        getline(fallfile,line);
                        stringstream ss(line);

                        while(ss>>tempnum)
                            hvt_fallmap[tempword].push_back(tempnum);
                        if(linecount==8)
                            linecount=0;
                        cout <<line<<endl;
                    }
                    
                    cout<<linecount<<endl;
                        
                }            
                linecount=0;
            risetransitionfile.open(argv[11]);
                while(risetransitionfile.good())
                {

                    if(linecount==0)
                    {
                        linecount++;
                        getline(risetransitionfile,line);
                        stringstream ss(line);
                        ss>>tempword;
                        cout<<tempword<<endl;

                    }
                    else if(linecount<=7)
                    {
                        linecount++; 
                        getline(risetransitionfile,line);
                       stringstream ss(line);

                        while(ss>>tempnum)
                        {
                            hvt_risetransitionmap[tempword].push_back(tempnum);
                            cout<<tempnum<<"\t";
                        }
                        if(linecount==8)
                            linecount=0;
                        cout<<"\n";
                    }
                 
                }
                        
                 linecount=0;
            falltransitionfile.open(argv[12]);
                while(falltransitionfile.good())
                {
                    if(linecount==0)
                    {
                        linecount++;

                        getline(falltransitionfile,line);
                        stringstream ss(line);
                        ss>>tempword;
                        cout<<tempword<<endl;
                    }
                    else if(linecount<=7)
                    {
                        linecount++;

                    
                        getline(falltransitionfile,line);
                        stringstream ss(line);

                        while(ss>>tempnum)
                            hvt_falltransitionmap[tempword].push_back(tempnum);
                        if(linecount==8)
                            linecount=0;
                        cout <<line<<endl;
                    }
//                    linecount++;
                 
                }           
            cout<<"hvt done"<<endl;

            ifstream capfile(argv[13]);
            while(capfile.good())
            {
                getline(capfile,line);
                stringstream ss(line);
                ss>>tempword;
                while(ss>>tempnum)
                    capacitancemap[tempword]=tempnum;
            }


            cout<<"cap done"<<endl;
            // Reading gate parity added on 12-09-2013
            map<string,int> paritymap;
            ifstream parityfile(argv[14]);
            while(parityfile.good())
            {
                 getline(parityfile,line);
                 stringstream ss(line);
                 ss >> tempword;
                 while(ss>>tempnum)
                     paritymap[tempword]=tempnum;
            }

            //assigning parity and capacitance to each vertex
            // for(int c=0)
           for (int f=0;f<num1-1;f++)
            {

                    if(g[f].type!="PI" && g[f].type!="PO"){
                       string temp= g[f].type;
                       string searchby=lvt_gate_fp_map[temp];
                       g[f].capacitance= capacitancemap[searchby];
                       g[f].parity= paritymap[searchby];
                    }
            } 

            int tempnode;
        vector<int> povector;
        vector<int>::iterator intit1;
        cout<<"num1 is"<<num1<<endl;
            int picount=0,pocount=0;        
          for(int i=0;i<num;i++)
          {
            if(g[i].type=="PO")
            {
                cout<<"FOR PO "<<i<<" :fanins are"<<"\t";
                povector.push_back(i);
                for(intit1=g[i].predecessors.begin();intit1<g[i].predecessors.end();intit1++)
                {
                    cout<<*intit1<<"\t";
                    
                }
                cout<<"fanout size "<<g[i].successors.size();
                cout<<"\n";
                pocount++;
            }
            else if(g[i].type=="PI")
                picount++;

        }
        for (int i=0;i<=num;i++)
            g[i].level=-1;
        int tempo,templevel=-1;
        for(int i=0;i<povector.size();i++)
        {
            tempo=povector[i];
            g[tempo].level=gate_level(tempo,g);
            if(g[tempo].level>templevel)
                templevel=g[tempo].level;
            

        }
         cout<<"max level is "<<templevel;      
         for(int i=0;i<num1;i++)
             cout<<i<<" "<<g[i].level<<"\n";
// slew calculation
    //vector<double> index_1{0.0,1.0,2.0,4.0,8.0,16.0,32.0}; // initialize with index_1 values
    //vector<double> index_2{5.00,30.00,50.00,80.0,140.0,200.0,300.00,500.00 };//initialize with load_capacitance_values
   // vector<double> rvt_index_1 {0.0,1.0,2.0,4.0,8.0,16.0,32.0};// initialize with index_1 values 
    vector<double> lvt_index_fall_1{0.019562,0.060972,0.110998,0.211567,0.413195,0.783246,1.624531};
    vector<double> lvt_index_2{0.001500,0.003306,0.007287,0.016062,0.035404,0.078035,0.172000};


    vector<double> lvt_index_rise_1{0.022742,0.066977,0.121474,0.231219,0.451314,0.854802,1.772129};
    //vector<double> hvt_index_rise_2{0.001500,0.003306,0.007287,0.016062,0.035404,0.078035,0.172000};

    vector<double> hvt_index_rise_1{0.017584,0.040787,0.071822,0.133905,0.258152,0.630810,1.562352};
    //vector<double> lvt_index_rise_2{0.002500,0.006232,0.015536,0.038730,0.096549,0.240685,0.600000};
    vector<double> hvt_index_2{0.002500,0.006232,0.015536,0.038730,0.096549,0.240685,0.600000};

    vector<double> hvt_index_fall_1{0.021475,0.050215,0.088438,0.164972,0.317854,0.775861,1.921861};
    //vector<double> lvt_index_fall_2{0.002500,0.006232,0.015536,0.038730,0.096549,0.240685,0.600000};

        /*    slew_calculation(rvt_index_1,index_2,rvt_risetransitionmap,rvt_falltransitionmap,0, templevel,num1,g);
//            slew_calculation(rvt_index_1,index_2,rvt_risetransitionmap,rvt_falltransitionmap,1, templevel,num1,g);

            delay_calculation(rvt_index_1,index_2,rvt_risemap,rvt_fallmap,0,templevel,num1,2,g);
            cout<<"rvt rise delay\n";
            delay_calculation(rvt_index_1,index_2,rvt_risemap,rvt_fallmap,1,templevel,num1,3,g);
            cout<<"rvt fall delay\n";*/

            // map<string,vector<double> >::iterator itr;
            // for (itr=hvt_risemap.begin();itr!=hvt_risemap.end();++itr){
            //     cout<<itr->first<<":"<<endl;
            //     std::vector<double>::iterator it;
            //     for(it=itr->second.begin();it!=itr->second.end();++it){
            //         cout<<*it<<" ,";
            //     }

            //     cout<<endl;
            // }

            cout<<"Hitting first lvt slew calculation"<<endl;
            slew_calculation(lvt_index_fall_1,lvt_index_2,lvt_risetransitionmap,lvt_falltransitionmap,0, templevel,num1,g,1,lvt_gate_fp_map,hvt_gate_fp_map);
  //          slew_calculation(lvt_index_1,index_2,lvt_risetransitionmap,lvt_falltransitionmap,1, templevel,num1,g);
            cout<<"Hitting first delay calculation"<<endl;    
            delay_calculation(lvt_index_fall_1,lvt_index_2,lvt_risemap,lvt_fallmap,0,templevel,num1,0,g,1,lvt_gate_fp_map,hvt_gate_fp_map);
            cout<<"lvt rise delay\n";
            delay_calculation(lvt_index_fall_1,lvt_index_2,lvt_risemap,lvt_fallmap,1,templevel,num1,1,g,1,lvt_gate_fp_map,hvt_gate_fp_map);
            cout<<"lvt fall delay\n";
            slew_calculation(hvt_index_fall_1,hvt_index_2,hvt_risetransitionmap,hvt_falltransitionmap,0, templevel,num1,g,2,lvt_gate_fp_map,hvt_gate_fp_map);
    //        slew_calculation(hvt_index_1,index_2,hvt_risetransitionmap,hvt_falltransitionmap,1, templevel,num1,g);

            delay_calculation(hvt_index_fall_1,hvt_index_2,hvt_risemap,hvt_fallmap,0,templevel,num1,2,g,2,lvt_gate_fp_map,hvt_gate_fp_map);
            cout<<"hvt rise delay\n";
            delay_calculation(hvt_index_fall_1,hvt_index_2,hvt_risemap,hvt_fallmap,1,templevel,num1,3,g,2,lvt_gate_fp_map,hvt_gate_fp_map);
            cout<<"hvt fall delay\n";


        string filename=argv[1];
        ofstream myfile;
         string::size_type pos=filename.find(".");
    if(pos !=string::npos){
        filename.replace(pos,4,".rise_131");
    }
        myfile.open(filename.c_str());

        int size=num1-1;
        for(int i=0;i<size;i++)
        {
            myfile<<g[i].delay[0]<<"\t"<<g[i].delay[2]<<"\n";//<<g[i].delay[4]<<"\n";
            // myfile<<g[i].delay[0]<<"\t"<<g[i].delay[2]<<"\t"<<g[i].delay[4]<<"\n";
           
        
        }
        myfile.close();

        pos=filename.find(".");
        if(pos!=string::npos){
            filename.replace(pos,5,".fall_131");
        }
        myfile.open(filename.c_str());
        for(int i=0;i<size;i++)
        {
            myfile<<g[i].delay[1]<<"\t"<<g[i].delay[3]<<"\n";//<<g[i].delay[5]<<"\n";
            // myfile<<g[i].delay[1]<<"\t"<<g[i].delay[3]<<"\t"<<g[i].delay[5]<<"\n";
           
        
        }
        myfile.close();






    }//closing braces for argument else

    return 0;
}//closing braces for int main()

