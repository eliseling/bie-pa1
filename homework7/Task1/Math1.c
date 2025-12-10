#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Node{
    char *s;
    struct Node *next;
} Node;

typedef struct HNode{
    long long key;
    Node *list;
    struct HNode *next;
} HNode;

HNode *htable[262144];

char digits[20];
int n;

unsigned hash64(long long x){
    x ^= x >> 33;
    x *= 0xff51afd7ed558ccdULL;
    x ^= x >> 33;
    x *= 0xc4ceb9fe1a85ec53ULL;
    x ^= x >> 33;
    return (unsigned)x & (262144-1);
}

HNode *find_bucket(long long key, unsigned h){
    HNode *p = htable[h];
    while(p){
        if(p->key==key) return p;
        p=p->next;
    }
    return NULL;
}

void insert_expr(long long key, const char *s){
    unsigned h = hash64(key);
    HNode *b = find_bucket(key,h);
    if(!b){
        b = (HNode*)malloc(sizeof(HNode));
        b->key = key;
        b->list = NULL;
        b->next = htable[h];
        htable[h] = b;
    }
    Node *nd = (Node*)malloc(sizeof(Node));
    nd->s = strdup(s);
    nd->next = NULL;
    if(!b->list) b->list = nd;
    else{
        Node *q = b->list;
        while(q->next) q=q->next;
        q->next = nd;
    }
}
void dfs(int pos, long long cur, long long last, char *expr, int epos){
    if(pos==n){
        expr[epos] = '\0';
        insert_expr(cur, expr);
        return;
    }
    long long val=0;
    int start=pos;
    while(pos<n){
        val = val*10 + (digits[pos]-'0');
        int len = pos-start+1;
        /* copy digits[start..pos] into expr at epos or after operator */
        if(epos==0){
            /* first number, just copy */
            memcpy(expr, digits+start, len);
            dfs(pos+1, val, val, expr, len);
        } else {
            /* try '+' */
            expr[epos] = '+';
            memcpy(expr+epos+1, digits+start, len);
            dfs(pos+1, cur+val, val, expr, epos+1+len);
            /* try '-' */
            expr[epos] = '-';
            memcpy(expr+epos+1, digits+start, len);
            dfs(pos+1, cur-val, -val, expr, epos+1+len);
            /* try '*' */
            expr[epos] = '*';
            memcpy(expr+epos+1, digits+start, len);
            dfs(pos+1, cur-last+last*val, last*val, expr, epos+1+len);
        }
        pos++;
    }
}

int main(){
    char buf[200];
    /* English-only output labels (no LANG detection) */
    printf("Digits:\n");
    if(!fgets(buf,sizeof(buf),stdin)){printf("Invalid input.\n");return 0;}
    if(sscanf(buf,"%s",digits)!=1){printf("Invalid input.\n");return 0;}
    n=strlen(digits);
    if(n==0||n>12){printf("Invalid input.\n");return 0;}
    for(int i=0;i<n;i++) if(!isdigit(digits[i])){printf("Invalid input.\n");return 0;}

    for(int i=0;i<262144;i++) htable[i]=NULL;

    char expr[64];
    dfs(0,0,0,expr,0);

    printf("Problems:\n");
    while(fgets(buf,sizeof(buf),stdin)){
        if(buf[0]=='\n') continue;
        char t;
        char numbuf[50];
        if(sscanf(buf," %c %49s",&t,numbuf)!=2){
            printf("Invalid input.\n");
            return 0;
        }
        if(t!='?'&&t!='#'){
            printf("Invalid input.\n");
            return 0;
        }
        char *e;
        long long want = strtoll(numbuf,&e,10);
        if(*e!='\0'){printf("Invalid input.\n");return 0;}

        unsigned h = hash64(want);
        HNode *b = find_bucket(want,h);
        long long cnt=0;
        if(b){
            Node *p=b->list;
            while(p){ cnt++; p=p->next; }
        }
        if(t=='?'){
            if(b){
                Node *p=b->list;
                while(p){
                    printf("= %s\n", p->s);
                    p=p->next;
                }
            }
        }
        printf("Total: %lld\n",cnt);
    }
    return 0;
}