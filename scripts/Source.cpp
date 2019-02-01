#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <cmath>
#include <vector> 
#include <functional>
#include <string>
#include <queue>
#include <Windows.h>
#include <stdlib.h>     
#include <time.h>       
#include <random>
#include <iomanip>
using namespace std;

const int dx[4] = { -1, 0, 1, 0 };
const int dy[4] = { 0, -1, 0, 1 };
const float weight[2] = { 0.4, 0.3 };
#define MaxEpochs 1000000

class State {
	public:
		int a[20][20];
		int x, y;
		int u, v;
		int n, m;
		int step;
		int sum,sum2;
		int isMask;
};
class Action
{
public: int x, y;
};


void dijkstra(int source);
int rand_bernoulli(float a, float b);
void move_decide(State S);
float Estimate_probability(State S, int epochs);
int random_state_simulate(State S1);
bool isLive(int x, int y, int u, int v); 
bool isLegal(State S1, int u, int v);
Action random_act(State S1, int x, int y);


void dijkstra(int source) {
	bool vis[200]; int val[200];
	vector <int> a[200];
	memset(vis, false, sizeof vis);
	typedef pair <int, int> data;
	priority_queue <data, vector <data>, greater <data>> heap;
	heap.push({ 0, source });
	for (int d, u; !heap.empty(); heap.pop()) {
		d = heap.top().first; u = heap.top().second;
		if (vis[u]) continue;
		vis[u] = true;
		for (int v : a[u])
			heap.push({ d + val[v], v });
	}
}
/*
int main() {
	freopen("map.inp", "r", stdin);
	freopen("move.out", "w", stdout);

	///-----INPUT-----
	State S1;

	cin >> S1.n >> S1.m >> S1.step;
	cin >> S1.x >> S1.y >> S1.u >> S1.v;
	cin >> S1.sum >> S1.isMask; getchar();

	memset(S1.a, 0, sizeof S1.a);
	for (int i = 1; i <= S1.n; ++i) {
		string tmp; getline(cin, tmp);
		int j = 1;
		for (char c : tmp)
			if ('0' <= c && c <= '9')
				S1.a[i][j] = S1.a[i][j] * 10 + c - 48;
			else if (c == 'W') S1.a[i][j] = S1.isMask ? 0 : -1e9;
			else if (c == 'M') S1.a[i][j] = S1.isMask ? 0 : 1e9;
			else ++j;
	}
	///-----END OF INPUT-----

	///-----PROCESS-----
	int res = -1e9, x, y;
	if (!S1.x) {
		for (int i = 1; i <= S1.n; ++i)
			for (int j = 1; j <= S1.m; ++j)
				if (!S1.a[i][j]) {
					int tmp = max(max(S1.a[i - 1][j], S1.a[i + 1][j]), max(S1.a[i][j - 1], S1.a[i][j + 1]));
					if (res < tmp) res = tmp, x = i, y = j;
				}
	}
	else {
		// move decide 
		for (int i = 0; i < 4; ++i) {
			int u = S1.x + dx[i];
			int v = S1.y + dy[i];
			if (1 <= u && u <= S1.n && 1 <= v && v <= S1.m) {
				if (S1.a[u][v] > res) res = S1.a[u][v], x = u, y = v;
			}
		}
	}
	cout << x << ' ' << y;
	///-----END OF PROCESS-----
}
*/
int main() {

	srand(time(0));
	freopen("map.inp", "r", stdin);

	freopen("move.out", "w", stdout);
	///-----INPUT-----
	State S1;
	cin >> S1.n >> S1.m >> S1.step;
	cin >> S1.x >> S1.y >> S1.u >> S1.v;
	cin >> S1.sum >> S1.isMask; getchar();
	memset(S1.a, 0, sizeof S1.a);
	for (int i = 1; i <= S1.n; ++i) {
		string tmp; getline(cin, tmp);
		int j = 1;
		for (char c : tmp)
			if ('0' <= c && c <= '9')
				S1.a[i][j] = S1.a[i][j] * 10 + c - 48;
			else if (c == 'W') S1.a[i][j] = S1.isMask ? 0 : -1e9;
			else if (c == 'M') S1.a[i][j] = S1.isMask ? 0 : 1e9;
			else ++j;
	}
	///-----END OF INPUT-----
	
	///-----PROCESS-----
	//	
	int res = -1e9, x, y;
	if (!S1.x) {
		for (int i = 1; i <= S1.n; ++i)
			for (int j = 1; j <= S1.m; ++j)
				if (!S1.a[i][j]) {
					int tmp = max(max(S1.a[i - 1][j], S1.a[i + 1][j]), max(S1.a[i][j - 1], S1.a[i][j + 1]));
					if (res <= tmp) res = tmp, x = i, y = j;
				}
	}
	else {
		// move decide
		S1.sum = 0; // Should I do this ???
		S1.sum2 = 0;
		S1.step = min(7,S1.step);
		move_decide(S1);
		return 0;
	}
	cout << x << ' ' << y;
	return 0;
}
int distance(int x, int y, int u, int v) {
	return (abs(x - u) + abs(y - v));
}
int rand_bernoulli(float a, float b) {
	random_device rd;
	mt19937 gen(rd());
	// give "true" 0.4 of the first time
	// give "false" 0.6 of the first time
	// give 0.2 of the second time
	bernoulli_distribution aa(a);
	bernoulli_distribution bb(b);
	if (aa(gen))
		return 1;
	else if (bb(gen))
		return 0;
	else if (bb(gen))
		return 2;
	else 
		return 3;

}
float euclide_dis(int a, int b)
{
	a = max(a, 0);
	b = max(b, 0);
	return sqrt(a*a + b * b);
}
void move_decide(State S) {
	int	res = 0; 
	int	u = S.x;
	int	v = S.y;
	int	x = S.x;
	int y = S.y;

	int d[4] = { 0, 1, 2, 3 };
	random_shuffle(d, d + 4);
	for (int i = 0; i < 4; ++i) {
		S.x = x + dx[d[i]];
		S.y = y + dy[d[i]];
		if (1 <= S.x && S.x <= S.n && 1 <= S.y && S.y <= S.m) {
			State tmpS = S;
			if (S.a[S.x][S.y] < 0) continue; 
			int loop = 500;
			float tmp = Estimate_probability(tmpS, loop);
			//cout << i << ' ' << tmp << endl;
			if (tmp >= res) res = tmp, u = S.x, v = S.y;
			
		}
	}
	float threshold = 0;
	int quickCal = S.a[x + 1][y - 1] + S.a[x + 1][y] + S.a[x + 1][y + 1] + S.a[x][y - 1] + 4*S.a[x][y] + S.a[x][y + 1] + S.a[x - 1][y - 1] + S.a[x - 1][y] + S.a[x - 1][y + 1];
	int tempDis = 0;
	if (res <= 0.3) {
		if (quickCal == 0)
		{
			for (int i = 1; i <= S.n; ++i)
				for (int j = 1; j <= S.m; ++j)
					if (S.a[i][j] > 0) {
						int quickCal = S.a[i + 1][j - 1] + S.a[i + 1][j] + S.a[i + 1][j + 1] + S.a[i][j - 1] + 8 * S.a[i][j] + S.a[i][j + 1] + S.a[i - 1][j - 1] + S.a[i - 1][j] + S.a[i - 1][j + 1];
						if (S.isMask) {
							int point_scaler = 30;
							if (distance(x, y, i, j) < distance(u, v, i, j))
								if (euclide_dis(distance(x, y, i, j) - tempDis, point_scaler * (quickCal - threshold)) > 0) {
									threshold = quickCal;
									tempDis = distance(x, y, i, j);
									u = i;
									v = j;
								}
						}
						else if (S.a[i][j] > threshold) {
							threshold = S.a[i][j];
							u = i;
							v = j;
						}
					}
			
		}
		else
		{
			for (int i = 0; i < 4; ++i) {
				u = x + dx[i];
				v = y + dy[i];
				if (S.a[u][v] > 0) break;
			}
		}
		if (u > x) if (S.a[x + 1][y] >= 0){ x += 1, cout << x << ' ' << y; return;}
		if (u < x) if (S.a[x - 1][y] >= 0){ x -= 1, cout << x << ' ' << y; return;}
		if (v > y) if (S.a[x][y + 1] >= 0){ y += 1, cout << x << ' ' << y; return;}
		if (v < y) if (S.a[x][y - 1] >= 0){ y -= 1, cout << x << ' ' << y; return;}
												   
		return;
	}
	

	cout << u << ' ' << v;
}

float Estimate_probability(State S, int epochs) {
	float global_sum = 0;

	//cout << "debug";
	for (int i = 0; i < epochs; i++) {
		State S1 = S;
		global_sum += random_state_simulate(S1);

	}
	float result = global_sum / epochs;
	return result;
}
int random_state_simulate(State S1) {
	
	if (S1.step > 0)
	{
		//____Init Character_____
		Action char_move = random_act(S1, S1.x, S1.y);
		S1.x = char_move.x;
		S1.y = char_move.y;
		// _____Init Enemy___
		Action enemy_move = random_act(S1, S1.u, S1.v);
		S1.u = enemy_move.x;
		S1.v = enemy_move.y;
		if (S1.sum < 0) return 0;
		if (!(isLive(S1.x, S1.y, S1.u, S1.v))) return (0);
		S1.sum += S1.a[S1.x][S1.y];
		S1.sum2 += S1.a[S1.u][S1.v];
		S1.a[S1.x][S1.y] = -1; 
		S1.a[S1.u][S1.v] = 0;
		--S1.step;
		return (random_state_simulate(S1));
	}
	else {
		//cout << S1.sum << ' ' << S1.sum2 << " WIN" << endl;
		return (S1.sum >= S1.sum2 ? 1 : 0);
	
	}
}
Action random_act(State S1, int x, int y) {
	Action sim;
	// ____Init____
	int u = x, v = y;
	int _u, _v;
	int tmp = 0;
	
	//int d[4] = { 0, 1, 2, 3 };
	//random_shuffle(d, d + 4);
	for (int i = 0; i < 4; ++i) {
		_u = x + dx[rand_bernoulli(weight[0], weight[1])];
		_v = y + dy[rand_bernoulli(weight[0], weight[1])];
		if (isLegal(S1, _u, _v)) {
			if (S1.a[_u][_v] >= tmp) {
				tmp = S1.a[_u][_v], u = _u, v = _v; break;
			}

		}
	}
	sim.x = u;
	sim.y = v;
//	cout << u << ' ' << v << endl;
	return sim; 
}
bool isLive(int x, int y, int u, int v) {
	return ((x - u) || (y - v));
}
bool isLegal(State S1, int u, int v)
{
	if (1 <= u && u <= S1.n && 1 <= v && v <= S1.m)
		if (S1.a[u][v] >= 0 )
			return 1;
	return 0;
}