#include <iostream>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <set>
#include <algorithm>

#define MAXLINE 256

using namespace std;

struct Numpartset {
	vector<int> nums;
	// vector<int> prepart;
	
	Numpartset(char* filename) {
		ifstream infile(filename, ios::in);
		char buff[MAXLINE];

		infile.getline(buff, MAXLINE);
		while (!infile.eof()) {
			nums.push_back(stoi(buff));
			infile.getline(buff, MAXLINE);
		}

		sort(nums.begin(), nums.end());
	}

	int KK() {
		while(nums.size() > 1) {
			int n = nums.size();
			int a = nums[n-1];
			int b = nums[n-2];
			int c = abs(a-b);

			nums.pop_back();
			nums[n-2] = c;

			sort(nums.begin(), nums.end());
		}

		return(nums[0]);
	}

	int RR() {
		
	}
};

int main(int argc, char* argv[]) {
	int flag = stoi(argv[1]);
	int algorithm = stoi(argv[2]);
	char* inputfile = argv[3];

	Numpartset test(inputfile);

	switch(algorithm) {
		case 0:
			cout << test.KK() << endl;
			break;
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		case 11:
			break;
		case 12:
			break;
		case 13:
			break;
	}
}