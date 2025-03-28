#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <math.h>

#define PI 3.1415926

void draw(char* screen, int width, int height)
{
    for(int i = height - 1; i >= 0; i--)
    {
        for(int j = 0; j < width; j++)
        {
            std::cout << screen[i * width + j];
        }
        std::cout << std::endl;
    }
}

int screen_width = 0;
int screen_height = 0;
int screen_size = 0;

double camera_distance = 100;
double obj_x_center;
double obj_y_center;
double obj_z_center = 100;
double obj_size;
double A = 0;
double B = 0;
double C = 0;

char* screen_arr;
double* deep_arr;

void sig_handler(int sig)
{
    //restore cursor visibility
    std::cout << "\x1B[?25h" << std::endl;
    //restore color
    std::cout << "\x1B[0m" << std::endl;
    //clear terminal
    system("clear");
    delete [] screen_arr;
    delete [] deep_arr;
    exit(0);
}

void winch_handler(int sig){
    signal(SIGWINCH, SIG_IGN);

    usleep(100000);
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    screen_width = w.ws_col;
    screen_height = w.ws_row;
    screen_size = screen_width * screen_height;
    screen_arr = new char[screen_size];
    deep_arr = new double[screen_size];
    obj_size = (screen_width/double(screen_height) > 4 ? int(std::min(sqrt(screen_width), sqrt(screen_height))) : int(std::max(sqrt(screen_width), sqrt(screen_height))));

    //teleport to near  edge if out of bounds due to window resize
    if(obj_x_center >= 1.83 * screen_width - int(2 * obj_size))
        obj_x_center = 1.83 * screen_width - int(2.5 * obj_size);
    if(obj_x_center <= int(2 * obj_size))
        obj_x_center = int(2.5 * obj_size);
    if(obj_y_center >= 2 * screen_height - int(2 * obj_size))
        obj_y_center = 2 * screen_height - int(2.5 * obj_size);
    if (obj_y_center <= int(1.7 * obj_size))
        obj_y_center = int(2.0 * obj_size);
    
    signal(SIGWINCH, winch_handler);
}

double calculateX(int x, int y, int z) {
  return y * sin(A) * sin(B) * cos(C) - z * cos(A) * sin(B) * cos(C) +
         y * cos(A) * sin(C) + z * sin(A) * sin(C) + x * cos(B) * cos(C);
}

double calculateY(int x, int y, int z) {
  return y * cos(A) * cos(C) + z * sin(A) * cos(C) -
         y * sin(A) * sin(B) * sin(C) + z * cos(A) * sin(B) * sin(C) -
         x * cos(B) * sin(C);
}

double calculateZ(int x, int y, int z) {
  return z * cos(A) * cos(B) - y * sin(A) * cos(B) + x * sin(B);
}

void set_surface(double x, double y, double z, char symbol)
{
    double xx = obj_x_center + calculateX(x - obj_x_center, y - obj_y_center, z - obj_z_center);
    double yy = obj_y_center + calculateY(x - obj_x_center, y - obj_y_center, z - obj_z_center);
    double zz = obj_z_center + calculateZ(x - obj_x_center, y - obj_y_center, z - obj_z_center);
    double coef = camera_distance / (zz + camera_distance);
    int x_idx = xx * coef;
    int y_idx = yy * coef;
    int idx = y_idx * screen_width + x_idx;
    double length = sqrt(xx*xx + yy*yy + zz*zz);
    if(x_idx >= 0 && x_idx < screen_width && y_idx >= 0 && y_idx < screen_height && 1/length > deep_arr[idx])
    {
        deep_arr[idx] = 1/length; 
        screen_arr[idx] = symbol;
    }
}

int main()
{
    signal(SIGINT, sig_handler);

    //clear screen
    std::cout << "\x1B[2J" << std::endl;
    
    //hide cursor
    std::cout << "\x1B[?25l" << std::endl;

    //set color
    std::cout << "\x1b[32m" << std::endl;

    int x_v = 6;
    int y_v = 6;

    //set term sizes
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    screen_width = w.ws_col;
    screen_height = w.ws_row;
    screen_size = screen_width * screen_height;
    screen_arr = new char[screen_size];
    deep_arr = new double[screen_size];
    obj_x_center = screen_width/2;
    obj_y_center = screen_height/2;
    obj_size = (screen_width/double(screen_height) > 4 ? int(std::min(sqrt(screen_width), sqrt(screen_height))) : int(std::max(sqrt(screen_width), sqrt(screen_height))));

    //set resize signal
    signal(SIGWINCH, winch_handler);

    while (true)
    { 
        //move cursor to start
        std::cout << "\x1B[H" << std::endl;

        //fill arrays
        memset(screen_arr, ' ', screen_size);
        memset(deep_arr, 0, sizeof(double) * screen_size);

        //compute cube
        for(double i = -obj_size; i < obj_size; i += 0.7)
        {
            for(double j = -obj_size; j < obj_size; j += 0.7)
            {
                //uncomment code below to turn squares into circles
                // if(i*i + j*j < obj_size * obj_size/4)
                // {
                    //compute all 6 planes
                    set_surface(obj_x_center - obj_size, obj_y_center + i, obj_z_center + j, ',');
                    set_surface(obj_x_center + obj_size, obj_y_center + i, obj_z_center + j, '#');
                    set_surface(obj_x_center + i, obj_y_center - obj_size, obj_z_center + j, '*');
                    set_surface(obj_x_center + i, obj_y_center + obj_size, obj_z_center + j, '%');
                    set_surface(obj_x_center + i, obj_y_center + j, obj_z_center - obj_size, '=');
                    set_surface(obj_x_center + i, obj_y_center + j, obj_z_center + obj_size, '!');
                //}
            }
        }
        
        //draw cube
        draw(screen_arr, screen_width, screen_height);

        //needs to human perception 
        usleep(50000);

        //change cube speed and rotation angles
        obj_x_center += x_v;
        obj_y_center += y_v;
        A += PI/100;
        B += PI/100;
        C += PI/200;

        //check out of bounce and change direction
        if(obj_x_center >= 2 * screen_width - int(2.2 * obj_size) || obj_x_center <= int(2.2 * obj_size))
            x_v *= -1;
        if(obj_y_center >= 2 * screen_height - int(2 * obj_size) || obj_y_center <= int(1.7 * obj_size))
            y_v *= -1;
    }
    return 0;
}
