#pragma warning(disable:4996)
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cctype>
#include <queue>

using namespace std;

int K, M;
int origin_arr[5][5];
int ans;
int dy[4] = { 1,-1,0,0 };
int dx[4] = { 0,0,1,-1 };
queue<int>parts;

struct Cmp {
	bool operator()(pair<int, int>a, pair<int, int>b) {
		if (a.second != b.second) {
			return a.second > b.second;
		}
		return a.first < b.first;
	}
};

struct info {
	int value, rot, row, col;
	priority_queue<pair<int, int>, vector<pair<int, int>>, Cmp>pq;
};

struct Cmp1 {
	bool operator()(info a, info b) {
		if (a.value != b.value) return a.value < b.value;
		else if (a.rot != b.rot) return a.rot > b.rot;
		else if (a.col != b.col) return a.col > b.col;
		else return a.row > b.row;
	}
};


void input() {
	cin >> K >> M;
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			cin >> origin_arr[i][j];
		}
	}
	for (int i = 0; i < M; i++) {
		int p;
		cin >> p;
		parts.push(p);
	}
}

bool isValid(int y, int x) {
	return y >= 0 && x >= 0 && y < 5 && x < 5;
}

info calValue(int arr[][5], int row, int col, int rot) {
	queue<pair<int, int>>q;
	priority_queue<pair<int, int>, vector<pair<int, int>>, Cmp>pq;

	bool visited[5][5] = { false, };
	int value = 0;
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			queue<pair<int, int>>q2;
			if (visited[i][j])continue;
			q.push({ i,j });
			q2.push({ i,j });
			visited[i][j] = true;
			int cnt = 1;
			while (!q.empty()) {
				int cy = q.front().first;
				int cx = q.front().second;
				int cv = arr[cy][cx];
				q.pop();
				for (int k = 0; k < 4; k++) {
					int ny = cy + dy[k];
					int nx = cx + dx[k];
					if (visited[ny][nx])continue;
					if (!isValid(ny, nx))continue;
					int nv = arr[ny][nx];
					if (cv != nv)continue;
					q.push({ ny,nx });
					q2.push({ ny,nx });
					visited[ny][nx] = true;
					cnt++;
				}
			}
			if (cnt >= 3) {
				value += cnt;
				while (!q2.empty()) {
					pq.push(q2.front());
					q2.pop();
				}
			}
		}
	}
	info result = { value, rot, row, col, pq };
	return result;
}


info func1() {

	// 모든 회전에 대해 우선순위로 정렬
	priority_queue<info, vector<info>, Cmp1>pq;
	info v = {};
	// 3x3 선택 (1. 가치 최대, 2. 회전 최소, 3. 열행 최소)	
	for (int i = 1; i < 4; i++) {
		for (int j = 1; j < 4; j++) {
			// 회전
			// 원본 배열 복사
			int copy_arr[5][5] = { 0, };
			for (int i = 0; i < 5; i++) {
				for (int j = 0; j < 5; j++) {
					copy_arr[i][j] = origin_arr[i][j];
				}
			}
			int sy = i - 1;
			int sx = j - 1;

			for (int k = 1; k <= 3; k++) {
				int ccopy_arr[5][5] = { 0, };
				for (int y = 0; y < 5; y++) {
					for (int x = 0; x < 5; x++) {
						ccopy_arr[y][x] = copy_arr[y][x];
					}
				}
				for (int y = 0; y < 3; y++) {
					for (int x = 0; x < 3; x++) {
						copy_arr[x + sy][2 - y + sx] = ccopy_arr[y + sy][x + sx];
					}
				}
				v = calValue(copy_arr, sy, sx, k);
				pq.push(v);
			}

		}
	}
	return pq.top();
}

void func2(info a) {

	// origin_arr를 회전시키고 유물 조각 교체
	int sy = a.row;
	int sx = a.col;
	int sr = a.rot;

	int copy_arr[5][5] = { 0, };
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			copy_arr[i][j] = origin_arr[i][j];
		}
	}

	for (int k = 1; k <= sr; k++) {
		int ccopy_arr[5][5] = { 0, };
		for (int y = 0; y < 5; y++) {
			for (int x = 0; x < 5; x++) {
				ccopy_arr[y][x] = copy_arr[y][x];
			}
		}
		for (int y = 0; y < 3; y++) {
			for (int x = 0; x < 3; x++) {
				copy_arr[x + sy][2 - y + sx] = ccopy_arr[y + sy][x + sx];
			}
		}
	}

	while (!a.pq.empty()) {
		int ny = a.pq.top().first;
		int nx = a.pq.top().second;
		int nv = parts.front();
		copy_arr[ny][nx] = nv;
		a.pq.pop();
		parts.pop();
	}

	ans += a.value;

	while (1) {
		info next = calValue(copy_arr, 0, 0, 0);
		if (next.value == 0)break;
		while (!next.pq.empty()) {
			int ny = next.pq.top().first;
			int nx = next.pq.top().second;
			int nv = parts.front();
			copy_arr[ny][nx] = nv;
			next.pq.pop();
			parts.pop();
		}
		ans += next.value;
	}
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			origin_arr[i][j] = copy_arr[i][j];
		}
	}
}

bool func() {

	// 1. 탐사 진행 (모든 회전을 통한 격자 선택)
	info selected_rot = func1();
	if (selected_rot.value == 0)return false;

	// 2. 유물 획득
	func2(selected_rot);

	return true;
}

int main(void) {
	//freopen("input.txt", "r", stdin);
	input();
	for (int i = 0; i < K; i++) {
		ans = 0;
		if (!func()) break;
		cout << ans << " ";
	}
	return 0;
}