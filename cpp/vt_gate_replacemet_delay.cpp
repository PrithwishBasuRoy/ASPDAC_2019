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

vector<pair<int,double>> slackset;
    struct VertexProperty
{
    vector<int> fanouts;
    vector<int> fanins;
    string type;
    double slack;
    double delay;
    double arrival;
    double leakage;
    int level;
    int parity; 
    int numreplacements;
    int critical;
    double cost[1];
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
                    if(maxdelay>delay)
                        delay=maxdelay;
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

int main(int argc,char *argv[])
{
    //string variable to read lines from files
    string line;
    
    //vertex_count stores the number of vertices
    int vertex_count;
    
    //num is a temporary variable, used for parsing
    int num;
    if(argc!=12)//rvt leakage removed 
        cout<<"usage:./a.out hgrfile typefile fanout fanin lvt_gate_lookup hvt_gate_lookup risedelayfile falldelayfile hvtleakage lvtleakage parityfile"<<endl;
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

            //integer variable to store the number of PIs
            int pi;

            //hash table where key is the gate type and value is the gate load capacitance - not used in the program
            map<string,double> capacitancemap;

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
            map<string,int> paritymap;
            map<int,vector<double>> risedelaymap;
            map<int,vector<double>> falldelaymap;
            map<string,double> lvtleakagemap;
            // map<string,double> rvtleakagemap;
            map<string,double> hvtleakagemap;

            ifstream risedelayfile(argv[7]);

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

            ifstream falldelayfile(argv[8]);
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

            ifstream hvtleakagefile(argv[9]);
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

            ifstream lvtleakagefile(argv[10]);
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

            int tempnode;
            ifstream parityfile(argv[11]);
            while(parityfile.good())
            {
                getline(parityfile,line);
                stringstream ss(line);
                ss >> gate_type;
                while(ss>>gate_parameter)
                    paritymap[gate_type]=gate_parameter;
            }

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
         // vector<int> list_of_nodes_on_path{2073,22203,132952,75469,57714,14927,103161,121002,66894,66893,66892,163588};
         vector<int> list_of_nodes_on_path{0,2,3,4,5,6,7,8,9,10,1};



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
        /************************************************************************/


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

            cout<<" Info dump starts here "<<endl;

            for(int j=0;j<vertex_count;j++)
            {
                //if(j==26149 || j==2789|| j==8616|| j==8602|| j==132791|| j==97948|| j==97947|| j==97945|| j==97944|| j==9663 || j==9662 || j==163436 ){

                //if(j==22203||j==132958||j==75469||j==57714||j==14926||j==103161||j==121002||j==66894||j==66893||j==66892){
                //if( j==129188|| j==87016|| j==79105|| j==124092|| j==146548|| j==146896|| j==118008|| j==92196|| j==92195){
                //if( j==106163|| j==106163|| j==106163|| j==142942|| j==145265|| j==113449|| j==120820|| j==27369|| j==27368){    
                if( j==129188|| j==87016|| j==79105|| j==124092|| j==146547|| j==146895|| j==118008|| j==92196|| j==92195){
                //if(g[j].slack>=-0.0001 && g[j].slack<=0.0001){

                  if(g[j].level>0){  
                    cout<<"Node :"<<j<<"g["<<j<<"].level :"<<g[j].level<<endl;
                    cout<<"Node :"<<j<<"g["<<j<<"].type :"<<g[j].type<<endl;
                    cout<<"Node :"<<j<<"g["<<j<<"].leakage :"<<g[j].leakage<<endl;
                    cout<<"Node :"<<j<<"g["<<j<<"].arrival :"<<g[j].arrival<<endl;
                    cout<<"Node :"<<j<<"g["<<j<<"].delay :"<<g[j].delay<<endl;
                    cout<<"Node :"<<j<<"g["<<j<<"].slack :"<<g[j].slack<<endl;
                    cout<<"Node :"<<j<<"g["<<j<<"].fanins :"<<endl;

                    cout<<"---------------------------------------"<<endl;
                    vector<int>::iterator iter;
                    for(iter=g[j].fanins.begin();iter!=g[j].fanins.end();++iter){
                        int pred=*iter;
                        cout<<"pred :"<<pred<<" Pred.type :"<<g[pred].type<<endl;

                    }
                    cout<<"---------------------------------------"<<endl;

                    for(iter=g[j].fanouts.begin();iter!=g[j].fanouts.end();++iter){
                        int succ=*iter;
                        cout<<"succ :"<<succ<<" succ.type :"<<g[succ].type<<endl;
                        
                    }
                    cout<<"---------------------------------------"<<endl;

                  } /*else if (j== 2789 && g[j].type == "PI" && g[j].level == 0 ){

                    cout<<"Node :"<<j<<"g["<<j<<"].level :"<<g[j].level<<endl;
                    cout<<"Node :"<<j<<"g["<<j<<"].type :"<<g[j].type<<endl;
                    cout<<"Node :"<<j<<"g["<<j<<"].leakage :"<<g[j].leakage<<endl;
                    cout<<"Node :"<<j<<"g["<<j<<"].arrival :"<<g[j].arrival<<endl;
                    cout<<"Node :"<<j<<"g["<<j<<"].delay :"<<g[j].delay<<endl;
                    cout<<"Node :"<<j<<"g["<<j<<"].slack :"<<g[j].slack<<endl;
                    cout<<"Node :"<<j<<"g["<<j<<"].fanins :"<<endl;

                    cout<<"---------------------------------------"<<endl;
                    vector<int>::iterator iter;
                    for(iter=g[j].fanins.begin();iter!=g[j].fanins.end();++iter){
                        int pred=*iter;
                        cout<<"pred :"<<pred<<" Pred.type :"<<g[pred].type<<endl;

                    }
                    cout<<"---------------------------------------"<<endl;

                    for(iter=g[j].fanouts.begin();iter!=g[j].fanouts.end();++iter){
                        int succ=*iter;
                        cout<<"succ :"<<succ<<" succ.type :"<<g[succ].type<<endl;
                        
                    }
                    cout<<"---------------------------------------"<<endl;

                  }*/
                  


                }



            }
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
        
        // vector<int>::iterator path_itr;
        // for(path_itr=list_of_nodes_on_path.begin();path_itr!=list_of_nodes_on_path.end();++path_itr){
        //     cout<<"---------------------------------------------------------------------------"<<endl;
        //     int current_node_on_path=*path_itr;
        //     cout<<"g["<<current_node_on_path<<"] Arrival Time -- "<<g[current_node_on_path].arrival<<endl;
        //     //cout<<"g["<<current_node_on_path<<"] required time -- "<<g[current_node_on_path].required_time<<endl;
        //     cout<<"g["<<current_node_on_path<<"] delay time -- "<<g[current_node_on_path].delay<<endl;
        //     cout<<"g["<<current_node_on_path<<"] number of replacements -- "<<g[current_node_on_path].numreplacements<<endl;
        //     cout<<"g["<<current_node_on_path<<"] critical -- "<<g[current_node_on_path].critical<<endl;
        //     cout<<"g["<<current_node_on_path<<"] slack -- "<<g[current_node_on_path].slack<<endl;
        //     cout<<"g["<<current_node_on_path<<"] average(risedelaymap[current_node_on_path][1],falldelaymap[current_node_on_path][1]) -- "<<average(risedelaymap[current_node_on_path][1],falldelaymap[current_node_on_path][1])<<endl;
        //     cout<<"average(risedelaymap[markednode][1],falldelaymap[markednode][1])-g[markednode].delay "<<average(risedelaymap[current_node_on_path][1],falldelaymap[current_node_on_path][1])-g[current_node_on_path].delay<<endl;
        //     cout<<"---------------------------------------------------------------------------"<<endl;
        // }



        


    }//closing braces for argument else

    return 0;
}//closing braces for int main()

