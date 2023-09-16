#include "heap/heap.h"


#ifdef __cplusplus
template<typename T> void swap(T &a, T &b) {
    T tmp{a}; a = b;
    b = tmp;
}

template <class T>
class Stack
{
public:
    struct Node;
    typedef struct Node *PtrToNode;
    typedef PtrToNode Stack_t;
    struct Node{
        T           Element;
        PtrToNode   Next;
    };
    int IsEmpty(Stack_t S){
        return S->Next = NULL;
    }
    Stack_t CreateStack(void){
        Stack_t S;
        S = _Malloc1(sizeof(struct Node));
        if(S == NULL){
            return (Stack_t)-1;
        }
        S->Next = NULL;
        MakeEmpty(S);
        return S;
    }

    void Pop(Stack_t S){
        PtrToNode FirstCell;
        if(IsEmpty(S)){
            return;
        }else{
            FirstCell = S->Next;
            S->Next = S->Next->Next;
            _Free(FirstCell);
        }
    }

    void Push(T X,Stack_t S){
        PtrToNode TmpCell;

        TmpCell = _Malloc1(sizeof(struct Node));
        if(TmpCell == NULL){
            return;
        }else{
            TmpCell->Element = X;
            TmpCell->Next = S->Next;
            S->Next = TmpCell;
        }
    }

    T Top(Stack_t S){
        if(!IsEmpty(S))
            return S->Next->Element;
        return 0;
    }

    void MakeEmpty(Stack_t S){
        if(S == NULL){
            return;
        }else{
            while (!IsEmpty(S))
            {
                Pop(S);
            }
            
        }
    }
};
#else
    #define swap(a,b,t)   t tmp = a;a = b; b = tmp;    
#endif

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))