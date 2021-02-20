import turtle
import sys


POINTS_FILE = sys.argv[1]
VERTICES_FILE = sys.argv[2]
WIDTH = int(sys.argv[3])
HEIGHT = int(sys.argv[4])
NUM_POINTS = int(sys.argv[5])
NUM_VERTICES = int(sys.argv[6])

# change these values
COLOR_WINDOW = "#1a1a1a"
COLOR_TEXT = "white"
COLOR_DOTS = "crimson"
COLOR_VERTICES = "green"
COLOR_LINE = "white"
LINE_SPEED = 4
POINTS_SHAPE = "circle"
POINTS_SIZE = 0.4
VERTICES_SHAPE = "circle"
VERTICES_SIZE = 0.9


# WINDOW
w = turtle.Screen()
w.setup(WIDTH, HEIGHT)
w.bgcolor(COLOR_WINDOW)

# POINTS COUNTER
pointsCounter = turtle.Turtle()
pointsCounter.penup()
pointsCounter.speed(0)
pointsCounter.hideturtle()
pointsCounter.color(COLOR_TEXT)
pointsCounter.setposition(-WIDTH/2 + 16, HEIGHT/2 - 58)
pointsCounter.write("points 0", align='left', font=('Arial', 12, 'italic'))

# VERTICES COUNTER
verticesCounter = turtle.Turtle()
verticesCounter.penup()
verticesCounter.speed(0)
verticesCounter.hideturtle()
verticesCounter.color(COLOR_TEXT)
verticesCounter.setposition(-WIDTH/2 + 15, HEIGHT/2 - 78)
verticesCounter.write("vertices 0", align='left', font=('Arial', 12, 'italic'))

# DOT
dot = turtle.Turtle()
dot.penup()
dot.hideturtle()
dot.speed(0)

# PERIMETER LINE
line = turtle.Turtle()
line.penup()
line.hideturtle()
line.pencolor(COLOR_LINE)
line.speed(LINE_SPEED)



def readCoordinates(filename, num, xList, yList):
    coordsFile = open(filename, "r")
    lines = coordsFile.readlines()
    for i in range(0, num*2):
        # X coordinates are stored in the even lines of the file
        if (i % 2 == 0):
            xList.append(int(lines[i]))
        # Y coordinates are stored in the odd lines of the file
        else:
            yList.append(int(lines[i]))
    coordsFile.close()


def writeCounter(counter, description, value):
        text = "{} {}".format(description, value)
        counter.undo()
        counter.write(text, align='left', font=('Arial', 12, 'italic'))


def drawPoints():
    X = []
    Y = []
    readCoordinates(POINTS_FILE, NUM_POINTS, X, Y)

    dot.color(COLOR_DOTS)
    dot.shape(POINTS_SHAPE)
    dot.shapesize(POINTS_SIZE)
    for i in range(NUM_POINTS):
        dot.goto(X[i], Y[i])
        dot.stamp()
        writeCounter(pointsCounter, "points:", i+1)


def connect():
    X = []
    Y = []
    readCoordinates(VERTICES_FILE, NUM_VERTICES, X, Y)

    dot.color(COLOR_VERTICES)
    dot.shape(VERTICES_SHAPE)
    dot.shapesize(VERTICES_SIZE)
    n = 0
    for i in reversed(range(-1, NUM_VERTICES)):
        line.goto(X[i], Y[i])
        dot.goto(X[i], Y[i])
        dot.stamp()
        line.pendown()
        writeCounter(verticesCounter, "vertices:", n)
        n += 1



if __name__ == '__main__':
    drawPoints()
    connect()
    closeInput = input("Press ENTER to close the script...")
    exit(0)
