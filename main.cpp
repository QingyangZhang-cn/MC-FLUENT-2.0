#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include "SortOut.h"
#include "ReadFR.h"
#include "rewritepower.h"
#include "ModifyMate.h"
#include "filestat.h"
#include "RMSE.h"
#include "submit.h"
using namespace std;

int main()
{
    Input_File Input;
    int tag, tag2;
    int step = 0;
    int N = Input.m_Ni;
    double ero = 100;
    double rmsero = Input.m_error;
    vector<vector<string>> tempTR;    
    vector<vector<string>> lastTR;
	int n_nodes = Input.m_nodes;
    string n_list = Input.m_nodelist;
    const char* node_list = n_list.c_str();
	int mc_nodes = Input.min_mcnodes;
	int fl_nodes = Input.min_flnodes;
    char num_mc[10];
    char num_fl[10];
    double timemc, timemc_old, timefl, timefl_old;
    bool mc_up = true;
    bool fl_up = true;

//===========================================================MC-Fluent================================================================================
    cout << "MC-Fluent is written by Qingyang Zhang. If you have any questions, please contact the following email: zhangqingyang18@nudt.edu.cn" << endl;
    cout << "***********************************************" << endl;
    cout << "Checking nessesary files...." << endl;
    //======================================================
    if(!( Check("geometry.xml") && Check("materials.xml") && Check("setting.xml") && Check("tallies.xml")  && Check(Input.m_Hname) && Check("run.jou")))
    {
        return -1;
    }
    //======================================================
    cout << "Checking complete." << endl;
    cout << "***********************************************" << endl;
    cout << "PROGRAMMING START:" << endl;
    while(step < N || ero >= rmsero)
    {
        //Run Openmc。
        cout << "Openmc running..." << endl;
        snprintf(num_mc, sizeof(num_mc), "%d", mc_nodes);
        timemc = run_mc(num_mc, node_list);
        cout << "Fission rate has been obtained." << endl;
        cout <<" Ready to rewrite the power density of FLUENT UDF...." << endl;
        rewrite_UDFH();
        cout << "The power density transmission was successful." << endl;
        //RUN Fluent。
        cout << "Fluent running..."<< endl;
        snprintf(num_fl, sizeof(num_fl), "%d", fl_nodes);
        timefl = run_fl(num_fl, node_list);
        cout << "Modifying properties of materials..."
        //Computing ero and scheduling nodes.
        if(step == 0)
		{
			GetTandRou(lastTR);
            timemc_old = timemc;
            timefl_old = timefl;
            timemc = ((timemc*2>n_nodes)? n_nodes:timemc*2);
            timefl = ((timefl*2>n_nodes)? n_nodes:timefl*2);
		}
		else
		{
			GetTandRou(tempTR);
			ero = RMSE(tempTR,lastTR);
			lastTR = tempTR;
            if(mc_up)
            {   
                if(timemc < timemc_old)
                {
                    mc_nodes = ((mc_nodes*2>n_nodes)? n_nodes:mc_nodes*2);
                }
                else if(timemc = timemc_old)
                {
                    mc_up = false;
                }
                else{
                    mc_nodes = mc_nodes/2;
                    mc_up = false;
                }
            }
            if(fl_up)
            {   
                if(timefl < timefl_old)
                {
                    fl_nodes = ((fl_nodes*2>n_nodes)? n_nodes:fl_nodes*2);
                }
                else if(timefl = timefl_old)
                {
                    fl_up = false;
                }
                else{
                    fl_nodes = fl_nodes/2;
                    fl_up = false;
                }
            }
		}
        //**************************************************
        ModifyMaterial();
        ++step;
    }
    return 0;
}