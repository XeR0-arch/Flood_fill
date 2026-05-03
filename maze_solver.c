#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#define MAX_SIZE 256
#define m 16

int x = 0;
int y = 0;
int top = -1;
int bottom = -1;
//int m = 16;
uint8_t orientiation; //0b11000000(192)+y 0b00110000(48) +x
                      //0b00001100(12) -y 0b00000011(3)  -x
//if(PINX == HIGH){
//orientiation = 192;
//}
//else{
//orientiation = 48;
//}

typedef struct cell{
    bool updated;
    bool wall_px;
    bool wall_nx;
    bool wall_py;
    bool wall_ny;
    uint8_t value;
}MazeCell;

typedef struct coordinate{
    int x;
    int y;
}XY;

XY arr[MAX_SIZE];
MazeCell maze[16][16];

//uint16_t maze[16][16]; 5-8 4 bits for walls: 0b0000 (no walls), 0b0001 (wall to the right),
//0b0010 (wall down), 0b0100 (wall to the left), 0b1000 (wall up) first 4 bits for visited
//first 4 bits for status: 0b1111xxxx.... (visited), 0b0000xxxx.... (not visited);
//end eight for the cell value ie. distance from center(calculated)

void enque(int x, int y){
    if(bottom <= 255){
        ++bottom;
        arr[bottom].x = x;
        arr[bottom].y = y;
    }
    // else{
    //     return;
    // }
}

XY dequeue(){
    if(top <= bottom){
        ++top;
        return arr[top];
    }
    XY not_valid;
    not_valid.x = -1;
    not_valid.y = -1;
    return not_valid;
}

// int peek_x(){
//     if(top <= bottom){
//         return arr[top].x;
//     }
// }

// int peek_y(){
//     if(top <= bottom){
//         return arr[top].y;
//     }
// }

void reset_queue(){
    top = -1;
    bottom = -1;
    enque(7,7);
    enque(7,8);
    enque(8,7);
    enque(8,8);
}

bool turn_right(){
    //set the motor pins to turn right and return true if the turn is successful
    return true;
}
bool turn_left(){
    //set the motor pins to turn left and return true if the turn is successful
    return true;
}
bool move_forward(){
    //set the motor pins to move forward and return true if the turn is successful
    return true;
}
bool turn_around(){
    //set the motor pins to turn around and return true if the turn is successful
    return true;
}

void move(uint8_t direction){
    //0b11000000 = right, 0b00110000 = left, 0b00001100 = front, 0b00000011 = back
    //if and only if the turn is sucessfull will the orientiation be updated
    //TODO: convert the mess to switch
    //Convert the directions to clock and anticlock and steps to clear the mess below
    if(direction == 192 && turn_right()){
        if(orientiation == 192){
            orientiation = 48;
            x += 1;
        }
        else if(orientiation == 48){
            orientiation = 12;
            y -= 1;
        }
        else if(orientiation == 12){
            orientiation = 3;
            x -= 1;
        }
        else if(orientiation == 3){
            orientiation = 192;
            y += 1;
        }
    }
    else if(direction == 48 && turn_left()){
        if(orientiation == 48){
            orientiation = 192;
            y += 1;
        }
        else if(orientiation == 192){
            orientiation = 3;
            x -= 1;
        }
        else if(orientiation == 12){
            orientiation = 48;
            x += 1;
        }
        else if(orientiation == 3){
            orientiation = 12;
            y -= 1;
        }
    }
    else if(direction == 12 && move_forward()){
        if(orientiation == 12){
            orientiation = 12;
            y -= 1;
        }
        else if(orientiation == 192){
            orientiation = 192;
            y += 1;
        }
        else if(orientiation == 48){
            orientiation = 48;
            x += 1;
        }
        else if(orientiation == 3){
            orientiation = 3;
            x -= 1;
        }
    }
    else if(direction == 3 && turn_around()){
        if(orientiation == 3){
            orientiation = 48;
            x += 1;
        }
        else if(orientiation == 12){
            orientiation = 192;
            y += 1;
        }
        else if(orientiation == 192){
            orientiation = 12;
            y -= 1;
        }
        else if(orientiation == 48){
            orientiation = 3;
            x -= 1;
        }
    }
}

bool is_valid(int x_prev, int y_prev, int x, int y){
    if(x>=0 && y>=0 && x<=15 && y<=15 && !maze[x][y].updated) return true;
    else return false;
}

void update_cell(bool array[], int x, int y){ //the array should be of ones and zeros not actual
    //adc values if possible combine left_front and right_front and change index accordingly
    //scan adc leftmost to rightmost (can skip diagonals for this reading)
    //adc values stored in a array left_dig, left, left_front, right_front, right, right_dig
    //                                0        1       2            3         4        5
    if(orientiation == 192){
        maze[x][y].wall_nx = array[1];
        maze[x][y].wall_py = array[2];
        maze[x][y].wall_px = array[4];
    }
    else if(orientiation == 48){
        maze[x][y].wall_py = array[1];
        maze[x][y].wall_px = array[2];
        maze[x][y].wall_ny = array[4];
    }
    else if(orientiation == 12){
        maze[x][y].wall_nx = array[4];
        maze[x][y].wall_px = array[1];
        maze[x][y].wall_ny = array[2];
    }
    else if(orientiation == 3){
        maze[x][y].wall_nx = array[2];
        maze[x][y].wall_py = array[4];
        maze[x][y].wall_ny = array[1];
    }
    
    if(x == 15){
        if(y == 0){
            maze[x-1][y].wall_px = maze[x][y].wall_nx;
            maze[x][y+1].wall_ny = maze[x][y].wall_py;
        }
        else if(y == 15){
            maze[x-1][y].wall_px = maze[x][y].wall_nx;
            maze[x][y-1].wall_py = maze[x][y].wall_px;
        }
        else if(y < 15 && y > 0){
            maze[x-1][y].wall_px = maze[x][y].wall_nx;
            maze[x][y+1].wall_ny = maze[x][y].wall_py;
            maze[x][y-1].wall_py = maze[x][y].wall_px;
        }
    }
    else if(x == 0){
        if(y == 0){
            maze[x+1][y].wall_nx = maze[x][y].wall_px;
            maze[x][y+1].wall_ny = maze[x][y].wall_py;
        }
        else if(y == 15){
            maze[x+1][y].wall_nx = maze[x][y].wall_px;
            maze[x][y-1].wall_py = maze[x][y].wall_ny;
        }
        else if(y < 15 && y > 0){
            maze[x+1][y].wall_nx = maze[x][y].wall_px;
            maze[x][y+1].wall_ny = maze[x][y].wall_py;
            maze[x][y-1].wall_py = maze[x][y].wall_ny;
        }
    }
    else if(y == 0){
        maze[x+1][y].wall_nx = maze[x][y].wall_px;
        maze[x][y+1].wall_ny = maze[x][y].wall_py;
        maze[x-1][y].wall_px = maze[x][y].wall_nx;
    }
    else if(y == 15){
        maze[x+1][y].wall_nx = maze[x][y].wall_px;
        maze[x][y-1].wall_py = maze[x][y].wall_ny;
        maze[x-1][y].wall_px = maze[x][y].wall_nx;
    }
}

void mark_un_updated(){
    for(int i = 0; i < m; i++) {
        for (int j = 0; j < m; j++) {
            maze[i][j].updated = false;
        }
    }
    maze[7][7].updated = true;
    maze[7][8].updated = true;
    maze[8][7].updated = true;
    maze[8][8].updated = true;
    maze[7][7].value = 0;
    maze[7][8].value = 0;
    maze[8][7].value = 0;
    maze[8][8].value = 0;
}

bool all_visited(){
    for(int i = 0; i < m; i++) {
        for (int j = 0; j < m; j++) {
            if(maze[i][j].updated = false){
                return false;
            }
        }
    }
    return true;
}

void update_maze(){
    reset_queue();
    mark_un_updated();
    XY obj;
    int x_fill;
    int y_fill;
    while(!all_visited()){
        obj = dequeue();
        x_fill = obj.x;
        y_fill = obj.y;
        if(is_valid(x_fill,y_fill,x_fill+1,y_fill) && !maze[x_fill][y_fill].wall_px){
            maze[x_fill+1][y_fill].value = maze[x_fill][y_fill].value + 1;
            maze[x_fill+1][y_fill].updated = true;
            enque(x_fill+1,y_fill);
        }
        if(is_valid(x_fill,y_fill,x_fill-1,y_fill) && !maze[x_fill][y_fill].wall_nx){
            maze[x_fill-1][y_fill].value = maze[x_fill][y_fill].value + 1;
            maze[x_fill-1][y_fill].updated = true;
            enque(x_fill-1,y_fill);
        }
        if(is_valid(x_fill,y_fill,x_fill,y_fill+1) && !maze[x_fill][y_fill].wall_py){
            maze[x_fill][y_fill+1].value = maze[x_fill][y_fill].value + 1;
            maze[x_fill][y_fill+1].updated = true;
            enque(x_fill,y_fill+1);
        }
        if(is_valid(x_fill,y_fill,x_fill,y_fill-1) && !maze[x_fill][y_fill].wall_ny){
            maze[x_fill][y_fill-1].value = maze[x_fill][y_fill].value + 1;
            maze[x_fill][y_fill-1].updated = true;
            enque(x_fill,y_fill-1);
        }
    }

}

uint8_t next_move(){
        uint8_t px = 255;
        uint8_t py = 255;
        uint8_t nx = 255;
        uint8_t ny = 255;
    if(orientiation == 192){
        if(x+1 <= 15 && x+1 >= 0 && y >= 0 && y <= 15 && !maze[x][y].wall_px){
            px = maze[x+1][y].value;
        }
        if(x-1 <= 15 && x-1 >= 0 && y >= 0 && y <= 15 && !maze[x][y].wall_nx){
            nx = maze[x-1][y].value;
        }
        if(x <= 15 && x >= 0 && y+1 >= 0 && y+1 <= 15 && !maze[x][y].wall_py){
            py = maze[x][y+1].value;
        }
        if(x <= 15 && x >= 0 && y-1 >= 0 && y-1 <= 15 && !maze[x][y].wall_ny){
            ny = maze[x][y-1].value;
        }
        
        
    }
}

int main() {
    for(int i = 0; i < m/2; i++) {
        for (int j = 0; j < m; j++) {                
            if (i < m/2 && j < m/2) {
                maze[i][j].value = m - 2 - i - j;
                maze[i][m - 1 - j].value = maze[i][j].value;
            }
                maze[m - 1 - i][j].value = maze[i][j].value;
        }
    }

    while(maze[x][y].value != 0){

    }
    // for (int i = 0; i < m; i++) {
    //     for (int j = 0; j < m; j++) {      
    //         if(maze[i][j].value < 10) {
    //             printf("%d ",maze[i][j].value);
    //         }
    //         else {
    //             printf("%d ",maze[i][j].value);
    //         }
    //     }
    //     printf("\n");
    // }
    return 0;
}