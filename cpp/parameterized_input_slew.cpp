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
#define PI_SLEW_R 0.8//0.231266
#define PI_SLEW_F 0.8//0.06695
                         //0.066965//1.312//1.772627//0.854977 //0.231266//0.066965//0.022726;//0.066965//0.121472
#define NET_DELAY_HVT 0.02
#define NET_DELAY_LVT 0.0125//0.001594//0.001594
int parity_on=1;
float pi_slew_r;
float pi_slew_f;

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

// void slew_calculation( vector<double> index_1,vector<double> index_2,map<string,vector<double> > &risetransitionmap,map<string,vector<double> > &falltransitionmap, int val,int levels,int size,Graph &g,int vt,map<string,string> gate_fp_map_lvt,map<string,string> gate_fp_map_hvt)
void slew_calculation( map<string,vector<float>> &inputslew_r,map<string,vector<float>> &loadCapacitance_r,map<string,vector<float>> &inputslew_f,map<string,vector<float>> &loadCapacitance_f,map<string,vector<double> > &risetransitionmap,map<string,vector<double> > &falltransitionmap, int val,int levels,int size,Graph &g,int vt,map<string,string> gate_fp_map_lvt,map<string,string> gate_fp_map_hvt)
{

    int count=0;
    //cout<<"iNSIDE sLEW Calculation"<<endl;
    vector<int>::iterator intit;
    g[size-1].slew[val]=0; // val 0 rise val 1 fall ? check.
    double slew=DBL_MIN;
    int tempnode;

       int x1,x2,x3,x4;//x1,x2 are for scanning the cap, x1,x2 for ip slew
    double xleft,xright,yleft,yright,xval,minslew,maxslew;
    for (int i=0;i<=levels;i++)
    {
        cout<<"inside 1st for -- "<<i<<endl;
        for(int f=0;f<size;f++)
        {
             
            cout<<"inside 2nd for -- "<<f<<endl;

             
            cout<<"for Node - "<<f<<" of type - "<<g[f].type<<endl;

            count=0;
           if(g[f].level==i && i==levels)//the code for nodes having max levels starts here
            {       
                         
                        cout<<"when first g[f].level==i && i==levels"<<endl;

                         
                        cout<<"for Node - "<<f<<" of type - "<<g[f].type<<endl;
                        int parity=g[f].parity;
                        while(count<2)
                        {
                             
                            cout<<"inside while count val -- "<<count<<endl;
                                     slew=DBL_MIN;
                            for(intit=g[f].predecessors.begin();intit!=g[f].predecessors.end();intit++)
                            {
                                
                                 
                                cout<<"inside the for "<<endl;
                                tempnode=*intit;
                                
                                 
                                cout<<"TX: g[tempnode].slew[val^parity] "<<g[tempnode].slew[val^parity]<<"parity "<<parity<<endl;
                                 
                                cout<<"val : "<<val<<"slew :"<<slew<<endl;
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
                                // val=~val;
                                if(val ==0)
                                val=1;
                                else if(val == 1)
                                val =0;

                                count++;
                        }

                            //cout<<g[f].slew[val]<<endl;
                             
                            cout<<"exited while count val -- "<<count<<endl;
                    
            }//the code for nodes having max levels ends here



            if(g[f].level==i && i==0)//The code for nodes having level == 0 starts here
            {                
                            cout<<"when g[f].level==i && i==0"<<endl;    

                             
                            cout<<"for Node - "<<f<<" of type - "<<g[f].type<<endl;
                            //g[f].slew[0]=PI_SLEW_R;
                            g[f].slew[0]=pi_slew_r;
                            g[f].slew[1]=pi_slew_f;
                            //g[f].slew[1]=PI_SLEW_F;

                             
                            cout<<"<<g[f].slew[0] :"<<g[f].slew[0]<<"<<g[f].slew[1] :"<<g[f].slew[1]<<endl;
                            
        
        
                        
            }//The code for nodes having level == 0 ends here

            cout<<"The slew value is ---- "<<slew<<endl;
            if(g[f].level==i && i!=levels && i >0 && g[f].type=="PO")// The code for nodes that are POs but not on the last levels starts here
            {

                int parity=g[f].parity;
                 
                cout<<"when g[f].level==i && i!=levels && i >0 && g[f].type==PO"<<endl;
                
                 
                cout<<"for Node - "<<f<<" of type - "<<g[f].type<<endl;    

                 
                cout<<" parity :"<<parity<<" g[f].level :"<<g[f].level<<endl;
                

                while(count<2)//this part stays safe 
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

                                 
                                cout<<"TX: g[tempnode].slew[val^parity] "<<g[tempnode].slew[val^parity]<<"parity "<<parity<<endl;
                                 
                                cout<<"val : "<<val<<"slew :"<<slew<<endl;
                            
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
                            // val=~val;
                            if(val ==0)
                                val=1;
                            else if(val == 1)
                                val =0;
                            count++;
                }

                                  
                    cout<<"exited while count val -- "<<count<<endl;
        
            }//// The code for nodes that are POs but not on the last levels ends here

            


            //The code for all the nodes that lies between o and mal_level and are not a PO starts here
            if(g[f].level==i && i>0 && i!=levels && g[f].type!="PO")
            {

                        int parity=g[f].parity;
                         
                        cout<<"when g[f].level==i && i>0 && i!=levels && g[f].type!=PO"<<endl;
                                
                         
                        cout<<"for Node - "<<f<<" of type - "<<g[f].type<<endl;               
                           //cout<<"f:"<<f<<"TEMPVAL"<<tempval<<"VAL"<<val<<endl; 
                        while(count<2)
                        {
                        slew=DBL_MIN;
                             
                            cout<<"inside while count val -- "<<count<<endl;
                            for(intit=g[f].predecessors.begin();intit<g[f].predecessors.end();intit++)//this part stays safe
                            {
                                 
                                cout<<"inside the first for "<<endl;
                                // cout<<"TX: g[tempnode].slew[val^parity] "<<g[tempnode].slew[val^parity]<<"parity "<<parity<<endl;
                                 
                                cout<<"val : "<<val<<"slew :"<<slew<<endl;

                                tempnode=*intit;
                                if(parity_on==1){
                                     
                                    cout<<"predecessors Node Type "<<g[tempnode].type<<"Present Node type :"<<g[f].type<<endl;
                                     
                                    cout<<"TX: g[tempnode].slew[val^parity] "<<g[tempnode].slew[val^parity]<<"parity "<<parity<<endl;
                                     if(g[tempnode].slew[val^parity]>slew)//removed ^parity
                                         slew=g[tempnode].slew[val^parity];//removed ^parity
            
                                } else{

                                      if(g[tempnode].slew[val]>slew)//removed ^parity
                                    slew=g[tempnode].slew[val];//removed ^parity  
                                    
                                }
                            }
                            //cout<<"max slew here: "<<slew<<endl;
                            

                            if(val==0)
                            {
                                string searchby="";
                                if(vt == 2){
                                //assuming that the netlist was given in lvt
                                    string temp=gate_fp_map_lvt[g[f].type];
                                    string fm=temp.substr(0,temp.length()-3);
                                    searchby=fm+"hvt";    
                                } else
                            //if(vt_type==1){
                                searchby=gate_fp_map_lvt[g[f].type];

                                vector<float> index_1=inputslew_r[searchby];
                                for (int t=0;t<index_1.size();t++)
                                {
                                     
                                    cout<<"for Node - "<<f<<" of type - "<<g[f].type<<endl;
                                     
                                    cout<<"inside the second for - is slew<index_1[i]"<<" slew "<<slew<<" index_1[i] "<<index_1[t]<<endl;
                                    //if(slew<index_1[t])- previously this was i
                                    if(t==index_1.size()-1)
                                    {
                                            x1=t-1;
                                            x2=t;
                                         break;
                                    }


                                if(slew>index_1[t])
                                {
                                    x1=t-1;
                                    x2=t;
                                    continue;
                                } else {
                                    
                                    if(t!=0){
                                        x1=t-1;
                                        x2=t;
                                        
                                    } else{
                                        x1=-1;
                                        x2=0;
                                    }
                                    
                                    break;
                                }   

                                // else if(t==index_1.size()-1)
                                // {
                                //     x1=t-1;
                                //     x2=t;
                                // }

                            }

                             
                            cout<<"for Node - "<<f<<" of type - "<<g[f].type<<endl;

                             
                           cout<<"X1:"<<x1<<"X2:"<<x2<<endl;
                           vector<float> index_2=loadCapacitance_r[searchby];
                           
                            for(int t=0;t<index_2.size();t++)
                            {

                                 
                                cout<<"for Node - "<<f<<" of type - "<<g[f].type<<endl;
                                 
                                cout<<"inside the third for - g[f].capacitance : "<<g[f].capacitance<<"index_2[t] : "<<index_2[t]<<endl;
                                 if(t==index_2.size()-1)
                                 {
                                     x3=t-1;
                                     x4=t;
                                 }

                                 if(g[f].capacitance>index_2[t])
                                 {
                                     x3=t-1;
                                     x4=t;
                                     continue;
                                 }
                                 else{

                                    if(t!=0){
                                        x3=t-1;
                                        x4=t;
                                        
                                    } else{
                                        x3=0;
                                        x4=1;
                                    }
                                    
                                    break;

                                 } 

                            }

                            
                           cout<<"X3:"<<x3<<"X4:"<<x4<<endl;
                            

                            
                            //} else{}
                                                            
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


                                    //yleft=risetransitionmap[g[f].type][7*x1+x3];
                                    yleft=risetransitionmap[searchby][7*x1+x3];
                                    
                                    cout<<" X1: "<<x1<<" X4: "<<x4<<" X3: "<<x3;    
                                     
                                    cout<<"risetransitionmap[searchby][7*x1+x3] "<<risetransitionmap[searchby][7*x1+x3]<<"risetransitionmap[searchby][7*x1+x4] :"<<risetransitionmap[searchby][7*x1+x4]<<endl;
                                    //yright=risetransitionmap[g[f].type][7*x1+x4];   
                                    yright=risetransitionmap[searchby][7*x1+x4];   
                               //     cout<<"xleft:"<<xleft<<" xright: "<<xright<<" yleft: "<<yleft<<" yright: "<<yright<<endl;
                                }
                                else if(x1==-1)
                                {
                                    x1=0;
                                    x2=1;

                                    xleft=0;
                                    yleft=0;
                                    xright=index_2[x4];
                                    // string searchby="";
                                    // if(vt_type==1){
                                    //         searchby=gate_fp_map[g[f].type];

                                    // }
                                    // yright=risetransitionmap[g[f].type][x4];

                                     
                                    cout<<"for Node - "<<f<<" of type - "<<g[f].type<<endl;

                                     
                                    cout<<" X1: "<<x1<<"X2: "<<x2<<" X4: "<<x4<<" X3: "<<x3;    
                                    // cout<<"risetransitionmap[searchby][7*x1+x3] "<<risetransitionmap[searchby][7*x1+x3]<<"risetransitionmap[searchby][7*x1+x4] :"<<risetransitionmap[searchby][7*x1+x4]<<endl;
                                     
                                    cout<<"xleft "<<xleft<<"yleft "<<yleft<<" xright "<<xright<<" yright"<<yright<<endl;
                                    
                                    yright=risetransitionmap[searchby][x4];
                                     
                                    cout<<searchby<<" :searchby "<<yright<<" :yright"<<endl;
                                }

                                 
                                cout<<"for Node - "<<f<<" of type - "<<g[f].type<<endl;

                                 
                                cout<<"start"<<endl;
                                  
                                cout<<"xleft "<<xleft<<"yleft "<<yleft<<" xright "<<xright<<" yright"<<yright<<endl;
                                minslew=interpolate(xleft,xright,yleft,yright,g[f].capacitance);
                                
                                 
                                cout<<"MIN SLEW : "<<minslew<<endl;
                                xleft=index_2[x3];
                                xright=index_2[x4];
                                 
                                cout<<" X1: "<<x1<<"X2: "<<x2<<" X4: "<<x4<<" X3: "<<x3;
                             
                                cout<<"risetransitionmap[searchby][7*x2+x3] "<<risetransitionmap[searchby][7*x2+x3]<<"risetransitionmap[searchby][7*x2+x4] :"<<risetransitionmap[searchby][7*x2+x4]<<endl;

                                    // yleft=risetransitionmap[g[f].type][7*x2+x3];
                             
                                cout<< searchby <<" :searchby"<<7*x2+x3<<"7*x2+x3"<<endl;
                                double old_y_left=yleft;
                                yleft=risetransitionmap[searchby][7*x2+x3];
                                 
                                cout<<"old yleft | new yleft "<<old_y_left<<" | "<<yleft<<endl;

                                    // yright=risetransitionmap[g[f].type][7*x2+x4];   
                                double old_y_right= yright;
                                yright=risetransitionmap[searchby][7*x2+x4];  
                                 
                                cout<<"old yright | new yright "<<old_y_right<<" | "<<yright<<endl;
                                maxslew=interpolate(xleft,xright,yleft,yright,g[f].capacitance);
                                 
                                cout<<"xleft "<<xleft<<"yleft "<<yleft<<" xright "<<xright<<" yright"<<yright<<"g[f].capacitance :"<<g[f].capacitance<<endl;
                                
                                cout<<"MAX SLEW : "<<maxslew<<endl;
                                if(x1==-1){
                                     x1=0;
                                     x2=1; 
                                
                                    g[f].slew[val]=interpolate(index_1[x1],index_1[x2],minslew,maxslew,slew);
                                } else {
                                    g[f].slew[val]=interpolate(index_1[x1],index_1[x2],minslew,maxslew,slew);
                                }
                                 
                                cout<<"SLEW: when val 0"<<g[f].slew[val]<<endl;
                                 
                                cout<<"end"<<endl;            
                            }else if(val==1){


                                                                string searchby="";
                                if(vt == 2){
                                //assuming that the netlist was given in lvt
                                    string temp=gate_fp_map_lvt[g[f].type];
                                    string fm=temp.substr(0,temp.length()-3);
                                    searchby=fm+"hvt";    
                                } else
                            //if(vt_type==1){
                                searchby=gate_fp_map_lvt[g[f].type];

                                vector<float> index_1=inputslew_f[searchby];
                                for (int t=0;t<index_1.size();t++)
                                {
                                     
                                    cout<<"inside the second for - is slew<index_1[i]"<<" slew "<<slew<<" index_1[i] "<<index_1[t]<<endl;
                                    //if(slew<index_1[t])- previously this was i
                                    if(t==index_1.size()-1)
                                    {
                                            x1=t-1;
                                            x2=t;
                                         break;
                                    }


                                if(slew>index_1[t])
                                {
                                    x1=t-1;
                                    x2=t;
                                    continue;
                                } else {
                                    
                                    if(t!=0){
                                        x1=t-1;
                                        x2=t;
                                        
                                    } else{
                                        x1=-1;
                                        x2=0;
                                    }
                                    
                                    break;
                                }   

                                // else if(t==index_1.size()-1)
                                // {
                                //     x1=t-1;
                                //     x2=t;
                                // }

                            }

                             
                            cout<<"for Node - "<<f<<" of type - "<<g[f].type<<endl;
                             
                           cout<<"X1:"<<x1<<"X2:"<<x2<<endl;
                           vector<float> index_2=loadCapacitance_f[searchby];
                           
                            for(int t=0;t<index_2.size();t++)
                            {    
                                cout<<"inside the third for - g[f].capacitance : "<<g[f].capacitance<<"index_2[t] : "<<index_2[t]<<endl;
                                 if(t==index_2.size()-1)
                                 {
                                     x3=t-1;
                                     x4=t;
                                 }

                                 if(g[f].capacitance>index_2[t])
                                 {
                                     x3=t-1;
                                     x4=t;
                                     continue;
                                 }
                                 else{

                                    if(t!=0){
                                        x3=t-1;
                                        x4=t;
                                        
                                    } else{
                                        x3=0;
                                        x4=1;
                                    }
                                    
                                    break;

                                 } 

                            }
                             
                           cout<<"X3:"<<x3<<"X4:"<<x4<<endl;
                            



                                 
                                cout<<"when val==1"<<endl;
                                if(x1!=-1)
                                {
                                     
                                    cout<<"when x1 != -1 x3 "<<x3<<" x4 "<<x4<<endl;
                                    xleft=index_2[x3];
                                    xright=index_2[x4];
                                     
                                   cout<<"xleft:"<<xleft<<" xright: "<<xright<<endl;//"num elements: "<<falltransitionmap[g[f].type].size()<<endl;

                                    // yleft=falltransitionmap[g[f].type][7*x1+x3];
                                     
                                    cout<<" X1: "<<x1<<"X2: "<<x2<<" X4: "<<x4<<" X3: "<<x3;
                                     
                                    cout<<"falltransitionmap[searchby][7*x1+x3]; "<<falltransitionmap[searchby][7*x1+x3]<<"falltransitionmap[searchby][7*x1+x4]; :"<<falltransitionmap[searchby][7*x1+x4]<<endl;
                                    yleft=falltransitionmap[searchby][7*x1+x3];
                                    // yright=falltransitionmap[g[f].type][7*x1+x4];   
                                    yright=falltransitionmap[searchby][7*x1+x4];   
                                     
                                    cout<<"xleft:"<<xleft<<" xright: "<<xright<<" yleft: "<<yleft<<" yright: "<<yright<<endl;

                                }
                                else if(x1==-1)
                                {
                                    xleft=0;
                                    yleft=0;
                                    xright=index_2[x4];
                                    // yright=falltransitionmap[g[f].type][x4];
                                    yright=falltransitionmap[searchby][x4];
                                    x1=0;
                                    x2=1;
                                }

                                 
                                cout<<"start"<<endl;
                                minslew=interpolate(xleft,xright,yleft,yright,g[f].capacitance);
                                    xleft=index_2[x3];
                                    xright=index_2[x4];
                                    // yleft=falltransitionmap[g[f].type][7*x2+x3];
                                    yleft=falltransitionmap[searchby][7*x2+x3];
                                    // yright=falltransitionmap[g[f].type][7*x2+x4];   
                                    yright=falltransitionmap[searchby][7*x2+x4];   
                                    maxslew=interpolate(xleft,xright,yleft,yright,(double) g[f].capacitance);
                                    if(x1!=-1){
                                         
                                        cout<<"for Node - "<<f<<" of type - "<<g[f].type<<endl;

                                         
                                        cout<<" X1 not equals - 1"<<endl;
                                         
                                        cout<<"index_1[x1] :"<< index_1[x1]<<" index_1[x2] :"<<index_1[x2]<<"MIN SLEW "<<minslew<<" max slew"<<maxslew<<" slew "<<slew<<endl;
                                        g[f].slew[val]=interpolate(index_1[x1],index_1[x2],minslew,maxslew, (double) slew);
                                         
                                        cout<<"SLEW when val 1 :"<<g[f].slew[val];
                                    }
                                         
                                cout<<"end"<<endl;    
                            }
                            // val=~val;
                            if (val == 0)
                                 val = 1;
                            else if(val == 1)
                                 val = 0;  
                            count++;
                        }
                         
                        cout<<"outside while count val -- "<<count<<endl;
                         
                        cout<<f<<": f "<<g[f].slew[0]<<" g[f].slew[0] "<<g[f].slew[1]<<" g[f].slew[1] "<<endl;
            }
            }
        cout<<"outside 2nd for -- "<<endl;
    } 

    cout<<"outside 1st for -- "<<endl;
}

void delay_calculation(map<string,vector<float>> &inputslew_r,map<string,vector<float>> &loadCapacitance_r,map<string,vector<float>> &inputslew_f,map<string,vector<float>> &loadCapacitance_f,map<string,vector<double> >risemap,map<string,vector<double> >fallmap, int tempval,int levels,int size,int position,Graph &g,int vt,map<string,string> gate_fp_map_lvt,map<string,string> gate_fp_map_hvt)
{
    cout<<"Inside delay calculation"<<endl;

       double tempdelay,mindelay,maxdelay,minslew,maxslew,slew;
       int x1,x2,x3,x4;
       double xleft,xright,yleft,yright,xval;
       for (int f=0;f<size-1;f++)
       {
             
            cout<<"Inside the first for - Delay - "<<f<<endl;
           if(g[f].type!="PI" && g[f].type!="PO")
           {

             
            cout<<"for Node - "<<f<<" of type - "<<g[f].type<<endl;

            if(tempval==0)
            {
                string searchby="";
                if(vt == 2){
                                //assuming that the netlist was given in lvt
                    string temp=gate_fp_map_lvt[g[f].type];
                    string fm=temp.substr(0,temp.length()-3);
                    searchby=fm+"hvt";    
                } else
                            //if(vt_type==1){
                    searchby=gate_fp_map_lvt[g[f].type];

                vector<float> index_1=inputslew_r[searchby];  
                   
                cout<<"when g[f].type!=PI && g[f].type!=PO"<<endl;
                 for (int t=0;t<index_1.size();t++)
                 {
                     
                        cout<<"Inside the second for 1 - Delay"<<endl;
                                // if(g[f].slew[tempval]<index_1[t])
                                // {
                                //     x1=t-1;
                                //     x2=t;
                                // }
                                // else if(t==index_1.size()-1)
                                // {
                                //     x1=t-1;
                                //     x2=t;
                                // }

                        if(t==index_1.size()-1)
                        {
                            x1=t-1;
                            x2=t;
                            break;
                        }


                        if(g[f].slew[tempval]>index_1[t])
                        {
                            x1=t-1;
                            x2=t;
                            continue;
                        } else {
                                    
                            if(t!=0){
                                x1=t-1;
                                x2=t;
                                        
                            } else{
                                x1=-1;
                                x2=0;
                            }
                                    
                            break;
                        }

                }//this is where the for index scanning ends 

                 
            cout<<"for Node - "<<f<<" of type - "<<g[f].type<<endl;
                 
                cout<<"X1: "<<x1<<" X2: "<<x2<<endl;

                vector<float> index_2=loadCapacitance_r[searchby];
                for(int t=0;t<index_2.size();t++)
                {
                     
                    cout<<"Inside the second for 2 - Delay"<<endl;
                                 //if(g[f].capacitance<index_2[t])
                                 // {
                                 //     x3=t-1;
                                 //     x4=t;
                                 // }
                                 // else if(t==index_1.size()-1)
                                 // {
                                 //     x3=t-1;
                                 //     x4=t;
                                 // }

                     if(t==index_2.size()-1)
                    {
                            x3=t-1;
                            x4=t;
                            break;
                    }


                    if(g[f].capacitance>index_2[t])
                    {
                        x3=t-1;
                        x4=t;
                        continue;
                    } else {
                                    
                        if(t!=0){
                            x3=t-1;
                            x4=t;
                                        
                        } else{
                            x3=-1;
                            x4=0;
                        }
                                    
                        break;
                    }   

                }// end of the second for 

                 
                cout<<"for Node - "<<f<<" of type - "<<g[f].type<<endl;

                 
                cout<<"X3: "<<x3<<" X4: "<<x4<<endl;

                
                            //if(vt_type==1){
                 
                cout<<g[f].type<<endl;
                    // 
                    // string searchby="";

                // if(vt == 2){
                //                 //assuming that the netlist was given in lvt
                //     string temp=gate_fp_map_lvt[g[f].type];
                //     string fm=temp.substr(0,temp.length()-3);
                //     searchby=fm+"hvt";    
                // } else
                //             //if(vt_type==1){
                //     searchby=gate_fp_map_lvt[g[f].type];




                // if(tempval==0)
                // {
                     
                    cout<<"Inside tempval==0"<<endl;
                     
                    cout<<searchby<<"-g[f].type "<<x1<<" -x1 "<<x2<<" -x2 "<<x3<<" -x3 "<<x4<<" -x4 "<<endl;
                        // cout<<risemap["OR2KHD"][1]<<endl;
                        //map  xcd       
                        if(x1!=-1)
                        {
                            xleft=index_2[x3];
                            xright=index_2[x4];
                             
                            cout<<"for Node - "<<f<<" of type - "<<g[f].type<<endl;

                             
                            cout<<"XLEFT: "<<xleft<<"XRIGHT: "<<xright<<endl;
                                    // cout<<risemap[g[f].type]<<endl;
                                    // yleft=risemap[g[f].type][7*x1+x3];
                            yleft=risemap[searchby][7*x1+x3];
                                    // yright=risemap[g[f].type][7*x1+x4];   
                            yright=risemap[searchby][7*x1+x4];   
                                //    cout<<"YLEFT: "<<yleft<<"YRIGHT: "<<yright<<endl;
                        }else if(x1==-1)
                        {
                            xleft=0;
                            yleft=0;
                            xright=index_2[x4];
                                    // yright=risemap[g[f].type][x4];
                            yright=risemap[searchby][x4];
                            x1=0;
                            x2=1;

                        }
                        mindelay=interpolate(xleft,xright,yleft,yright,g[f].capacitance);
                        xleft=index_2[x3];
                        xright=index_2[x4];
                                    
                                    // yleft=risemap[g[f].type][7*x2+x3];
                        yleft=risemap[searchby][7*x2+x3];
                                    // yright=risemap[g[f].type][7*x2+x4];   
                        yright=risemap[searchby][7*x2+x4];   
                        maxdelay=interpolate(xleft,xright,yleft,yright,g[f].capacitance);
                         
                        cout<<"for Node - "<<f<<" of type - "<<g[f].type<<endl;
                         
                        cout<<"MINDELAY :"<<mindelay<<" MAXDELAY "<<maxdelay<<"NUM ELTS: "<<risemap[g[f].type].size()<<endl;
                                    //cout<<g[f].slew[tempval]<<endl;
                        if(x1!=-1)
                            g[f].delay[position]=interpolate(index_1[x1],index_1[x2],mindelay,maxdelay,g[f].slew[tempval]);
                                    
                                     
                        cout<<"For val : "<<tempval<<"||"<<g[f].delay[position]<<"is delay of pos "<<position<<"For node "<<f<<"of type "<<g[f].type<<endl; 
                                //cout<<g[f].delay[0]<<endl;
                }
                else if(tempval==1)
                {

                    string searchby="";
                    if(vt == 2){
                                //assuming that the netlist was given in lvt
                        string temp=gate_fp_map_lvt[g[f].type];
                        string fm=temp.substr(0,temp.length()-3);
                        searchby=fm+"hvt";    
                    } else
                            //if(vt_type==1){
                        searchby=gate_fp_map_lvt[g[f].type];

                    vector<float> index_1=inputslew_f[searchby];   
                      
                    cout<<"when g[f].type!=PI && g[f].type!=PO"<<endl;
                    for (int t=0;t<index_1.size();t++)
                    {
                         
                        cout<<"Inside the second for 1 - Delay"<<endl;
                                // if(g[f].slew[tempval]<index_1[t])
                                // {
                                //     x1=t-1;
                                //     x2=t;
                                // }
                                // else if(t==index_1.size()-1)
                                // {
                                //     x1=t-1;
                                //     x2=t;
                                // }

                        if(t==index_1.size()-1)
                        {
                            x1=t-1;
                            x2=t;
                            break;
                        }


                        if(g[f].slew[tempval]>index_1[t])
                        {
                            x1=t-1;
                            x2=t;
                            continue;
                        } else {
                                    
                            if(t!=0){
                                x1=t-1;
                                x2=t;
                                        
                            } else{
                                x1=-1;
                                x2=0;
                            }
                                    
                            break;
                        }

                    }//this is where the for index scanning ends 
                 
                cout<<"for Node - "<<f<<" of type - "<<g[f].type<<endl;

                 
                    cout<<"X1: "<<x1<<" X2: "<<x2<<endl;

                vector<float> index_2=loadCapacitance_f[searchby];
                for(int t=0;t<index_2.size();t++)
                {
                     
                    cout<<"Inside the second for 2 - Delay "<<f<<endl;
                                 //if(g[f].capacitance<index_2[t])
                                 // {
                                 //     x3=t-1;
                                 //     x4=t;
                                 // }
                                 // else if(t==index_1.size()-1)
                                 // {
                                 //     x3=t-1;
                                 //     x4=t;
                                 // }

                     if(t==index_2.size()-1)
                    {
                            x3=t-1;
                            x4=t;
                            break;
                    }


                    if(g[f].capacitance>index_2[t])
                    {
                        x3=t-1;
                        x4=t;
                        continue;
                    } else {
                                    
                        if(t!=0){
                            x3=t-1;
                            x4=t;
                                        
                        } else{
                            x3=-1;
                            x4=0;
                        }
                                    
                        break;
                    }   

                }// end of the second for 
                 
                cout<<"X3: "<<x3<<" X4: "<<x4<<endl;

                
                            //if(vt_type==1){
                 
                cout<<"f -:"<<f<<g[f].type<<endl;                    

                     
                    cout<<"Inside tempval==1"<<endl;
                    if(x1!=-1)
                    {
                        xleft=index_2[x3];
                        xright=index_2[x4];
                                    // yleft=fallmap[g[f].type][7*x1+x3];
                        yleft=fallmap[searchby][7*x1+x3];
                                    // yright=fallmap[g[f].type][7*x1+x4];   
                        yright=fallmap[searchby][7*x1+x4];   
                    }
                    else if(x1==-1)
                    {

                        xleft=0;
                        yleft=0;
                        xright=index_2[x4];
                                    // yright=fallmap[g[f].type][x4];
                        yright=fallmap[searchby][x4];
                        x1=0;
                        x2=1;
                    }

                                mindelay=interpolate(xleft,xright,yleft,yright,g[f].capacitance);
                                 
                                cout<<"for Node - "<<f<<" of type - "<<g[f].type<<endl;
                                 
                                cout<<"MIN Delay :"<<mindelay<<endl;
                                    xleft=index_2[x3];
                                    xright=index_2[x4];
                                    // yleft=fallmap[g[f].type][7*x2+x3];
                                    yleft=fallmap[searchby][7*x2+x3];
                                    // yright=fallmap[g[f].type][7*x2+x4];   
                                    yright=fallmap[searchby][7*x2+x4];   
                                    maxdelay=interpolate(xleft,xright,yleft,yright,g[f].capacitance);
                                     
                                    cout<<"for Node - "<<f<<" of type - "<<g[f].type<<endl;
                                     
                                    cout<<"MAX Delay :"<<maxdelay<<endl;
                                    if(x1!=-1)
                                        g[f].delay[position]=interpolate(index_1[x1],index_1[x2],mindelay,maxdelay,g[f].slew[tempval]);
                                    
                                     
                                     cout<<"For val : "<<tempval<<"||"<<g[f].delay[position]<<"is delay of pos "<<position<<"For node "<<f<<"of type "<<g[f].type<<endl; 
                    }



               
        }
       }

}

    

int main(int argc,char *argv[])
{

    string line;
    int num,num1;
    if(argc!=25)
       // cout<<"usage:./a.out hgrfile typefile lvtrise lvtfall lvtrisetransition lvtfalltransition rvtrise rvtfall rvtrisetransition rvtfalltransition   hvtrise hvtfall hvtrisetransition hvtfalltransition capacitance parityfile "<<endl;
        cout<<"usage:../a.out c17.hgr c17.type lvt_gate_lookup hvt_gate_lookup  lvt_cell_rise lvt_cell_fall lvt_rise_transition lvt_fall_transition hvt_cell_rise hvt_cell_fall  hvt_rise_transition hvt_fall_transition lvt_capacitance_rec lvt_parity_rectified lvt_index_cell_rise_rec lvt_index_cell_fall_rec lvt_index_rise_transition_rec lvt_index_fall_transition_rec hvt_index_cell_rise_rec hvt_index_cell_fall_rec hvt_index_rise_transition_rec hvt_index_fall_transition_rec "<<endl; 
    
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
                //cout<<line<<endl;
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
               // cout<<*vitr<<" "<<g[*vitr].type<<endl;
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
               // cout<<"Fan in i started - "<<i<<endl;
                vector<int>successors;
                for(int j=0;j<num;j++)
                    if(boost::edge(i,j,g).second)
                        successors.push_back(j);
                g[i].successors=successors;
               // cout<<"Fan in i done - "<<i<<endl;
            }
            cout<<"fanins done"<<endl;
            for(int i=0;i<num;i++)
            {
              //  cout<<"Fan out i started - "<<i<<endl;
                vector<int>predecessors;
                for(int j=0;j<num;j++)
                    if(boost::edge(j,i,g).second )
                        predecessors.push_back(j);
                g[i].predecessors=predecessors;
                //cout<<"Fan out i done - "<<i<<endl;
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
                   // cout<<"before inserting"<<endl;   
                    lvt_gate_fp_map.insert(std::pair<string,string>(result[0],s)); 
                    lvt_fp_gate_map.insert(std::pair<string,string>(s,result[0])); 
                   // cout<<"after inserting"<<endl;       
                }

            map<string,string > hvt_gate_fp_map;
            map<string,string > hvt_fp_gate_map;
            ifstream hvtmapfile(argv[4]);
            //while(hvtmapfile.good())
                //{
                    

                    while(getline(hvtmapfile,line)){    
                   // cout<<"In hvt map file"<<endl;
                    vector<string> result; 
                    boost::split(result, line, boost::is_any_of(":"));
                    // std::string s1;
                    string s1 = result[1];
                    int len=s1.length();
                    string s=s1.substr(0,len);
                   // cout<<s<<endl;
                    //std::replace( s.begin(), s.end(), '\n', '');

                    hvt_gate_fp_map.insert(pair<string,string>(result[0],s));
                    hvt_fp_gate_map.insert(pair<string,string>(s,result[0])); 

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
            cout<<"Reading the rise file --"<<endl;
            ifstream risefile(argv[5]);
                while(risefile.good())
                {
                    if(linecount==0)
                    {
                        linecount++;
                        getline(risefile,line);
                        stringstream ss(line);
                        ss>>tempword;
                      //  cout<<tempword<<endl;

                    }
                    else if(linecount<=7)
                    {
                        linecount++;
                    
                        getline(risefile,line);
                        stringstream ss(line);

                        while(ss>>tempnum){
                           // cout<<tempnum<<endl;
                            lvt_risemap[tempword].push_back(tempnum);
                        
                        }
                        cout<<"---------------------"<<endl;
                        if(linecount==8)
                            linecount=0;
                        cout <<"linecount - "<<line<<endl;
                    }
//                    linecount++;
                   // cout<<linecount<<endl;
                        
                }
            
            cout<<"Entering Fall file :"<<endl;
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
                       // cout<<tempword<<endl;
                    }
                    else if(linecount<=7)
                    {

                        linecount++;
                        getline(fallfile,line);
                        stringstream ss(line);

                        while(ss>>tempnum){
                           // cout<<tempnum<<endl;
                            lvt_fallmap[tempword].push_back(tempnum);
                        }
                      //  cout<<"---------------------"<<endl;
                        if(linecount==8)
                            linecount=0;
                   //     cout <<line<<endl;
                    }
                    
                  //  cout<<linecount<<endl;
                        
                }

                cout<<"Entering Rise Transitions : "<<endl;            
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
                      //  cout<<tempword<<endl;

                    }
                    else if(linecount<=7)
                    {
                        linecount++; 
                        getline(risetransitionfile,line);
                       stringstream ss(line);

                        while(ss>>tempnum){
                       //     cout<<tempnum<<endl;
                            lvt_risetransitionmap[tempword].push_back(tempnum);
                        }
                       // cout<<"---------------------"<<endl;
                        if(linecount==8)
                            linecount=0;
                    }
                 
                }
                
                 cout<<"Entering Fall transition :"<<endl;
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
                     //   cout<<tempword<<endl;
                    }
                    else if(linecount<=7)
                    {
                        linecount++;

                    
                        getline(falltransitionfile,line);
                        stringstream ss(line);

                        while(ss>>tempnum){
                        //    cout<<tempnum<<endl;
                            lvt_falltransitionmap[tempword].push_back(tempnum);
                        }
                        if(linecount==8)
                            linecount=0;
                      //  cout <<line<<endl;
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
            cout<<"Entering HVt rise file "<<endl;    
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
                      // cout <<line<<endl;
                    }
//                    linecount++;
                    //cout<<linecount<<endl;
                        
                }


            cout<<"Entering the HVT fall files "<<endl;
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
                        //cout<<tempword<<endl;
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
                        //cout <<line<<endl;
                    }
                    
                    cout<<linecount<<endl;
                        
                }

                cout<<"Entering the hvt rise transition file "<<endl;            
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
                       //     cout<<tempnum<<"\t";
                        }
                        if(linecount==8)
                            linecount=0;
                        cout<<"\n";
                    }
                 
                }
                        
                cout<<"Entering the hvt fall transition file "<<endl;        
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
                       // cout<<tempword<<endl;
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
                       // cout <<line<<endl;
                    }
//                    linecount++;
                 
                }           
            cout<<"hvt done"<<endl;

            cout<<"cap done"<<endl;
            ifstream capfile(argv[13]);
            while(capfile.good())
            {
                getline(capfile,line);
                stringstream ss(line);
                ss>>tempword;
                cout<<"tempword :"<<tempword<<endl;
                while(ss>>tempnum){
                    capacitancemap[tempword]=tempnum;
                   // cout<<" com on --"<<capacitancemap[tempword]<<endl;
                }

            }

             // ifstream hvtmapfile(argv[4]);
            //while(hvtmapfile.good())
                //{
                    

                    // while(getline(capfile,line)){    
                    // cout<<"In cap file"<<endl;
                    // vector<string> result; 
                    // boost::split(result, line, boost::is_any_of(":"));
                    // // std::string s1;
                    // string s1 = result[1];
                    // int len=s1.length();
                    // string s=s1.substr(0,len);
                    // cout<<s<<endl;
                    // //std::replace( s.begin(), s.end(), '\n', '');

                //}   


            
            // Reading gate parity added on 12-09-2013
            map<string,int> paritymap;
            ifstream parityfile(argv[14]);
            while(parityfile.good())
            {
                 getline(parityfile,line);
                 stringstream ss(line);
                 ss >> tempword;
                 cout<<"Parity being read "<<tempword<<endl;
                 while(ss>>tempnum){
                     paritymap[tempword]=tempnum;
                     cout<<tempnum<<endl;   
                 }
            }
            cout<<"parity done"<<endl;
            //assigning parity and capacitance to each vertex
            // for(int c=0)

            //////////////////////////////////////////////////////////////////////////////////////////////////
            //                                                                                              // 
            //                    Inserting the code considering different Idexes -Start                    //
            //                                                                                              // 
            //////////////////////////////////////////////////////////////////////////////////////////////////                                                                                              
vector<float> index_1;
    vector<float> index_2;
    // vector< vector<float>, vector<float> >  index_1_2;
    map<string, vector<float>> lvt_cell_rise_name_index_1;
    map<string, vector<float>> lvt_cell_rise_name_index_2;
    // reading the lvt_cell_rise file 
    ifstream lvtcellrisemap(argv[15]);
    linecount = 0;
    string current_token_name;
    //size_t sz;
    string line="";
    cout<<"Reading the Cell Rise Index File started"<<endl;
    while(getline(lvtcellrisemap,line)){

        //cout<<"I am in lvt cell rise map"<<endl;
        if(linecount == 0){
            // linecount++;
            int len= line.length();
            current_token_name= line.substr(0,len);
            linecount++;

        }

        vector<float>::iterator itr;
        if(linecount == 1){
            getline(lvtcellrisemap,line);
            string index_vals;
            stringstream ss(line);
            while(ss>>index_vals){
                double index_vals_temp=::atof(index_vals.c_str());
                index_1.push_back(index_vals_temp);

            }

            // vector<float>::iterator itr;
            // for(itr=index_1.begin();itr!=index_1.end();++itr){

            //  cout<<*itr<<"\t";
            // }
            // cout<<endl;
            // cout<<"---------------------------------------------"<<endl;
            
            lvt_cell_rise_name_index_1[current_token_name]=index_1;
            index_1=vector<float>();
            //cout<<endl;
            linecount++;


        }

        if(linecount == 2){
            getline(lvtcellrisemap,line);
            string index_vals;
            stringstream ss(line);
            while(ss>>index_vals){
                float index_vals_temp=::atof(index_vals.c_str());
                index_2.push_back(index_vals_temp);
            }
            

            lvt_cell_rise_name_index_2[current_token_name]=index_2;
            index_2=vector<float>();
            linecount=0;
        }

    }

    cout<<"Reading the rise index file is done"<<endl;
    cout<<"Reading the Cell Fall Index File started"<<endl;

    //This code deal with the lvt_cell_fall index
    map<string, vector<float>> lvt_cell_fall_name_index_1;
    map<string, vector<float>> lvt_cell_fall_name_index_2;
    // reading the lvt_cell_rise file 
    ifstream lvtcellfallmap(argv[16]);
    linecount = 0;
    current_token_name="";
    //size_t sz;
    //string line;
    while(getline(lvtcellfallmap,line)){

        //cout<<"I am in lvt cell fall map"<<endl;
        if(linecount == 0){
            // linecount++;
            int len= line.length();
            current_token_name= line.substr(0,len);
            linecount++;

        }

        vector<float>::iterator itr;
        if(linecount == 1){
            getline(lvtcellfallmap,line);
            string index_vals;
            stringstream ss(line);
            while(ss>>index_vals){
                double index_vals_temp=::atof(index_vals.c_str());
                index_1.push_back(index_vals_temp);

            }

            
            lvt_cell_fall_name_index_1[current_token_name]=index_1;
            index_1=vector<float>();
            //cout<<endl;
            linecount++;


        }

        if(linecount == 2){
            getline(lvtcellfallmap,line);
            string index_vals;
            stringstream ss(line);
            while(ss>>index_vals){
                float index_vals_temp=::atof(index_vals.c_str());
                index_2.push_back(index_vals_temp);
            }
            

            lvt_cell_fall_name_index_2[current_token_name]=index_2;
            index_2=vector<float>();
            linecount=0;
        }

    }

    cout<<"Reading the fall index file is done"<<endl;
    cout<<"Now reading the Rise Transition File "<<endl;

    map<string, vector<float>> lvt_rise_transition_name_index_1;
    map<string, vector<float>> lvt_rise_transition_name_index_2;
    // reading the lvt_cell_rise file 
    ifstream lvtrisetransitionmap(argv[17]);
    linecount = 0;
    current_token_name="";
    //size_t sz;
    //string line;
    while(getline(lvtrisetransitionmap,line)){

        //cout<<"I am in lvt cell rise map"<<endl;
        if(linecount == 0){
            // linecount++;
            int len= line.length();
            current_token_name= line.substr(0,len);
            linecount++;

        }

        vector<float>::iterator itr;
        if(linecount == 1){
            getline(lvtrisetransitionmap,line);
            string index_vals;
            stringstream ss(line);
            while(ss>>index_vals){
                double index_vals_temp=::atof(index_vals.c_str());
                index_1.push_back(index_vals_temp);

            }
            
            lvt_rise_transition_name_index_1[current_token_name]=index_1;
            index_1=vector<float>();
            //cout<<endl;
            linecount++;


        }

        if(linecount == 2){
            getline(lvtrisetransitionmap,line);
            string index_vals;
            stringstream ss(line);
            while(ss>>index_vals){
                float index_vals_temp=::atof(index_vals.c_str());
                index_2.push_back(index_vals_temp);
            }
            

            lvt_rise_transition_name_index_2[current_token_name]=index_2;
            index_2=vector<float>();
            linecount=0;
        }

    }

    cout<<"Reading the round transition file is done"<<endl;
    cout<<"Reading LVT fall transition file started"<<endl;
    map<string, vector<float>> lvt_fall_transition_name_index_1;
    map<string, vector<float>> lvt_fall_transition_name_index_2;
    // reading the lvt_cell_rise file 
    ifstream lvtfalltransitionmap(argv[18]);
    linecount = 0;
    current_token_name="";
    //size_t sz;
    //string line;
    while(getline(lvtfalltransitionmap,line)){

        // cout<<"I am in lvt cell rise map"<<endl;
        if(linecount == 0){
            // linecount++;
            int len= line.length();
            current_token_name= line.substr(0,len);
            linecount++;

        }

        vector<float>::iterator itr;
        if(linecount == 1){
            getline(lvtfalltransitionmap,line);
            string index_vals;
            stringstream ss(line);
            while(ss>>index_vals){
                double index_vals_temp=::atof(index_vals.c_str());
                index_1.push_back(index_vals_temp);

            }

            
            lvt_fall_transition_name_index_1[current_token_name]=index_1;
            index_1=vector<float>();
           // cout<<endl;
            linecount++;


        }

        if(linecount == 2){
            getline(lvtfalltransitionmap,line);
            string index_vals;
            stringstream ss(line);
            while(ss>>index_vals){
                float index_vals_temp=::atof(index_vals.c_str());
                index_2.push_back(index_vals_temp);
            }
            

            lvt_fall_transition_name_index_2[current_token_name]=index_2;
            index_2=vector<float>();
            linecount=0;
        }

    }

    cout<<"LVT Fall transition index reading done"<<endl;
    // following code is used to print the elements present in the index1 for each of the token/cell
    // map<string,vector<float>>::iterator itr;
    // for(itr=lvt_name_index_1.begin();itr!=lvt_name_index_1.end();++itr){

    //      cout<<itr->first<<"\n";
    //      vector<float> id=itr->second;
    //      vector<float>::iterator idItr;
    //      for(idItr=id.begin();idItr!=id.end();idItr++){
    //          cout<<*idItr<<"\t";
    //      }
    //      cout<<endl;
    // }
    cout<<endl;
    cout<<"----------------------Begining to read the HVT cell indices-----------------------"<<endl;

    map<string, vector<float>> hvt_cell_rise_name_index_1;
    map<string, vector<float>> hvt_cell_rise_name_index_2;
    // reading the lvt_cell_rise file 
    ifstream hvtcellrisemap(argv[19]);
    linecount = 0;
    current_token_name="";
    //size_t sz;
    line="";
    cout<<"Reading the Cell Rise Index File started"<<endl;
    while(getline(hvtcellrisemap,line)){

        //cout<<"I am in lvt cell rise map"<<endl;
        if(linecount == 0){
            // linecount++;
            int len= line.length();
            current_token_name= line.substr(0,len);
            linecount++;

        }

        vector<float>::iterator itr;
        if(linecount == 1){
            getline(hvtcellrisemap,line);
            string index_vals;
            stringstream ss(line);
            while(ss>>index_vals){
                double index_vals_temp=::atof(index_vals.c_str());
                index_1.push_back(index_vals_temp);

            }

            // vector<float>::iterator itr;
            // for(itr=index_1.begin();itr!=index_1.end();++itr){

            //  cout<<*itr<<"\t";
            // }
            // cout<<endl;
            // cout<<"---------------------------------------------"<<endl;
            
            hvt_cell_rise_name_index_1[current_token_name]=index_1;
            index_1=vector<float>();
            //cout<<endl;
            linecount++;


        }

        if(linecount == 2){
            getline(hvtcellrisemap,line);
            string index_vals;
            stringstream ss(line);
            while(ss>>index_vals){
                float index_vals_temp=::atof(index_vals.c_str());
                index_2.push_back(index_vals_temp);
            }
            

            hvt_cell_rise_name_index_2[current_token_name]=index_2;
            index_2=vector<float>();
            linecount=0;
        }

    }

    cout<<"Reading the rise index file is done"<<endl;
    cout<<"Reading the Cell Fall Index File started"<<endl;

    //This code deal with the lvt_cell_fall index
    map<string, vector<float>> hvt_cell_fall_name_index_1;
    map<string, vector<float>> hvt_cell_fall_name_index_2;
    // reading the lvt_cell_rise file 
    ifstream hvtcellfallmap(argv[20]);
    linecount = 0;
    current_token_name="";
    //size_t sz;
    //string line;
    while(getline(hvtcellfallmap,line)){

        //cout<<"I am in lvt cell fall map"<<endl;
        if(linecount == 0){
            // linecount++;
            int len= line.length();
            current_token_name= line.substr(0,len);
            linecount++;

        }

        vector<float>::iterator itr;
        if(linecount == 1){
            getline(hvtcellfallmap,line);
            string index_vals;
            stringstream ss(line);
            while(ss>>index_vals){
                double index_vals_temp=::atof(index_vals.c_str());
                index_1.push_back(index_vals_temp);

            }

            
            hvt_cell_fall_name_index_1[current_token_name]=index_1;
            index_1=vector<float>();
            //cout<<endl;
            linecount++;


        }

        if(linecount == 2){
            getline(hvtcellfallmap,line);
            string index_vals;
            stringstream ss(line);
            while(ss>>index_vals){
                float index_vals_temp=::atof(index_vals.c_str());
                index_2.push_back(index_vals_temp);
            }
            

            hvt_cell_fall_name_index_2[current_token_name]=index_2;
            index_2=vector<float>();
            linecount=0;
        }

    }

    cout<<"Reading the fall index file is done"<<endl;
    cout<<"Now reading the Rise Transition File "<<endl;

    map<string, vector<float>> hvt_rise_transition_name_index_1;
    map<string, vector<float>> hvt_rise_transition_name_index_2;
    // reading the lvt_cell_rise file 
    ifstream hvtrisetransitionmap(argv[21]);
    linecount = 0;
    current_token_name="";
    //size_t sz;
    //string line;
    while(getline(hvtrisetransitionmap,line)){

        //cout<<"I am in lvt cell rise map"<<endl;
        if(linecount == 0){
            // linecount++;
            int len= line.length();
            current_token_name= line.substr(0,len);
            linecount++;

        }

        vector<float>::iterator itr;
        if(linecount == 1){
            getline(hvtrisetransitionmap,line);
            string index_vals;
            stringstream ss(line);
            while(ss>>index_vals){
                double index_vals_temp=::atof(index_vals.c_str());
                index_1.push_back(index_vals_temp);

            }
            
            hvt_rise_transition_name_index_1[current_token_name]=index_1;
            index_1=vector<float>();
            cout<<endl;
            linecount++;


        }

        if(linecount == 2){
            getline(hvtrisetransitionmap,line);
            string index_vals;
            stringstream ss(line);
            while(ss>>index_vals){
                float index_vals_temp=::atof(index_vals.c_str());
                index_2.push_back(index_vals_temp);
            }
            

            hvt_rise_transition_name_index_2[current_token_name]=index_2;
            index_2=vector<float>();
            linecount=0;
        }

    }

    cout<<"Reading the round transition file is done"<<endl;
    cout<<"Reading HVT fall transition file started"<<endl;
    map<string, vector<float>> hvt_fall_transition_name_index_1;
    map<string, vector<float>> hvt_fall_transition_name_index_2;
    // reading the lvt_cell_rise file 
    ifstream hvtfalltransitionmap(argv[22]);
    linecount = 0;
    current_token_name="";
    //size_t sz;
    //string line;
    while(getline(hvtfalltransitionmap,line)){

        // cout<<"I am in lvt cell rise map"<<endl;
        if(linecount == 0){
            // linecount++;
            int len= line.length();
            current_token_name= line.substr(0,len);
            linecount++;

        }

        vector<float>::iterator itr;
        if(linecount == 1){
            getline(hvtfalltransitionmap,line);
            string index_vals;
            stringstream ss(line);
            while(ss>>index_vals){
                double index_vals_temp=::atof(index_vals.c_str());
                index_1.push_back(index_vals_temp);

            }

            
            hvt_fall_transition_name_index_1[current_token_name]=index_1;
            index_1=vector<float>();
            // cout<<endl;
            linecount++;


        }

        if(linecount == 2){
            getline(hvtfalltransitionmap,line);
            string index_vals;
            stringstream ss(line);
            while(ss>>index_vals){
                float index_vals_temp=::atof(index_vals.c_str());
                index_2.push_back(index_vals_temp);
            }
            

            hvt_fall_transition_name_index_2[current_token_name]=index_2;
            index_2=vector<float>();
            linecount=0;
        }

    }

    cout<<"HVT Fall transition index reading done"<<endl;




            //////////////////////////////////////////////////////////////////////////////////////////////////
            //                                                                                              // 
            //                    Inserting the code considering different Idexes -End                      //
            //                                                                                              // 
            //////////////////////////////////////////////////////////////////////////////////////////////////                                                                                              


    string s_pi_r= argv[23];
    string s_pi_f= argv[24];
    pi_slew_r=::atof(s_pi_r.c_str())/100;
    pi_slew_f=::atof(s_pi_f.c_str())/100;
        




            //this is done for the lvt part

           for (int f=0;f<num1-1;f++)
            {

                    if(g[f].type!="PI" && g[f].type!="PO"){
                       string temp= g[f].type;
                       string searchby=lvt_gate_fp_map[temp];
                       // g[f].capacitance= capacitancemap[searchby];
                       // cout<<"search by : "<<searchby<<" capacitance :"<<g[f].capacitance<<endl;
                       // cout<<g[f].capacitance<<endl;
                       g[f].parity= paritymap[searchby];
                       cout<<g[f].parity<<endl;
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
   
        for(int i = 0;i<=num;i++){
            vector<int> successor_map= g[i].successors;
            std::vector<int>::iterator Vitr;
            // float loadCapacitance=0.001594;
            float loadCapacitance=0;
            int count=0;
            for(Vitr=successor_map.begin();Vitr!=successor_map.end();Vitr++){
                int cell_no=*Vitr;
                string searchby="";
                searchby=lvt_gate_fp_map[g[cell_no].type];
                float current_cell_load = capacitancemap[searchby];
                loadCapacitance= loadCapacitance+current_cell_load;
                count=count+1;
            }

            int number_of_fanouts=successor_map.size();
            double initial_net_capacitance= 0.028;
            double initial_net_capacitance_less_fanout= 0.02;
            double decline_rate=0.000435;
            int rf=  number_of_fanouts - 10;
            if(loadCapacitance == 0){
                 //g[i].capacitance=NET_DELAY_LVT;
               // g[i].capacitance=(initial_net_capacitance-(number_of_fanouts)*decline_rate);
                 // g[i].capacitance=number_of_fanouts*initial_net_capacitance/(((int) number_of_fanouts/10)+1);
                //g[i].capacitance=initial_net_capacitance+(number_of_fanouts - 1)*0.01;
                if(number_of_fanouts <=10)
                    g[i].capacitance= initial_net_capacitance_less_fanout+(number_of_fanouts - 1)*0.01;
                    // g[i].capacitance=initial_net_capacitance;

                else        
                    g[i].capacitance=initial_net_capacitance+(rf-1)*0.001+9*0.01;

            }else{ 
                // if(g[i].level== 1)
                //     g[i].capacitance=loadCapacitance;
                // else                        
                   // g[i].capacitance=loadCapacitance+(initial_net_capacitance-(number_of_fanouts)*decline_rate);
                //g[i].capacitance=loadCapacitance+number_of_fanouts*initial_net_capacitance/(((int) number_of_fanouts/10)+1);
                // g[i].capacitance=loadCapacitance+initial_net_capacitance+(number_of_fanouts - 1)*0.01;
                if(number_of_fanouts <=10)
                    // g[i].capacitance=loadCapacitance+initial_net_capacitance;
                    g[i].capacitance=loadCapacitance+initial_net_capacitance_less_fanout+(number_of_fanouts - 1)*0.01;
                else   
                    g[i].capacitance=loadCapacitance+initial_net_capacitance+(rf-1)*0.001+9*0.01;
                    // g[i].capacitance=loadCapacitance+initial_net_capacitance+(number_of_fanouts - 10)*0.001;
                //g[i].capacitance=loadCapacitance+NET_DELAY_LVT;
            }

            cout<<"XXOO : LVT "<<endl;
            // if(i==26149 || i==2789|| i==8616|| i==8602|| i==132791|| i==97948|| i==97947|| i==97945|| i==97944|| i==9663 || i==9662 || i==163436 ){
            //if(i==1222 || i==8470|| i==34941|| i==90979|| i==114959|| i==97723|| i==28195|| i==89227|| i==12963|| i==12964 || i==97720 || i==163311 ){
               cout<<"XXOO : For i = "<<i<<" Capacitance :"<<g[i].capacitance<<" Initial Net Capacitance : "<<initial_net_capacitance<<" (number_of_fanouts)*decline_rate) :"<< (number_of_fanouts)*decline_rate<<endl;
               cout<<"number_of_fanouts :"<<number_of_fanouts<<"Decline rate :"<<decline_rate; 
            //}

            // else 
                // g[i].capacitance=loadCapacitance+NET_DELAY_LVT;
            
        }
   
    // vector<double> lvt_index_fall_1{0.019562,0.060972,0.110998,0.211567,0.413195,0.783246,1.624531};
    // vector<double> lvt_index_2{0.001500,0.003306,0.007287,0.016062,0.035404,0.078035,0.172000};


    // vector<double> lvt_index_rise_1{0.022742,0.066977,0.121474,0.231219,0.451314,0.854802,1.772129};
    // //vector<double> hvt_index_rise_2{0.001500,0.003306,0.007287,0.016062,0.035404,0.078035,0.172000};

    // vector<double> hvt_index_rise_1{0.017584,0.040787,0.071822,0.133905,0.258152,0.630810,1.562352};
    // //vector<double> lvt_index_rise_2{0.002500,0.006232,0.015536,0.038730,0.096549,0.240685,0.600000};
    // vector<double> hvt_index_2{0.002500,0.006232,0.015536,0.038730,0.096549,0.240685,0.600000};

    // vector<double> hvt_index_fall_1{0.021475,0.050215,0.088438,0.164972,0.317854,0.775861,1.921861};

    // vector<double> cell_rise_fall_index_lvt{0.001500,0.004166,0.011568,0.032125,0.089212,0.247745,0.688000};
    // vector<double> cell_rise_fall_index_hvt{0.002500,0.005552,0.012331,0.027386,0.060822,0.135080,0.300000};
   
            cout<<"Hitting first lvt slew calculation"<<endl;
            slew_calculation(lvt_rise_transition_name_index_1,lvt_rise_transition_name_index_2,lvt_fall_transition_name_index_1,lvt_fall_transition_name_index_2,lvt_risetransitionmap,lvt_falltransitionmap,0, templevel,num1,g,1,lvt_gate_fp_map,hvt_gate_fp_map);
  //          slew_calculation(lvt_index_1,index_2,lvt_risetransitionmap,lvt_falltransitionmap,1, templevel,num1,g);
            cout<<"Hitting first delay calculation"<<endl;    
            delay_calculation(lvt_cell_rise_name_index_1,lvt_cell_rise_name_index_2,lvt_cell_fall_name_index_1,lvt_cell_fall_name_index_2,lvt_risemap,lvt_fallmap,0,templevel,num1,0,g,1,lvt_gate_fp_map,hvt_gate_fp_map);
            // delay_calculation(lvt_index_rise_1,lvt_index_2,lvt_risemap,lvt_fallmap,0,templevel,num1,0,g,1,lvt_gate_fp_map,hvt_gate_fp_map);
            cout<<"lvt rise delay\n";
            delay_calculation(lvt_cell_rise_name_index_1,lvt_cell_rise_name_index_2,lvt_cell_fall_name_index_1,lvt_cell_fall_name_index_2,lvt_risemap,lvt_fallmap,1,templevel,num1,1,g,1,lvt_gate_fp_map,hvt_gate_fp_map);
            cout<<"lvt fall delay\n";
            

            cout<<"======= Following are the list of Slews for different Node============="<<endl;
            for(int i=0;i<num1-1;i++)
            {
                 
                cout<<i<<":: "<<g[i].slew[0]<<" g[f].slew[0] "<<g[i].slew[1]<<" g[f].slew[1] "<<endl;

            }
            cout<<"======= Following are the list of Slews for different Node============="<<endl;
            cout<<"======= Following are the list of laod capacitance of PO============="<<endl;
            for(int i=0;i<num1-1;i++)
            {
                // if(g[i].type=="PI")
                
                cout<<i<<"g[i].type"<<g[i].type<<":: "<<" g[i].capacitance ::"<<g[i].capacitance<<endl;

            }


            for (int f=0;f<num1-1;f++)
            {

                    if(g[f].type!="PI" && g[f].type!="PO"){
                       // string temp= g[f].type;
                       string temp=lvt_gate_fp_map[g[f].type];
                       string fm=temp.substr(0,temp.length()-3);
                       string searchby=fm+"hvt";
                       // g[f].type = 


                       // string searchby=hvt_gate_fp_map[temp];
                       // g[f].capacitance= capacitancemap[searchby];
                       // cout<<"search by : "<<searchby<<" capacitance :"<<g[f].capacitance<<endl;
                       // cout<<g[f].capacitance<<endl;
                       g[f].parity= paritymap[searchby];
                       cout<<g[f].parity<<endl;
                    }
            }


            for(int i = 0;i<=num;i++){
            vector<int> successor_map= g[i].successors;
            std::vector<int>::iterator Vitr;
            // float loadCapacitance=0.0027;
            float loadCapacitance=0;
            int count=0;
            for(Vitr=successor_map.begin();Vitr!=successor_map.end();Vitr++){
                int cell_no=*Vitr;
                string temp=lvt_gate_fp_map[g[cell_no].type];
                string fm=temp.substr(0,temp.length()-3);
                string searchby=fm+"hvt";
                // searchby=lvt_gate_fp_map[g[i].type];
                float current_cell_load = capacitancemap[searchby];
                loadCapacitance= loadCapacitance+current_cell_load;
                count=count+1;
            }

            int number_of_fanouts=successor_map.size();
            double initial_net_capacitance= 0.028;
            double decline_rate=0.000435;
            double initial_net_capacitance_less_fanout= 0.02;
            int rf=  number_of_fanouts - 10;
            if(loadCapacitance == 0){
                 //g[i].capacitance=NET_DELAY_LVT;
               // g[i].capacitance=(initial_net_capacitance-(number_of_fanouts)*decline_rate);
                 // g[i].capacitance=number_of_fanouts*initial_net_capacitance/(((int) number_of_fanouts/10)+1);
                //g[i].capacitance=initial_net_capacitance+(number_of_fanouts - 1)*0.01;
                if(number_of_fanouts <=10)
                    g[i].capacitance= initial_net_capacitance_less_fanout+(number_of_fanouts - 1)*0.01;
                    // g[i].capacitance=initial_net_capacitance;

                else        
                    g[i].capacitance=initial_net_capacitance+(rf-1)*0.001+9*0.01;

            }else{ 
                // if(g[i].level== 1)
                //     g[i].capacitance=loadCapacitance;
                // else                        
                   // g[i].capacitance=loadCapacitance+(initial_net_capacitance-(number_of_fanouts)*decline_rate);
                //g[i].capacitance=loadCapacitance+number_of_fanouts*initial_net_capacitance/(((int) number_of_fanouts/10)+1);
                // g[i].capacitance=loadCapacitance+initial_net_capacitance+(number_of_fanouts - 1)*0.01;
                if(number_of_fanouts <=10)
                    // g[i].capacitance=loadCapacitance+initial_net_capacitance;
                    g[i].capacitance=loadCapacitance+initial_net_capacitance_less_fanout+(number_of_fanouts - 1)*0.01;
                else   
                    g[i].capacitance=loadCapacitance+initial_net_capacitance+(rf-1)*0.001+9*0.01;
                    // g[i].capacitance=loadCapacitance+initial_net_capacitance+(number_of_fanouts - 10)*0.001;
                //g[i].capacitance=loadCapacitance+NET_DELAY_LVT;
            }
            cout<<"XXOO : HVT "<<endl;
            //if(i==26149 || i==2789|| i==8616|| i==8602|| i==132791|| i==97948|| i==97947|| i==97945|| i==97944|| i==9663 || i==9662 || i==163436 ){
            //if(i==1222 || i==8470|| i==34941|| i==90979|| i==114959|| i==97723|| i==28195|| i==89227|| i==12963|| i==12964 || i==97720 || i==163311 ){
               cout<<"XXOO : For i = "<<i<<" Capacitance :"<<g[i].capacitance<<" Initial Net Capacitance : "<<initial_net_capacitance<<" (number_of_fanouts)*decline_rate) :"<< (number_of_fanouts)*decline_rate<<endl;
               cout<<"number_of_fanouts :"<<number_of_fanouts<<"Decline rate :"<<decline_rate; 
            //} 
        

        }


            //HVT slew calculation
            cout<<" Before calling the slew calculation - - HVT "<<endl;
            slew_calculation(hvt_rise_transition_name_index_1,hvt_rise_transition_name_index_2,hvt_fall_transition_name_index_1,hvt_fall_transition_name_index_2,hvt_risetransitionmap,hvt_falltransitionmap,0, templevel,num1,g,2,lvt_gate_fp_map,hvt_gate_fp_map);
    //        slew_calculation(hvt_index_1,index_2,hvt_risetransitionmap,hvt_falltransitionmap,1, templevel,num1,g);
            // delay_calculation(hvt_index_rise_1,hvt_index_2,hvt_risemap,hvt_fallmap,0,templevel,num1,2,g,2,lvt_gate_fp_map,hvt_gate_fp_map);
            cout<<" Before  rise delay calculation  - - HVT "<<endl;
            delay_calculation(hvt_cell_rise_name_index_1,hvt_cell_rise_name_index_2,hvt_cell_fall_name_index_1,hvt_cell_fall_name_index_2,hvt_risemap,hvt_fallmap,0,templevel,num1,2,g,2,lvt_gate_fp_map,hvt_gate_fp_map);
            cout<<"hvt rise delay\n";
            cout<<" Before  fall delay calculation  - - HVT "<<endl;
            delay_calculation(hvt_cell_rise_name_index_1,hvt_cell_rise_name_index_2,hvt_cell_fall_name_index_1,hvt_cell_fall_name_index_2,hvt_risemap,hvt_fallmap,1,templevel,num1,3,g,2,lvt_gate_fp_map,hvt_gate_fp_map);
            cout<<"hvt fall delay\n";


        string filename=argv[1];
        ofstream myfile;
         string::size_type pos=filename.find(".");
    if(pos !=string::npos){
        filename.replace(pos,4,".rise_"+s_pi_r+"_"+s_pi_f);
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
            filename.replace(pos,5,".fall");
        }
        myfile.open(filename.c_str());
        for(int i=0;i<size;i++)
        {
            myfile<<g[i].delay[1]<<"\t"<<g[i].delay[3]<<"\n";//<<g[i].delay[5]<<"\n";
            // myfile<<g[i].delay[1]<<"\t"<<g[i].delay[3]<<"\t"<<g[i].delay[5]<<"\n";
           
        
        }


        myfile.close();
        cout<<"======= Following are the list of Slews for different Node============="<<endl;
        for(int i=0;i<size;i++)
        {
             
            cout<<i<<":: "<<g[i].slew[0]<<" g[f].slew[0] "<<g[i].slew[1]<<" g[f].slew[1] "<<endl;

        }


        vector<int> delay_for_path{22204,132953,75470,57715,14927,103162,121003,66895,66894,66893};

        std::vector<int>::iterator ditr;
        float sum_delay_rise=0;
        float sum_delay_rise_neg=0;
        myfile.open("total_path_delay"+s_pi_r+"_"+s_pi_f);
        // myfile.open("total_fall_delay");
        for(ditr=delay_for_path.begin();ditr!=delay_for_path.end();++ditr){
            if(*ditr!=14927){
                sum_delay_rise=sum_delay_rise+g[*ditr].delay[0];
                sum_delay_rise_neg=sum_delay_rise_neg+g[*ditr-1].delay[0];
                myfile<<"Type for ditr "<<" g[*ditr].type :"<<g[*ditr].type<<endl;
                myfile<<"Type for ditr "<<" g[*ditr - 1].type :"<<g[*ditr - 1].type<<endl;
                myfile<<g[*ditr].delay[0]<<"\t"<<g[*ditr-1].delay[0]<<endl;
                // sum_delay_fall=sum_delay_fall+g[*ditr].delay[2];
                // sum_delay_fall_neg=sum_delay_fall_neg+g[*ditr-1].delay[2];
            }

            else{
                sum_delay_rise=sum_delay_rise+g[*ditr].delay[1];
                sum_delay_rise_neg=sum_delay_rise_neg+g[*ditr-1].delay[1];
                myfile<<"Type for ditr "<<" g[*ditr].type :"<<g[*ditr].type<<endl;
                myfile<<"Type for ditr "<<" g[*ditr - 1].type :"<<g[*ditr - 1].type<<endl;
                myfile<<g[*ditr].delay[1]<<"\t"<<g[*ditr-1].delay[1]<<endl;
                // sum_delay_rise=sum_delay_rise+g[*ditr].delay[0];
                // sum_delay_rise_neg=sum_delay_rise_neg+g[*ditr-1].delay[0];
            }

            cout<<"Type for ditr "<<g[*ditr].type<<endl;
        }

        myfile<<sum_delay_rise<<"\t"<<sum_delay_rise_neg<<"\n"<<endl;
        myfile<<"================== checking for path 2 ===========================\n";
        vector<int> delay_for_path2{8616,8602,26149,132791,97948,97947,97945,97944,9663,9662};

        // std::vector<int>::iterator ditr;
        sum_delay_rise=0;
        sum_delay_rise_neg=0;
        // myfile.open("total_path_delay"+s_pi_r+"_"+s_pi_f);
        // myfile.open("total_fall_delay");
        for(ditr=delay_for_path2.begin();ditr!=delay_for_path2.end();++ditr){
            if(*ditr!=14927){
                sum_delay_rise=sum_delay_rise+g[*ditr].delay[0];
                sum_delay_rise_neg=sum_delay_rise_neg+g[*ditr-1].delay[0];
                myfile<<"Type for ditr "<<" g[*ditr].type :"<<g[*ditr].type<<endl;
                myfile<<"Type for ditr "<<" g[*ditr - 1].type :"<<g[*ditr - 1].type<<endl;
                myfile<<g[*ditr].delay[0]<<"\t"<<g[*ditr-1].delay[0]<<endl;
                // sum_delay_fall=sum_delay_fall+g[*ditr].delay[2];
                // sum_delay_fall_neg=sum_delay_fall_neg+g[*ditr-1].delay[2];
            }

            else{
                sum_delay_rise=sum_delay_rise+g[*ditr].delay[1];
                sum_delay_rise_neg=sum_delay_rise_neg+g[*ditr-1].delay[1];
                myfile<<"Type for ditr "<<" g[*ditr].type :"<<g[*ditr].type<<endl;
                myfile<<"Type for ditr "<<" g[*ditr - 1].type :"<<g[*ditr - 1].type<<endl;
                myfile<<g[*ditr].delay[1]<<"\t"<<g[*ditr-1].delay[1]<<endl;
                // sum_delay_rise=sum_delay_rise+g[*ditr].delay[0];
                // sum_delay_rise_neg=sum_delay_rise_neg+g[*ditr-1].delay[0];
            }

            cout<<"Type for ditr "<<g[*ditr].type<<endl;
        }

        myfile<<sum_delay_rise<<"\t"<<sum_delay_rise_neg<<"\n"<<endl;

        myfile.close();





    }//closing braces for argument else

    return 0;
}//closing braces for int main()

