#include "Point.h"

Point::Point() : row(0), col(0) {}

Point::Point(int row, int col) : row(row), col(col) {}

int Point::getRow() const {
    return row;
}

int Point::getCol() const {
    return col;
}

Point Point::translated(int deltaRow, int deltaCol) const {
    return Point(row + deltaRow, col + deltaCol);
}

bool Point::operator==(const Point& other) const {
    return row == other.row && col == other.col;
}

bool Point::operator!=(const Point& other) const {
    return !(*this == other);
}
