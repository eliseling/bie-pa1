#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

typedef long long ll;
typedef __int128 i128;

static ll absll(ll x){ return x < 0 ? -x : x; }

void trim_trailing_newline(char *s){
    size_t L = strlen(s);
    if(L>0 && s[L-1]=='\n') s[L-1]=0;
}

/* Extended gcd: returns g = gcd(a,b). Finds x,y such that a*x + b*y = g */
ll extended_gcd(ll a, ll b, ll *x, ll *y){
    if(b==0){
        *x = (a>=0) ? 1 : -1;
        *y = 0;
        return a>=0 ? a : -a;
    }
    ll x1=0,y1=0;
    ll g = extended_gcd(b, a % b, &x1, &y1);
    *x = y1;
    *y = x1 - (a / b) * y1;
    return g;
}

/* floor division for possibly negative numerators. returns floor(num/den) */
ll floordiv(i128 num, ll den){
    i128 d = den;
    if(den < 0){ num = -num; d = -d; } // make den positive
    i128 q = num / d;
    i128 r = num % d;
    if(r != 0 && ((r < 0) != (d < 0))) q -= 1;
    return (ll)q;
}

/* ceil division for possibly negative numerators. returns ceil(num/den) */
ll ceildiv(i128 num, ll den){
    i128 d = den;
    if(den < 0){ num = -num; d = -d; }
    i128 q = num / d;
    i128 r = num % d;
    if(r != 0 && ((r > 0) == (d > 0))) q += 1;
    return (ll)q;
}

int main(void){
    char line[1024];

    /* Read track lengths */
    printf("Track length:\n");
    if(!fgets(line, sizeof(line), stdin)){
        printf("Invalid input.\n");
        return 0;
    }
    trim_trailing_newline(line);
    // parse two integers
    char *p = line;
    // skip leading spaces
    while(*p==' '||*p=='\t') p++;
    char *endptr;
    errno = 0;
    ll a = strtoll(p, &endptr, 10);
    if(p==endptr || errno!=0){
        printf("Invalid input.\n");
        return 0;
    }
    p = endptr;
    // skip spaces between numbers
    while(*p==' '||*p=='\t') p++;
    if(*p==0){
        printf("Invalid input.\n");
        return 0;
    }
    errno = 0;
    ll b = strtoll(p, &endptr, 10);
    if(p==endptr || errno!=0){
        printf("Invalid input.\n");
        return 0;
    }
    // ensure no extra non-space garbage after second number
    p = endptr;
    while(*p==' '||*p=='\t') p++;
    if(*p != '\0'){
        printf("Invalid input.\n");
        return 0;
    }

    if(a <= 0 || b <= 0 || a == b){
        printf("Invalid input.\n");
        return 0;
    }

    /* Read distance line */
    printf("Distance:\n");
    if(!fgets(line, sizeof(line), stdin)){
        printf("Invalid input.\n");
        return 0;
    }
    trim_trailing_newline(line);
    // parse sign char and number
    p = line;
    while(*p==' '||*p=='\t') p++;
    if(*p != '+' && *p != '-'){
        printf("Invalid input.\n");
        return 0;
    }
    char mode = *p;
    p++;
    // after sign, at least one space (they suggested using "%c" or " %c" but examples show a space)
    // skip spaces
    while(*p==' '||*p=='\t') p++;
    if(*p == '\0'){
        printf("Invalid input.\n");
        return 0;
    }
    errno = 0;
    ll n = strtoll(p, &endptr, 10);
    if(p==endptr || errno!=0){
        printf("Invalid input.\n");
        return 0;
    }
    p = endptr;
    while(*p==' '||*p=='\t') p++;
    if(*p != '\0'){
        printf("Invalid input.\n");
        return 0;
    }
    if(n < 0){
        printf("Invalid input.\n");
        return 0;
    }

    /* Solve a*x + b*y = n with x,y >= 0 integers */
    // Choose enumeration order A: x from 0..n/a
    if(mode == '+'){
        ll count = 0;
        ll maxx = (a == 0) ? 0 : n / a;
        for(ll x = 0; x <= maxx; ++x){
            ll rem = n - x * a;
            if(rem < 0) break;
            if(rem % b == 0){
                ll y = rem / b;
                printf("= %lld * %lld + %lld * %lld\n", a, x, b, y);
                count++;
            }
        }
        if(count == 0){
            printf("No solution.\n");
        } else {
            printf("Total variants: %lld\n", count);
        }
        return 0;
    } else {
        // fast counting for '-' mode
        // Solve a*x + b*y = n, x,y >=0
        ll xcoef = 0, ycoef = 0;
        ll g = extended_gcd(a, b, &xcoef, &ycoef);
        if(n % g != 0){
            printf("No solution.\n");
            return 0;
        }
        // scale to get one particular solution (may be negative)
        // use 128-bit for safety in multiplication
        i128 n_over_g = (i128)(n / g);
        i128 x0_128 = (i128)xcoef * n_over_g;
        i128 y0_128 = (i128)ycoef * n_over_g;
        // reduced coefficients
        ll a1 = a / g;
        ll b1 = b / g;
        // general solution: x = x0 + k*(b1), y = y0 - k*(a1)
        // find k such that x >= 0 and y >= 0
        // k >= ceil( -x0 / b1 )
        // k <= floor( y0 / a1 )
        ll kmin = ceildiv(-x0_128, b1);
        ll kmax = floordiv(y0_128, a1);
        ll total = 0;
        if(kmax >= kmin){
            // compute count = kmax - kmin + 1
            // safe because difference fits in long long
            total = kmax - kmin + 1;
            if(total < 0) total = 0; // defensive
        } else total = 0;
        if(total == 0){
            printf("No solution.\n");
        } else {
            printf("Total variants: %lld\n", total);
        }
        return 0;
    }
}
