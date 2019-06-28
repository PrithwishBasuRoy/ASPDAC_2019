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

double arrival_time(int levels,int size,Graph &g,int vt) //changed val to tempval because of inverting gates
{

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
                cout<<"Everything comes here"<<endl;
                g[f].arrival=0;       
            }
            if(g[f].level==i && i!=0)
            {
                //int parity=paritymap[g[f].type];
                for(intit=g[f].fanins.begin();intit<g[f].fanins.end();intit++)
                {
                    tempnode=*intit;
                    cout<<"I am here "<<arrival<<"- arrival|| "<<g[tempnode].arrival<<" - tempnode arrival "<<g[f].delay<<"g[delay]"<<endl;
                    if(g[tempnode].arrival+g[f].delay>arrival){
                        arrival=g[tempnode].arrival+g[f].delay;
                        cout<<arrival<<" - This is the arrival "<<g[f].delay<<" - delay "<<endl;
                    }


                }
                g[f].arrival=arrival;
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


// for required time val need not be changed to tempval as both rise arrival,fall arrival and rise delay and fall delay values are known. rise required =rise arrival+rise delay. 

vector<int> required_time(double delay,int levels,int size,Graph &g)
{
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
         for(int f=num;f>=0;f--)
         {
             if(g[f].level==i && g[f].type=="PO")
             {
                 
                 g[f].slack=delay - g[f].arrival;
             }
//             else if(g[f].level==i && i!=levels && i!=0 && g[f].type=="PO")
//             {
//                 g[f].slack[0]=delay-g[f].arrival[0];
//                 g[f].slack[1]=delay-g[f].arrival[1];
//             }
             
             else if(g[f].level==i && g[f].type!="PI" && g[f].type!="PO")
             {

                     for(intit=g[f].fanouts.begin();intit<g[f].fanouts.end();intit++)
                    {
                        tempnode=*intit;
                        double output_arrival_time=g[tempnode].slack + g[tempnode].arrival;
                        if(output_arrival_time - g[tempnode].delay<required)
                            required=output_arrival_time - g[tempnode].delay;
                   
                    }

                        g[f].slack=required - g[f].arrival;
//                        if(g[f].averageslack>maxslack)
//                        {
//                            markednode=f;
//                            maxslack=g[f].averageslack;
//                        }
                              
                required=DBL_MAX;
             }
         }
        }

             for(int i=0;i<num-1;i++)
                 if(g[i].type!="PI"&&g[i].type!="PO"&&g[i].numreplacements<1&&g[i].critical==0)//num replacement was previously 2
                 {
                     slackset.push_back(std::make_pair(i,g[i].cost[0]));
                     //slackset.push_back(std::make_pair(i,g[i].cost[1]));
                 }

        sort(slackset.begin(),slackset.end(),mysortfunction);
        ofstream myfile;
        myfile.open("slackfile.txt",ios::out|ios::app);
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
            int temp=slackset[i].first;
            nodevect.push_back(temp);
        }
        return nodevect; 


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
         }
         else
         {
             g[f].leakage=0;
         }
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
        cout<<"usage:./a.out hgrfile typefile fanout fanin lvt_gate_fp_map hvt_gate_fp_map risedelayfile falldelayfile hvtleakage lvtleakage parityfile"<<endl;
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
            ifstream fanoutfile(argv[3]);
            for(int i=0;i<num;i++)
            {
                
                if(fanoutfile.is_open())
                {
                    vector<int>fanouts;
                    getline(fanoutfile,line);
                    std::istringstream ss(line);
                    while(ss>>j)
                        fanouts.push_back(j);
                    g[i].fanouts=fanouts;
                }
            

            }
            
            //Thiis for loop will identify fanins for each node
            ifstream faninfile(argv[4]);
            for(int i=0;i<num;i++)
            {
                if(faninfile.is_open())
                {
                    vector<int>fanins;
                    getline(faninfile,line);
                    //edge contains two fields - second:boolean variable, which is true if edge exists from j->i
                    //                                                    which is false otherwise
                    std::istringstream ss(line);
                    while(ss>>j)
                        fanins.push_back(j);
                    g[i].fanins=fanins;
                }
            }
            cout<<"fanouts done"<<endl;

            //new gate mapping code inserted by PBR on 22nd June 2019
            // map<string,string > lvt_gate_fp_map;
            // map<string,string > lvt_fp_gate_map;
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
                    string s=s1.substr(0,len-2);
                    //std::replace( s.begin(), s.end(), '\n', '');

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
        
        // maxlevel=14;
        //double delay=arrival_time(10,vertex_count,g,1);
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
            for(int j=0;j<vertex_count-1;j++)
            {
                double tempdelay=average(risedelaymap[j][1],falldelaymap[j][1]);
                string searchby= hvt_gate_fp_map[g[j].type];
                double leakage=hvtleakagemap[searchby];
                // double tempdelay1=average(risedelaymap[j][2],falldelaymap[j][2]);
                // double leakage1=hvtleakagemap[g[j].type];
                double hvt_cost=(leakage-g[j].leakage)/(tempdelay-g[j].delay);
                // double hvt_cost=(leakage1-g[j].leakage)/(tempdelay1-g[j].delay);
                g[j].cost[0]=hvt_cost;
                // g[j].cost[1]=hvt_cost;
                        
            }
        cout<<"TOTAL LEAKAGE Before Replacement:"<<totalleakage<<endl;

        int count=0;
        double tempdelay;
        int undocount=0;
        struct timeval start,end;
        gettimeofday(&start,0);

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
                   if(g[markednode].numreplacements==0 && (g[markednode].slack>average(risedelaymap[markednode][1],falldelaymap[markednode][1])-g[markednode].delay))
                    {

                       string searchby= hvt_gate_fp_map[g[markednode].type]; 
                       g[markednode].leakage=hvtleakagemap[searchby];
                       g[markednode].delay=average(risedelaymap[markednode][1],falldelaymap[markednode][1]);
                       g[markednode].numreplacements=1;
                       g[markednode].critical=1;
                       tempdelay=arrival_time(maxlevel,vertex_count,g,1);

                    }
                    /*else if(g[markednode].numreplacements==1 && (g[markednode].slack>average(risedelaymap[markednode][2],falldelaymap[markednode][2])-g[markednode].delay))
                    {
                        g[markednode].leakage=hvtleakagemap[g[markednode].type];
                        g[markednode].delay=average(risedelaymap[markednode][2],falldelaymap[markednode][2]);
                        g[markednode].numreplacements=2;
                        g[markednode].critical=1;
                        tempdelay=arrival_time(maxlevel,vertex_count,g);

                    }*/
                    else
                               g[markednode].critical=1;
                
//              }
//            cout<<"doing delay"<<endl;

            // if replacing a node violates the delay , that particular replacement is undone.
            if(tempdelay>delay)
            {
//                for(int i=0;i<windowsize;i++)
//                {
//                    markednode=tempvect[i];
                    if(g[markednode].numreplacements==1)
                    {
                        string searchby= lvt_gate_fp_map[g[markednode].type]; 
                        g[markednode].leakage=lvtleakagemap[searchby];
                        g[markednode].delay=average(risedelaymap[markednode][0],falldelaymap[markednode][0]);
                        g[markednode].critical=1;
                    }
                    /*else if(g[markednode].numreplacements==2)
                    {
                        g[markednode].leakage=rvtleakagemap[g[markednode].type];
                        g[markednode].delay=average(risedelaymap[markednode][1],falldelaymap[markednode][1]);
                        g[markednode].critical=1;
                    }*/
//                }
                    arrival_time(maxlevel,vertex_count,g,1);
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
                              
                tempvect=required_time(delay,maxlevel,vertex_count,g);
            }
        //cout<<tempvect.size()<<endl;        
        //count variable keeps a track of number of moves made so far
        count++;
        }while(tempvect.size()!=0);//do until tempvect is empty.
                    gettimeofday(&end,0);
                    unsigned long long elapsed=(end.tv_sec+(end.tv_usec/1000000.0))-(start.tv_sec+(start.tv_usec/1000000.0));

        cout<<"undid moves"<<undocount<<"totalmoves:"<<count<<"\n";



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
        cout<<"delay before replacement:"<<checkdelay<<" "<<"delay after replacement:"<<delay<<endl;





    }//closing braces for argument else

    return 0;
}//closing braces for int main()

