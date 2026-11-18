// Tetris.h - the rules: 10x20 board, 7 pieces (I O T S Z J L), SRS rotation, line clears.
#pragma once
#include <cstdlib>
#include <vector>
#include <utility>

class Tetris {
public:
    static const int COLS = 10, ROWS = 20;
    enum Piece { I, O, T, S, Z, J, L };

    Tetris() { reset(); }

    void reset() {
        for (auto& r : grid_) for (int& c : r) c = -1;
        lines_ = 0; over_ = false; lastClear_ = 0; full_.clear(); blocked_.clear(); eaten_.clear();
        next_ = std::rand() % 7;
        spawn();
    }

    // cells of a piece in a rotation (0..3 clockwise). I and O turn in a 4x4 box, the rest in 3x3.
    static std::vector<std::pair<int, int>> cells(int type, int rot) {
        static const int SHAPES[7][4][2] = {
            {{0,1},{1,1},{2,1},{3,1}}, {{1,0},{2,0},{1,1},{2,1}}, {{1,0},{0,1},{1,1},{2,1}},
            {{1,0},{2,0},{0,1},{1,1}}, {{0,0},{1,0},{1,1},{2,1}}, {{0,0},{0,1},{1,1},{2,1}},
            {{2,0},{0,1},{1,1},{2,1}} };
        std::vector<std::pair<int, int>> p;
        for (auto& c : SHAPES[type]) p.push_back({ c[0], c[1] });
        if (type == O) return p;
        int n = (type == I) ? 4 : 3;
        for (int k = 0; k < (rot & 3); ++k)
            for (auto& c : p) c = { n - 1 - c.second, c.first };
        return p;
    }

    void move(int dx) { if (!over_ && !clearing() && fits(cur_, rot_, x_ + dx, y_)) x_ += dx; }
    void rotate() {
        if (over_ || clearing()) return;
        for (int kick : { 0, -1, 1, -2, 2 })
            if (fits(cur_, rot_ + 1, x_ + kick, y_)) { rot_ = (rot_ + 1) & 3; x_ += kick; return; }
    }
    // one row down; returns lines cleared if the piece locked
    int step() {
        if (over_ || clearing()) return 0;
        if (fits(cur_, rot_, x_, y_ + 1)) { ++y_; return 0; }
        return lock();
    }
    int hardDrop() {
        if (over_ || clearing()) return 0;
        while (fits(cur_, rot_, x_, y_ + 1)) ++y_;
        return lock();
    }
    // full rows wait here (flashing) until the game calls collapse()
    bool clearing() const { return !full_.empty(); }
    const std::vector<int>& clearingRows() const { return full_; }
    int collapse() {
        int n = (int)full_.size();
        for (int y : full_) {                                    // rows are sorted top to bottom
            for (int yy = y; yy > 0; --yy) for (int x = 0; x < COLS; ++x) grid_[yy][x] = grid_[yy - 1][x];
            for (int x = 0; x < COLS; ++x) grid_[0][x] = -1;
        }
        full_.clear();
        lines_ += n;
        spawn();
        return n;
    }

    // read-only access for the scenes
    int at(int x, int y) const { return grid_[y][x]; }
    int current() const { return cur_; }
    int rotation() const { return rot_; }
    int px() const { return x_; }
    int py() const { return y_; }
    int next() const { return next_; }
    int lines() const { return lines_; }
    bool over() const { return over_; }
    int ghostY() const { int y = y_; while (fits(cur_, rot_, x_, y + 1)) ++y; return y; }
    void setLines(int n) { lines_ = n; }   // for testing

    // ---- round 4 extras: cells blocked by a werewolf stain; a piece that lands on one is eaten (not placed);
    //      a tentacle can snatch the falling piece away
    void setBlocked(const std::vector<std::pair<int, int>>& cells) { blocked_ = cells; }
    bool blocked(int x, int y) const { for (auto& c : blocked_) if (c.first == x && c.second == y) return true; return false; }
    std::vector<std::pair<int, int>> takeEaten() { auto e = eaten_; eaten_.clear(); return e; }   // board cells of the last eaten piece
    void dropCurrent() { if (!over_ && !clearing()) spawn(); }             // the falling piece is taken away

private:
    int grid_[ROWS][COLS];
    int cur_ = 0, rot_ = 0, x_ = 3, y_ = 0, next_ = 0, lines_ = 0, lastClear_ = 0;
    std::vector<int> full_;
    std::vector<std::pair<int, int>> blocked_, eaten_;
    bool over_ = false;

    bool fits(int type, int rot, int x, int y) const {
        for (auto& c : cells(type, rot)) {
            int gx = x + c.first, gy = y + c.second;
            if (gx < 0 || gx >= COLS || gy >= ROWS) return false;
            if (gy >= 0 && grid_[gy][gx] >= 0) return false;
            if (blocked(gx, gy)) return false;
        }
        return true;
    }
    void spawn() {
        cur_ = next_; next_ = std::rand() % 7; rot_ = 0; x_ = 3; y_ = -1;
        if (!fits(cur_, rot_, x_, y_)) over_ = true;
    }
    // returns how many rows are full (they flash first; collapse() removes them)
    int lock() {
        for (auto& c : cells(cur_, rot_))                          // resting on a stain: it gets eaten instead
            if (blocked(x_ + c.first, y_ + c.second + 1)) {
                eaten_.clear();
                for (auto& q : cells(cur_, rot_)) if (y_ + q.second >= 0) eaten_.push_back({ x_ + q.first, y_ + q.second });
                spawn();
                return 0;
            }
        for (auto& c : cells(cur_, rot_))
            if (y_ + c.second >= 0) grid_[y_ + c.second][x_ + c.first] = cur_;
        full_.clear();
        for (int y = 0; y < ROWS; ++y) {
            bool full = true;
            for (int x = 0; x < COLS; ++x) if (grid_[y][x] < 0) full = false;
            if (full) full_.push_back(y);
        }
        if (full_.empty()) spawn();
        return (int)full_.size();
    }
};
