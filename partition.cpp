#include <iostream>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <set>
#include <algorithm>
#include <random>
#include <chrono>
#include <math.h>

#define MAXLINE 1024

using namespace std;

struct Numpartset {
	vector<long> nums;
	vector<long> prepart;
	vector<long> combined;
	int length;
	long best;

	Numpartset(char* filename) {
		ifstream infile(filename, ios::in);
		char buff[MAXLINE];

		infile.getline(buff, MAXLINE);
		// for (int _ = 0; _ < 100; _++) {
		while(!infile.eof()) {
			nums.push_back(stol(buff));
			infile.getline(buff, MAXLINE);
		}

		sort(nums.begin(), nums.end());

		length = nums.size();

		best = __LONG_MAX__;
	}

	Numpartset(vector<long> randnums) {
		for (auto& num: randnums)
			nums.push_back(num);
		length = nums.size();
		best = __LONG_MAX__;
	}

	bool combine() {
		combined.erase(combined.begin(), combined.end());

		for(int _ = 0; _ < length; _++) {
			combined.push_back(0);
		}

		for(int i = 0; i < length; i++) 
			combined[prepart[i]] += nums[i];

		long res = KK_2();

		if (best > abs(res)) {
			best = abs(res);
			return true;
		}
		return false;
	}

	long combine_SA() {
		combined.erase(combined.begin(), combined.end());

		for(int _ = 0; _ < length; _++) {
			combined.push_back(0);
		}

		for(int i = 0; i < length; i++) 
			combined[prepart[i]] += nums[i];

		long res = KK_2();

		return abs(res);
	}

	// Using list in nums, usually original list
	long KK() {
		while(nums.size() > 1) {
			int n = nums.size();
			long a = nums[n-1];
			long b = nums[n-2];
			long c = abs(a-b);

			nums.pop_back();
			nums[n-2] = c;

			sort(nums.begin(), nums.end());
		}

		return(nums[0]);
	}

	// Using prepartition combined list
	long KK_2() {
		while(combined.size() > 1) {
			int n = combined.size();
			long a = combined[n-1];
			long b = combined[n-2];
			long c = abs(a-b);

			combined.pop_back();
			combined[n-2] = c;

			sort(combined.begin(), combined.end());
		}

		return(combined[0]);
	}

	// Check residue given a sequence of +1/-1
	bool check() {
		long res = 0;
		
		for(int i = 0; i < length; i++) {
			res += prepart[i] * nums[i];
		}
		
		if (best > abs(res)) {
			best = abs(res);
			return true;
		}
		return false;
	}
	
	// For simulated annealing
	long check_SA() {
		long res = 0;
		
		for(int i = 0; i < length; i++) {
			res += prepart[i] * nums[i];
		}
	
		return abs(res);
	}

	// Repeated Random
	long RR(int max_iters) {
		unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    	mt19937 generator(seed);
    	bernoulli_distribution bern(0.5);
		// Random solution S
		for(int _ = 0; _ < 100; _++) {
			prepart.push_back(bern(generator) ? 1 : -1);
		}
		check();
		for(int _ = 0; _ < max_iters; _ ++) {
			// New solution S'
			for(int i = 0; i < 100; i++) {
				prepart[i] = bern(generator) ? 1 : -1;
			}
			// Check if res(S') < res(S) and update if so
			check();
		}

		return best;
	}

	// Prepartitioned Repeated Random
	long P_RR(int max_iters) {
		unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    	mt19937 generator(seed);
    	uniform_int_distribution<int> unif(1,100);
		// Random solution S
		for(int _ = 0; _ < 100; _++) {
			prepart.push_back(unif(generator));
		}
		combine();
		for(int _ = 0; _ < max_iters; _ ++) {
			// New solution S'
			for(int i = 0; i < 100; i++) {
				prepart[i] = unif(generator);
			}
			// Check if res(S') < res(S) and update if so
			combine();
		}

		return best;
	}

	// Hill Climbing
	long HC(int max_iters) {
		unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    	mt19937 generator(seed);
    	bernoulli_distribution bern(0.5);
		uniform_int_distribution<int> randind(1,100);
		// Random solution S
		for(int _ = 0; _ < 100; _++) {
			prepart.push_back(bern(generator) ? 1 : -1);
		}
		check();

		for(int _ = 0; _ < max_iters; _ ++) {
			// Random neighbor S'
			int i = randind(generator);
			int j = randind(generator);
			while (i == j)
				j = randind(generator);

			int old_i = prepart[i];
			int old_j = prepart[j];

			prepart[i] *= -1;
			if (bern(generator)) {
				prepart[j] *= -1;
			}

			if(!check()) {
				prepart[i] = old_i;
				prepart[j] = old_j;
			}
		}

		return best;
	}

	// Prepartitioned Hill Climbing
	long P_HC(int max_iters) {
		unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    	mt19937 generator(seed);
		uniform_int_distribution<int> randind(1,100);
		// Random solution S
		for(int _ = 0; _ < 100; _++) {
			prepart.push_back(randind(generator));
		}
		combine();
		
		for(int _ = 0; _ < max_iters; _ ++) {
			// Random neighbor S'
			int i = randind(generator);
			int j = randind(generator);
			long old_i = prepart[i];

			while(prepart[i] == j) {
				j = randind(generator);
			}

			prepart[i] = j;

			if(!combine()) {
				prepart[i] = old_i;
			}
		}

		return best;
	}

	double T(int iter) {
		return(pow(10,10) * pow(0.8, floor(iter/300)));
	}

	long SA(int max_iters) {
		unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    	mt19937 generator(seed);
    	bernoulli_distribution bern(0.5);
		uniform_real_distribution<double> prob(0,1);
		uniform_int_distribution<int> randind(1,100);

		long sres;
		long spres;
		// Random solution S
		for(int _ = 0; _ < 100; _++) {
			prepart.push_back(bern(generator) ? 1 : -1);
		}
		best = check_SA();
		sres = best;

		for(int iter = 0; iter < max_iters; iter ++) {
			// Random neighbor S'
			int i = randind(generator);
			int j = randind(generator);
			while (i == j)
				j = randind(generator);

			int old_i = prepart[i];
			int old_j = prepart[j];

			prepart[i] *= -1;
			if (bern(generator)) {
				prepart[j] *= -1;
			}

			spres = check_SA();
			double p = exp(-(spres - sres)/T(iter));

			if(spres > sres && prob(generator) > p) {
				prepart[i] = old_i;
				prepart[j] = old_j;
			} else if (spres > sres) {
				sres = spres;
			} else {
				if (spres < best) 
					best = spres;
				sres = spres;
			}
		}
		return best;
	}

	long P_SA(int max_iters) {
		unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    	mt19937 generator(seed);
    	bernoulli_distribution bern(0.5);
		uniform_real_distribution<double> prob(0,1);
		uniform_int_distribution<int> randind(1,100);

		long sres;
		long spres;
		
		// Random solution S
		for(int _ = 0; _ < 100; _++) {
			prepart.push_back(randind(generator));
		}
		best = combine_SA();
		sres = best;

		for(int iter = 0; iter < max_iters; iter ++) {
			// Random neighbor S'
			int i = randind(generator);
			int j = randind(generator);
			while(prepart[i] == j)
				j = randind(generator);
			long old_i = prepart[i];

			prepart[i] = j;

			spres = combine_SA();
			double p = exp(-(spres - sres)/T(iter));

			if(spres > sres && prob(generator) > p) {
				prepart[i] = old_i;
			} else if (spres > sres) {
				sres = spres;
			} else {
				if (spres < best) 
					best = spres;
				sres = spres;
			}
		}
		return best;
	}
};

int main(int argc, char* argv[]) {
	int flag = stoi(argv[1]);
	int algorithm = stoi(argv[2]);
	char* inputfile = argv[3];

	// Task 5
	if (flag == 25) {
		long upper = pow(10, 12);
		unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    	mt19937 generator(seed);
		uniform_int_distribution<long> randnum(1, upper);

		std::chrono::microseconds kktime;
		std::chrono::microseconds rrtime;
		std::chrono::microseconds prrtime;
		std::chrono::microseconds hctime;
		std::chrono::microseconds phctime;
		std::chrono::microseconds satime;
		std::chrono::microseconds psatime;

		ofstream results;
		results.open("results.txt", ios::out | ios::trunc);

		for (int instance = 0; instance < 100; instance++) {
			vector<long> ins;

			for(int i = 0; i < 100; i++)
				ins.push_back(randnum(generator));

			Numpartset kkn(ins);
			auto start = chrono::high_resolution_clock::now();
			long kk = kkn.KK();
			auto stop = chrono::high_resolution_clock::now();
			kktime += chrono::duration_cast<chrono::microseconds>(stop - start);

			Numpartset rrn(ins);
			start = chrono::high_resolution_clock::now();
			long rr = rrn.RR(25000);
			stop = chrono::high_resolution_clock::now();
			rrtime += chrono::duration_cast<chrono::microseconds>(stop - start);

			Numpartset prrn(ins);
			start = chrono::high_resolution_clock::now();
			long prr = prrn.P_RR(25000);
			stop = chrono::high_resolution_clock::now();
			prrtime += chrono::duration_cast<chrono::microseconds>(stop - start);

			Numpartset hcn(ins);
			start = chrono::high_resolution_clock::now();
			long hc = hcn.HC(25000);
			stop = chrono::high_resolution_clock::now();
			hctime += chrono::duration_cast<chrono::microseconds>(stop - start);

			Numpartset phcn(ins);
			start = chrono::high_resolution_clock::now();
			long phc = phcn.P_HC(25000);
			stop = chrono::high_resolution_clock::now();
			phctime += chrono::duration_cast<chrono::microseconds>(stop - start);

			Numpartset san(ins);
			start = chrono::high_resolution_clock::now();
			long sa = san.SA(25000);
			stop = chrono::high_resolution_clock::now();
			satime += chrono::duration_cast<chrono::microseconds>(stop - start);

			Numpartset psan(ins);
			start = chrono::high_resolution_clock::now();
			long psa = psan.P_SA(25000);
			stop = chrono::high_resolution_clock::now();
			psatime += chrono::duration_cast<chrono::microseconds>(stop - start);

			results << kk << "," << rr << "," << prr << "," << hc << "," << phc << "," << sa << "," << psa << endl;
		}

		results << endl << kktime.count() / 100 << "," << rrtime.count() / 100 << "," << prrtime.count() / 100 << "," << hctime.count() / 100 << "," << phctime.count() / 100 << "," << satime.count() / 100 << "," << psatime.count() / 100 << endl;

		results.close();

		return 0;
	}





	Numpartset test(inputfile);

	switch(algorithm) {
		case 0:
			cout << test.KK() << endl;
			break;
		case 1:
			cout << test.RR(25000) << endl;
			break;
		case 2:
			cout << test.HC(25000) << endl;
			break;
		case 3:
			cout << test.SA(25000) << endl;
			break;
		case 11:
			cout << test.P_RR(25000) << endl;
			break;
		case 12:
			cout << test.P_HC(25000) << endl;
			break;
		case 13:
			cout << test.P_SA(25000) << endl;
			break;
	}

	return 0;
}