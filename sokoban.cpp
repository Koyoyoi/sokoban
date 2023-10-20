#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <vector>
#include <curses.h>
#include <algorithm>

#define MAX_ROW  25
#define MAX_COL  80

// COLOR
#define back_color   COLOR_BLACK
#define wall_color   COLOR_YELLOW
#define boxs_color   COLOR_RED
#define dest_color   COLOR_CYAN
#define worker_color COLOR_WHITE
#define pass_color   COLOR_WHITE
// Character
#define wall   'H'
#define boxs   'B'
#define dest   'D'
#define worker 'W'

using namespace std;
int w_c = 0; // Worker's number.
int pass = 0, point = 0, step = 0, nbox = 0;
int map_row = 0, map_col = 0, scale = 1;
char fileName[11];

class C_map{
public:
    char ch;
    int x, y, color;
    C_map(char c = ' ', int x = 0, int y = 0, int color = 0): ch(c), x(x), y(y)>
};

vector<C_map> map;
enum Direction {LEFT = 104, UP = 106, DOWN = 107, RIGHT = 108} d;

bool compare (const C_map& A , const C_map& B){
    if(A.color != B.color)
        return A.color < B.color;
    else if(A.x != B.y)
        return A.x < B.x;
    else
        return A.y < B.y;
}

void initial(){
    erase();
    map.clear();
    pass = 0;
    nbox = 0;
    step = 0;
    map_col = 0;
    map_row = 1;
}

int ReadMap(int n){
    initial();
    sprintf(fileName, "map%03d.txt", n);
    ifstream ifs(fileName, ios::in);
    if(!ifs.is_open())
        cout << "Failed to open " << fileName << '.' << endl;
    else{
        string tmp;
        for(int i = 0; getline(ifs, tmp); i++){
            for(int j = 0, r = 0, t = 0; j <= tmp.size(); j++){
                if(tmp[j] == wall)
                    map.push_back(C_map(tmp[j], i, j + r + t, 1));
                else if(tmp[j] == worker)
                    map.push_back(C_map(tmp[j], i, j + r + t, 4));
                else if(tmp[j] == dest){
                    map.push_back(C_map(tmp[j], i, j + r + t, 2));
                    pass += 1;
                }
                else if(tmp[j] == boxs){
                    map.push_back(C_map(tmp[j], i, j + r + t, 3));
                    nbox += 1;
                }
                else if(tmp[j] == '\r')
                    r = j;
                else if(tmp[j] == '\t')
                    t = 6;
                else if(tmp[j] == 'C'){
                    map.push_back(C_map(boxs, i, j + r + t, 6));
                    map.push_back(C_map(dest, i, j + r + t, 2));
                    pass += 1;
                    nbox += 1;
                }
                if(map_col < tmp.size())
                    map_col = tmp.size();
            }
            map_row = i + 1;
        }
    ifs.close();
    }
}

char find_ch(int x, int y){
    for(int i = 0; i < map.size(); i++)
        if(map[i].x == x && map[i].y == y && map[i].ch != dest && map[i].ch !=  ' '){
            return map[i].ch;
            break;
        }
}

int find_box(int x, int y){
    for(int i = 0; i < map.size(); i++)
        if(map[i].x == x && map[i].y == y && map[i].ch == boxs){
            return i;
            break;
        }
}

int find_dest(int x, int y){
    for(int i = 0; i < map.size(); i++){
        if(map[i].x == x && map[i].y == y && map[i].ch == dest){
            return true;
            break;
        }
    }
    return false;
}

void find_worker(){
    for(int i = w_c; i < map.size(); i++)
        if(map[i].ch == worker && i != w_c){
            w_c = i;
            break;
        }
}

int check(){
    point = 0;
    for(int i = 0; i < map.size(); i++){
        if(map[i].ch == boxs){
            if(find_dest(map[i].x, map[i].y))
                map[i].color = 6;
            else
                map[i].color = 3;
        }
        else if(map[i].ch == worker){
            if(find_dest(map[i].x, map[i].y))
                map[i].color = 5;
            else
                map[i].color = 4;
        }
        if(map[i].color == 6)
            point += 1;
    }
    return point /= scale * scale;
}

void MoveBox(Direction d){
    int b_c;
    switch(d){
        case LEFT:
            b_c = find_box(map[w_c].x, map[w_c].y - scale);
            map[b_c].y -= scale; break;
        case UP:
            b_c = find_box(map[w_c].x - scale, map[w_c].y);
            map[b_c].x -= scale; break;
        case DOWN:
            b_c = find_box(map[w_c].x + scale, map[w_c].y);
            map[b_c].x += scale; break;
        case RIGHT:
            b_c = find_box(map[w_c].x, map[w_c].y + scale);
            map[b_c].y += scale; break;
    }
}

char NextNextToWorker(Direction d){
    switch(d){
        case LEFT:  return find_ch(map[w_c].x, map[w_c].y - 2 * scale); break;
        case UP:    return find_ch(map[w_c].x - 2 * scale, map[w_c].y); break;
        case DOWN:  return find_ch(map[w_c].x + 2 * scale, map[w_c].y); break;
        case RIGHT: return find_ch(map[w_c].x, map[w_c].y + 2 * scale); break;
    }
}

char NextToWorker(Direction d){
    char ch;
    switch(d){
        case LEFT:  ch = find_ch(map[w_c].x, map[w_c].y - scale); break;
        case UP:    ch = find_ch(map[w_c].x - scale, map[w_c].y); break;
        case DOWN:  ch = find_ch(map[w_c].x + scale, map[w_c].y); break;
        case RIGHT: ch = find_ch(map[w_c].x, map[w_c].y + scale); break;
    }
    if(ch == boxs){
        if(NextNextToWorker(d) != wall && NextNextToWorker(d) != boxs){
            MoveBox(d);
            return ch;
        }
        else
            return wall;
    }
    else
        return ch;
}

void MoveWorker(Direction d){
    for(int i = 0; i < scale * scale; i++){
        find_worker();
        if(NextToWorker(d) != wall){
            move(map[w_c].x, map[w_c].y); addch(' ');
            switch(d){
                case LEFT:  map[w_c].y -= scale; step++; break;
                case UP:    map[w_c].x -= scale; step++; break;
                case DOWN:  map[w_c].x += scale; step++; break;
                case RIGHT: map[w_c].y += scale; step++; break;
            }
       }
    }
}

void ShowMessage(int x){
    if(x == 1){
        move(5, COLS / 2 + 20); addstr("PASS!");
        getch();
    }
    else if(x == 0){
        move(5, COLS / 2 + 20); addstr("find bugs!");
        move(6, COLS / 2 + 20); printw("%d of boxs", nbox);
        move(7, COLS / 2 + 20); printw("%d of dest", pass);
        getch();
    }
    else if(x == -1){
        move(5, COLS / 2 + 20); addstr("GAME END!");
        getch();
    }
}

void ResetMap(int n){
    map.clear();
    ReadMap(n);
}

void ExpandMap(int n){
    int size = map.size();
    if(n > 1)
        for(int i = 0; i < size; i++){
            map[i].x *= n;
            map[i].y *= n;
            for(int j = 1; j < scale; j++){
                map.push_back(C_map(map[i].ch, map[i].x, map[i].y + j, map[i].color));
                for(int k = 0; k < scale; k++)
                    map.push_back(C_map(map[i].ch, map[i].x + j, map[i].y + k, map[i].color));
            }
        }
    clear();
}

void ResizeMap(char ch, int n){
    if(ch == '-' && scale > 1){
        ResetMap(n);
        ExpandMap(scale -= 1);
    }
    if(ch == '+' && map_row * (scale + 1) <= MAX_ROW && map_col * (scale + 1) 
        ResetMap(n);
        ExpandMap(scale += 1);
    }
}

void ShowMap(int point){
    sort(map.begin(), map.end(), compare);
    for(int i = 0; i < map.size(); i++){
        move(map[i].x, map[i].y);
        if(map[i].ch == worker)
            w_c = i;
        attron(COLOR_PAIR(map[i].color));
        if(step <= 1 || map[i].ch != wall)
 printw("%c", map[i].ch);
        attroff(COLOR_PAIR(map[i].color));
        refresh();
    }
    move(0, COLS / 2 + 20); printw("%s", fileName);
    move(1, COLS / 2 + 20); addstr("Arrivals: "); printw("%d", point);
    move(2, COLS / 2 + 20); addstr("Need box: "); printw("%d", pass);
    move(3, COLS / 2 + 20); addstr("Steps: "); printwprintw("%d", step / (scale * scale));
    move(4, COLS / 2 + 20); addstr("Scale: "); printw("%d", scale);
    move(LINES - 2, COLS / 2 - 35);
    addstr("you can press 'n' to next or 'r' to"
           " resert and or 'q' to quit the map.");
    move(map[w_c].x, map[w_c].y);
    w_c = 0;
    if(point == pass)
        ShowMessage(1);
}

int main(){
    initscr();
    curs_set(0);
    start_color();
    init_pair(1, wall_color, back_color);
    init_pair(2, dest_color, dest_color);
    init_pair(3, boxs_color, back_color);
    init_pair(4, back_color, worker_color);
    init_pair(5, pass_color, dest_color);
    init_pair(6, pass_color, dest_color);
    char k;
    for(int n = 0; n <= 150 ; n++){
        ReadMap(n);
         while((map_row * scale > MAX_ROW || map_col * scale > MAX_COL) && scale > 1)
            scale -= 1;
        ExpandMap(scale);
        while(check() != pass){
            ShowMap(check());
            k = getch();
            d = (enum Direction)k;
            if(pass != nbox){
                ShowMessage(0);
                break;
            }
            else if(k == 'n'){
                initial();
                break;
            }
            else if(k == 'r'){
                ResetMap(n);
                scale = 1;
            }
            else if(k == 'q'){
                initial();
                endwin();
                return 0;
            }
            else if(k == '-' || k == '+')
                ResizeMap(k, n);
            else
                MoveWorker(d);
        }
        if(k != 'n')
            ShowMap(point);
    }
    ShowMessage(-1);
    endwin();
    return 0;
}




