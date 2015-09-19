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
#define MAXPLAYDRIVE 3
#define HOURHALFDAY 4
#define HOURFULLDAY 8
#define HOURCITY 24
#define HOURTWODAY 30
using namespace std;

class Province {
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
    int scenes;//number of scenes
    int minl;//shortest time
    int *mina;//shortest path in index

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
    
    int timeCalc(int a[]) {
        double time=DRIVESTART,drived=0,remaining=MAXDRIVE;
        int result=1;
        double timeFromStart=static_cast<double>(distanceFromStart)/SPEEDHIGH;
        drive(drived,timeFromStart, result,remaining, time);
        int current=0;
        bool halfplayed=false;
        for(int i=0;i<scenes;i++) {
            int next=a[i];
            if(current<next) {
                drive(drived,Shortest[current][next],result,remaining,time);
            }else if (current>next) {
                drive(drived,Shortest[next][current],result,remaining,time);
            }
            play(drived,left[next],result,remaining,time,halfplayed);
            current=next;
        }
        if(current!=0) {
            drive(drived,Shortest[0][current],result,remaining,time);
        }
        drive(drived, timeFromStart, result, remaining, time);
        return result;
    }
    
    void drive(double& drived,double length,int& result,double& remaining,double& time) {
        while(length>remaining) {
            length-=remaining;
            time=DRIVESTART;
            result++;
            remaining=MAXDRIVE;
        }
        time+=length;
        drived=length;
        remaining-=length;
    }
    
    void play(double& drived,int length,int& result,double& remaining, double& time,bool& halfplayed) {
        switch (length) {
            case HOURHALFDAY:
                if(halfplayed&&drived<=MAXPLAYDRIVE&&time+length<=SCENEEND) {
                    time+=length;
                    if(remaining>DRIVEEND-time) {
                        remaining=DRIVEEND-time;
                    }
                    if(remaining>MAXPLAYDRIVE) {
                        remaining=MAXPLAYDRIVE;
                    }
                }
                else if(!halfplayed && drived<=MAXHALFDRIVE && time + length<=SCENEEND) {
                    time+=length;
                    halfplayed=true;
                    if(remaining>DRIVEEND-time) {
                        remaining=DRIVEEND-time;
                    }
                    if(remaining>MAXHALFDRIVE) {
                        remaining=MAXHALFDRIVE;
                    }
                } else {
                    result++;
                    time=SCENESTART+length;
                    drived=0;
                    remaining=MAXHALFDRIVE;
                    halfplayed=true;
                }
                break;
            case HOURFULLDAY:
                if(drived<=MAXPLAYDRIVE&&time+length<=SCENEEND) {
                    time+=length;
                    if(remaining>DRIVEEND-time) {
                        remaining=DRIVEEND-time;
                    }
                    if(remaining>MAXPLAYDRIVE) {
                        remaining=MAXPLAYDRIVE;
                    }
                }else {
                    result++;
                    time=SCENESTART+length;
                    drived=0;
                    remaining=MAXPLAYDRIVE;
                }
                break;
            case HOURCITY:
                result++;
                drived=0;
                remaining=DRIVEEND-time;
                if(remaining>MAXDRIVE) {
                    remaining=MAXDRIVE;
                }
                break;
            case HOURTWODAY:
                if(time+HOURFULLDAY<=SCENEEND) {
                    result++;
                }else {
                    result+=2;
                }
                time=SCENESTART+HOURFULLDAY;
                drived=0;
                remaining=MAXPLAYDRIVE;
                break;
        }
    }
    
    
};