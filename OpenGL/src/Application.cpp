#include <GLFW/glfw3.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define PI 3.1415926535
#define P2 PI/2
#define P3 3*PI/2
#define DR 0.0174533

/*
    PORTE DA ENGINE DE RAYCAST DO 3DSAGE FEITA EM FREEGLUT PARA GLFW
    ENGINE E LOGICA ORIGINAl: https://www.youtube.com/watch?v=gYRrGTC7GtA
*/

float px, py; // player position
float pdx, pdy, pa; //delta x, y e angulo
float speed = 0.3; // velocidade de movimento

int mapX = 8, mapY = 8;
int screenWidth = 800, screenHeight = 600; // nova resolução
int map[] = {
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 1, 0, 0, 0, 1,
    1, 0, 0, 1, 0, 0, 0, 1,
    1, 0, 0, 1, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 0, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1
};

float dist(float ax, float ay, float bx, float by, float ang) {
    return (sqrt((bx - ax) * (bx - ax) + (by - ay) * (by - ay)));
}

void drawRays2D() {
    int r, mx, my, mp, dof;
    float rx, ry, ra, xo, yo, disT;

    ra = pa-DR*30;
    if (ra < 0) ra += 2 * PI;
    if (ra > 2 * PI) ra -= 2 * PI;
 
    for (r = 0; r < 60; r++) {
        //horizontal line
        dof = 0;
        float disH = 9999999, hx = px, hy = py;
        float aTan = -1 / tan(ra);
        if (ra>PI) { //up
            ry = (((int)py >> 6) << 6) - 0.0001;
            rx = (py - ry) * aTan + px;
            yo = -64;
            xo = -yo * aTan;
        }
        if (ra < PI) {//down
            ry = (((int)py >> 6) << 6) + 64;
            rx = (py - ry) * aTan + px;
            yo = 64;
            xo = -yo * aTan;
        }
        if (ra == 0 || ra == PI) { //looking straight
            rx = px;
            ry = py;
            dof = 8;
        }

        while (dof < 8) {
            mx = (int)(rx) >> 6;
            my = (int)(ry) >> 6;
            mp = my * mapX + mx;
            if (mp > 0 && mp < mapX * mapY && map[mp] == 1) {
                hx = rx;
                hy = ry;
                disH = dist(px, py, hx, hy, ra);
                dof = 8; //hit wall
            }
            else { //se nao encontrou nenhuma parede soma na direcao ate encontrar uma
                rx += xo;
                ry += yo;
                dof += 1;
            }

        }

        //vertical line
        dof = 0;
        float disV = 9999999, vx = px, vy = py;
        float nTan = -tan(ra);
        if (ra > P2 && ra < P3) {//left
            rx = (((int)px >> 6) << 6) - 0.0001;
            ry = (px - rx) * nTan + py;
            xo = -64;
            yo = -xo * nTan;
        }
        if (ra < P2 || ra > P3) { //right
            rx = (((int)px >> 6) << 6) + 64;
            ry = (px - rx) * nTan + py;
            xo = 64;
            yo = -xo * nTan;
        }
        if (ra == 0 || ra == PI) { //looking straight up or down
            rx = px;
            ry = py;
            dof = 8;
        }

        while (dof < 8) {
            mx = (int)(rx) >> 6;
            my = (int)(ry) >> 6;
            mp = my * mapX + mx;
            if (mp > 0 && mp < mapX * mapY && map[mp] == 1) {
                vx = rx;
                vy = ry;
                disV = dist(px, py, vx, vy, ra);
                dof = 8; //hit wall
            }
            else { //se nao encontrou nenhuma parede soma na direcao ate encontrar uma
                rx += xo;
                ry += yo;
                dof += 1;
            }
        }
        if (disV < disH) {
            rx = vx;
            ry = vy;
            disT = disV;
        }
        if (disH < disV) {
            rx = hx;
            ry = hy;
            disT = disH;
        }

        //draw gizmo
        glColor3f(0, 1, 0);
        glLineWidth(3);
        glBegin(GL_LINES);
        glVertex2i(px, py);
        glVertex2i(rx, ry);
        glEnd();


        //DRAW 3D WALLS
        float ca = pa - ra;
        if (ca < 0)
            ca += 2 * PI;
        if (ca > 2 * PI)
            ca -= 2 * PI;
        disT = disT * cos(ca); //fisheye effect fix
        float lineH = (64 * 320) / disT;
        if (lineH > 320) {
            lineH = 320;
        }
        float lineO = 160 - lineH / 2;

        glColor3f(1, 0, 0);
        glLineWidth(8);
        glBegin(GL_LINES);
        glVertex2i(r * 8 + 530, lineO);
        glVertex2i(r * 8 + 530, lineH+lineO);
        glEnd();

        ra += DR;
        if (ra < 0) ra += 2 * PI;
        if (ra > 2 * PI) ra -= 2 * PI;
    }
}

void drawMap2D() {
    int x, y, xo, yo;
    float mapWidth = mapX; //* mapS;
    float mapHeight = mapY; //* mapS;

    for (y = 0; y < mapY; y++) {
        for (x = 0; x < mapX; x++) {
            /*create the map, using white squares for walls and black for empty spaces*/
            if (map[y * mapX + x] == 1) {
                glColor3f(1, 1, 1);
            }
            else {
                glColor3f(0, 0, 0);
            }

            xo = x * 64;
            yo = y * 64;

            glBegin(GL_QUADS);
            glVertex2i(xo + 1, yo + 1);
            glVertex2i(xo + 1, yo + 64 - 1);
            glVertex2i(xo + 64 - 1, yo + 64 - 1);
            glVertex2i(xo + 64 - 1, yo + 1);
            glEnd();
        }
    }
}

void drawPlayer() {
    glColor3f(1, 1, 0);
    glPointSize(8);

    glBegin(GL_POINTS);
    glVertex2f(px, py);
    glEnd();

    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(px, py);
    glVertex2f(px + pdx * 5, py + pdy * 5);
    glEnd();
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Atualize a posição do jogador com base na tecla pressionada
    if (key == GLFW_KEY_W) {
        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            px += pdx * speed;
            py += pdy * speed;
        }
    }
    if (key == GLFW_KEY_S) {
        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            px -= pdx * speed;
            py -= pdy * speed;
        }
    }
    if (key == GLFW_KEY_A) {
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
        {
            pa -= 0.1;
            if (pa < 0)
                pa += 2 * PI;
            pdx = cos(pa) * 5;
            pdy = sin(pa) * 5;
        }
    }
    if (key == GLFW_KEY_D) {
        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            pa += 0.1;
            if (pa > 2 * PI)
                pa -= 2 * PI;
            pdx = cos(pa) * 5;
            pdy = sin(pa) * 5;
        }
    }
}


void init() {
    px = screenHeight / 2;
    py = screenWidth / 2;
    glClearColor(0.3, 0.3, 0.3, 0);
    glOrtho(0, screenWidth, screenHeight, 0, -1, 1); // A matriz de projeção é configurada para a nova resolução
}

int main(void) {
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(screenWidth, screenHeight, "Raycast engine", NULL, NULL); // Use a nova resolução
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);
    init();

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        //--------- Render here
        //init();
        drawMap2D();
        drawPlayer();
        drawRays2D();
        //--------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}