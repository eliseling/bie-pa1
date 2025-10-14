#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define EPS 1e-9
#define PI 3.14159265358979323846

int main(void)
{
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

    // Case 1: Identical circles
    if (fabs(dx) < EPS && fabs(dy) < EPS && fabs(r1 - r2) < EPS) {
        overlap = PI * r1 * r1;
        printf("The circles are identical, overlap: %.6lf\n", overlap);
        return 0;
    }

    // Case 2: Separate circles (no overlap)
    if (d > r1 + r2 + EPS) {
        printf("The circles lie outside each other, no overlap.\n");
        return 0;
    }

    // Case 3: External touch
    if (fabs(d - (r1 + r2)) <= EPS) {
        printf("External touch, no overlap.\n");
        return 0;
    }

    // Case 4: One inside another, no intersection
    if (d + fmin(r1, r2) < fmax(r1, r2) - EPS) {
        double rBig = fmax(r1, r2);
        double rSmall = fmin(r1, r2);
        overlap = PI * rSmall * rSmall;
        if (r1 > r2)
            printf("Circle #2 lies inside circle #1, overlap: %.6lf\n", overlap);
        else
            printf("Circle #1 lies inside circle #2, overlap: %.6lf\n", overlap);
        return 0;
    }

    // Case 5: Internal touch (one inside other, touching)
    if (fabs(d + fmin(r1, r2) - fmax(r1, r2)) <= EPS) {
        double rSmall = fmin(r1, r2);
        overlap = PI * rSmall * rSmall;
        if (r1 > r2)
            printf("Internal touch, circle #2 lies inside circle #1, overlap: %.6lf\n", overlap);
        else
            printf("Internal touch, circle #1 lies inside circle #2, overlap: %.6lf\n", overlap);
        return 0;
    }

    // Case 6: Intersecting circles
    {
        double alpha = 2 * acos((r1 * r1 + d * d - r2 * r2) / (2 * r1 * d));
        double beta = 2 * acos((r2 * r2 + d * d - r1 * r1) / (2 * r2 * d));
        double area1 = 0.5 * r1 * r1 * (alpha - sin(alpha));
        double area2 = 0.5 * r2 * r2 * (beta - sin(beta));
        overlap = area1 + area2;
        printf("The circles intersect, overlap: %.6lf\n", overlap);
    }

    return 0;
}
