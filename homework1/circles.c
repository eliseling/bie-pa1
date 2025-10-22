#include <stdio.h>
#include <math.h>
#include <locale.h>

#define EPS 1e-9
#define PI 3.14159265358979323846
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

int main(void)
{
    setlocale(LC_ALL, "C"); // sikrer at punktum brukes som desimaltegn

    double x1, y1, r1, x2, y2, r2;

    printf("Enter circle #1 parameters:\n");
    if (scanf("%lf %lf %lf", &x1, &y1, &r1) != 3 || r1 <= 0) {
        printf("Invalid input.\n");
        return 0;
    }

    printf("Enter circle #2 parameters:\n");
    if (scanf("%lf %lf %lf", &x2, &y2, &r2) != 3 || r2 <= 0) {
        printf("Invalid input.\n");
        return 0;
    }

    double dx = x2 - x1;
    double dy = y2 - y1;
    double d = sqrt(dx * dx + dy * dy);
    double overlap = 0.0;

    /* Identical circles */
    if (fabs(dx) < EPS && fabs(dy) < EPS && fabs(r1 - r2) < EPS) {
        overlap = PI * r1 * r1;
        printf("The circles are identical, overlap: %.6f\n", overlap);
        return 0;
    }

    /* Disjoint (outside) */
    if (d > r1 + r2 + EPS) {
        printf("The circles lie outside each other, no overlap.\n");
        return 0;
    }

    /* External touch */
    if (fabs(d - (r1 + r2)) <= EPS) {
        printf("External touch, no overlap.\n");
        return 0;
    }

    /* One inside the other (no touch) */
    if (d + MIN(r1, r2) < MAX(r1, r2) - EPS) {
        double rSmall = MIN(r1, r2);
        overlap = PI * rSmall * rSmall;
        if (r1 > r2)
            printf("Circle #2 lies inside circle #1, overlap: %.6f\n", overlap);
        else
            printf("Circle #1 lies inside circle #2, overlap: %.6f\n", overlap);
        return 0;
    }

    /* Internal touch */
    if (fabs(d + MIN(r1, r2) - MAX(r1, r2)) <= EPS) {
        double rSmall = MIN(r1, r2);
        overlap = PI * rSmall * rSmall;
        if (r1 > r2)
            printf("Internal touch, circle #2 lies inside circle #1, overlap: %.6f\n", overlap);
        else
            printf("Internal touch, circle #1 lies inside circle #2, overlap: %.6f\n", overlap);
        return 0;
    }

    /* Intersecting circles */
    {
        double r1sq = r1 * r1, r2sq = r2 * r2;
        double alpha = 2.0 * acos((r1sq + d * d - r2sq) / (2.0 * r1 * d));
        double beta  = 2.0 * acos((r2sq + d * d - r1sq) / (2.0 * r2 * d));
        double area1 = 0.5 * r1sq * (alpha - sin(alpha));
        double area2 = 0.5 * r2sq * (beta - sin(beta));
        overlap = area1 + area2;
        printf("The circles intersect, overlap: %.6f\n", overlap);
    }

    return 0;
}
