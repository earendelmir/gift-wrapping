/**
 * @file gift_wrapping.c
 * @author earendelmir
 * @date 19 February 2021
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>
// you can modify this value (keep it above 2)
#define NUM_POINTS 100
// you can modify these values
#define WIDTH 1000
#define HEIGHT 1000
#define POINTS_FILE ".points"
#define VERTICES_FILE ".vertices"


struct Point {
  float x;
  float y;
};


struct Array {
  struct Point coords[NUM_POINTS+1];
  unsigned int dim;
};



// randomly generates all the points needed for the simulation
void randomPoints(struct Array *array);
// returns wether the array already has a point with that coordinates
bool alreadyStored(const struct Array *array, const struct Point *point);
// returns wether the two points have the same coordinates
bool equals(const struct Point *a, const struct Point *b);
// finds the furthest left point
void furthestLeft(const struct Array *points, struct Array *vertices);
// checks if the given point is the furthest right one
bool isFurthestRight(const struct Point *point, const struct Array *array);
// finds the next vertix
bool nextVertix(const struct Point *lastVertix, struct Array *points, struct Array *vertices);
// calculates the angle between the two points
float angleBetween(const struct Point *a, const struct Point *b, bool reversed);
// removes the point in the index position from the points array
void removePoint(struct Array *array, unsigned int index);
FILE *openFile(const char *path, const char *mode);
void writePointsToFile(const char *filePath, const struct Array *array);



int main() {
  if (NUM_POINTS < 3) {
    printf("Can't operate with less than 3 points.\n");
    exit(EXIT_FAILURE);
  }

  srand(time(NULL));

  struct Array points = {.dim=0}, vertices = {.dim=0};

  // It generates N random coordinates set in the range of given WIDTH and HEIGHT.
  // N <= NUM_POINTS because it will not save duplicates.
  printf("Generating random points...\n");
  randomPoints(&points);

  printf("Finding vertices...\n");
  // finds the point at the furthest left; this is the first vertix
  furthestLeft(&points, &vertices);
  while (true) {
    // the loop stops if it can't find a new vertix to add to the array
    if (!nextVertix(&vertices.coords[vertices.dim-1], &points, &vertices))
      break;
    // if the last vertix found is equal to the first one the wrapping is complete
    if (vertices.dim > 0 && equals(&(vertices.coords[vertices.dim-1]), &(vertices.coords[0])))
      break;
  }

  // writes all the coordinates onto the file
  printf("Writing files...\n");
  writePointsToFile(POINTS_FILE, &points);
  writePointsToFile(VERTICES_FILE, &vertices);

  printf("\nORIGINAL NUMBER OF DOTS: %hd\nACTUAL NUMBER OF DOTS: %hd\n", NUM_POINTS, points.dim);
  printf("NUMBER OF VERTICES: %hd\n", vertices.dim-1);

  // calls the Python script to generate a Turtle visualitation of the problem.
  char cmnd[120];
  sprintf(cmnd, "python3 ./drawPoints.py %s %s %d %d %d %d", POINTS_FILE, VERTICES_FILE, WIDTH, HEIGHT, points.dim, vertices.dim-1);
  system(cmnd);
  return (EXIT_SUCCESS);
}



void randomPoints(struct Array *array) {
  struct Point point;

  for (unsigned long long int i = 0; i < NUM_POINTS; i++) {
    point.x = rand() % (WIDTH/2 - 40);
    point.y = rand() % (HEIGHT/2 - 100);
    // every coordinate has a 50% probability of being negative
    point.x = rand() % 2 ? -point.x : point.x;
    point.y = rand() % 2 ? -point.y : point.y;

    // only adds the point if it is a new one (to speed up the Python script's execution)
    if (!alreadyStored(array, &point)) {
      array->coords[array->dim] = point;
      array->dim += 1;
    }
  }
}



bool alreadyStored(const struct Array *array, const struct Point *point) {
  for (unsigned int i = 0; i < array->dim; i++)
    if (equals(point, &array->coords[i]))
      return true;
  return false;
}



bool equals(const struct Point *a, const struct Point *b) {
  return (a->x == b->x && a->y == b->y);
}



void furthestLeft(const struct Array *points, struct Array *vertices) {
  struct Point left = points->coords[0];

  for (unsigned int i = 0; i < points->dim; i++)
    if (points->coords[i].x < left.x)
      left = points->coords[i];

  vertices->coords[vertices->dim] = left;
  vertices->dim += 1;
}



bool isFurthestRight(const struct Point *point, const struct Array *array) {
  for (unsigned long long int i = 0; i < array->dim; i++)
    if (array->coords[i].x > point->x)
      return false;
  return true;
}



bool nextVertix(const struct Point *lastVertix, struct Array *points, struct Array *vertices) {
  float angle, maxAngle = -361;
  int index = -1;
  int u = 1;
  static bool reversed = false;

  // if the point is the furthest to the right then the algorithm should proceed backwards (right to left)
  if (isFurthestRight(lastVertix, points))
    reversed = true;

  // if going backwards I multiply both point's x to -1 if (-points[i].x < -p.x) it discards points[i]
  if (reversed)
    u = -1;

  for (unsigned long long int i = 0; i < points->dim; i++) {
    // if going onwards and the point at index i is on the left of the current one (u = 1)
    // then the point is skipped. Same goes if going backwards and the point is on the right (u = -1)
    if (u * points->coords[i].x  <  u * lastVertix->x)
      continue;
    if (equals(lastVertix, &points->coords[i]))
      continue;

    // angle between lastVertix and the point at index i in the array
    angle = angleBetween(lastVertix, &points->coords[i], reversed);
    // if the current angle is bigger than the "maxAngle" then
    // the point at index i is the best fit for the next vertix
    if (angle > maxAngle) {
      maxAngle = angle;
      index = i;
    }
  }

  // next vertix not found
  if (index < 0)
    return false;

  // adds vertix to the array
  vertices->coords[vertices->dim] = points->coords[index];
  vertices->dim += 1;
  // removes the vertix from the array of points since it won't be taken in consideration anymore
  removePoint(points, index);
  return true;
}



float angleBetween(const struct Point *a, const struct Point *b, bool reversed) {
  // I calculate the angle between them by considering the right triangle they form.
  // The legs are formed by the distance between the Xs and the Ys of the points.
  // The hypotenuse is calculated according to the Pythagorean theorem.
  // The angle is then the acos between the Xs leg and the hypotenuse.
  float cat_x = fabs(a->x - b->x);
  float cat_y = fabs(a->y - b->y);
  float ipo = sqrt(pow(cat_x, 2) + pow(cat_y, 2));
  float ang = acos(cat_x / ipo);
  // if proceeding backwards and point A has a lower Y than B, then it considers the angle to be negative
  // this is because I want to find the lowest points as vertix
  if (reversed && b->y > a->y)
    return -ang;
  // if going from left to right I want the angle to be positive only when A is lower than B
  if (!reversed && b->y < a->y)
    return -ang;
  return ang;
}



void removePoint(struct Array *array, unsigned int index) {
  for (unsigned int i = index; i < array->dim - 1; i++)
    array->coords[i] = array->coords[i+1];
  array->dim -= 1;
}



FILE *openFile(const char *path, const char *mode) {
  FILE *file = fopen(path, mode);
  if (!file) {
    printf("Couldn't open file \"%s\".\n", path);
    exit (EXIT_FAILURE);
  }
  return file;
}



void writePointsToFile(const char *filePath, const struct Array *array) {
  FILE *file = openFile(filePath, "w");
  for (unsigned int i = 0; i < array->dim; i++)
    fprintf(file, "%.0f\n%.0f\n", array->coords[i].x, array->coords[i].y);
  fclose(file);
}
