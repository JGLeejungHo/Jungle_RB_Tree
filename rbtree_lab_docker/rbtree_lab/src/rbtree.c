#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *nil = (node_t *)calloc(1, sizeof(node_t));

  nil->color = RBTREE_BLACK;  // nil 노드는 항상 BLACK
  p->root = nil;
  p->nil = nil;     // 빈 트리는 nil
  return p;
}

void free_tree(rbtree *t, node_t *node){
  // 노드의 왼쪽 자식이 nil이 아닌 경우
  if(node->left != t->nil)
    free_tree(t, node->left); // 왼쪽 자식 노드부터 재귀적으로 메모리 해제 수행
  // 노드의 오른쪽 자식이 nil이 아닌 경우
  if(node->right != t->nil)
    free_tree(t, node->right); // 오른쪽 자식 노드부터 재귀적으로 메모리 해제 수행
  // 현재 노드의 메모리 해제
  free(node);
}

void delete_rbtree(rbtree *t) {
  if (t==NULL) {
  return;  //비어 있다면 리턴
  }
  node_t *cur = t->root;
  if(cur != t->nil){ 
    free_tree(t, cur);
  }
  free(t->nil);
  free(t);
}

void left_rotate(rbtree *t, node_t *x) {
  node_t *y = x->right; // y는 x의 오른쪽 자식

  // 1) x의 오른쪽을 y의 왼쪽으로 연결
  x->right = y->left;
  if (y->left != t->nil) {
    y->left->parent = x;
  }
  // 2) y를 x의 위치로 올림
  y->parent = x->parent;

  if (x->parent == t->nil) {    
    t->root = y; // x가 루트였음
  } 
  else if (x == x->parent->left) {
    x->parent->left = y;
  } 
  else {
    x->parent->right = y;
  }

  // 3) x를 y의 왼쪽 자식으로
  y->left = x;
  x->parent = y;
}

void right_rotate(rbtree *t, node_t *x) {
  node_t *y = x->left; // y는 x의 왼쪽 자식

  // 1) x의 왼쪽을 y의 오른쪽으로 연결
  x->left = y->right;
  if (y->right != t->nil) {
    y->right->parent = x;
  }

  // 2) y를 x의 위치로 올림
  y->parent = x->parent;
  if (x->parent == t->nil) {
    t->root = y;
  } else if (x == x->parent->right) {
    x->parent->right = y;
  } else {
    x->parent->left = y;
  }

  // 3) x를 y의 오른쪽 자식으로
  y->right = x;
  x->parent = y;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *in_node = (node_t *)malloc(sizeof(node_t)); //추가할 노드 할당
  if (in_node == NULL){ // 할당에 실패했을경우 리턴
    return NULL;
  }
  // 새 노드 초기화: 자식은 nil, 색은 RED
  in_node->key = key; //할당한 노드에 추가할 키 집어넣고
  in_node->left = t->nil;  //할당한 노드의 양쪽에 nill 넣기
  in_node->right = t->nil;
  in_node->color = RBTREE_RED; // 추가하는 node 는 빨간색

  node_t *cur = t->root; // 현재 비교 하면서 내려갈 노드
  node_t *parent = t->nil;  // 다내려왔을때 추가할 키가 오른쪽으로갈지 왼족으로 갈지 알려줄 마지막으로 지나온 노드

  // BST 삽입 위치 찾기
  while (cur != t->nil) {  // 만약 t.root가 마지막(nil)에 도달 하지 않았다면
    parent = cur; // parent에 현재 위치 저장
    if (in_node->key < cur->key){  // 키값을 비교하며 오른쪽으로 갈지 왼쪽으로 갈지 계속해서 결정
      cur = cur->left;
    }
    else {
      cur = cur->right;
  }
  }
  // 부모 연결
  in_node->parent = parent; // 위에서 위치가 정해졌다면 그위치가 추가할 노드의 부모의 위치

  if (parent == t->nil) { // 만약 부모가 nil 이라면 위에 와일을 안거쳐서 왓다는뜻
    t->root = in_node;
  } 
  else if (in_node->key < parent->key) { // 만약 현재 추가할 키가 부모의 키보다 작다면 왼쪽 크다면 오른쪽
    parent->left = in_node;
  } 
  else {
    parent->right = in_node;
  }
  rbtree_insert_fixup(t,in_node);   // 추가한 노드의 색으로인해 색깔이 맞는지 안맞는지 확인하면서 교체하는 작업
  return in_node; //추가한 노드 리턴
}

void rbtree_insert_fixup(rbtree *t, node_t *in_node){  // 교체작업진행
  while (in_node->parent->color == RBTREE_RED) {   // 만약 추가한 노드의 부모의 색이 빨간색이라면  // 현재 추가한 노드의 색도 빨간색
    if (in_node->parent == in_node->parent->parent->left) { // 부모의 색과 삼촌의 색이 빨간색 으로 같다면
      node_t *uncle = in_node->parent->parent->right; // 추가한 노드의 삼촌값 저장
      if (uncle->color == RBTREE_RED) {  // 삼촌의 색이 빨간색이라면
        // case 1: 부모와 삼촌이 둘 다 RED라면
        in_node->parent->color = RBTREE_BLACK; // 부모의 색을 검은색으로 바꾸고 
        uncle->color = RBTREE_BLACK; //삼촌의 색도 검은색으로바꾸고
        in_node->parent->parent->color = RBTREE_RED; // 조부모의 색을 빨간색으로 바꿈

        in_node = in_node->parent->parent; // 추가한 노드는 조부모의 위치로 이동해서 한번더 RB의 규칙에 맞는지 확인
      } 
      else {
        // case 2 & 3 부모는 빨간색이지만 삼촌은 검은색인 경우
        if (in_node == in_node->parent->right) { // 추가한 노드의 위치가 부모의 오른쪽이라면
          in_node = in_node->parent;   //추가한 노드는 추가한 노드의 부모로 이동
          left_rotate(t, in_node); // 왼쪽으로 돌려서 case 3으로 만듬
        }
        // case 3: 추가한 노드가 부모의 왼쪽인 경우
        in_node->parent->color = RBTREE_BLACK;  //추가한 노드의 부모님을 색을 검은색으로 교체
        in_node->parent->parent->color = RBTREE_RED; // 조부모의 색을 빨간색으로 교체
        right_rotate(t, in_node->parent->parent);  // 오른쪽으로 돌림
      }
    } 
    else {
      node_t *uncle = in_node->parent->parent->left; // 조부모의 왼쪽에 삼촌의 위치를 저장
      if (uncle->color == RBTREE_RED) {  //삼촌의 색이 빨간색이라면
        in_node->parent->color = RBTREE_BLACK; // 추가한 노드의 부모님은 검은색
        uncle->color = RBTREE_BLACK; //삼촌의 색은 검은색
        in_node->parent->parent->color = RBTREE_RED; // 조부모의 색은 빨간색

        in_node = in_node->parent->parent;  //추가한노드를 조부모로 이동해서 다시한번더 검사
      } else { // 삼촌의 색이 검은색이라면
        if (in_node == in_node->parent->left) { // 추가한노드가 부모의 왼쪽에 있다면 // case 2
          in_node = in_node->parent; // 추가한노드에 노드의 부모를 저장하고
          right_rotate(t, in_node); //오른쪽으로 돌림
        }
        in_node->parent->color = RBTREE_BLACK; // 추가한 노드의 부모의색을 검은색으로 // case 3
        in_node->parent->parent->color = RBTREE_RED; // 조부모의 색은 빨간색
        left_rotate(t, in_node->parent->parent); //조부모님의 위치를 왼쪽으로 돌림
      }
    }
  }


  t->root->color = RBTREE_BLACK;  // 루트는 항상 BLACK
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *cur = t->root;   //cur 에다가 t의 root(맨위) 를 넣음
  while (cur != t->nil) { //cur 이 맨끝(nil)을 만나기전까지 반복
    if (cur->key == key){ //지나갈려는 노드의 키와 찾는 키가 같다면
      return cur; // 리턴 cur
    } 
    else if (key < cur->key) { // 찾는키가 지나가려는 노드의 키보다 작다면 왼쪽
      cur = cur->left;
    } 
    else { // 찾는키가 지나가려는 노드의 키보다 크다면 오른쪽
      cur = cur->right;
    }
  }
  return NULL; // 못 찾았다면
}

node_t *rbtree_min(const rbtree *t) {
  if(t == NULL){
    return NULL;
  }
  node_t *cur = t->root; // cur 에다가 t의 root(맨위)를 주고
  while (cur->left != t->nil){ // cur 이 맨끝(nil)에 도달하기전까지 반복
    cur = cur->left; // 이진 탐색 트리의 특성으로인해 최소값인 왼쪽으로 계속가면된다
  }
  return cur; // 찾은값 반환
}

node_t *rbtree_max(const rbtree *t) {
    if(t == NULL){
    return NULL;
  }
  node_t *cur = t->root; // 마찬가지로 cur 에다가 t의 root(맨위)를 주고
  while (cur->right != t->nil) { // cur 이 맨끝(nil)에 도달하기전까지 반복
    cur = cur->right; // 이진 탐색 트리의 특성으로인해 최대값인 오른쪽으로 계속가면된다
  }
    return cur; // 찾은값 반환
}

void rbtree_transplant(rbtree *t, node_t *u, node_t *v) {
  if (u->parent == t->nil) { // u노드의 부모님이 nil 즉 u가 root 라면
    t->root = v;             // v를 t의 root로 교체
  }
  else if (u == u->parent->left) { // u가 부모의 왼쪽에 있다면
    u->parent->left = v;           // v를 u의부모의 왼쪽에다가 덮어 씌움
  }
  else {                    // u가 부모의 오른쪽에 있다면
    u->parent->right = v;   // v를 u의 부모의 오른쪽에다가 덮어 씌움
  }
  v->parent = u->parent; // 마지막으로 v가 가르키는 부모를 u가 가르키는 부모로 바꿈
}

node_t *node_successor(rbtree *t, node_t *node) { // 현재 노드의값보다 제일 가까우면서 큰값
  if (node->right != t->nil) { // 찾는노드의 오른쪽이 끝(nil)이 아니라면
    node_t *cur = node->right; // cur은 노드의 오른쪽
    
    while (cur->left != t->nil) { // 현재 위치에서 왼쪽이 (nil)끝이 아니라면
      cur = cur->left; //  현재 찾는노드보다는 크지만 오른쪽에서 제일 작은값
    }
    return cur; // 찾은 값을 리턴
  }
  else{  //못 찾았다면
  return NULL; // 리턴 NULL
  }
}

void rbtree_erase_fixup(rbtree *t, node_t *p) {
  while (p != t->root && p->color == RBTREE_BLACK) { // p 가 root(맨 위) 가 아니고 p의 색이검은색이라면
    if (p == p->parent->left) {     // 현재의 위치가 부모의 왼쪽이라면
      node_t *bro = p->parent->right; // p의 형제
      // case 1: 형제가 RED이면 색 바꾸고 좌회전
      if (bro->color == RBTREE_RED) { // 형제의 색이 빨간색이라면
        bro->color = RBTREE_BLACK; //형제의 색은 검은색
        p->parent->color = RBTREE_RED; //부모의 색은 빨간색
        left_rotate(t, p->parent); // 왼쪽으로 돌림
        bro = p->parent->right; // 바뀐 형제 갱신
      }
      if (bro->left->color == RBTREE_BLACK && bro->right->color == RBTREE_BLACK) { // case 2: 형제와 형제의 자식들이 모두 BLACK
        bro->color = RBTREE_RED;  // 형제는 빨간색으로 변경
        p = p->parent; // 현재위치를 현재위치의 부모로 옴김
      } 
      else {
        if (bro->right->color == RBTREE_BLACK) { // case 3: 형제의 오른쪽 자식이 BLACK인 경우
          bro->left->color = RBTREE_BLACK; //형제의 왼쪽을 검은색으로
          bro->color = RBTREE_RED; // 형제의색은 빨간색으로 // 부모와 자식의 색을 바꿔줌
          right_rotate(t, bro); // 오른쪽으로 돌림
          bro = p->parent->right; // 바뀐 형제 갱신
        }
        bro->color = p->parent->color; // case 4: 형제의 오른쪽이 RED인 경우
        p->parent->color = RBTREE_BLACK; // 목표의 부모는 검은색
        bro->right->color = RBTREE_BLACK; // 형제의 오른쪽은 검은색
        left_rotate(t, p->parent); // 목표의 부모를 왼쪽으로 돌림
        p = t->root; // 더 이상 extra black 없음
      }
    } else {
      // 대칭 케이스
      node_t *bro = p->parent->left;
      if (bro->color == RBTREE_RED) {
        bro->color = RBTREE_BLACK;
        p->parent->color = RBTREE_RED;
        right_rotate(t, p->parent);
        bro = p->parent->left;
      }
      if (bro->left->color == RBTREE_BLACK && bro->right->color == RBTREE_BLACK) {
        bro->color = RBTREE_RED;
        p = p->parent;
      } else {
        if (bro->left->color == RBTREE_BLACK) {
          bro->right->color = RBTREE_BLACK;
          bro->color = RBTREE_RED;
          left_rotate(t, bro);
          bro = p->parent->left;
        }
        bro->color = p->parent->color;
        p->parent->color = RBTREE_BLACK;
        bro->left->color = RBTREE_BLACK;
        right_rotate(t, p->parent);
        p = t->root;
      }
    }
  }
  p->color = RBTREE_BLACK; // 최종적으로 p는 BLACK
}

int rbtree_erase(rbtree *t, node_t *p) {

  node_t *y = p; // 삭제할 노드를 저장
  color_t y_original_color = y->color; // 삭제할 y의 색을 저장
  node_t *x; //삭제된 노드를 대체할 노드

  if (p->left == t->nil) { //삭제한 노드를 대체할 왼쪽 자식이 없다면
    // 오른쪽로 대체 // 오른쪽이 없더라도 오른쪽으로 대체
    x = p->right;
    rbtree_transplant(t, p, p->right); // 위 오른쪽아래 변경
  } 
  else if (p->right == t->nil) {
    // 오른쪽 자식이 없으면 왼쪽으로 대체 //왼쪽이 없더라도 위에서 걸러짐
    x = p->left;
    rbtree_transplant(t, p, p->left); // 위 왼쪽아래 변경
  } 
  else {
    // 자식이 둘다 있는 후임자를 찾아서
    y = node_successor(t, p);
    y_original_color = y->color; //삭제하려던 노드의 색을 넣고
    x = y->right; // 둘다있으니 삭제한 노드의 반대쪽을 저장
    if (y->parent == p) { // y의 부모가 삭제할 노드라면
      // x의 부모를 y로 설정
      x->parent = y;
    } 
    else {
      rbtree_transplant(t, y, y->right); // y랑를 내리고 y오른쪽을 올림
      y->right = p->right; // p 의 오른쪽이 y의 오른쪽을 가르키게하고
      y->right->parent = y; // y의 오른쪽의 부모는 y
    }
    rbtree_transplant(t, p, y); // p를 y로 대체
    y->left = p->left; // p의 왼쪽을 y의 왼쪽으로 바꾸고
    y->left->parent = y; // y의 왼쪽 부모가 y를 가르키게 바꿈
    y->color = p->color; //y의 색은 삭제된p의 색으로 교체
  }

  free(p); // 원래 삭제할 노드 메모리 해제

  if (y_original_color == RBTREE_BLACK) {
    // 원래 삭제된 노드의 색이 BLACK이었다면 균형/색 보정 필요
    rbtree_erase_fixup(t, x);
  }
  return 0;
}

int in_array(node_t *x, const rbtree *t, key_t *arr, int i) {
  if (x == t->nil){
    return i;
  }
  i = in_array(x->left , t, arr, i);
  arr[i++] = x->key;
  i = in_array(x->right , t, arr, i);
  return i;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  if (t->root != t->nil){
    in_array(t->root , t, arr,0);
  }
  return 0;
}