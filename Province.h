#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <set>
#include <limits>
#define SPEEDHIGH 90
#define SPEEDLOW 40
#define DRIVESTART 7
#define DRIVEEND 19
#define SCENESTART 8
#define SCENEEND 18
#define MAXDRIVE 8
#define MAXHALFDRIVE 5
#define MAXPLAYDRIVE 3(<#args#>
using namespace std;

class Province{
public:
	string name;
	int cityNum;
	vector<string> cities;
	int distanceFromStart;
	int **AdjMat;
	int **Shortest;//only for Floyd
	//bool *visited;//only for DFS,BFS,Dijkstra
	//int *distance; //only for Dijkstra
	//int *path;//only for Dijkstra
	string *tour;
	vector<int> left;
    int scenes;
    int minl;
    int *mina;

    Province(string Name):name(Name){
        minl = numeric_limits<int>::min();
    }

	void init(){
		//ifstream infile(string(name));
		//init
		//update cityNum;cities;distancef;AdjMat;
		//infile.close();
	}
	
	void Floyd(){
		//update Shortest
	}

	void ShortestPath(){
        vector<int> remainings;
        for(int i=0;i<scenes;i++) {
            remainings.push_back(i);
        }
        int *a=new int[scenes];
        ShortestPathHelper(0,remainings,a);
        delete [] a;
	}
    
    void ShortestPathHelper(int index,vector<int>& remainings,int a[]) {
        if(index<scenes) {
        for(int i=0;i<remainings.size();i++) {
            vector<int> newremainings;
            for(int j=0;j<i;j++) {
                newremainings.push_back(remainings[j]);
            }
            for(int j=i+1;j<remainings.size();j++) {
                newremainings.push_back(remainings[j]);
            }
            int *b=new int[scenes];
            memcpy(b,a,sizeof(int)*index);
            b[index]=remainings[i];
            ShortestPathHelper(index+1,newremainings,b);
            delete [] b;
        }
        } else {
            int current=timeCalc(a);
            if(current<minl) {
                minl=current;
                if(mina!=0) {
                    delete[] mina;
                }
                mina=new int[scenes];
                memcpy(mina, a, sizeof(int)*scenes);
            }
        }
    }
    
    double timeCalc(int a[]) {
        double drived=static_cast<double>(distanceFromStart)/SPEEDHIGH,time=DRIVESTART;
        int result=1;
        drive(drived, result, MAXDRIVE, time);
        // TODO
        return result;
    }
    
    void drive(double& drived,int& result,double remaining,double& time) {
        while(drived>remaining) {
            drived-=remaining;
            time=DRIVESTART;
            result++;
            remaining=MAXDRIVE;
        }
        time+=drived;
    }
    
    
};