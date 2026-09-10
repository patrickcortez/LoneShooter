// pathfinder.hpp - A* Pathfinding for LoneShooter
// Include after worldMap is declared
// Usage: 
//   Pathfinder::Init(worldMapPtr, collisionCallback);
//   auto path = Pathfinder::FindPath(startX, startY, targetX, targetY);

#ifndef PATHFINDER_HPP
#define PATHFINDER_HPP

#include <vector>
#include <cmath>
#include <algorithm>
#include <queue>
#include <cstring>

namespace Pathfinder {

const int PATH_MAP_WIDTH = 64;
const int PATH_MAP_HEIGHT = 64;
const int MAX_SEARCH_NODES = 500;

struct PathNode {
    int x, y;
    float g, h, f;
    int parentX, parentY;
    
    bool operator>(const PathNode& other) const {
        return f > other.f;
    }
};

static int (*worldMapPtr)[PATH_MAP_HEIGHT] = nullptr;

typedef bool (*ExternalCollisionFunc)(float x, float y);
static ExternalCollisionFunc externalCollisionCheck = nullptr;

// Stamps array to replace O(4096) loop
static int visitStamp[PATH_MAP_WIDTH][PATH_MAP_HEIGHT];
static int currentVisitStamp = 0;
// Re-usable tracking arrays
static float gScoreMap[PATH_MAP_WIDTH][PATH_MAP_HEIGHT];
static int parentXMap[PATH_MAP_WIDTH][PATH_MAP_HEIGHT];
static int parentYMap[PATH_MAP_WIDTH][PATH_MAP_HEIGHT];

inline void Init(int (*wm)[PATH_MAP_HEIGHT], ExternalCollisionFunc extCollision = nullptr) {
    worldMapPtr = wm;
    externalCollisionCheck = extCollision;
    memset(visitStamp, 0, sizeof(visitStamp));
    currentVisitStamp = 0;
}

inline bool IsBlocked(int x, int y) {
    if (x < 0 || x >= PATH_MAP_WIDTH || y < 0 || y >= PATH_MAP_HEIGHT) return true;
    if (worldMapPtr[x][y] != 0) return true;
    
    float cellCenterX = x + 0.5f;
    float cellCenterY = y + 0.5f;
    if (externalCollisionCheck && externalCollisionCheck(cellCenterX, cellCenterY)) return true;
    
    return false;
}

inline bool LineOfSight(int x0, int y0, int x1, int y1) {
    int dx = std::abs(x1 - x0);
    int dy = -std::abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;
    
    while (true) {
        if (IsBlocked(x0, y0)) return false;
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        
        // Avoid corner cutting
        if (e2 >= dy && e2 <= dx) {
            if (IsBlocked(x0 + sx, y0) || IsBlocked(x0, y0 + sy)) return false;
        }
        
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
    return true;
}

inline float Heuristic(int x1, int y1, int x2, int y2) {
    float dx = (float)(x2 - x1);
    float dy = (float)(y2 - y1);
    return std::sqrt(dx*dx + dy*dy);
}

inline std::vector<std::pair<int,int>> FindPath(float startX, float startY, float targetX, float targetY) {
    std::vector<std::pair<int,int>> result;
    if (!worldMapPtr) return result;
    
    int sx = (int)startX;
    int sy = (int)startY;
    int tx = (int)targetX;
    int ty = (int)targetY;
    
    if (sx < 0 || sx >= PATH_MAP_WIDTH || sy < 0 || sy >= PATH_MAP_HEIGHT) return result;
    if (tx < 0 || tx >= PATH_MAP_WIDTH || ty < 0 || ty >= PATH_MAP_HEIGHT) return result;
    
    if (IsBlocked(tx, ty)) {
        for (int ddx = -1; ddx <= 1; ddx++) {
            for (int ddy = -1; ddy <= 1; ddy++) {
                if (ddx == 0 && ddy == 0) continue;
                int nx = tx + ddx;
                int ny = ty + ddy;
                if (!IsBlocked(nx, ny)) {
                    tx = nx;
                    ty = ny;
                    goto found_valid_target;
                }
            }
        }
        return result;
    }
    found_valid_target:
    
    if (sx == tx && sy == ty) {
        result.push_back({tx, ty});
        return result;
    }
    
    // Increment stamp instead of clearing 4096 elements
    currentVisitStamp++;
    
    std::priority_queue<PathNode, std::vector<PathNode>, std::greater<PathNode>> openSet;
    
    PathNode start;
    start.x = sx;
    start.y = sy;
    start.g = 0;
    start.h = Heuristic(sx, sy, tx, ty);
    start.f = start.g + start.h;
    start.parentX = -1;
    start.parentY = -1;
    
    openSet.push(start);
    gScoreMap[sx][sy] = 0;
    
    PathNode bestNode = start;
    
    int nodesSearched = 0;
    
    const int dx8[] = {-1, 0, 1, -1, 1, -1, 0, 1};
    const int dy8[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    const float cost8[] = {1.414f, 1.0f, 1.414f, 1.0f, 1.0f, 1.414f, 1.0f, 1.414f};
    
    bool targetFound = false;
    
    while (!openSet.empty() && nodesSearched < MAX_SEARCH_NODES) {
        PathNode current = openSet.top();
        openSet.pop();
        
        if (visitStamp[current.x][current.y] == currentVisitStamp) continue;
        visitStamp[current.x][current.y] = currentVisitStamp;
        nodesSearched++;
        
        if (current.x == tx && current.y == ty) {
            bestNode = current;
            targetFound = true;
            break;
        }
        
        for (int i = 0; i < 8; i++) {
            int nx = current.x + dx8[i];
            int ny = current.y + dy8[i];
            
            if (IsBlocked(nx, ny)) continue;
            if (visitStamp[nx][ny] == currentVisitStamp) continue;
            
            // Fix diagonal movement clipping (block if EITHER adjacent is blocked)
            if (dx8[i] != 0 && dy8[i] != 0) {
                if (IsBlocked(current.x + dx8[i], current.y) || 
                    IsBlocked(current.x, current.y + dy8[i])) continue;
            }
            
            float tentativeG = gScoreMap[current.x][current.y] + cost8[i];
            
            // If not visited in this stamp, treat gScore as infinity
            bool firstVisit = (parentXMap[nx][ny] == -1 || visitStamp[nx][ny] != currentVisitStamp);
            
            if (firstVisit || tentativeG < gScoreMap[nx][ny]) {
                gScoreMap[nx][ny] = tentativeG;
                parentXMap[nx][ny] = current.x;
                parentYMap[nx][ny] = current.y;
                
                PathNode neighbor;
                neighbor.x = nx;
                neighbor.y = ny;
                neighbor.g = tentativeG;
                neighbor.h = Heuristic(nx, ny, tx, ty);
                neighbor.f = neighbor.g + neighbor.h;
                neighbor.parentX = current.x;
                neighbor.parentY = current.y;
                
                if (neighbor.h < bestNode.h) {
                    bestNode = neighbor;
                }
                
                openSet.push(neighbor);
            }
        }
    }
    
    // Traceback from bestNode
    int cx = bestNode.x;
    int cy = bestNode.y;
    while (cx != -1 && cy != -1 && !(cx == sx && cy == sy)) {
        result.push_back({cx, cy});
        int px = parentXMap[cx][cy];
        int py = parentYMap[cx][cy];
        cx = px;
        cy = py;
    }
    result.push_back({sx, sy});
    std::reverse(result.begin(), result.end());
    
    // Path smoothing (string-pulling)
    std::vector<std::pair<int,int>> smoothed;
    if (!result.empty()) {
        smoothed.push_back(result[0]);
        int current = 0;
        while (current < (int)result.size() - 1) {
            int next = current + 1;
            // Raycast forward to find furthest visible node
            for (int i = (int)result.size() - 1; i > current + 1; --i) {
                if (LineOfSight(result[current].first, result[current].second, result[i].first, result[i].second)) {
                    next = i;
                    break;
                }
            }
            smoothed.push_back(result[next]);
            current = next;
        }
    }
    
    return smoothed;
}

inline bool GetNextPathPoint(float currentX, float currentY, 
                             std::vector<std::pair<int,int>>& path, int& pathIndex,
                             float& outX, float& outY) {
    if (path.empty() || pathIndex >= (int)path.size()) return false;
    
    float targetX = path[pathIndex].first + 0.5f;
    float targetY = path[pathIndex].second + 0.5f;
    
    float dx = targetX - currentX;
    float dy = targetY - currentY;
    float dist = std::sqrt(dx*dx + dy*dy);
    
    if (dist < 0.5f) {
        pathIndex++;
        if (pathIndex >= (int)path.size()) return false;
        targetX = path[pathIndex].first + 0.5f;
        targetY = path[pathIndex].second + 0.5f;
    }
    
    outX = targetX;
    outY = targetY;
    return true;
}

}

#endif
