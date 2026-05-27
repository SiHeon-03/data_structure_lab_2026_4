import sys

## 입력 받는 코드입니다. 수정할 필요 없습니다.
sys.stdin = open('case.txt')
N, M = list(map(int,input().split()))
print(f"콘서트장 크기: {N} x {M}")
concerts = []
for v in range(N):
    values = list(map(int, input().split()))
    concerts.append(values)

# print(concerts)
# [[1, 0, 0, 1, 1, 0], [1, 0, 1, 1, 0, 0], [1, 1, 1, 1, 0, 1], [0, 1, 1, 0, 1, 1], [0, 1, 0, 0, 1, 0]]
###################################



def count_stages(concerts):
    """
    콘서트장 지도에서 펜스(1)로 분리된 독립적인 무대 공간(0)의 개수를 산출.
    
    [설계 원리]
    1. 지도의 모든 칸을 순회하며 빈 공간('0')이고 아직 방문하지 않은 곳을 찾는다.
    2. 해당 지점을 발견하면 새로운 무대 공간이 시작된 것으로 간주하고 answer를 1 증가시킨다.
    3. DFS를 통해 해당 지점과 상하좌우로 연결된 모든 '0'을 찾아 '방문 완료(1)'로 표시한다.
    4. 이 과정을 반복하여 더 이상 방문하지 않은 '0'이 없을 때까지 수행한다.
    
    [히든 케이스 대응]
    1.len() 함수를 사용하여 N과 M이 다른 직사각형 구조나 다양한 크기의
       히든 케이스에서도 코드 수정 없이 작동하도록 설계함
    2.재귀 호출 시 인덱스 범위를 초과하는  IndexError를 방지하기 위해
       탐색 시작 전 경계 조건을 엄격히 체크함
    3.방문한 무대(0)를 펜스(1)로 변경함으로써 별도의 메모리 
       사용 없이 중복 방문을 차단하고 무한 루프 위험을 제거
    4.무대가 아예 없거나(0개), 전체가 무대인 경우(1개) 등 
       극단적인 입력값에서도 정확한 결과값을 도출
       
    """
    """


    :param concerts: 0(무대)과 1(펜스)로 구성된 2차원 리스트
    :return: 독립된 무대 공간의 총 개수 (int)
    
    """
    answer = 0
        
    # 지도의 가로 세로 길이를 정의하여 탐색 범위 설정
    rows = len(concerts)
    cols = len(concerts[0])

    def dfs(r, c):
        """
        재귀적으로 상하좌우를 탐색하며 연결된 빈 공간을 방문 처리

        
        :param r: 현재 행 위치
        :param c: 현재 열 위치
        :return: None
        """
        # 지도를 벗어나거나, 이미 방문했거나, 펜스(1)인 경우 탐색 중단
        if r < 0 or r >= rows or c < 0 or c >= cols or concerts[r][c] == 1:
            return
        
        # 현재 위치 방문 처리 (다시 방문하지 않도록 1로 변경)
        concerts[r][c] = 1
        
        # 상, 하, 좌, 우 네 방향으로 깊이 우선 탐색(DFS) 진행
        dfs(r - 1, c) # 상
        dfs(r + 1, c) # 하
        dfs(r, c - 1) # 좌
        dfs(r, c + 1) # 우

    # 전체 지도를 한 칸씩 확인하기
    for i in range(rows):
        for j in range(cols):
            # 빈 공간(0)을 발견하면 새로운 무대 탐색 시작
            if concerts[i][j] == 0:
                # 연결된 모든 공간을 '1'로 메우기 위해 DFS 호출
                dfs(i, j)
                # 하나의 독립된 공간을 모두 찾았으므로 무대 수(answer)증가
                answer += 1
                
    return answer

# 결과 출력
print(f"총 독립 무대 공간 개수: {count_stages(concerts)}")
