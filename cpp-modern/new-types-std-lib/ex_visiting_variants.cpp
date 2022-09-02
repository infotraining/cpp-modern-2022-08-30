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

template <typename... TClosures>
struct overloaded : TClosures...
{
	using TClosures::operator()...;
};

TEST_CASE("visit a shape variant and calculate area")
{
    using Shape = std::variant<Circle, Rectangle, Square>;

    std::vector<Shape> shapes = {Circle {1}, Square {10}, Rectangle {10, 1}};

    double total_area {};

    const auto visitor = overloaded(
        [](const Circle& circ) -> double { return 3.14 * circ.radius * circ.radius; },
        [](const Rectangle& rect) -> double { return rect.width * rect.height; },
        [](const Square& square) -> double { return square.size * square.size; }
    );

    for (const auto& shape : shapes) {
        total_area += std::visit(visitor, shape);
    }
    
    REQUIRE(total_area == Approx(113.14));

    for (const auto& shape : shapes) 
    {
        std::visit([](const auto& shp) { shp.draw(); }, shape);
    }
}

///////////////////////////////////////////////////////////
// variant as value||error handler

[[nodiscard]] std::variant<std::string, std::errc> load_from_file(const std::string& filename)
{
    if (filename == "secret")
        return std::errc::permission_denied;

    return std::string{"content of "} + filename;
}

template <typename TVariant, typename... TLambdas>
auto process_result(TVariant&& v, TLambdas&&... ls)
{
	return std::visit(overloaded { std::forward<TLambdas>(ls)... }, std::forward<TVariant>(v));
}

TEST_CASE("handling errors using variant")
{
    std::variant result = load_from_file("secret");

    std::visit(overloaded{
            [](const std::string& s) { std::cout << "Success: " << s << "\n"; },
		    [](std::errc ec) { std::cout << "ERROR #" << static_cast<int>(ec) << "\n"; } 
        }, load_from_file("secret"));

    process_result(load_from_file("secret"), 
		[](const std::string& s) { std::cout << "Success: " << s << "\n"; },
		[](std::errc ec) { std::cout << "ERROR #" << static_cast<int>(ec) << "\n"; }
    );
}