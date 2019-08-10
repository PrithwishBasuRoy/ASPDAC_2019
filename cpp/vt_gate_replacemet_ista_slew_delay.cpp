#include <iostream>
#include <stdlib.h>
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
#include <bits/stdc++.h> 
#define MIN_SLACK 0
#define maxwindowsize 10
#define PI_SLEW 0.049986
#define MAX(a,b) ((a)>(b)?(a):(b))
#define average(a,b)  (0.5*(a+b))
using namespace std;
map<string,int> paritymap;
queue<int> slackqueue;
map<string,string > lvt_gate_fp_map;
map<string,string > lvt_fp_gate_map;
map<string,string > hvt_gate_fp_map;
map<string,string > hvt_fp_gate_map;
int parity_on=1;
float pi_slew_r;
float pi_slew_f;
map<int,vector<double>> risedelaymap;
map<int,vector<double>> falldelaymap;
map<string,double> lvtleakagemap;
// map<string,double> rvtleakagemap;
map<string,double> hvtleakagemap;



vector<pair<int,double>> slackset;
    struct VertexProperty
{
    vector<int> fanouts;
    vector<int> fanins;
    string type;
    double slack;
    double delay;
    double slew[2];
    double delay_from_slew[2];
    double arrival;
    double leakage;
    int level;
    int parity; 
    int numreplacements;
    int critical;
    double cost[1];
    int lvt_hvt=1;
    double capacitance; 
    double cellCapacitance;
};
bool mysortfunction(const pair<int,double> &i,const pair<int,double> &j)
{
    return i.second>j.second;
}
typedef boost::adjacency_list<boost::vecS,boost::vecS,boost::bidirectionalS,VertexProperty> Graph;
int gate_level(int element,Graph &g)
{
    vector<int>::iterator intit;
    int i,l,lmax=-1;
    int tempnode;
    if(g[element].level==-1)
    {
        lmax=-1;
        for(intit=g[element].fanins.begin();intit<g[element].fanins.end();intit++)
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

double interpolate(double xleft,double xright,double yleft,double yright,double xval)
{
    cout<<"Inside Interpolate "<<xleft<<" "<<xright<<" "<<yleft<<" "<<yright<<" "<<xval<<"---------"<<endl; 
    double y=yleft+(((xval-xleft)*(yright-yleft))/(xright-xleft));
    return y;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Adding code for finding the nodes in level l-3 to l+3 where l is the current level of 
// the present node
//////////////////////////////////////////////////////////////////////////////////////////
// vector<int> list_of_predecessors = find_predecessors();

void find_predecessors(Graph &g,int CurrentNode,vector<int> &fanin_list_upto_3,int CurrentNodeLevel, int CurrentLevel){

   cout<<"Inside find_predecessors for "<<CurrentNode<<" For Node Level "<<CurrentLevel<<endl; 
   if(CurrentLevel<=2){ 
    cout<<"Inside If"<<endl;
    vector<int> fanins= g[CurrentNode].fanins;
    vector<int>::iterator fanins_iterator;

    // fanin_list_upto_3.insert(fanin_list_upto_3.begin(),fanins.begin(),fanins.end());

    for(fanins_iterator=fanins.begin();fanins_iterator!=fanins.end();++fanins_iterator){
        if(CurrentNodeLevel - g[*fanins_iterator].level<=3){
           // cout<<*fanins_iterator<<" - is Inserted "<<endl;
            fanin_list_upto_3.insert(fanin_list_upto_3.begin(),*fanins_iterator);    
            // fanin_list_upto_3.insert(0);    
        }
        


    }

    for(fanins_iterator=fanins.begin();fanins_iterator!=fanins.end();++fanins_iterator){
        //if(CurrentNodeLevel - g[*fanins_iterator].level<=3)
        //fanin_list_upto_3.insert(*fanins_iterator);
        cout<<"Before calling find_predecessors"<<endl;
        find_predecessors(g,*fanins_iterator,fanin_list_upto_3,CurrentNodeLevel,CurrentLevel+1);

    }    
   } else {
    cout<<"Inside else"<<endl;
    // return fanin_list_upto_3;
   } 


}

//////////////////////////////////////////////////////////////////////////////////////////
// Adding code for finding the nodes in level l-3 to l+3 where l is the current level of 
// the present node
//////////////////////////////////////////////////////////////////////////////////////////
// vector<int> list_of_predecessors = find_predecessors();

void find_successors(Graph &g,int CurrentNode,vector<int> &fanout_list_upto_3,int CurrentNodeLevel, int CurrentLevel){

   cout<<"Inside find_successors for "<<CurrentNode<<" For Node Level "<<CurrentLevel<<endl; 
   if(CurrentLevel<=2){ 
    cout<<"Inside If"<<endl;
    vector<int> fanouts= g[CurrentNode].fanouts;
    vector<int>::iterator fanouts_iterator;

    // fanin_list_upto_3.insert(fanin_list_upto_3.begin(),fanins.begin(),fanins.end());

    for(fanouts_iterator=fanouts.begin();fanouts_iterator!=fanouts.end();++fanouts_iterator){
        if( g[*fanouts_iterator].level - CurrentNodeLevel <=3){
          //  cout<<*fanouts_iterator<<" - is Inserted "<<endl;
            fanout_list_upto_3.insert(fanout_list_upto_3.begin(),*fanouts_iterator);    
            // fanin_list_upto_3.insert(0);    
        }
        


    }

    for(fanouts_iterator=fanouts.begin();fanouts_iterator!=fanouts.end();++fanouts_iterator){
        //if(CurrentNodeLevel - g[*fanins_iterator].level<=3)
        //fanin_list_upto_3.insert(*fanins_iterator);
        cout<<"Before calling find_successors"<<endl;
        find_successors(g,*fanouts_iterator,fanout_list_upto_3,CurrentNodeLevel,CurrentLevel+1);

    }    
   } else {
    cout<<"Inside else"<<endl;
    // return fanin_list_upto_3;
   } 


}



vector<int> all_nodes_at_level_plus_minus_n(int current_node,Graph &g){

        vector<int>::iterator v_itr;
        vector<int>::iterator v_itr_fanin;
        vector<int>::iterator v_itr_fanout;
        vector<int> fanin_list_upto_3;
        vector<int> fanout_list_upto_3;   
        vector<int> list_of_all_predecessors_and_successors;

        // for(v_itr=list_of_nodes_on_path.begin();v_itr!=list_of_nodes_on_path.end();++v_itr){
            cout<<"Finding predecessors for the node -- "<<current_node<<endl;
            find_predecessors(g,current_node,fanin_list_upto_3,g[current_node].level,0);

            for(v_itr_fanin=fanin_list_upto_3.begin();v_itr_fanin!=fanin_list_upto_3.end();++v_itr_fanin){
                cout<<"Fan in list has ----"<<endl;
                cout<<*v_itr_fanin<<endl;
            }

            //fanin_list_upto_3={};

        //}

         //Here starts the code for finding the fanout

        //for(v_itr=list_of_nodes_on_path.begin();v_itr!=list_of_nodes_on_path.end();++v_itr){
            cout<<"Finding successors for the node -- "<<current_node<<endl;
            find_successors(g,current_node,fanout_list_upto_3,g[current_node].level,0);

            for(v_itr_fanout=fanout_list_upto_3.begin();v_itr_fanout!=fanout_list_upto_3.end();++v_itr_fanout){
                cout<<"Fan out list has ----"<<endl;
                cout<<*v_itr_fanout<<endl;
            }

            list_of_all_predecessors_and_successors.insert(list_of_all_predecessors_and_successors.begin(),fanin_list_upto_3.begin(),fanin_list_upto_3.end());
            list_of_all_predecessors_and_successors.insert(list_of_all_predecessors_and_successors.begin(),current_node);
            list_of_all_predecessors_and_successors.insert(list_of_all_predecessors_and_successors.begin(),fanout_list_upto_3.begin(),fanout_list_upto_3.end());
            //fanout_list_upto_3={};
            return list_of_all_predecessors_and_successors; 
        //}
}


void slew_calculation( map<string,vector<float>> &inputslew_r,map<string,vector<float>> &loadCapacitance_r,map<string,vector<float>> &inputslew_f,map<string,vector<float>> &loadCapacitance_f,map<string,vector<double> > &risetransitionmap,map<string,vector<double> > &falltransitionmap, int val,int levels,int size,Graph &g,map<string,string> gate_fp_map_lvt,map<string,string> gate_fp_map_hvt,vector<int> list_of_nodes_on_path)
{

    int count=0;
    cout<<"iNSIDE sLEW Calculation"<<endl;
    vector<int>::iterator intit;
    g[size-1].slew[val]=0; // val 0 rise val 1 fall ? check.
    double slew=DBL_MIN;
    int tempnode;
    int vt=1;

       int x1,x2,x3,x4;//x1,x2 are for scanning the cap, x1,x2 for ip slew
    double xleft,xright,yleft,yright,xval,minslew,maxslew;
    for (int i=0;i<=levels;i++)
    {
        cout<<"inside 1st for -- "<<i<<endl;
        vector<int>::iterator itr;
        //for(int f=0;f<size;f++)
        for(itr=list_of_nodes_on_path.begin();itr!=list_of_nodes_on_path.end();++itr)
        {
            int f=*itr;
            if(g[f].lvt_hvt == 2)
                vt=2;
            else 
                vt=1;
            cout<<"inside 2nd for -- "<<f<<endl;
            count=0;
           if(g[f].level==i && i==levels)//the code for nodes having max levels starts here
            {
                        cout<<"when first g[f].level==i && i==levels"<<endl;
                        int parity=g[f].parity;
                        while(count<2)
                        {
                            cout<<"inside while count val -- "<<count<<endl;
                                     slew=DBL_MIN;
                                     for(intit=g[f].fanins.begin();intit!=g[f].fanins.end();intit++)
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
                cout<<" parity :"<<parity<<" g[f].level :"<<g[f].level<<endl;
                

                while(count<2)//this part stays safe 
                {
                            cout<<"inside while count val -- "<<count<<endl;
                            slew=DBL_MIN;

                            for(intit=g[f].fanins.begin();intit!=g[f].fanins.end();intit++)
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
                                               
                           //cout<<"f:"<<f<<"TEMPVAL"<<tempval<<"VAL"<<val<<endl; 
                        while(count<2)
                        {
                        slew=DBL_MIN;
                            cout<<"inside while count val -- "<<count<<endl;
                            for(intit=g[f].fanins.begin();intit<g[f].fanins.end();intit++)//this part stays safe
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

                                //index one has to vary as per the type of gate
                                vector<float> index_1=inputslew_r[searchby];
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

                           cout<<"X1:"<<x1<<"X2:"<<x2<<endl;

                           //index two has to vary as per the type of gate
                           vector<float> index_2=loadCapacitance_r[searchby];
                           
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
                                    cout<<" X1: "<<x1<<"X2: "<<x2<<" X4: "<<x4<<" X3: "<<x3;    
                                    // cout<<"risetransitionmap[searchby][7*x1+x3] "<<risetransitionmap[searchby][7*x1+x3]<<"risetransitionmap[searchby][7*x1+x4] :"<<risetransitionmap[searchby][7*x1+x4]<<endl;
                                    cout<<"xleft "<<xleft<<"yleft "<<yleft<<" xright "<<xright<<" yright"<<yright<<endl;
                                    
                                    yright=risetransitionmap[searchby][x4];
                                    cout<<searchby<<" :searchby "<<yright<<" :yright"<<endl;
                                }

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

                                //index_1 has to change here
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

                           cout<<"X1:"<<x1<<"X2:"<<x2<<endl;
                           //index 2 has to change here
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
                        cout<<g[f].slew[0]<<" g[f].slew[0] "<<g[f].slew[1]<<" g[f].slew[1] "<<endl;
            }
            }
        cout<<"outside 2nd for -- "<<endl;
    } 

    cout<<"outside 1st for -- "<<endl;
}

void delay_calculation(map<string,vector<float>> &inputslew_r,map<string,vector<float>> &loadCapacitance_r,map<string,vector<float>> &inputslew_f,map<string,vector<float>> &loadCapacitance_f,map<string,vector<double> >risemap,map<string,vector<double> >fallmap, int tempval,int levels,int size,int position,Graph &g,map<string,string> gate_fp_map_lvt,map<string,string> gate_fp_map_hvt,vector<int> list_of_nodes_on_path)
{
    cout<<"Inside delay calculation"<<endl;

       double tempdelay,mindelay,maxdelay,minslew,maxslew,slew;
       int x1,x2,x3,x4;
       double xleft,xright,yleft,yright,xval;
       vector<int>::iterator itr;
       int vt=1;
        //for(int f=0;f<size;f++)
        for(itr=list_of_nodes_on_path.begin();itr!=list_of_nodes_on_path.end();++itr)
        {
            int f=*itr;
            if(g[f].lvt_hvt == 2)
                vt=2;
            else 
                vt=1;
            cout<<"Inside the first for - Delay"<<endl;
           if(g[f].type!="PI" && g[f].type!="PO")
           {


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

                //index 1 has to change here    
                vector<float> index_1=inputslew_r[searchby];    
                cout<<"when g[f].type!=PI && g[f].type!=PO"<<endl;
                 for (int t=0;t<index_1.size();t++)
                 {
                        cout<<"Inside the second for 1 - Delay"<<endl;
                            
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
                
                cout<<"X1: "<<x1<<" X2: "<<x2<<endl;

                //index 2 has to change here
                vector<float> index_2=loadCapacitance_r[searchby];
                for(int t=0;t<index_2.size();t++)
                {
                    cout<<"Inside the second for 2 - Delay"<<endl;
                
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
                cout<<g[f].type<<endl;
                
                    cout<<"Inside tempval==0"<<endl;
                    cout<<searchby<<"-g[f].type "<<x1<<" -x1 "<<x2<<" -x2 "<<x3<<" -x3 "<<x4<<" -x4 "<<endl;
                        // cout<<risemap["OR2KHD"][1]<<endl;
                        //map  xcd       
                        if(x1!=-1)
                        {
                            xleft=index_2[x3];
                            xright=index_2[x4];
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
                        cout<<"MINDELAY :"<<mindelay<<" MAXDELAY "<<maxdelay<<"NUM ELTS: "<<risemap[g[f].type].size()<<endl;
                                    //cout<<g[f].slew[tempval]<<endl;
                        if(x1!=-1)
                            //g[f].delay[position]=interpolate(index_1[x1],index_1[x2],mindelay,maxdelay,g[f].slew[tempval]);
                            g[f].delay_from_slew[position]=interpolate(index_1[x1],index_1[x2],mindelay,maxdelay,g[f].slew[tempval]);        
                                    
                        cout<<"For val : "<<tempval<<"||"<<g[f].delay_from_slew[position]<<"is delay of pos "<<position<<"For node "<<f<<"of type "<<g[f].type<<endl; 
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
                
                    cout<<"X1: "<<x1<<" X2: "<<x2<<endl;

                vector<float> index_2=loadCapacitance_f[searchby];
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
                cout<<"X3: "<<x3<<" X4: "<<x4<<endl;

                
                            //if(vt_type==1){
                cout<<g[f].type<<endl;                    


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
                                cout<<"MIN Delay :"<<mindelay<<endl;
                                    xleft=index_2[x3];
                                    xright=index_2[x4];
                                    // yleft=fallmap[g[f].type][7*x2+x3];
                                    yleft=fallmap[searchby][7*x2+x3];
                                    // yright=fallmap[g[f].type][7*x2+x4];   
                                    yright=fallmap[searchby][7*x2+x4];   
                                    maxdelay=interpolate(xleft,xright,yleft,yright,g[f].capacitance);
                                    cout<<"MAX Delay :"<<maxdelay<<endl;
                                    if(x1!=-1)
                                        //g[f].delay[position]=interpolate(index_1[x1],index_1[x2],mindelay,maxdelay,g[f].slew[tempval]);
                                        g[f].delay_from_slew[position]=interpolate(index_1[x1],index_1[x2],mindelay,maxdelay,g[f].slew[tempval]);
                                     cout<<"For val : "<<tempval<<"||"<<g[f].delay_from_slew[position]<<"is delay of pos "<<position<<"For node "<<f<<"of type "<<g[f].type<<endl; 
                    }



               
        }
       }

}




// vector<int> all_nodes_at_level_plus_minus_n(int current_node,Graph &g){

//         vector<int>::iterator v_itr;
//         vector<int>::iterator v_itr_fanin;
//         vector<int>::iterator v_itr_fanout;
//         vector<int> fanin_list_upto_3;
//         vector<int> fanout_list_upto_3;   
//         vector<int> list_of_all_predecessors_and_successors;

//         // for(v_itr=list_of_nodes_on_path.begin();v_itr!=list_of_nodes_on_path.end();++v_itr){
//             cout<<"Finding predecessors for the node -- "<<current_node<<endl;
//             find_predecessors(g,current_node,fanin_list_upto_3,g[current_node].level,0);

//             for(v_itr_fanin=fanin_list_upto_3.begin();v_itr_fanin!=fanin_list_upto_3.end();++v_itr_fanin){
//                 cout<<"Fan in list has ----"<<endl;
//                 cout<<*v_itr_fanin<<endl;
//             }

//             //fanin_list_upto_3={};

//         //}

//          //Here starts the code for finding the fanout

//         //for(v_itr=list_of_nodes_on_path.begin();v_itr!=list_of_nodes_on_path.end();++v_itr){
//             cout<<"Finding successors for the node -- "<<current_node<<endl;
//             find_successors(g,current_node,fanout_list_upto_3,g[current_node].level,0);

//             for(v_itr_fanout=fanout_list_upto_3.begin();v_itr_fanout!=fanout_list_upto_3.end();++v_itr_fanout){
//                 cout<<"Fan out list has ----"<<endl;
//                 cout<<*v_itr_fanout<<endl;
//             }

//             list_of_all_predecessors_and_successors.insert(list_of_all_predecessors_and_successors.begin(),fanin_list_upto_3.begin(),fanin_list_upto_3.end());
//             list_of_all_predecessors_and_successors.insert(list_of_all_predecessors_and_successors.begin(),current_node);
//             list_of_all_predecessors_and_successors.insert(list_of_all_predecessors_and_successors.begin(),fanout_list_upto_3.begin(),fanout_list_upto_3.end());
//             //fanout_list_upto_3={};
//             return list_of_all_predecessors_and_successors; 
//         //}
// }

        

///////////////////////////////////////////////////////////////////////////////////////////
vector<int> sorting_by_cost_function(Graph &g,vector<int> list_of_nodes_on_path){

    slackset.clear();
    vector<int>::iterator v_itr;
    vector<int>::iterator scanned_node;
    vector<int> all_nodes;

    for(v_itr=list_of_nodes_on_path.begin();v_itr!=list_of_nodes_on_path.end();++v_itr){
         //cout<<"g[i].type!=PI &&g[i].type!=PO && g[i].numreplacements<1&&g[i].critical==0"<<endl;   
         //cout<<" pushing i:g[i].cost[0] --> "<<i<<" : "<<g[i].cost[0]<<endl;
         if(g[*v_itr].critical==0)
         slackset.push_back(std::make_pair(*v_itr,g[*v_itr].cost[0]));

        //slackset.push_back(std::make_pair(i,g[i].cost[1]));
    }

    sort(slackset.begin(),slackset.end(),mysortfunction);
    ofstream myfile;
    myfile.open("slackfile_ista.txt",ios::out|ios::app);
    myfile<<"calling from new sorting func\n";
    myfile<<"the nodes in the decreasing order of slack are\n";
    for(int i=0;i<slackset.size();i++)
    {
        int temp=slackset[i].first;
        myfile<<temp<<" "<<g[temp].cost[0]<<" "<<g[temp].numreplacements<<" "<<g[temp].critical<<"\n";     
    } 
    myfile<<"\n";

    vector<int> nodevect;
    for(int i=0;i<slackset.size();i++)
    {
        cout<<"pushing slackset[i].first :"<<slackset[i].first<<endl;
        int temp=slackset[i].first;
        nodevect.push_back(temp);
    }
    return nodevect; 

}


double arrival_time(int levels,int size,Graph &g,int vt) //changed val to tempval because of inverting gates
{

    cout<<"Inside arrival time calculation"<<endl;
    vector<int>::iterator intit;

    int num=size-1;
    double arrival,slack,required;
    arrival=DBL_MIN;
    double delay=DBL_MIN;
//    double delay=0;
    int tempnode;        
    double maxdelay=0;
    g[num-1].arrival=0;
    cout<<"The levels "<<levels<<endl;
    for(int i=0;i<=levels;i++)
    {
        for(int f=0;f<num;f++)
        {
            if(g[f].level==i && i==0)
            {
                cout<<"when g[f].level==i && i==0"<<endl;
                g[f].arrival=0;       
            }
            if(g[f].level==i && i!=0)
            {

                cout<<"g[f].level==i && i!=0 and level - "<<i<<endl;
                //int parity=paritymap[g[f].type];
                for(intit=g[f].fanins.begin();intit<g[f].fanins.end();intit++)
                {
                    tempnode=*intit;
                    // cout<<"I am here "<<arrival<<"- arrival|| "<<g[tempnode].arrival<<" - tempnode arrival "<<g[f].delay<<"g[delay]"<<endl;
                    cout<<arrival<<" - This is the arrival || "<<g[f].delay<<" - delay of current cell "<<g[f].type<<" - arrival time of parent cell "<<g[tempnode].type<<" - "<<g[tempnode].arrival<<endl;
                    if(g[tempnode].arrival+g[f].delay>arrival){
                        cout<<"Inside if"<<endl;
                        arrival=g[tempnode].arrival+g[f].delay;
                        cout<<arrival<<" - This is the arrival || "<<g[f].delay<<" - delay of current cell "<<g[f].type<<" - arrival time of parent cell "<<g[tempnode].type<<" - "<<g[tempnode].arrival<<endl;
                    }


                }
                g[f].arrival=arrival;

                cout<<"g[f].arrival "<<g[f].arrival<<" || type "<<g[f].type<<endl;
               // cout<<" RISE ARRIVAL TIME FOR "<<f<<" IS "<<g[f].arrival[0]<<" FALL ARRIVAL TIME FOR "<<g[f].arrival[1]<<" LEVEL ="<<g[f].level<<" TYPE IS "<<g[f].type<<" DELAY OF GATE IS "<<g[f].delay[0]<<" FALL DELAY OF GATE IS "<<g[f].delay[1]<<" PARITY OF GATE" << g[f].parity<<endl;
                arrival=DBL_MIN;
                if(g[f].type=="PO")
                {
                     maxdelay=g[f].arrival;
                    if(maxdelay>delay){
                        delay=maxdelay;
                        cout<<"Highest delay till now -- "<<delay;
                    }
                }

            }
        }
    }
// delay is initialized to zero
//        cout<<"averagedelay"<<averagedelay<<"Delay"<<delay<<endl;        
        return delay;
}


double arrival_time_for_selected_path(Graph &g,vector<int> list_of_nodes_on_path){
    double arrival = DBL_MIN;
    double delay = DBL_MIN;
    double max_delay_for_path=0;
    double level;
    vector<int>::iterator v_itr;
    vector<int>::iterator scanned_node;
         // vector<int>::iterator v_itr_fanout;
         // vector<int> fanin_list_upto_3;
         // vector<int> fanout_list_upto_3;   
    vector<int> all_nodes;

    cout<<"Inside arrival_time_for_selected_path"<<endl;
    for(v_itr=list_of_nodes_on_path.begin();v_itr!=list_of_nodes_on_path.end();++v_itr){
        cout<<"for each node in list_of_nodes_on_path -- present node -  "<<*v_itr<<endl;
        all_nodes=all_nodes_at_level_plus_minus_n(*v_itr,g);
        
        for(level =  g[*v_itr].level -3; level<=  g[*v_itr].level + 3; level++){
            cout<<" for each level - current level - "<<level<<endl;
            cout<<g[4].arrival<<" -  -  - - - - g[4].arrival "<<endl;
            for(scanned_node=all_nodes.begin();scanned_node!=all_nodes.end();++scanned_node){
               //cout<<" list has ----"<<endl;
               //cout<<*v_itr_full<<endl;
                cout<<"For each node fanin fanout -- current scanned node  -- "<<*scanned_node<<endl;
                if(g[*scanned_node].level == level && level == 0){
                    cout<<"g[*scanned_node].level == level && level == 0"<<endl;
                    g[*scanned_node].arrival = 0;
                }

                if(g[*scanned_node].level == level && level != 0){
                    cout<<"g[*scanned_node].level == level && level != 0"<<endl;
                    vector<int> scanned_node_fanins= g[*scanned_node].fanins;
                    vector<int>::iterator scanned_node_fanins_itr;
                    cout<<g[4].arrival<<" -  -  - - - - g[4].arrival "<<endl;

                    for(scanned_node_fanins_itr=scanned_node_fanins.begin();scanned_node_fanins_itr != scanned_node_fanins.end();++scanned_node_fanins_itr){
                        cout<<arrival<<" - This is the arrival || "<<g[*scanned_node].delay<<" - delay of current cell "<<g[*scanned_node].type<<" - arrival time of parent cell "<<g[*scanned_node_fanins_itr].type<<" - "<<g[*scanned_node_fanins_itr].arrival<<endl;
                        if(g[*scanned_node_fanins_itr].arrival+g[*scanned_node].delay>arrival){
                            arrival = g[*scanned_node_fanins_itr].arrival+g[*scanned_node].delay;
                            cout<<"arrival"<<arrival<<"g[*scanned_node_fanins_itr].arrival"<<g[*scanned_node_fanins_itr].arrival<<"g[*scanned_node].delay"<<g[*scanned_node].delay<<" *scanned_node_fanins_itr"<<*scanned_node_fanins_itr<<endl;
                        }
                         cout<<arrival<<" - This is the arrival || "<<g[*scanned_node].delay<<" - delay of current cell "<<g[*scanned_node].type<<" - arrival time of parent cell "<<g[*scanned_node_fanins_itr].type<<" - "<<g[*scanned_node_fanins_itr].arrival<<endl;
                    }

                }

                g[*scanned_node].arrival = arrival;
                cout<<"*scanned_node - "<<*scanned_node<<" g[*scanned_node].arrival "<<g[*scanned_node].arrival<<endl;
                arrival = DBL_MIN;
                if(g[*scanned_node].type == "PO"){
                    max_delay_for_path = g[*scanned_node].arrival;

                    if(max_delay_for_path > delay)
                        delay = max_delay_for_path;
                    cout<<"delay inside if"<<delay<<endl;
                }


            }



        }    



    }

    return delay;

}

// for required time val need not be changed to tempval as both rise arrival,fall arrival and rise delay and fall delay values are known. rise required =rise arrival+rise delay. 

vector<int> required_time(double delay,int levels,int size,Graph &g)
{

    cout<<"------------------Inside the required_time ----------------------------"<<endl;
    slackset.clear();
    vector<int>::iterator intit;
    int num=size-1;
    double arrival,slack,required;
        required=DBL_MAX;
        int tempnode,markednode=size-1;
        double maxslack=DBL_MIN;
        int count=0;
        int val;
        for(int i=levels;i>=0;i--)
        {
            cout<<"Inside leveL for - "<<i<<endl;
         for(int f=num;f>=0;f--)
         {
            cout<<"Inside the second for for node no  - "<<f<<endl;
             if(g[f].level==i && g[f].type=="PO")
             {  
                cout<<"g[f].level==i && g[f].type==PO"<<endl;
                 
                 g[f].slack=delay - g[f].arrival;

                cout<<g[f].level<<"|| "<<g[f].type<<" - ||g[f].slack "<<g[f].slack<<" delay :"<<delay<<" arrival :"<<g[f].arrival; 
             }
//             else if(g[f].level==i && i!=levels && i!=0 && g[f].type=="PO")
//             {
//                 g[f].slack[0]=delay-g[f].arrival[0];
//                 g[f].slack[1]=delay-g[f].arrival[1];
//             }
             
             else if(g[f].level==i && g[f].type!="PI" && g[f].type!="PO")
             {
                    cout<<"g[f].level==i && g[f].type!=PI && g[f].type!=PO"<<endl;
                     for(intit=g[f].fanouts.begin();intit<g[f].fanouts.end();intit++)
                    {

                        tempnode=*intit;
                        double output_arrival_time=g[tempnode].slack + g[tempnode].arrival;
                        cout<<tempnode<<" - tempnode||"<<" tempnode type :"<<g[tempnode].type<<" g[tempnode].slack :"<<g[tempnode].slack<<" g[tempnode].arrival :"<<g[tempnode].arrival<<"required"<<required<<endl;
                        if(output_arrival_time - g[tempnode].delay<required){
                            
                            cout<<"Printing required before -- "<<required<<endl;    
                            required=output_arrival_time - g[tempnode].delay;
                            cout<<"Printing required after -- "<<required<<endl;    
                        }


                        cout<<required<<" : required"<<endl;
                    }

                        g[f].slack=required - g[f].arrival;
                        cout<<g[f].level<<"|| "<<g[f].type<<"g[f].slack "<<g[f].slack<<" required "<<required<<" g[f].arrival :"<<g[f].arrival<<endl;
//                        if(g[f].averageslack>maxslack)
//                        {
//                            markednode=f;
//                            maxslack=g[f].averageslack;
//                        }
                              
                required=DBL_MAX;
             }
         }
        }

        //      for(int i=0;i<num;i++)
        //          if(g[i].type!="PI"&&g[i].type!="PO"&&g[i].numreplacements<1&&g[i].critical==0)//num replacement was previously 2
        //          {
        //              cout<<"g[i].type!=PI &&g[i].type!=PO && g[i].numreplacements<1&&g[i].critical==0"<<endl;   
        //              cout<<" pushing i:g[i].cost[0] --> "<<i<<" : "<<g[i].cost[0]<<endl;
        //              slackset.push_back(std::make_pair(i,g[i].cost[0]));

        //              //slackset.push_back(std::make_pair(i,g[i].cost[1]));
        //          }

        // sort(slackset.begin(),slackset.end(),mysortfunction);
        // ofstream myfile;
        // myfile.open("slackfile.txt",ios::out|ios::app);
        // myfile<<"the nodes in the decreasing order of slack are\n";
        // for(int i=0;i<slackset.size();i++)
        // {
        //     int temp=slackset[i].first;
        //     myfile<<temp<<" "<<g[temp].cost[0]<<" "<<g[temp].numreplacements<<" "<<g[temp].critical<<"\n";     
        // } 
        // myfile<<"\n";

         vector<int> nodevect;
        // for(int i=0;i<slackset.size();i++)
        // {
        //     cout<<"pushing slackset[i].first :"<<slackset[i].first<<endl;
        //     int temp=slackset[i].first;
        //     nodevect.push_back(temp);
        // }
         return nodevect; 



}

void sort_function_1(Graph &g, vector<int> list_of_nodes_on_path){
//SF1 -diff_leakage_power/diff_delay
    vector<int>::iterator itr;


            //for(int j=0;j<vertex_count-1;j++)
            
            for(itr = list_of_nodes_on_path.begin();itr!=list_of_nodes_on_path.end();++itr){
                int j=*itr;

                cout<<"-- Inside the For ---"<<endl;
                double tempdelay=average(risedelaymap[j][1],falldelaymap[j][1]);
                cout<<" tempdelay : "<<tempdelay<<endl;
                string temp=lvt_gate_fp_map[g[j].type];
                string fm=temp.substr(0,temp.length()-3);
                string searchby=fm+"hvt";    

                // string searchby= hvt_gate_fp_map[g[j].type];
                double leakage=hvtleakagemap[searchby];
                // double tempdelay1=average(risedelaymap[j][2],falldelaymap[j][2]);
                // double leakage1=hvtleakagemap[g[j].type];
                double hvt_cost=(leakage-g[j].leakage)/(tempdelay-g[j].delay);
                cout<<" leakage : "<<leakage<<" g[j].leakage :"<<g[j].leakage<<" tempdelay :"<<tempdelay<<" g[j].delay : "<<g[j].delay<<endl;
                // double hvt_cost=(leakage1-g[j].leakage)/(tempdelay1-g[j].delay);
                g[j].cost[0]=hvt_cost;
                cout<<" g[j].cost[0] "<<g[j].cost[0]<<endl;
                // g[j].cost[1]=hvt_cost;
                        
            }

}

void sort_function_2(Graph &g, vector<int> list_of_nodes_on_path){
//SF2= -diff_leakage_power*slack
            vector<int>::iterator itr;
            for(itr = list_of_nodes_on_path.begin();itr!=list_of_nodes_on_path.end();++itr){
                int j=*itr;
                
                
                double slack = g[j].slack;
                 string temp=lvt_gate_fp_map[g[j].type];
                string fm=temp.substr(0,temp.length()-3);
                string searchby=fm+"hvt";    

                // string searchby= hvt_gate_fp_map[g[j].type];
                double leakage=hvtleakagemap[searchby];
                // double tempdelay1=average(risedelaymap[j][2],falldelaymap[j][2]);
                // double leakage1=hvtleakagemap[g[j].type];
                double hvt_cost=(leakage-g[j].leakage)*slack;
                cout<<" leakage : "<<leakage<<" g[j].leakage :"<<g[j].leakage<<" slack :"<<slack<<endl;
                // double hvt_cost=(leakage1-g[j].leakage)/(tempdelay1-g[j].delay);
                g[j].cost[0]=hvt_cost;
                cout<<" g[j].cost[0] "<<g[j].cost[0]<<endl;
                // g[j].cost[1]=hvt_cost;
                        
            }
    
}

void sort_function_3(Graph &g, vector<int> list_of_nodes_on_path){
//SF3 = -diff_leakage_power/(diff_delay * nos_of_paths)
            vector<int>::iterator itr;
            for(itr = list_of_nodes_on_path.begin();itr!=list_of_nodes_on_path.end();++itr){
                int j=*itr;

                cout<<"-- Inside the For ---"<<endl;
                double tempdelay=average(risedelaymap[j][1],falldelaymap[j][1]);
                cout<<" tempdelay : "<<tempdelay<<endl;
                string temp=lvt_gate_fp_map[g[j].type];
                string fm=temp.substr(0,temp.length()-3);
                string searchby=fm+"hvt";    

                // string searchby= hvt_gate_fp_map[g[j].type];
                double leakage=hvtleakagemap[searchby];
                // double tempdelay1=average(risedelaymap[j][2],falldelaymap[j][2]);
                // double leakage1=hvtleakagemap[g[j].type];
                double hvt_cost=(leakage-g[j].leakage)/((tempdelay-g[j].delay)*(g[j].fanins.size()*g[j].fanouts.size()));
                cout<<" leakage : "<<leakage<<" g[j].leakage :"<<g[j].leakage<<" tempdelay :"<<tempdelay<<" g[j].delay : "<<g[j].delay<<endl;
                cout<<" number of paths : "<<g[j].fanins.size()*g[j].fanouts.size()<<endl;
                // double hvt_cost=(leakage1-g[j].leakage)/(tempdelay1-g[j].delay);
                g[j].cost[0]=hvt_cost;
                cout<<" g[j].cost[0] "<<g[j].cost[0]<<endl;
                // g[j].cost[1]=hvt_cost;
                        
            }            

}

void sort_function_4(Graph &g, vector<int> list_of_nodes_on_path){
// SF4 = -diff_leakage_power*slack/nos_of_paths
            vector<int>::iterator itr;
            for(itr = list_of_nodes_on_path.begin();itr!=list_of_nodes_on_path.end();++itr){
                int j=*itr;

                cout<<"-- Inside the For ---"<<endl;
                //double tempdelay=average(risedelaymap[j][1],falldelaymap[j][1]);
                //cout<<" tempdelay : "<<tempdelay<<endl;
                string temp=lvt_gate_fp_map[g[j].type];
                string fm=temp.substr(0,temp.length()-3);
                string searchby=fm+"hvt";    

                double slack = g[j].slack;
                // string searchby= hvt_gate_fp_map[g[j].type];
                double leakage=hvtleakagemap[searchby];
                // double tempdelay1=average(risedelaymap[j][2],falldelaymap[j][2]);
                // double leakage1=hvtleakagemap[g[j].type];
                double hvt_cost=(leakage-g[j].leakage)* slack/(g[j].fanins.size()*g[j].fanouts.size());
                cout<<" leakage : "<<leakage<<" g[j].leakage :"<<g[j].leakage<<" g[j].delay : "<<g[j].delay<<endl;
                cout<<" number of paths : "<<g[j].fanins.size()*g[j].fanouts.size()<<endl;
                // double hvt_cost=(leakage1-g[j].leakage)/(tempdelay1-g[j].delay);
                g[j].cost[0]=hvt_cost;
                cout<<" g[j].cost[0] "<<g[j].cost[0]<<endl;
                // g[j].cost[1]=hvt_cost;
                        
            }
    
}

void sort_function_5(Graph &g, vector<int> list_of_nodes_on_path){
// SF5 = -diff_leakage_power*slack/diff_delay * nos_of_paths
            vector<int>::iterator itr;
        for(itr = list_of_nodes_on_path.begin();itr!=list_of_nodes_on_path.end();++itr){
                int j=*itr;

                cout<<"-- Inside the For ---"<<endl;
                double tempdelay=average(risedelaymap[j][1],falldelaymap[j][1]);
                cout<<" tempdelay : "<<tempdelay<<endl;
                string temp=lvt_gate_fp_map[g[j].type];
                string fm=temp.substr(0,temp.length()-3);
                string searchby=fm+"hvt";    

                double slack = g[j].slack;
                // string searchby= hvt_gate_fp_map[g[j].type];
                double leakage=hvtleakagemap[searchby];
                // double tempdelay1=average(risedelaymap[j][2],falldelaymap[j][2]);
                // double leakage1=hvtleakagemap[g[j].type];
                double hvt_cost=(leakage-g[j].leakage)* slack/((tempdelay-g[j].delay)*(g[j].fanins.size()*g[j].fanouts.size()));
                cout<<" leakage : "<<leakage<<" g[j].leakage :"<<g[j].leakage<<" tempdelay :"<<tempdelay<<" g[j].delay : "<<g[j].delay<<endl;
                cout<<" number of paths : "<<g[j].fanins.size()*g[j].fanouts.size()<<endl;
                // double hvt_cost=(leakage1-g[j].leakage)/(tempdelay1-g[j].delay);
                g[j].cost[0]=hvt_cost;
                cout<<" g[j].cost[0] "<<g[j].cost[0]<<endl;
                // g[j].cost[1]=hvt_cost;
                        
            }
    
}

void sort_function_slack(Graph &g, vector<int> list_of_nodes_on_path){

            vector<int>::iterator itr;
        for(itr = list_of_nodes_on_path.begin();itr!=list_of_nodes_on_path.end();++itr){
                int j=*itr;

                cout<<"-- Inside the For ---"<<endl;
                
                double cost=g[j].slack;
                cout<<" cost : "<<cost<<" slack :"<<g[j].slack<<endl;

                // double hvt_cost=(leakage1-g[j].leakage)/(tempdelay1-g[j].delay);
                g[j].cost[0]=cost;
                cout<<" g[j].cost[0] "<<g[j].cost[0]<<endl;
                // g[j].cost[1]=hvt_cost;
                        
            }
}   

void sort_function_fanouts(Graph &g, vector<int> list_of_nodes_on_path){

        vector<int>::iterator itr;
        for(itr = list_of_nodes_on_path.begin();itr!=list_of_nodes_on_path.end();++itr){
                int j=*itr;

                cout<<"-- Inside the For ---"<<endl;
                
                int fanouts = g[j].fanouts.size();
                double cost=fanouts;
                cout<<" cost : "<<cost<<" # of fan_outs :"<<fanouts<<endl;

                // double hvt_cost=(leakage1-g[j].leakage)/(tempdelay1-g[j].delay);
                g[j].cost[0]=cost;
                cout<<" g[j].cost[0] "<<g[j].cost[0]<<endl;
                // g[j].cost[1]=hvt_cost;
                        
            }

}

vector<int> required_time_for_selected_path(double delay, Graph &g,vector<int> list_of_nodes_on_path){
    double arrival,slack,required;
    double maxslack=DBL_MIN;
    required=DBL_MAX;
    int tempnode;
    int count;
    double val;

    double level;
    vector<int>::iterator v_itr;
    vector<int>::iterator scanned_node;
    vector<int> all_nodes;
    cout<<"Inside - required_time_for_selected_path "<<endl;
    for(v_itr=list_of_nodes_on_path.begin();v_itr!=list_of_nodes_on_path.end();++v_itr){
        all_nodes=all_nodes_at_level_plus_minus_n(*v_itr,g);
        
        for(level =  g[*v_itr].level + 3; level>=  g[*v_itr].level - 3; level--){
            
            cout<<"----------------------------- Relative Level ------------"<<level<<endl;
            for(scanned_node=all_nodes.begin();scanned_node!=all_nodes.end();++scanned_node){
                if(g[*scanned_node].level == level && g[*scanned_node].type=="PO"){
                    cout<<"Inside g[*scanned_node].level == level && g[*scanned_node].type==PO"<<endl;
                    g[*scanned_node].slack = delay - g[*scanned_node].arrival;
                    cout<<*scanned_node<<" g[*scanned_node].slack "<<g[*scanned_node].slack<<" delay "<<delay<<" g[*scanned_node].arrival "<<g[*scanned_node].arrival<<endl;

                } 
                else if(g[*scanned_node].level==level && g[*scanned_node].type!="PI" && g[*scanned_node].type!="PO")
                {
                    cout<<"g[*scanned_node].level==level && g[*scanned_node].type!=PI && g[*scanned_node].type!=PO"<<endl;
                    vector<int> scanned_node_fanouts= g[*scanned_node].fanouts;
                    vector<int>::iterator scanned_node_fanouts_itr;
                    cout<<"--------*scanned_node------------ "<<*scanned_node<<endl;
                    for(scanned_node_fanouts_itr=scanned_node_fanouts.begin();scanned_node_fanouts_itr != scanned_node_fanouts.end();++scanned_node_fanouts_itr){
                        tempnode = *scanned_node_fanouts_itr;
                        double output_arrival_time=g[tempnode].slack + g[tempnode].arrival;
                         cout<<tempnode<<" - tempnode||"<<" tempnode type :"<<g[tempnode].type<<" g[tempnode].slack :"<<g[tempnode].slack<<" g[tempnode].arrival :"<<g[tempnode].arrival<<"required"<<required<<endl;
                        if(output_arrival_time - g[tempnode].delay<required){
                            
                            cout<<"Printing required before -- "<<required<<endl;    
                            required=output_arrival_time - g[tempnode].delay;
                            cout<<"Printing required after -- "<<required<<endl;    
                        }
                        

                    }
                    g[*scanned_node].slack=required - g[*scanned_node].arrival;
                    required=DBL_MAX;

                }
                
            }            

        }    
    }

    return sorting_by_cost_function(g,list_of_nodes_on_path);
    //return sort_function_slack(g,list_of_nodes_on_path);
}



void leakage_assignment(map<string,double> leakagemap,int level,int size,Graph &g,int vt)
{
     vector<int>::iterator intit;
    set<string>::iterator setit;
    int num=size;
    int tempnode;
    bool tempstate=0;
    for(int f=0;f<num-1;f++)
     {
         if(g[f].type!="PI" && g[f].type!="PO")
         {
            string searchby="";
            if(vt==1)
                searchby=lvt_gate_fp_map[g[f].type];
            else{
                string temp=lvt_gate_fp_map[g[f].type];
                string fm=temp.substr(0,temp.length()-3);
                searchby=fm+"hvt";
            }
            g[f].leakage=leakagemap[searchby];
            cout<<"g[f].leakage :"<<g[f].leakage<<" searchby :"<<searchby<<endl;
         }
         else
         {
             g[f].leakage=0;
         }
         cout<<"g[f].leakage :"<<g[f].leakage<<endl;
    }



}

void updating_load_capacitance_for_a_particular_cell(Graph &g, int i){
            vector<int> successor_map= g[i].fanouts;
            std::vector<int>::iterator Vitr;
            // float loadCapacitance=0.001594;
            float loadCapacitance=0;
            int count=0;
            for(Vitr=successor_map.begin();Vitr!=successor_map.end();Vitr++){
                int cell_no=*Vitr;
                string searchby="";
                searchby=lvt_gate_fp_map[g[cell_no].type];
                //float current_cell_load = capacitancemap[searchby];
                float current_cell_load = g[cell_no].cellCapacitance;
                loadCapacitance= loadCapacitance+current_cell_load;
                count=count+1;
            }

            int number_of_fanouts=successor_map.size();
            double initial_net_capacitance= 0.028;
            double initial_net_capacitance_less_fanout= 0.028;
            double decline_rate=0.000435;
            int rf=  number_of_fanouts - 10;
            if(loadCapacitance == 0){
                if(number_of_fanouts == 1)
                    g[i].capacitance= 0.02;

                else if(number_of_fanouts>1 && number_of_fanouts <=10)
                    g[i].capacitance= initial_net_capacitance_less_fanout+(number_of_fanouts - 1)*0.01;
                
                else        
                    g[i].capacitance=initial_net_capacitance+(rf-1)*0.001+9*0.01;

            }else{ 
                
                if(number_of_fanouts == 1)
                    g[i].capacitance= 0.02+loadCapacitance;
                else if(number_of_fanouts >1 && number_of_fanouts <=10)
                     g[i].capacitance=loadCapacitance+initial_net_capacitance_less_fanout+(number_of_fanouts - 1)*0.01;
                else   
                    g[i].capacitance=loadCapacitance+initial_net_capacitance+(rf-1)*0.001+9*0.01;
                
            }
}

int main(int argc,char *argv[])
{
    //string variable to read lines from files
    string line;
    
    //vertex_count stores the number of vertices
    int vertex_count;
    
    //num is a temporary variable, used for parsing
    int num;
    if(argc!=31)//rvt leakage removed 
        //cout<<"usage:./a.out hgrfile typefile fanout fanin lvt_gate_lookup hvt_gate_lookup risedelayfile falldelayfile hvtleakage lvtleakage parityfile"<<endl;
        cout<<"usage:./a.out hgrfile typefile fanout fanin lvt_gate_lookup hvt_gate_lookup lvt_cell_rise lvt_cell_fall lvt_rise_transition lvt_fall_transition hvt_cell_rise hvt_cell_fall hvt_rise_transition hvt_fall_transition load_capacitance_max_combined_2 parity_combined lvt_index_cell_rise_rec lvt_index_cell_fall_rec lvt_index_rise_transition_rec lvt_index_fall_transition_rec hvt_index_cell_rise_rec hvt_index_cell_fall_rec hvt_index_rise_transition_rec hvt_index_fall_transition_rec  aes_128.rise_file aes_128.fall_file hvt_cell_leakage_power lvt_cell_leakage_power parity_combined <rise slew> <fall slew>";
    else
    {   
        //declaring a acyclic bidirectional (allowing two way traversal, 1 for fanin, 1 for fanout) graph
        Graph g; 
        
        //replacement window size used in adaptive sta
        //int windowsize=atoi(argv[9]);
        
        //reading hypergraph file
        ifstream hgrfile(argv[1]);
        if(hgrfile.is_open())
        {
            getline(hgrfile,line);
            //cout<<line<<endl;
            //line i contains all the edges pertaining to vertex i
            //the line is read and split into an array using stringstream
            std::istringstream ss(line);
            
            //first line contains total number of vertices
            ss >> vertex_count;
            
            //increment by 1, to allow for the dummy node (acts as a source node for all PIs, which makes BFS very easy)
            vertex_count++;
            cout<<"total number of nodes in graph="<<vertex_count<<endl;
            //cout<<"created graph"<<endl;
            int vertex_iterator=0;
            while(hgrfile.good())
            {
               // string line;
                getline(hgrfile,line);
                //cout<<line<<endl;
               
                if(!line.empty())
                {
                    istringstream ss(line);
                    /********************Adds edges between vertex_iterator and all its outgoing vertices*************************/
                    while(ss >> num)
                    {
                        boost::add_edge(vertex_iterator,num,g);
            //            //cout<<num<<"\t";
                    }
                     /*********************************************/
                }
                vertex_iterator++;
//                //cout<<vertex_iterator<<endl;
            }
        }
            //next, we will be assigning properties to each node, based on type
            //since dummy node does have any property, it is excluded by setting num to vertex_count-1
            //from now on, in any for loop, we will be iterating from 0 to num
            num=vertex_count-1;
            
            //line i in argv[2] contains type of node i in the graph (PI, PO or cell type)
            ifstream typefile(argv[2]);
            
            //cout<<"reading types"<<endl;
            for(int i=0;i<num;i++) // we assign the gates their types pi po or cell type
            {
                if(typefile.is_open())
                    getline(typefile,line);
                g[i].type=line;
            }
            int j,k;
            cout<<"identifying fanins/ fanouts"<<endl;
            //now, we identify fanins and fanouts for each node
            
            //This for loop will identify fanouts for each node
            cout<< "Fan outs "<<endl;
            ifstream fanoutfile(argv[3]);
            for(int i=0;i<=num;i++)
            {
                
                if(fanoutfile.is_open())
                {
                    vector<int>fanouts;
                    getline(fanoutfile,line);
                    std::istringstream ss(line);
                    
                    while(ss>>j){
                        // cout<<i<<" - "<<j<<endl;
                        fanouts.push_back(j);
                    }

                    if(i!=0){

                        g[i-1].fanouts=fanouts;

                        cout<<" Printing fan outs "<<endl;
                        cout<<g[i-1].type<<" --- "<<i-1<<endl;
                        vector<int>::iterator v;
                         for(v=fanouts.begin();v!=fanouts.end();++v){
                            cout<<*v<<"\t";
                        }
                        cout<<endl;
                    }

                    // cout<<i<<" - "<<g[i].fanouts<<endl;
                }
            

            }
            
            //Thiis for loop will identify fanins for each node
            cout<< " Fan ins "<<endl;
            ifstream faninfile(argv[4]);
            for(int i=0;i<=num;i++)
            {
                if(faninfile.is_open())
                {
                    vector<int>fanins;
                    getline(faninfile,line);
                    //edge contains two fields - second:boolean variable, which is true if edge exists from j->i
                    //                                                    which is false otherwise
                    std::istringstream ss(line);
                    
                    while(ss>>j){
                        // cout<<i<<" - "<<j<<endl;
                        fanins.push_back(j);

                    }
                    // if(i!=0){

                        g[i].fanins=fanins;

                        cout<<" Printing fan ins "<<endl;
                        cout<<g[i].type<<" --- "<<i<<endl;
                        vector<int>::iterator v;
                         for(v=fanins.begin();v!=fanins.end();++v){
                            cout<<*v<<"\t";
                        }
                        cout<<endl;
                    // }
                    // cout<<i<<" - "<<g[i].fanins<<endl;
                }
            }
            cout<<"fanouts and Fanins done"<<endl;

            //new gate mapping code inserted by PBR on 22nd June 2019
            // map<string,string > lvt_gate_fp_map;
            // map<string,string > lvt_fp_gate_map;
            cout<<"reading LVT map file "<<endl;
            ifstream lvtmapfile(argv[5]);
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
                    lvt_gate_fp_map.insert(std::pair<string,string>(s,result[0])); 
                    //cout<<"after inserting"<<endl;       
                }

            // map<string,string > hvt_gate_fp_map;
            // map<string,string > hvt_fp_gate_map;
            
            cout<<"reading HVT map file "<<endl;
            ifstream hvtmapfile(argv[6]);
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
                    //std::replace( s.begin(), s.end(), '\n', '');
                    cout<<s<<endl;    
                    hvt_gate_fp_map.insert(pair<string,string>(result[0],s));
                    hvt_gate_fp_map.insert(pair<string,string>(s,result[0])); 

                }    

            ////////////////////////////////////////////////////////////////////////////


            ///////////////////////////////////////////Inserting the additional code required for slew and delay calculation////////////////////////////////////////
                //int pi;
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
            ifstream risefile(argv[7]);
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
            ifstream fallfile(argv[8]);
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
            ifstream risetransitionfile(argv[9]);
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
            ifstream falltransitionfile(argv[10]);
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
        linecount=0;
        risefile.close();
        fallfile.close();
        risetransitionfile.close();
        falltransitionfile.close();    

        cout<<"starting with the hvt files"<<endl;
            cout<<"Entering HVt rise file "<<endl;    
            risefile.open(argv[11]);
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
            fallfile.open(argv[12]);
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
            risetransitionfile.open(argv[13]);
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
            falltransitionfile.open(argv[14]);
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
            ifstream capfile(argv[15]);
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

            map<string,int> paritymap;
            ifstream parityfile(argv[16]);
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
    ifstream lvtcellrisemap(argv[17]);
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
    ifstream lvtcellfallmap(argv[18]);
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
    ifstream lvtrisetransitionmap(argv[19]);
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
    ifstream lvtfalltransitionmap(argv[20]);
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
    ifstream hvtcellrisemap(argv[21]);
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
    ifstream hvtcellfallmap(argv[22]);
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
    ifstream hvtrisetransitionmap(argv[23]);
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
    ifstream hvtfalltransitionmap(argv[24]);
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





            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
            //integer variable to store the number of PIs
            int pi;

            //hash table where key is the gate type and value is the gate load capacitance - not used in the program
            //map<string,double> capacitancemap;

            //temporary string variable used 
            string gate_type;
            
            //temporary floating point variable to store gate parameters
            double gate_parameter;

            //adding edges from dummy node (vertex_iterator=num i.e last node in the graph), to all PIs
            for(int i=0;i<num;i++) 
            {
                if(g[i].type=="PI")
                    boost::add_edge(num,i,g);
            }
            //cout<<"dummy node added"<<endl;



            // Reading gate parity added on 12-09-2013
//            string choice=argv[6]; 

            //hash maps for different gate parameters- 
            //map<string,int> paritymap;
            // map<int,vector<double>> risedelaymap;
            // map<int,vector<double>> falldelaymap;
            // map<string,double> lvtleakagemap;
            // // map<string,double> rvtleakagemap;
            // map<string,double> hvtleakagemap;

            ifstream risedelayfile(argv[25]);

            //count is a debug variable, used to count the number of lines, while reading the file
            //int count=0;
            int vertex_iterator=0;
            while(risedelayfile.good())
            {
                getline(risedelayfile,line);
                //count=0;

                stringstream ss(line);
                
                while(ss>>gate_parameter)
                {
                    risedelaymap[vertex_iterator].push_back(gate_parameter);
                    
                }
                vertex_iterator++;
            }
            risedelayfile.close();

            ifstream falldelayfile(argv[26]);
            vertex_iterator=0;
            //count=0;
            while(falldelayfile.good())
            {
                getline(falldelayfile,line);
                //count=0;

                stringstream ss(line);
                
                while(ss>>gate_parameter)
                {

                        falldelaymap[vertex_iterator].push_back(gate_parameter);
                    
                }
                vertex_iterator++;
            }
            falldelayfile.close();

            /*ifstream rvtleakagefile(argv[7]);
            while (rvtleakagefile.good())
            {
                getline(rvtleakagefile,line);
                stringstream ss(line);
                ss>>gate_type;
                while(ss>>gate_parameter)
                {

                    rvtleakagemap[gate_type]=gate_parameter;
                }

            }
            //cout<<"rvtleakage done"<<endl;
            rvtleakagefile.close();*/

            ifstream hvtleakagefile(argv[27]);
            while (hvtleakagefile.good())
            {
                getline(hvtleakagefile,line);
                stringstream ss(line);
                ss>>gate_type;
                while(ss>>gate_parameter)
                {

                    hvtleakagemap[gate_type]=gate_parameter;
                }

                cout<<hvtleakagemap[gate_type]<<endl;

            }
            cout<<"hvtleakage done"<<endl;
            hvtleakagefile.close();

            ifstream lvtleakagefile(argv[28]);
            while (lvtleakagefile.good())
            {
                getline(lvtleakagefile,line);
                stringstream ss(line);
                ss>>gate_type;
                while(ss>>gate_parameter)
                {

                    lvtleakagemap[gate_type]=gate_parameter;
                }

                cout<<lvtleakagemap[gate_type]<<endl;

            }
            cout<<"lvtleakage done"<<endl;
            lvtleakagefile.close();

            string s_pi_r= argv[29];
            string s_pi_f= argv[30];
            pi_slew_r=::atof(s_pi_r.c_str())/100;
            pi_slew_f=::atof(s_pi_f.c_str())/100;
            int tempnode;
            // ifstream parityfile(argv[29]);
            // while(parityfile.good())
            // {
            //     getline(parityfile,line);
            //     stringstream ss(line);
            //     ss >> gate_type;
            //     while(ss>>gate_parameter)
            //         paritymap[gate_type]=gate_parameter;
            // }

            // cout<<"cap done"<<endl;
            // ifstream capfile(argv[12]);
            // while(capfile.good())
            // {
            //     getline(capfile,line);
            //     stringstream ss(line);
            //     ss>>tempword;
            //     cout<<"tempword :"<<tempword<<endl;
            //     while(ss>>tempnum){
            //         capacitancemap[tempword]=tempnum;
            //        // cout<<" com on --"<<capacitancemap[tempword]<<endl;
            //     }

            // }

        //declaring array of POs, for the purpose of level ordering
        vector<int> povector;

        //vector iterator, to iterate over any integer vector
        vector<int>::iterator intit1;

        //cout<<"vertex_count is"<<vertex_count<<endl;
            int picount=0,pocount=0;        
          //for(int i=0;i<num;i++)
          //    cout<<i<<"\t"<<g[i].delay[0]<<"\t"<<g[i].delay[1]<<endl;
          
          /**********************This loop calculates picount, pocount **************/
            for(int i=0;i<num;i++)
          {
            if(g[i].type=="PO")
            {
          //cout<<"FOR PO "<<i<<" :fanins are"<<"\t";
                povector.push_back(i);
          //      for(intit1=g[i].fanins.begin();intit1<g[i].fanins.end();intit1++)
          //        {
          //            cout<<*intit1<<"\t";
          //            
          //        }
          //        cout<<"fanout size "<<g[i].fanouts.size();
          //        cout<<"\n";
                pocount++;
            }
            else if(g[i].type=="PI")
                picount++;

        }


        /***************************************************************************/

        //initializing all gates to level=-1
        for (int i=0;i<=num;i++)
            g[i].level=-1;

        //markednode is used to identify which node to replace
        int markednode;
        //cout<<"did bfs"<<endl;
 
        //temporary primary output variable
        int tempo;

        //temporary variable storing level information
        int maxlevel=-1;

        //for safety sake, patanjali initializes level of dummy node to -1 again (redundancy)
        g[vertex_count-1].level=-1;

        /***********************************level ordering**********************/
        for(int i=0;i<povector.size();i++)
        {
            tempo=povector[i];
            g[tempo].level=gate_level(tempo,g);
            if(g[tempo].level>maxlevel)
                maxlevel=g[tempo].level;
            

        }

        cout<<"Max Level "<< maxlevel<<endl;
        /**********************************************************************/

        //////////////////////////////////////////////////////////////////////////////////////////
        // Adding code for finding the nodes in level l-3 to l+3 where l is the current level of 
        // the present node
        //////////////////////////////////////////////////////////////////////////////////////////
         //vector<int> list_of_nodes_on_path{2073,22203,132952,75469,57714,14926,103161,121002,66894,66893,66892,163589};
         //vector<int> list_of_nodes_on_path{129188, 87016, 79105, 124092, 146547, 146895, 118008, 92196, 92195};
        vector<int> list_of_nodes_on_path{111968,131550,131549,36040,50993,41372,12907,27155};


         //  vector<int>::iterator v_itr;
         //  vector<int>::iterator v_itr_full;
         // // vector<int>::iterator v_itr_fanout;
         // // vector<int> fanin_list_upto_3;
         // // vector<int> fanout_list_upto_3;   
         //  vector<int> all_nodes;

         //  for(v_itr=list_of_nodes_on_path.begin();v_itr!=list_of_nodes_on_path.end();++v_itr){
         //        all_nodes=all_nodes_at_level_plus_minus_n(*v_itr,g);
         //        for(v_itr_full=all_nodes.begin();v_itr_full!=all_nodes.end();++v_itr_full){
         //            cout<<" list has ----"<<endl;
         //            cout<<*v_itr_full<<endl;
         //        }    



         //  }

         // for(v_itr=list_of_nodes_on_path.begin();v_itr!=list_of_nodes_on_path.end();++v_itr){
         //    cout<<"Finding predecessors for the node -- "<<*v_itr<<endl;
         //    find_predecessors(g,*v_itr,fanin_list_upto_3,g[*v_itr].level,0);

         //    for(v_itr_fanin=fanin_list_upto_3.begin();v_itr_fanin!=fanin_list_upto_3.end();++v_itr_fanin){
         //        cout<<"Fan in list has ----"<<endl;
         //        cout<<*v_itr_fanin<<endl;
         //    }

         //    fanin_list_upto_3={};

         // }

         // //Here starts the code for finding the fanout

         // for(v_itr=list_of_nodes_on_path.begin();v_itr!=list_of_nodes_on_path.end();++v_itr){
         //    cout<<"Finding successors for the node -- "<<*v_itr<<endl;
         //    find_successors(g,*v_itr,fanout_list_upto_3,g[*v_itr].level,0);

         //    for(v_itr_fanout=fanout_list_upto_3.begin();v_itr_fanout!=fanout_list_upto_3.end();++v_itr_fanout){
         //        cout<<"Fan out list has ----"<<endl;
         //        cout<<*v_itr_fanout<<endl;
         //    }

         //    fanout_list_upto_3={};

         // }
        

        ///////////////////////////////////////////////////////////////////////////////////////////
        /***since input netlist is LVt, we assign LVt leakage and delay values***/
        leakage_assignment(lvtleakagemap,maxlevel,vertex_count,g,1);
        for(int i=0;i<vertex_count-1;i++)
        {
            g[i].delay=average(risedelaymap[i][0],falldelaymap[i][0]);
            g[i].numreplacements=0;
            g[i].critical=0;
        }

        /////////////////////////////////Calculating just the present cells capacitance//////////////////////
        for(int i=0;i<=num ; i++){
                string searchby="";
                searchby=lvt_gate_fp_map[g[cell_no].type];
                g[i].cellCapacitance=capacitancemap[searchby];
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////


        /************************************************************************/

        /////////////////////////////////////////////////////////////////////////////////////////////
        //Adding the capacitance assignment code
        for(int i = 0;i<=num;i++){
            vector<int> successor_map= g[i].fanouts;
            std::vector<int>::iterator Vitr;
            // float loadCapacitance=0.001594;
            float loadCapacitance=0;
            int count=0;
            for(Vitr=successor_map.begin();Vitr!=successor_map.end();Vitr++){
                int cell_no=*Vitr;
                string searchby="";
                searchby=lvt_gate_fp_map[g[cell_no].type];
                //float current_cell_load = capacitancemap[searchby];
                float current_cell_load = g[cell_no].cellCapacitance;
                loadCapacitance= loadCapacitance+current_cell_load;
                count=count+1;
            }

            int number_of_fanouts=successor_map.size();
            double initial_net_capacitance= 0.028;
            double initial_net_capacitance_less_fanout= 0.028;
            double decline_rate=0.000435;
            int rf=  number_of_fanouts - 10;
            if(loadCapacitance == 0){
                if(number_of_fanouts == 1)
                    g[i].capacitance= 0.02;

                else if(number_of_fanouts>1 && number_of_fanouts <=10)
                    g[i].capacitance= initial_net_capacitance_less_fanout+(number_of_fanouts - 1)*0.01;
                
                else        
                    g[i].capacitance=initial_net_capacitance+(rf-1)*0.001+9*0.01;

            }else{ 
                
                if(number_of_fanouts == 1)
                    g[i].capacitance= 0.02+loadCapacitance;
                else if(number_of_fanouts >1 && number_of_fanouts <=10)
                     g[i].capacitance=loadCapacitance+initial_net_capacitance_less_fanout+(number_of_fanouts - 1)*0.01;
                else   
                    g[i].capacitance=loadCapacitance+initial_net_capacitance+(rf-1)*0.001+9*0.01;
                
            }

            cout<<"XXOO : LVT "<<endl;
            // if(i==26149 || i==2789|| i==8616|| i==8602|| i==132791|| i==97948|| i==97947|| i==97945|| i==97944|| i==9663 || i==9662 || i==163436 ){
            //if(i==1222 || i==8470|| i==34941|| i==90979|| i==114959|| i==97723|| i==28195|| i==89227|| i==12963|| i==12964 || i==97720 || i==163311 ){
            if(j==111968|| j==131550|| j==131549|| j==36040|| j==50993|| j==41372|| j==12907|| j==27155){
               cout<<"XXOO : For i = "<<i<<" Capacitance :"<<g[i].capacitance<<" Initial Net Capacitance : "<<initial_net_capacitance<<" (number_of_fanouts)*decline_rate) :"<< (number_of_fanouts)*decline_rate<<endl;
               cout<<"number_of_fanouts :"<<number_of_fanouts<<"Decline rate :"<<decline_rate; 
            }

            
            
        }










        //////////////////////////////////////////////////////////////////////////////////////////////


        //cout<<"max level is "<<maxlevel;      

        //temporary vector used to store nodes sorted based on average slack
        vector<int> tempvect;


        vector<int>::iterator intit;

        
        //g[markednode].numreplacements==0 && (g[markednode].slack>average(risedelaymap[markednode][1],falldelaymap[markednode][1])-g[markednode].delay)



        /////////////////////////////////////////////////////////////////////////////////////////////
        // maxlevel=14;
        //double delay=arrival_time(10,vertex_count,g,1);
        cout<<"--------------------------- Calling the first arrival -----------------------------"<<endl;
        double delay=arrival_time(maxlevel,vertex_count,g,1);
        cout<<delay<<"Initial Delay"<<endl;
        //cout<<"delay="<<delay<<endl;

        //reading the filename, to extract the benchmark name
        string file=argv[1];

        tempvect=required_time(delay,maxlevel,vertex_count,g);
        
        double totalleakage=0;
            for(int j=0;j<vertex_count;j++)
            {
                
                totalleakage=g[j].leakage+totalleakage;
                       

            }


            sort_function_slack(g,list_of_nodes_on_path);
            // for(int j=0;j<vertex_count-1;j++)
            // {

            //     cout<<"-- Inside the For ---"<<endl;
            //     double tempdelay=average(risedelaymap[j][1],falldelaymap[j][1]);
            //     cout<<" tempdelay : "<<tempdelay<<endl;
            //     string temp=lvt_gate_fp_map[g[j].type];
            //     string fm=temp.substr(0,temp.length()-3);
            //     string searchby=fm+"hvt";    

            //     // string searchby= hvt_gate_fp_map[g[j].type];
            //     double leakage=hvtleakagemap[searchby];
            //     // double tempdelay1=average(risedelaymap[j][2],falldelaymap[j][2]);
            //     // double leakage1=hvtleakagemap[g[j].type];
            //     double hvt_cost=(leakage-g[j].leakage)/(tempdelay-g[j].delay);
            //     cout<<" leakage : "<<leakage<<" g[j].leakage :"<<g[j].leakage<<" tempdelay :"<<tempdelay<<" g[j].delay : "<<g[j].delay<<endl;
            //     // double hvt_cost=(leakage1-g[j].leakage)/(tempdelay1-g[j].delay);
            //     g[j].cost[0]=hvt_cost;
            //     cout<<" g[j].cost[0] "<<g[j].cost[0]<<endl;
            //     // g[j].cost[1]=hvt_cost;
                        
            // }
        cout<<"TOTAL LEAKAGE Before Replacement:"<<totalleakage<<endl;
        //////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////// Checking Arrival Time, required Time, rise delay, fall delay , 
        
        vector<int>::iterator path_itr;
        for(path_itr=list_of_nodes_on_path.begin();path_itr!=list_of_nodes_on_path.end();++path_itr){
            cout<<"---------------------------------------------------------------------------"<<endl;
            int current_node_on_path=*path_itr;
            cout<<"Node :"<<current_node_on_path<<"g["<<current_node_on_path<<"].capacitance :"<<g[current_node_on_path].capacitance<<endl;
            cout<<"Node :"<<current_node_on_path<<"g["<<current_node_on_path<<"].level :"<<g[current_node_on_path].level<<endl;
            cout<<"Node :"<<current_node_on_path<<"g["<<current_node_on_path<<"].type :"<<g[current_node_on_path].type<<endl;
            cout<<"Node :"<<current_node_on_path<<"g["<<current_node_on_path<<"].leakage :"<<g[current_node_on_path].leakage<<endl;
            cout<<"g["<<current_node_on_path<<"] Arrival Time -- "<<g[current_node_on_path].arrival<<endl;
            //cout<<"g["<<current_node_on_path<<"] required time -- "<<g[current_node_on_path].required_time<<endl;
            cout<<"g["<<current_node_on_path<<"] delay time -- "<<g[current_node_on_path].delay<<endl;
            cout<<"g["<<current_node_on_path<<"] number of replacements -- "<<g[current_node_on_path].numreplacements<<endl;
            cout<<"g["<<current_node_on_path<<"] critical -- "<<g[current_node_on_path].critical<<endl;
            cout<<"g["<<current_node_on_path<<"] slack -- "<<g[current_node_on_path].slack<<endl;
            cout<<"g["<<current_node_on_path<<"] average(risedelaymap[current_node_on_path][1],falldelaymap[current_node_on_path][1]) -- "<<average(risedelaymap[current_node_on_path][1],falldelaymap[current_node_on_path][1])<<endl;
            cout<<"average(risedelaymap[markednode][1],falldelaymap[markednode][1])-g[markednode].delay "<<average(risedelaymap[current_node_on_path][1],falldelaymap[current_node_on_path][1])-g[current_node_on_path].delay<<endl;
            cout<<"---------------------------------------------------------------------------"<<endl;
        }

          // cout<<"---------------------------------------------------------------------------"<<endl;
          //   int current_node_on_path=163435;
          //   cout<<"g["<<current_node_on_path<<"] Arrival Time -- "<<g[current_node_on_path].arrival<<endl;
          //   //cout<<"g["<<current_node_on_path<<"] required time -- "<<g[current_node_on_path].required_time<<endl;
          //   cout<<"g["<<current_node_on_path<<"] delay time -- "<<g[current_node_on_path].delay<<endl;
          //   cout<<"g["<<current_node_on_path<<"] number of replacements -- "<<g[current_node_on_path].numreplacements<<endl;
          //   cout<<"g["<<current_node_on_path<<"] critical -- "<<g[current_node_on_path].critical<<endl;
          //   cout<<"g["<<current_node_on_path<<"] slack -- "<<g[current_node_on_path].slack<<endl;
          //   cout<<"g["<<current_node_on_path<<"] average(risedelaymap[current_node_on_path][1],falldelaymap[current_node_on_path][1]) -- "<<average(risedelaymap[current_node_on_path][1],falldelaymap[current_node_on_path][1])<<endl;
          //   cout<<"average(risedelaymap[markednode][1],falldelaymap[markednode][1])-g[markednode].delay "<<average(risedelaymap[current_node_on_path][1],falldelaymap[current_node_on_path][1])-g[current_node_on_path].delay;
          //   cout<<"---------------------------------------------------------------------------"<<endl;




        int count=0;
        double tempdelay;
        int undocount=0;
        int changecount=0;
        struct timeval start,end;
        gettimeofday(&start,0);

        // double delay_path = arrival_time_for_selected_path(g,list_of_nodes_on_path);
        cout<<"delay  ++ "<<delay<<endl; 
        // cout<<"delay_path = "<<delay_path<<endl;
        //tempvect= required_time_for_selected_path(delay_path,g,list_of_nodes_on_path);
        tempvect= required_time_for_selected_path(delay,g,list_of_nodes_on_path);
        cout<<"-----------------------------------Starting the actual replacement algorithm---------------------"<<endl;
        /*********************************************************Till here verified at 1:00 PM, 21/11/2013 *****************************/
        do
        {
//            for(int i=0;i<windowsize;i++)
//            {
//              
//              // The tempvect stores the nodes sorted based on averageslack(metric can be changed to either rise slack or fall slack too 
//              //The markednode stores the ith node from tempvect.
                markednode=tempvect[0]; 
                tempvect.erase(tempvect.begin());
                //the numreplacements field indicates the number of replacements undergone by that particular node. numreplacments 0->lvt 1->svt 2->hvt
                //The risedelaymap contains the lvt(0),svt(1),hvt(2) delays for each node 
                //The rise delay and fall delay are stored as an array in each node
                
                //If the numreplacements=0(lvt) and the averageslack is greater than  svt delay - lvt delay we commit the change
                //else if numreplacements =1(hvt) and the averageslack is greater than hvt delay-svt delay we commit the change
                //else the node is marked critical
                //we do sta everytime a change is committed.
                cout<<"Current Marked Node is --- "<<markednode<<endl;
                cout<<"This is before testing the if in the do---------------------"<<endl;
                cout<<"g[markednode].numreplacements "<<g[markednode].numreplacements<<"g[markednode].slack "<<g[markednode].slack<<endl;
                cout<<"risedelaymap[markednode][1] "<<risedelaymap[markednode][1]<<"falldelaymap[markednode][1] "<<falldelaymap[markednode][1]<<"g[markednode].delay "<<g[markednode].delay<<endl;
                cout<<"average(risedelaymap[markednode][1],falldelaymap[markednode][1])-g[markednode].delay) "<<average(risedelaymap[markednode][1],falldelaymap[markednode][1])-g[markednode].delay<<endl;                
                cout<<"Temp Arrival Time "<<arrival_time(maxlevel,vertex_count,g,2);

                   if(g[markednode].numreplacements==0 && (g[markednode].slack>average(risedelaymap[markednode][1],falldelaymap[markednode][1])-g[markednode].delay))
                    {

                       cout<<"g[markednode].numreplacements==0 && (g[markednode].slack>average(risedelaymap[markednode][1],falldelaymap[markednode][1])-g[markednode].delay)"<<endl;
                       cout<<"g[markednode].type :"<<g[markednode].type<<endl; 
                       string temp=lvt_gate_fp_map[g[markednode].type];
                       string fm=temp.substr(0,temp.length()-3);
                       string searchby=fm+"hvt";

                       // string searchby= hvt_gate_fp_map[g[markednode].type]; 
                       // cout<<" Search by : "<<searchby<<endl;
                       g[markednode].leakage=hvtleakagemap[searchby];
                       cout<<"g[markednode].leakage : "<<g[markednode].leakage<<endl;
                       cout<<"previous g[markednode].delay :"<<g[markednode].delay<<endl;
                       g[markednode].delay=average(risedelaymap[markednode][1],falldelaymap[markednode][1]);
                       cout<<"new g[markednode].delay :"<<g[markednode].delay<<endl;
                       cout<<"g[markednode].delay : "<<g[markednode].delay<<endl;
                       g[markednode].numreplacements=1;
                       cout<<"g[markednode].numreplacements :"<<g[markednode].numreplacements<<endl;
                       g[markednode].critical=1;
                       g[markednode].lvt_hvt=2;

                       //changing the capacitance
                       g[markednode].cellCapacitance= capacitancemap[searchby];
                       cout<<"g[markednode].cellCapacitance :"<<g[markednode].cellCapacitance<<endl;
                       updating_load_capacitance_for_a_particular_cell(markednode);
                       slew_calculation(lvt_rise_transition_name_index_1,lvt_rise_transition_name_index_2,lvt_fall_transition_name_index_1,lvt_fall_transition_name_index_2,lvt_risetransitionmap,lvt_falltransitionmap,0, templevel,num1,g,1,lvt_gate_fp_map,hvt_gate_fp_map);
                       delay_calculation(lvt_cell_rise_name_index_1,lvt_cell_rise_name_index_2,lvt_cell_fall_name_index_1,lvt_cell_fall_name_index_2,lvt_risemap,lvt_fallmap,0,templevel,num1,0,g,1,lvt_gate_fp_map,hvt_gate_fp_map);
                       delay_calculation(lvt_cell_rise_name_index_1,lvt_cell_rise_name_index_2,lvt_cell_fall_name_index_1,lvt_cell_fall_name_index_2,lvt_risemap,lvt_fallmap,1,templevel,num1,1,g,1,lvt_gate_fp_map,hvt_gate_fp_map);
                       cout<<"g[markednode].critical :"<<g[markednode].critical<<endl;
                       tempdelay=arrival_time(maxlevel,vertex_count,g,2);
                       //tempdelay=arrival_time_for_selected_path(g,list_of_nodes_on_path);
                       cout<<"tempdelay xx :"<<tempdelay<<endl;

                    }
                    /*else if(g[markednode].numreplacements==1 && (g[markednode].slack>average(risedelaymap[markednode][2],falldelaymap[markednode][2])-g[markednode].delay))
                    {
                        g[markednode].leakage=hvtleakagemap[g[markednode].type];
                        g[markednode].delay=average(risedelaymap[markednode][2],falldelaymap[markednode][2]);
                        g[markednode].numreplacements=2;
                        g[markednode].critical=1;
                        tempdelay=arrival_time(maxlevel,vertex_count,g);

                    }*/
                    else{
                               g[markednode].critical=1;
                               cout<<markednode<<" - marked critical"<<endl;

                    }
                
//              }
//            cout<<"doing delay"<<endl;

            // if replacing a node violates the delay , that particular replacement is undone.
            if(tempdelay>delay)
            {
                cout<<"--------replacing a node violates the delay , that particular replacement is undone---------"<<endl;
//                for(int i=0;i<windowsize;i++)
//                {
//                    markednode=tempvect[i];
                cout<<" removing node - "<<markednode<<" - markednode "<<g[markednode].type<<" - g[markednode].type"<<endl;
                    if(g[markednode].numreplacements==1)
                    {

                        string searchby= lvt_gate_fp_map[g[markednode].type]; 
                        cout<<"searchby :"<<searchby<<endl;
                        g[markednode].leakage=lvtleakagemap[searchby];
                        cout<<"g[markednode].leakage :"<<g[markednode].leakage<<endl;
                        g[markednode].delay=average(risedelaymap[markednode][0],falldelaymap[markednode][0]);
                        cout<<"g[markednode].delay"<<g[markednode].delay<<endl;
                        g[markednode].critical=1;
                        g[markednode].numreplacements=g[markednode].numreplacements-1;
                        cout<<" inside  if(g[markednode].numreplacements==1) "<<endl;
                        cout<<" g[markednode].leakage"<<g[markednode].leakage<<" g[markednode].delay :"<<g[markednode].delay<<" g[markednode].critical : "<<g[markednode].critical<<endl;
                        g[markednode].lvt_hvt=1;
                        //putting the lvt cap value back
                        g[markednode].cellCapacitance=capacitancemap[searchby];

                    }

                    cout<<" g[markednode].leakage"<<g[markednode].leakage<<" g[markednode].delay :"<<g[markednode].delay<<" g[markednode].critical : "<<g[markednode].critical<<endl;
                    /*else if(g[markednode].numreplacements==2)
                    {
                        g[markednode].leakage=rvtleakagemap[g[markednode].type];
                        g[markednode].delay=average(risedelaymap[markednode][1],falldelaymap[markednode][1]);
                        g[markednode].critical=1;
                    }*/
//                }
                    cout<<"Reverting the arrival times as the change is undone "<<endl;
                    arrival_time(maxlevel,vertex_count,g,1);
                    //arrival_time_for_selected_path(g,list_of_nodes_on_path);
                    cout<<"Undoing the move for - "<<markednode<<endl;
                  //  g[markednode].numreplacements=g[markednode].numreplacements-1;
                 //   g[markednode].critical=1;
                  //  cout<<"undoing"<<endl;
                    undocount++;
                //    tempvect.erase(tempvect.begin()+i);
                //windowsize=2;
            
            }

            // if delay is not violated then we calculate slack and sort the gates according to slack again.
            else 
            {
                //if(windowsize<maxwindowsize)
                   // windowsize++;
                              
                // tempvect=required_time(delay,maxlevel,vertex_count,g);
                cout<<"Changing the node "<<markednode<<" of type - "<<g[markednode].type<<endl;
                //list_of_nodes_on_path.erase(list_of_nodes_on_path.begin());
                if(g[markednode].critical==1 && g[markednode].numreplacements==1){
                list_of_replaced_nodes.push_back(markednode);


                }

                tempvect = required_time_for_selected_path(delay,g,list_of_nodes_on_path);
                vector<int>::iterator vintItr;
                for(vintItr=tempvect.begin();vintItr!=tempvect.end();vintItr++)
                        cout<<" tempvect :"<< *vintItr <<endl;
                changecount++;
            }
        //cout<<tempvect.size()<<endl;        
        //count variable keeps a track of number of moves made so far
        count++;
        }while(tempvect.size()!=0);//do until tempvect is empty.
                    gettimeofday(&end,0);
                    unsigned long long elapsed=(end.tv_sec+(end.tv_usec/1000000.0))-(start.tv_sec+(start.tv_usec/1000000.0));

        cout<<"changecount - "<<changecount<<" undid moves - "<<undocount<<" totalmoves:"<<count<<"\n";            
       // cout<<"undid moves"<<undocount<<"totalmoves:"<<count<<"\n";



        //From this point on debugging after the replacement starts.
        //We count total leakage after replacement.
        //We check delay before and after replacement.
        totalleakage=0;
            for(int j=0;j<vertex_count;j++)
            {
                              totalleakage=g[j].leakage+totalleakage;
            }
            
        cout<<"TOTAL LEAKAGE After replacment:"<<totalleakage<<endl;
        double checkdelay=arrival_time(maxlevel,vertex_count,g,1);
        // cout<<"delay before replacement:"<<checkdelay<<" "<<"delay after replacement:"<<delay<<endl;
        cout<<" delay before replacement:"<<delay<<" "<<" delay after replacement:"<<checkdelay<<endl;

        for(path_itr=list_of_nodes_on_path.begin();path_itr!=list_of_nodes_on_path.end();++path_itr){
            cout<<"---------------------------------------------------------------------------"<<endl;
            int current_node_on_path=*path_itr;
            cout<<"Node :"<<current_node_on_path<<"g["<<current_node_on_path<<"].level :"<<g[current_node_on_path].level<<endl;
            cout<<"Node :"<<current_node_on_path<<"g["<<current_node_on_path<<"].type :"<<g[current_node_on_path].type<<endl;
            cout<<"Node :"<<current_node_on_path<<"g["<<current_node_on_path<<"].leakage :"<<g[current_node_on_path].leakage<<endl;
            cout<<"g["<<current_node_on_path<<"] Arrival Time -- "<<g[current_node_on_path].arrival<<endl;
            //cout<<"g["<<current_node_on_path<<"] required time -- "<<g[current_node_on_path].required_time<<endl;
            cout<<"g["<<current_node_on_path<<"] delay time -- "<<g[current_node_on_path].delay<<endl;
            cout<<"g["<<current_node_on_path<<"] number of replacements -- "<<g[current_node_on_path].numreplacements<<endl;
            cout<<"g["<<current_node_on_path<<"] critical -- "<<g[current_node_on_path].critical<<endl;
            cout<<"g["<<current_node_on_path<<"] slack -- "<<g[current_node_on_path].slack<<endl;
            cout<<"g["<<current_node_on_path<<"] lvt or hvt -- "<<g[current_node_on_path].lvt_hvt<<endl;
            cout<<"g["<<current_node_on_path<<"] average(risedelaymap[current_node_on_path][1],falldelaymap[current_node_on_path][1]) -- "<<average(risedelaymap[current_node_on_path][1],falldelaymap[current_node_on_path][1])<<endl;
            cout<<"average(risedelaymap[markednode][1],falldelaymap[markednode][1])-g[markednode].delay "<<average(risedelaymap[current_node_on_path][1],falldelaymap[current_node_on_path][1])-g[current_node_on_path].delay;
            cout<<"---------------------------------------------------------------------------"<<endl;
        }



            // //current_node_on_path=164683;
            // cout<<"---------------------------------------------------------------------------"<<endl;
            // current_node_on_path=163435;
            // cout<<"g["<<current_node_on_path<<"] Arrival Time -- "<<g[current_node_on_path].arrival<<endl;
            // //cout<<"g["<<current_node_on_path<<"] required time -- "<<g[current_node_on_path].required_time<<endl;
            // cout<<"g["<<current_node_on_path<<"] delay time -- "<<g[current_node_on_path].delay<<endl;
            // cout<<"g["<<current_node_on_path<<"] number of replacements -- "<<g[current_node_on_path].numreplacements<<endl;
            // cout<<"g["<<current_node_on_path<<"] critical -- "<<g[current_node_on_path].critical<<endl;
            // cout<<"g["<<current_node_on_path<<"] slack -- "<<g[current_node_on_path].slack<<endl;
            // cout<<"g["<<current_node_on_path<<"] average(risedelaymap[current_node_on_path][1],falldelaymap[current_node_on_path][1]) -- "<<average(risedelaymap[current_node_on_path][1],falldelaymap[current_node_on_path][1])<<endl;
            // cout<<"average(risedelaymap[markednode][1],falldelaymap[markednode][1])-g[markednode].delay "<<average(risedelaymap[current_node_on_path][1],falldelaymap[current_node_on_path][1])-g[current_node_on_path].delay;
            // cout<<"---------------------------------------------------------------------------"<<endl;

    }//closing braces for argument else

    return 0;
}//closing braces for int main()

