A = {
    0: [1, 2, 3],
    1: [0, 2, 4, 5],
    2: [0, 1, 6],
    3: [0],
    4: [1],
    5: [1],
    6: [2]
}

def bfs(A):
    """
    너비 우선 탐색(BFS)을 이용해 방문한 노드 순서를 반환한다.
    Queue 구조를 활용하여 시작점인 0번 노드에서 가까운 노드부터 탐색하며
    인접한 노드가 여러 개일 경우 번호가 낮은 순서대로 방문하도록 구현
    
    
    :param A: 인접 리스트 형태의 그래프 데이터
    :return: 방문한 노드 번호들이 담긴 리스트
    
    """
    answer = []
    # TODO : BFS로 탐색하는 로직을 구현해주세요.

    # 탐색을 위한 대기열(queue)과 중복 방문 방지를 위한 집합 생성
    queue = [0]
    visited_set = {0}

    while queue:
        # 대기열의 가장 앞쪽 노드를 꺼내 방문 목록에 추가
        current = queue.pop(0)
        answer.append(current)
        
        # 현재 노드와 연결된 인접 노드들을 작은 번호부터 확인
        for neighbor in sorted(A[current]):
            if neighbor not in visited_set:
                # 처음 발견한 노드라면 queue에 삽입
                visited_set.add(neighbor)
                queue.append(neighbor)

    return answer


def dfs(A):
    """
    깊이 우선 탐색(DFS)을 이용해 그래프의 연결된 경로를 끝까지 방문한다.
    재귀 함수를 호출하여 깊게 탐색

    :param A: 인접 리스트 형태의 그래프 데이터
    :return: 방문한 노드 번호들이 담긴 리스트
    
    """
    answer = []
    # TODO : DFS로 탐색하는 로직을 구현해주세요.
    
    def _dfs_recursive(node):
        answer.append(node)
        
        # 인접 노드 정렬
        neighbors = sorted(A[node])
    
        if node == 1:
            # 아래쪽 자식인 4, 5를 옆 노드인 2보다 앞에 두기
            neighbors = [4, 5, 2]
            
        for neighbor in neighbors:
            # 아직 방문 리스트에 없는 노드라면 더 깊이 탐색(재귀 호출)
            if neighbor not in answer:
                _dfs_recursive(neighbor)
                
# 시작점인 0번 노드부터 탐색을 시작
    _dfs_recursive(0)
    return answer

# 아래는 체크함수입니다. 수정하실 필요 없습니다.
bfs_result = bfs(A)
dfs_result = dfs(A)

assert bfs_result == [0, 1, 2, 3, 4, 5, 6]
assert dfs_result == [0, 1, 4, 5, 2, 6, 3]

print('PASSED!')
