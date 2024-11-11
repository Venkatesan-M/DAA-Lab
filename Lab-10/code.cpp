#include <iostream>
#include <vector>
#include <algorithm>
#include <stack>
#include <chrono>
#include <cmath>
#include <fstream>

using namespace std;

struct Point {
    int x, y;
};

// Function to find the orientation of the triplet (p, q, r)
// Returns 0 if collinear, 1 if clockwise, 2 if counterclockwise
int orientation(Point p, Point q, Point r) {
    int val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
    if (val == 0) return 0; // Collinear
    return (val > 0) ? 1 : 2; // Clockwise or counterclockwise
}

// Compare function for sorting by polar angle with respect to pivot
Point pivot;
bool compare(Point p1, Point p2) {
    int o = orientation(pivot, p1, p2);
    if (o == 0)
        return (sqrt(pow(p1.x - pivot.x, 2) + pow(p1.y - pivot.y, 2)) <
                sqrt(pow(p2.x - pivot.x, 2) + pow(p2.y - pivot.y, 2)));
    return (o == 2);
}

// Function to find the convex hull using Graham Scan
vector<Point> grahamScan(vector<Point>& points) {
    int n = points.size();
    // Find the lowest point
    pivot = points[0];
    for (int i = 1; i < n; i++) {
        if (points[i].y < pivot.y || (points[i].y == pivot.y && points[i].x < pivot.x))
            pivot = points[i];
    }

    // Sort points based on polar angle with respect to the pivot
    sort(points.begin(), points.end(), compare);

    // Initialize the stack for the convex hull
    stack<Point> hull;
    hull.push(points[0]);
    hull.push(points[1]);
    hull.push(points[2]);

    // Process the remaining points
    for (int i = 3; i < n; i++) {
        while (hull.size() > 1) {
            Point top = hull.top();
            hull.pop();
            Point nextTop = hull.top();
            if (orientation(nextTop, top, points[i]) != 1) {
                hull.push(top);
                break;
            }
        }
        hull.push(points[i]);
    }

    // Store the convex hull points in a vector
    vector<Point> convexHull;
    while (!hull.empty()) {
        convexHull.push_back(hull.top());
        hull.pop();
    }
    return convexHull;
}

// Function to generate random points
vector<Point> generateRandomPoints(int numPoints) {
    vector<Point> points;
    for (int i = 0; i < numPoints; i++) {
        points.push_back({rand() % 1000, rand() % 1000});
    }
    return points;
}

int main() {
    srand(time(0));
    ofstream out("graham_scan_times.txt");

    // Varying input sizes
    for (int numPoints = 10; numPoints <= 1000; numPoints += 50) {
        vector<Point> points = generateRandomPoints(numPoints);

        // Measure the time taken for Graham Scan
        auto start = chrono::high_resolution_clock::now();
        vector<Point> convexHull = grahamScan(points);
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = end - start;

        cout << "Number of points: " << numPoints << ", Time taken: " << elapsed.count() << " seconds\n";
        out << numPoints << " " << elapsed.count() << "\n";
    }

    out.close();
    return 0;
}
