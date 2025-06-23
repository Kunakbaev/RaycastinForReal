# RaycastinForReal
 
## 1. Annotation

Goal of this project was to try new algorithm for raycasting and actually implement it, to see if there will be any difference in quality of a resulting image and speed of computation of each frame (i.e how high will be FPS - frames per seconds) compared to the initial approach, where each ray is casted separately

## 2. Theory

What is raycasting actually? There is a Wikipedia page [<sup>[1]</sup>](#reference-1)on the topic. Also there's a [**good article**](https://proglib.io/p/raycasting-for-the-smallest) [<sup>[2]</sup>](#reference-2)in Russian. You can read those, if you want to, but I will try to explain raycasting quickly in simple words to give you an idea.

### 2.1 Small history behind it
Idea behind raycasting is to create a "pseudo" 3D image (or so called 2.5D [<sup>[3]</sup>](#reference-3)). This technique allows us not to implement actual 3D, instead we are creating image from 2d scene, by some tricks. This approach was widely used in early first person shooters(FPS) [<sup>[4]</sup>](#reference-4)as it doesn't require much computational power and is easy to implement. For example, Wolfenstein3D [<sup>[5]</sup>](#reference-5)and DOOM (1993) [<sup>[6]</sup>](#reference-6)relied on similar algorithm.

### 2.2 How it works?

There are 2 main things: scene (image on the right) and screen (image on the left). And some other terms, that you also need to know.

* scene is like a 2d map of a game level, it can be simple boolean matrix, where 1 means, that there's an obstacle and 0 stands for empty space or in more complex case, level map is a canvas with geometric shapes, that represent walls
* screen is way simpler than in modern 3d games, where each pixel is calculated separately, instead screen is divided into columns of same width (usually width is equal to one pixel), that are constructed in some way. Basically, we just count distance to the object, apply some formula to it to get the brightness and draw rectangle with according height (also proportional to the distance)
* player's field of view (FOV) [<sup>[7]</sup>](#reference-7), it's just some angle, segment sector. Humans usually have FOV of around 60 degrees, however we can increase it's value in raycasting just for fun and to see some interesting effects

![raycasting scene example](assetsForREADME/raycastingExampleFromWikipedia.gif)
*image from wikipedia [<sup>[8]</sup>](#reference-8)*

## 3. Graphics library

For this project I've used SFML library [<sup>[9]</sup>](#reference-9)for graphics. It's a good lib if you want to wark with graphics on C/C++ and other languages, easy to learn, has a lot of different classes, so for me it was very easy to work with all this polygons, vertices and colors.

## 4. Actual process of raycasting

I've already described how main concept looks like, but let's dive into details to get a better understanding of how raycasting works.

### 4.1 Geometry

We need some helper functions, to make our raycasting work. For that purpose I've implemented small geometry lib (actually just one file) in **geometryLib.cpp**. Operations that we will use:

* add/sub vectors, multiply vector by constant
* dot/cross product of 2 vectors
* get length of vector, normalize vector
* get angle between vector and some origin vector
* rotate vector by angle
* check if 2 segments are intersecting
* check if ray intersects segment
* check if point is inside triangle/convex polygon
* distance from point to segment
* distance from point to segment if we move along some particular direction (vector)

Checking if point is inside polygon is a common problem, so there is a wikipedia page about it [<sup>[10]</sup>](#reference-10)General case requires some nasty algorithm that uses ray cast from checked point and parity of it's intersections with polygon. But we will focus on convex case, as it's more easy. In that case we can simply divide our shape in triangles by diagonals.

And to check if point is inside triangle you can just find area of 3 subtriangles that are formed by our point and check if their sum is equal area of original triangle, if that's so, than point is inside. There are other more precise ways to solve that task, for example, by 2 checks if point is inside an angle, which can be done by simple dot product.

Also, I'm using **EPS** for comparisons and function **sign**, which is really important as floating point operations tend to accumulate error and as a result we can not compare 2 double variables with just **==**.

I use **long double** everywhere, but that's not that important and **double** should be sufficient.

### 4.2 Setting up scene

First we have to set up scene, where our player will be moving. In my case it's a canvas with convex polygons (it's harder to check if point is inside in non convex case), for now I've implemented rectangles and circles (just a regular polygon with many sides).

#### 4.2.1 Obstacle

<details>
<summary>Obstacle structure</summary>

```C++
struct Obstacle {
    size_t numberOfSides;
    Point* sides;
};
```
</details>

There are some methods implemented in **obstacle.cpp** file:

* construct different kinds of obstacles (rectangles and circles)
* get segment by an index for a particular obstacle
* check if 2 obstacles interest
* display an obstacle
* destruct an obstacle

For now I forbid any intersections between different obstacles on the scene, because I assume that there maybe some troubles with my algorithm in that case.

To check if there's an intersection between two convex polygons, I iterate through points both from first and second obstacles and check if it's inside other one.

#### 4.2.2 Player

<details>
<summary>Player struct</summary>

```C++
struct Player {
    Point       position;
    long double stepSize = 1;                  // how fast player moves across the scene
    long double angleRotationStep = PIE / 120; // how fast player rotates
    long double currentDirection;              // angle in radians
    long double FOV;                           // FOV = Field Of View, how much player sees (angle in radians)
    int         bodyRadius;
};
```
</details>
<br>

Movement is done by keyboard, with **WASD** keys or arrows, where **W** and **S** are used for moving forward and backwards accordingly and **A** and **D** for camera (player) rotation.

### 4.3 How screen rendering is done?

Now to the main part, we have our player that stands at some point on the level and looks in some direction, we need to display what he sees. For now, I will describe standard algorithm and will return to my implementation a bit later.

#### 4.3.1 How to cast rays
We begin with casting rays, first one's direction is: player's direction - FOV / 2, then next ray is computed by rotating previous one by some small degree step. We continue doing so, until we won't go out of player's FOV.

#### 4.3.2 Distance to collision
For each ray we count the distance to the first collision. For that, I iterate through all the obstacles and their sides (just segments) and find the one with the minimum distance to player's position. For that I use function **distanceToSegmByDirection** from **geometryLib.cpp** that counts distance from point to segment if we look in some fixed direction. Also, I have bounding box, so that ray doesn't go to infinity, but that's not very important.

#### 4.3.3 Column display for one ray

Once we get array of distances for each ray we can start displaying our scene on the screen. For each ray we draw column with height proportional to the distance, just how perspective [<sup>[11]</sup>](#reference-11)works: the further the object is, the smaller it becomes. To decide how bright our piece of wall will be, we use inverse square law [<sup>[12]</sup>](#reference-12).

#### 4.3.4 Fisheye effect fix

![fisheye effect explanation](assetsForREADME/rayCastExpForFishEye.png)

*img from proglib article about raycasting [<sup>[13]</sup>](#reference-13)*

Also, there's a common problem, when you look straight to the flat wall, image looks distorted, but it should be straight. This effect is called fisheye. The problem is that all rays that are inside player's FOV have different length. To solve the problem we just have to multiply distance for each ray by $cos(rayAngle - playerAngle)$.

## 5. Alternative approach to raycasting

Main idea, is that we don't need to cast all the rays that are inside player's FOV, as most of them are useless. Instead it's enough to cast rays only to vertices of polygons for all obstacles. However, we highly depend on "simplicity" of the walls, i.e. if we have distance to the beginning of a wall segment and to it's end, we can easily restore whole image by this 2 values, because we know that distance is changing linearly from minimal to the maximum of this 2 values. If wall is some complex shape, this approach can not be easily used, even with simple circles we have to use something more complex than linear formula.

I think that "simplicity" of walls can be described with term from calculus: continuous function [<sup>[14]</sup>](#reference-14), where function takes a direction (angle) as an argument and returns distance to the first collision. This means that small change in direction of a ray doesn't change distance that much.

### 5.1 More detailed description

For each polygon vertex we cast 2 rays, that is to wiggle initial ray a bit, this way we make sure that one of those rays definitely will hit the wall and other won't. Also due to precision errors it's dangerous to cast ray directly at vertex.

We consider initial ray to vertex to be "valid" if this point (also point of collision) is visible by player, i.e. no other segment is blocking this vertex. To check that we just need distance from player to this vertex to be smaller than minimum of all the distances from player to some segment, if look in direction to our fixed vertex.

For each casted ray, we save pair consisting of direction (just angle in radians) and distance to collision to the array. Also, we add most left and right rays that are in player's FOV. After that, array is sorted by direction. Now, $\forall i$ pairs with indexes $i$ and $i + 1$ form a "continuous" part of the wall.

To draw a scene on the screen, we look at each such pair and draw a trapezoid with parallel sides that are vertical and symmetrical along the middle line of the screen in height. Their height is calculated by the same formula, as in previous algorithm.

Also, there's an article on habr [<sup>[15]</sup>](#reference-15)that basically is like this README, but a bit different. This algorithm with casting rays only to vertices is also described there.

## 6. Comparison of 2 algorithms

### 6.1 Time complexity

Let's compare time complexity [<sup>[16]</sup>](#reference-16)of each algorithm. Also, let's name width of the screen in pixels a $W$ and the number of vertices (sum of count of vertices for all polygons) a $N$.

In the first one we cast ray for each column of pixels in the screen, so that requires $O(W)$. And for each ray we iterate through all the obstacles, that's $O(N)$. In total we need $O(W \cdot N)$.

In the second one, we iterate through all vertices and perform a check for each one, which takes $O(N)$ as we need to find the smallest distance and to do so we once again iterate through. In total that requires $O(N^2)$, also we sort array with $2N$ elements. So in total we have: $O(N^2 + 2\,N\,logN) = O(N^2)$.

However, I'm quite sure, that this can be reduced to just $O(N\,logN)$ if we use some smarter algorithm. I was thinking in direction of **scanline** algorithm, there is an article in Russian on that matter [<sup>[17]</sup>](#reference-17)If we just sort all points from polygons by angle (and distance to the player) and process events as: point spawns and point goes away, while having some structure that can efficiently tell us whether added point is blocked by some other segment, than we can solve our problem. But I thought that was too complex for my purposes and didn't implement this approach.

### 6.2 Realization details (constant factors)

While second algorithm seems to be working better as usually number of vertices is quite small ($N$) and screen width ($W$) is quite big (in my case it's 1920 pixels), on practice first one is better for several reasons:

* at some point we just want to make our scene bigger, which means more walls (polygons) and more vertices, so $N$ grows, however, even though $W$ is quite big, at some point it will become just a constant, so first algorithm will be linear, working in $O(N)$ with huge constant factor $W$. 

* also we can use parallel computing technique [<sup>[18]</sup>](#reference-18)as each ray is independent from others. This can reduce constant factor by a lot.

### 6.3 Capabilities

There's another flaw in second algorithm. It cannot process more "complex" walls, I already mentioned that problem when was first describing idea of this algorithm.

For example if we want to have mirrors on the walls, we have to cast all the rays, because function of distance is not continuous any more, as even slight change of direction may result in completely different ray path with another set of bounces.

### 6.4 Sum up

To my mind, simple raycasting algorithm, where each ray is casted separately is better, as it's both simple, can be further optimized and can handle more complex cases (textures on the walls, mirrors and so on). However, on such small scenes, second algorithm works way faster and is not that harder to implement.


 
## References


<a id="reference-1"></a>
1. [**^**](https://en.wikipedia.org/wiki/Ray_casting) Raycasting, wiki page.
<a id="reference-2"></a>
2. [**^**](https://proglib.io/p/raycasting-for-the-smallest) Article about raycasting in Russian.
<a id="reference-3"></a>
3. [**^**](https://en.wikipedia.org/wiki/2.5D) Wiki page about 2.5D.
<a id="reference-4"></a>
4. [**^**](https://en.wikipedia.org/wiki/First-person_shooter) Wiki page about FPS (first person shooter).
<a id="reference-5"></a>
5. [**^**](https://en.wikipedia.org/wiki/Wolfenstein_3D) Wolfenstein_3D.
<a id="reference-6"></a>
6. [**^**](https://en.wikipedia.org/wiki/Doom_(1993_video_game)) Doom_1993.
<a id="reference-7"></a>
7. [**^**](https://en.wikipedia.org/wiki/Field_of_view) FOV (field of view).
<a id="reference-8"></a>
8. [**^**](https://upload.wikimedia.org/wikipedia/commons/e/e7/Simple_raycasting_with_fisheye_correction.gif) Raycasting with fisheye effect correction GIF example, from wikipedia page.
<a id="reference-9"></a>
9. [**^**](https://www.sfml-dev.org/) SFML official page.
<a id="reference-10"></a>
10. [**^**](https://en.wikipedia.org/wiki/Point_in_polygon.) Point in polygon problem.
<a id="reference-11"></a>
11. [**^**](https://en.wikipedia.org/wiki/Perspective_(graphical)) What's perspective, wiki page.
<a id="reference-12"></a>
12. [**^**](https://en.wikipedia.org/wiki/Inverse-square_law) Inverse square law (for brightness of surface, depending on distance to it).
<a id="reference-13"></a>
13. [**^**](https://media.proglib.io/wp-uploads/2019/02/step8-2.png) Image to help understand fisheye effect, from proglib article.
<a id="reference-14"></a>
14. [**^**](https://en.wikipedia.org/wiki/Continuous_function) Continuous function.
<a id="reference-15"></a>
15. [**^**](https://habr.com/ru/articles/578110/) Habr article about raycasting.
<a id="reference-16"></a>
16. [**^**](https://en.wikipedia.org/wiki/Time_complexity) Time complexity.
<a id="reference-17"></a>
17. [**^**](https://ru.algorithmica.org/cs/decomposition/scanline/.) Scanline algorithm.
<a id="reference-18"></a>
18. [**^**](https://en.wikipedia.org/wiki/Parallel_computing) Parallel computing.