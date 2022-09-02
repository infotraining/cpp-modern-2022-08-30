#include "catch.hpp"

#include <iostream>
#include <string>
#include <variant>
#include <vector>

struct Circle
{
    int radius;

    void draw() const
    {
        std::cout << "Drawing a circle - r: " << radius << "\n";
    }
};

struct Rectangle
{
    int width, height;

    void draw() const
    {
        std::cout << "Drawing a rect - w: " << width << ", h: " << height << "\n";
    }
};

struct Square
{
    int size;

    void draw() const
    {
        std::cout << "Drawing a square - s: " << size << "\n";
    }
};

TEST_CASE("visit a shape variant and calculate area")
{
    using Shape = std::variant<Circle, Rectangle, Square>;

    std::vector<Shape> shapes = {Circle {1}, Square {10}, Rectangle {10, 1}};

    double total_area {};

    // TODO

    REQUIRE(total_area == Approx(113.14));
}