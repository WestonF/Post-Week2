#define _USE_MATH_DEFINES
#include <cmath>
//this pair allows me to use M_PI rather than me defining an approximate pi of my own

#include <iostream>
#include <cassert>
#include <vector>


class point
{

public:
	double x;
	double y;
	point(double i_x, double i_y):x(i_x), y(i_y){}
	point(const point& source)
	{
		x = source.x;
		y = source.y;
	}
	
	point& operator =(const point& source)
	{
		if(&source == this)
		{
			return *this;
		}
		x = source.x;
		y = source.y;
		return *this;
	}
};

class line
{

public:
	point first, second;
	line(point i_first, point i_second): first(i_first), second(i_second){}
	
	double length()
	{
		return pow(pow(second.x - first.x , 2) + pow(second.y - first.y, 2),0.5);	//distance formula
	}
	
	line(const line& source):first(source.first), second(source.second){}
	
	line& operator =(const line& source)
	{
		if(&source == this)
		{
			return *this;
		}
		first = source.first;
		second = source.second;
		return *this;
	}
};

class multiline
{
	double length = 0;
	public:
		std::vector<line> lines;
		std::vector<point> points;
		void add_point(point a)
		{
			if(points.size() == 0)
			{
				points.push_back(a);
				return;
			}

			lines.push_back({points.back(), a});
			points.push_back(a);
			findlength();
		}
		void findlength()
		{
			length = 0;

			for(int i = 0; i < lines.size(); i++)
			{
				length = length + lines.at(i).length();
			}
		}
	multiline(const multiline& source)
	{
		lines = source.lines;
		points = source.points;
	}
	
	multiline& operator =(const multiline& source)
	{
		if(&source == this)
		{
			return *this;
		}
		lines = source.lines;
		points = source.points;
		return *this;
	}

};

class Shape2D
{
	//does it even have traits? yes, but no way to define them
public:
	double area = 0;
	double perimeter = 0;
	double getarea()
	{
		return area;
	}
	double getperimeter()
	{
		return perimeter;
	}
	Shape2D(){};
	Shape2D(const Shape2D& source)
	{
		area = source.area;
		perimeter = source.perimeter;
	}
	
	Shape2D& operator =(const Shape2D& source)
	{
		if(&source == this)
		{
			return *this;
		}
		area = source.area;
		perimeter = source.perimeter;
		return *this;
	}
};
class polygon: public Shape2D
{
	public:
	std::vector<point> points;
	std::vector<line> sides;	// good math would demand these form a closed shape, but that's not really the point of this excersize, so I'm not going to.
	polygon()//point a, point b, point c)	// this would be a really good place for me to practice variable number of arguments, and I will later if I have time
	{
//		points.push_back(a);
//		points.push_back(b);
//		points.push_back(c);
//		sides.push_back({a,b});
//		sides.push_back({b,c});
//		sides.push_back({c,a});
		findperimeter();
		findarea();
	}

	void findperimeter()
	{
		perimeter = 0;
		for(int i = 0; i < sides.size(); i++)
		{
			perimeter = perimeter + sides.at(i).length();
		}
	}
	
	virtual void findarea()
	{
		//not interested in doing the math to solve area for a generalized polygon, so skipping it but keeping the function for the purposes of demonstrating inheritance
		area = 0;
	}

	polygon(const polygon& source)//Shape2D(source.Shape2D)
	{
		points = source.points;
		sides = source.sides;
	}
	
	polygon& operator =(const polygon& source)
	{
		if(&source == this)
		{
			return *this;
		}
		points = source.points;
		sides = source.sides;
		return *this;
	}

};

class triangle : public polygon
{
	public:
	double base;
	double height;
		
	triangle (point a, point b, point c)	//:polygon(a, b, c)	
	{
		points.push_back(a);
		points.push_back(b);
		points.push_back(c);
		sides.push_back({a,b});
		sides.push_back({b,c});
		sides.push_back({c,a});
		findperimeter();
		findarea();
	}

	virtual void findarea()
	{
		//not interested in doing the math to solve area for a generalized triangle, so skipping it too
		area = 1;	//different from polygon, so it can be seen which is called
	}
};

class right_triangle : public triangle
{

public:

	right_triangle(point ref, double i_base, double i_height): triangle(ref,{ref.x + i_base, ref.y},{ref.x, ref.y + i_height})	//no diagonal triangles allowed, only nice, clean orthangonal right triangles
	{
		base = i_base;
		height = i_height;

//		points.push_back(ref);
//		points.push_back(ref.x + base, ref.y);
//		points.push_back(ref.x, ref.y + height);

//		sides.push_back({points.at(0), points.at(1)});
//		sides.push_back({points.at(1), points.at(2)});
//		sides.push_back({points.at(2), points.at(0)});
		findarea();
	}

	virtual void findarea()
	{
		area = base * height * 0.5;
	}
};

class quadrilateral : public polygon
{
	public:
	quadrilateral(point a, point b, point c, point d)
	{
		points.push_back(a);
		points.push_back(b);
		points.push_back(c);
		points.push_back(d);
		sides.push_back({a,b});
		sides.push_back({b,c});
		sides.push_back({c,d});
		sides.push_back({d,a});
		findperimeter();
		findarea();
	}

	void findarea()
	{
		area = 1.5;	//not doing the math for generalized area
	}
	
	quadrilateral(const quadrilateral& source)//Shape2D(source.Shape2D)
	{
		points = source.points;
		sides = source.sides;
	}
	
	quadrilateral& operator =(const quadrilateral& source)
	{
		if(&source == this)
		{
			return *this;
		}
		points = source.points;
		sides = source.sides;
		return *this;
	}
};

class trapezoid : public quadrilateral
{
	public:
		float bottom_width, top_width, height;

		trapezoid(point ref, double i_bottom_width, double i_top_width, double i_height): quadrilateral(
				{ref.x,									ref.y},
				{ref.x			+ (0.5 * (i_bottom_width - i_top_width)),	ref.y + i_height},
				{ref.x + i_bottom_width	- (0.5 * (i_bottom_width - i_top_width)),	ref.y + i_height},
				{ref.x + i_bottom_width,						ref.y})
		{
			bottom_width = i_bottom_width;
			top_width = i_top_width;
			height = i_height;
			findarea();
		}
		void findarea()
		{
			area = (height * (bottom_width = top_width) / 2.);	//added the '.' after 2 to avoid implicit casting to integer division. *Might* not have needed to do that, but it doesnt hurt anything
		}
};

class rectangle: public quadrilateral
{

public:
	double base;
	double height;

	rectangle(point ref, double i_base, double i_height): quadrilateral(ref, {ref.x + i_base, ref.y}, {ref.x + i_base, ref.y + i_height}, {ref.x, ref.y + i_height})
	{
		base = i_base;
		height = i_height;

//		points.push_back(ref);
//		points.push_back({ref.x + base, ref.y});
//		points.push_back({ref.x + base, ref.y + height});
//		points.push_back({ref.x, ref.y + height});

//		sides.push_back({points.at(0), points.at(1)});
//		sides.push_back({points.at(1), points.at(2)});
//		sides.push_back({points.at(2), points.at(3)});
//		sides.push_back({points.at(3), points.at(0)});
//		findperimeter();
		findarea();
	}

	virtual void findarea()
	{
		area = base * height;
	}

	rectangle(const rectangle &source) : quadrilateral(source)
	{
		base = source.base;
		height = source.height;
		findarea();
		findperimeter();
	}

	rectangle& operator =(const rectangle &source)
	{
		if(&source == this)
		{
			return *this;
		}
		base = source.base;
		height = source.height;
		findarea();
		findperimeter();
		return *this;
	}
};

class square: public rectangle
{

public:
	square(point ref, double length): rectangle(ref, length, length)	//no diagonal shapes allowed, only nice, clean orthangonals
	{
//		base = length;
//		height = length;

//		points.push_back(ref);
//		points.push_back(ref.x + base, ref.y);
//		points.push_back(ref.x + base, ref.y + height);
//		points.push_back(ref.x, ref.y + height);

//		sides.push_back({points.at(0), points.at(1)});
//		sides.push_back({points.at(1), points.at(2)});
//		sides.push_back({points.at(2), points.at(3)});
//		sides.push_back({points.at(3), points.at(0)});
	}

	square(const square &source) : rectangle(source)
	{
		base = source.base;
		height = base;
		findarea();
		findperimeter();
	}

	square& operator =(const square &source)
	{
		if(&source == this)
		{
			return *this;
		}
		base = source.base;
		height = base;
		findarea();
		findperimeter();
		return *this;
	}
};

class ellipse : public Shape2D
{

public:
	point center;
	double major_r;
	double minor_r;
	ellipse(point ref, double i_major_r, double i_minor_r):center(ref)
	{
		major_r = i_major_r;
		minor_r = i_minor_r;
		findarea();
		findperimeter();
	}
	void findarea()
	{
		area = major_r * minor_r * M_PI;
	}
	void findperimeter()
	{
		perimeter = 5;	//this is a filler value as the formula is nastily complex and not really important to the point of this excercise
	}
	double getmajordiameter()
	{
		return 2 * major_r;
	}
	double getminordiameter()
	{
		return 2 * minor_r;
	}
	
	ellipse(const ellipse &source) : center(source.center)
	{
		major_r = source.major_r;
		minor_r = source.minor_r;
		findarea();
		findperimeter();
	}

	ellipse& operator =(const ellipse &source)
	{
		if(&source == this)
		{
			return *this;
		}
		major_r = source.major_r;
		minor_r = source.minor_r;
		findarea();
		findperimeter();
		return *this;
	}
	

};

class circle : public ellipse
{
	
public:
	circle(point center, double radius): ellipse(center, radius, radius)
	{
		findperimeter();
	}

	//findarea() from ellipse works just fine here, no need to overwrite
	
	void findperimeter()
	{
		perimeter = 2 * M_PI * major_r;	//could also use minor_r, since they are identical
	}	
	double getdiameter()
	{
		return 2 * major_r;
	}
	circle(const circle &source) : ellipse(source)
	{
		major_r = source.major_r;
		minor_r = major_r;
		findarea();
		findperimeter();
	}

	circle& operator =(const circle &source)
	{
		if(&source == this)
		{
			return *this;
		}
		major_r = source.major_r;
		minor_r = major_r;
		findarea();
		findperimeter();
		return *this;
	}
};

int main()
{
	polygon poly;
	right_triangle tri({0, 0}, 3, 4);
	rectangle recta({2, 3}, 3, 4);
	square squa({2,3}, 5);

	//implict copy constructor suitable for needs
	square sqa2 = squa;
	
	//implicit copy operator
	square sqa3({0,0}, 2);
	sqa2 = sqa3;
		
	assert(tri.perimeter == 12);
	assert(tri.area == 6);
	assert(recta.getperimeter() == 14);
	assert(squa.perimeter == 20);
	assert(recta.area == 12);
	assert(sqa2.area == sqa3.area);
	return 0;
}
