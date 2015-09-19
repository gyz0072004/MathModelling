#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "Province.h"

using namespace std;

class Country{
public:
	int provinceNum;
	vector<province> provinces;
	int StartIdx;
	int **AdjMat;

	string *tour;
	vector<int> left;

	Province(string Name):name(Name){};

	void init(){
		ifstream infile("???");
		//init
		//update cityNum;cities;distancef;AdjMat;
		infile.close();
	}
	
	void merge(){
		provinces[0].left
	}
}