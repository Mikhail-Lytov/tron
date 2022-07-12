//Лытов Михаил ККСО-03-20
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <string.h>
#include <sys/mman.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <signal.h>
#include <ncurses.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>


#define MAXLINE 1024
#define BUFLEN 512
#define PORT 12345
  

int work_flag = 1;
int fb, x = 500, y =500, y_car_2 = 500, x_car_2 = 500;
struct fb_var_screeninfo info;
size_t fb_size, map_size, page_size;
uint32_t *ptr, color_window = 0xfdc311, color_player_1 = 0x1a75b5, color_player_2 = 0x800000, color_game_over, color_won;
int up = 0,left = 0,down = 0,right = 0, ch = 0;
int centre_x, centre_y;
int window_x, window_y;
int stop_car_1 = 1, stop_car_2 = 1;
int up_car_2 = 0,left_car_2 = 0,down_car_2 = 0,right_car_2 = 0;
int start_game_car_1 = 0, start_game_car_2 = 0;
int passage_counter_car_1 = 0, passage_counter_car_2 = 0; 
int max_y = 0, max_x = 0, min_y = 0, min_x = 0;
int ran_up = 0,ran_down = 0, ran_left = 0, ran_right =0;
int ran_up_2 = 0,ran_down_2 = 0, ran_left_2 = 0, ran_right_2 = 0; 

struct sockaddr_in si_me, si_other;
int s, slen = sizeof(si_other) , recv_len;
int status_addr;
//pthread_mutex_t mutex;

void handler(int none){
  work_flag = 0;
}
int IP_int(char *str2){
    int length = strlen(str2) ;
    int c, i, j, namber, sum = 0;
    int degree = 0;

    for (i = 0, j = length - 1; i < j; i++, j--)
    {
        c = str2[i];
        str2[i] = str2[j];
        str2[j] = c;
    }
    //printf("\n%s", str2);
    for(i = 0; i < length; i++){
        if(str2[i] != '.'){
            namber = str2[i] - '0';
            sum += (namber * pow(10, degree));
            degree++;
        }else if(str2[i] == '.'){
            degree = 0;
        }
    }
    //printf("\n%d", sum);
    return sum;


}

int centre(){//поиск центра
  centre_x = info.xres_virtual / 2;
  centre_y = info.yres_virtual / 2;
  min_y = (centre_y - (window_y / 2));
  min_x = (centre_x - (window_x / 2));
  max_y = (centre_y + (window_y / 2));
  max_x = (centre_x + (window_x / 2));
  return 0;
}
int window(){//отрисовка рамки
  int i, j;
  for (i = min_y; i <= centre_y; i++){ // левая стенка

    ptr[i * info.xres_virtual + min_x] = color_window;
    ptr[i * info.xres_virtual + max_x] = color_window;
  }
  for (i = centre_y + 1; i <= max_y; i++){ // левая стенка

    ptr[i *  info.xres_virtual + min_x] = color_window;
    ptr[i * info.xres_virtual + max_x] = color_window;
  }
  for(i = min_x; i <= centre_x; i++){
    ptr[min_y * info.xres_virtual + i] =  color_window;
    ptr[max_y * info.xres_virtual + i] =  color_window;
  }
  for(i = centre_x; i <= max_x; i++){
    ptr[min_y * info.xres_virtual + i] = color_window;
    ptr[max_y * info.xres_virtual + i] = color_window;
  }

  return 0;
}


void die(char *a){
 perror(a);
 exit(1);
}

// не путайся
// вот это отправляет другому игроку, проверь норм или не норм
//

void user_2_server(char *argv[]){
  char buf[BUFLEN];
  int i;
  int ran = 0;
  recv_len = 0;
 
    while(work_flag && stop_car_1 && stop_car_2){

    if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1){
    die("recvfrom()");
    }
    //if(si_other.sin_addr.s_addr == inet_addr(argv[3])){
    if (si_other.sin_addr.s_addr == inet_addr(argv[3])){
      switch(buf[0]){
               case 'a':
               start_game_car_2 = 1;
                if((left_car_2 != 1) && (right_car_2 != 1)){
                  if(((up_car_2 == 1) && (ran_up_2 >= 3)) || ((down_car_2 == 1) && (ran_down_2 >= 3))){
                    left_car_2 = 1;
                    up_car_2 = 0;
                    down_car_2 = 0;
                    right_car_2 = 0;
                    ran_left_2 = 0;
                  }
                  }
                  break;
                case 'd':
                start_game_car_2 = 1;
                  if((right_car_2 != 1) && (left_car_2 != 1)){
                    if(((up_car_2 == 1) && (ran_up_2 >= 3)) || ((down_car_2 == 1) && (ran_down_2 >= 3))){
                    right_car_2 = 1;
                    left_car_2 = 0;
                    down_car_2 = 0;
                    up_car_2 = 0;
                    ran_right_2 = 0;
                  }
                  }
                  break;
                case 's':
                start_game_car_2 = 1;
                  if((down_car_2 != 1) && (up_car_2 != 1)){
                    if(((left_car_2 == 1) && (ran_left_2 >= 3)) || ((right_car_2 == 1) && (ran_right_2 >= 3))){
                    down_car_2 = 1;
                    right_car_2 = 0;
                    up_car_2 = 0;
                    left_car_2 = 0;
                    ran_down_2 = 0;
                  }
                  }
                 break;
                case 'w':
                start_game_car_2 = 1;
                  if((up_car_2 != 1) && (down_car_2 != 1)){
                    if(((left_car_2 == 1) && (ran_left_2 >= 3)) || ((right_car_2 == 1) && (ran_right_2 >= 3))){
                    up_car_2 = 1;
                    left_car_2 = 0;
                    right_car_2 = 0;
                    down_car_2 = 0;
                    ran_up_2 = 0;
                  }
                  } 
                 default:
                  if(buf[0] != '\0'){
                    start_game_car_2 = 1;
                     //mvprintw(0,0, "BLUE WON!");
                  }
                 
                 break;
                  
     }
     }
  //}
}

 close(s);

}
int check(){
  return 1;
}
void draw(){
  work_flag = 0;
  stop_car_1 = 0;
  stop_car_2 = 0;
  sleep(2);
  mvprintw(0, 0, "draw                ");
  refresh();
  endwin();
  sleep(5);
  system("clear");
  exit(1);

}

void won(){
  work_flag = 0;
  stop_car_1 = 0;
  stop_car_2 = 0;
  sleep(2);
  mvprintw(0, 0, "you won and took the first place");
  refresh();
  endwin();
  sleep(5);
  system("clear");
  exit(1);
}
void game_over(){
  int i = 0, j = 0;// очистка (не трогай)
    work_flag = 0;
    stop_car_1 = 0;
    stop_car_2 = 0;
    sleep(2);
    mvprintw(0, 0, "you lost and took second place");
  refresh();
  sleep(5);
  endwin();
  system("clear");
  exit(1);

}
void turn_up_car_2(int x_car_2, int y_car_2){
int i,j,k;
  for(i=0; i<=8; i++){
    for(j=-2; j<=2; j++){
        ptr[(y_car_2-i)*info.xres_virtual + (x_car_2+j)] = 0x00000000;
    }
  }
}
//from down
void turn_down_car_2(int x_car_2, int y_car_2){
int i,j;
  for(i=0; i<=8; i++){
    for(j=-2; j<=2; j++){
        ptr[(y_car_2+i)*info.xres_virtual + (x_car_2+j)] = 0x00000000;
    }
  }
}
//from right
void turn_right_car_2(int x_car_2, int y_car_2){
int i,j;
  for(i=0; i<=8; i++){
    for(j=-2; j<=2; j++){
        ptr[(y_car_2+j)*info.xres_virtual + (x_car_2+i)] = 0x00000000;
    }
  }
}

//from left
void turn_left_car_2(int x_car_2, int y_car_2){
int i,j;
  for(i=0; i<=8; i++){
    for(j=-2; j<=2; j++){
        ptr[(y_car_2+j)*info.xres_virtual + (x_car_2-i)] = 0x00000000;
    }
  }
}

void turn_up(int x, int y){
int i,j,k;
  for(i=0; i<=8; i++){
    for(j=-2; j<=2; j++){
        ptr[(y-i)*info.xres_virtual + (x+j)] = 0x00000000;
    }
  }
}
//from down
void turn_down(int x, int y){
int i,j;
  for(i=0; i<=8; i++){
    for(j=-2; j<=2; j++){
        ptr[(y+i)*info.xres_virtual + (x+j)] = 0x00000000;
    }
  }
}
//from right
void turn_right(int x, int y){
int i,j;
  for(i=0; i<=8; i++){
    for(j=-2; j<=2; j++){
        ptr[(y+j)*info.xres_virtual + (x+i)] = 0x00000000;
    }
  }
}

//from left
void turn_left(int x, int y){
int i,j;
  for(i=0; i<=8; i++){
    for(j=-2; j<=2; j++){
        ptr[(y+j)*info.xres_virtual + (x-i)] = 0x00000000;
    }
  }
}



void car_1(int flag_sides, int flag_sides_2){
  int i = 0, j = 0;
  int flag_stop = 0;
  int flag_stop_2 = 0;
  //int flag_sides = 0; // 0 - вверх, 1 - вниз, 2 - влево, 3 - вправо
    int stop = 1;
    int flag_game_over = 0, flag_won = 0;
    while((start_game_car_1 == 0) || start_game_car_2 == 0){
      if(flag_sides == 3){
          for(i=1; i<=8; i++){
            for(j=-2; j<=2; j++){
              ptr[(y+j)*info.xres_virtual + ((x)+i)] = color_player_1;
              ptr[(y_car_2+j)*info.xres_virtual + ((x_car_2)-i)] = color_player_2;
            }
          }
        }else if(flag_sides == 2){
          for(i=1; i<=8; i++){
            for(j=-2; j<=2; j++){
              ptr[(y+j)*info.xres_virtual + ((x)-i)] = color_player_1;
              ptr[(y_car_2+j)*info.xres_virtual + ((x_car_2)+i)] = color_player_2;
            }
          }
        } 
        window();
    }
    if(flag_sides == 2){
      for(i=1; i<7; i++){
          for(j=-2; j<=2; j++){
            ptr[(y-i)*info.xres_virtual + (x+j)] = 0x00000000;
            ptr[(y_car_2+j)*info.xres_virtual + (x_car_2+i)] = 0x00000000;
          }
        }
      left = 1;
      right = down = up = 0;
      right_car_2 = 1;
      left_car_2 = down_car_2 = up_car_2 = 0;
      }else if(flag_sides == 3){
        for(i=1; i<7; i++){
            for(j=-2; j<=2; j++){
              ptr[(y+j)*info.xres_virtual + (x+i)] = 0x00000000;
              ptr[(y_car_2-i)*info.xres_virtual + (x_car_2+j)] = 0x00000000;
            }
          }
        right = 1;
        left = down = up = 0;
        left_car_2 = 1;
        right_car_2 = down_car_2 = up_car_2 = 0;
      }
      
    while(work_flag && stop_car_1 && stop_car_2) {
      window();
      //pthread_mutex_lock(&mutex);
      if((start_game_car_1 && start_game_car_2) ){
        // чертова проверка на чертовы петли, но это все работает, но все равно самая первая версия лучше
        if((flag_sides == 3 && down) || (flag_sides == 2 && down)){
          if((y + 9) > max_y){
              flag_game_over = 1;
                stop_car_1 = 0;
                flag_stop = 1;    
          }
          if(flag_stop != 1){
            for(i=2; i<=8; i++){
              for(j=-2; j<=2; j++){
                if ((ptr[((y + 1)+i)*info.xres_virtual + (x+j)] == color_player_1)||
                  (ptr[((y + 1)+i)*info.xres_virtual + (x+j)] == color_player_2)||
                  (ptr[((y + 1)+i)*info.xres_virtual + (x+j)] == color_window)){
                  flag_game_over = 1;
                  stop_car_1 = 0;
                }
              }
            }
        }
        }else if((flag_sides == 3 && up) || (flag_sides == 2 && up)){
            if((y - 9) <= min_y){
              flag_game_over = 1;
                stop_car_1 = 0;
                flag_stop = 1;    
          }
          if(flag_stop != 1){
            for(i=2; i<=8; i++){
              for(j=-2; j<=2; j++){
                if ((ptr[((y - 1)-i)*info.xres_virtual + (x+j)] == color_player_1) ||
                  (ptr[((y - 1)-i)*info.xres_virtual + (x+j)] == color_player_2) ||
                  (ptr[((y - 1)-i)*info.xres_virtual + (x+j)] == color_window)){
                  flag_game_over = 1;
                  stop_car_1 = 0;
                }
              }
            }
        }
        }else if((flag_sides == 0 && left) || (flag_sides == 1 && left)){
          if((x - 9) <= min_x){
              flag_game_over = 1;
                stop_car_1 = 0;
                flag_stop = 1;    
          }
          if(flag_stop != 1){
            for(i=2; i<=8; i++){
              for(j=-2; j<=2; j++){
                if((ptr[(y+j)*info.xres_virtual + ((x-1)-i)] == color_player_1) ||
                  (ptr[(y+j)*info.xres_virtual + ((x-1)-i)] == color_player_2) ||
                  (ptr[(y+j)*info.xres_virtual + ((x-1)-i)] == color_window)){
                  flag_game_over = 1;
                  stop_car_1 = 0;
                }
              }
            }
        }
        }else if((flag_sides == 0 && right) || (flag_sides == 1 && right)){
          if((x+9) > max_x){
            flag_game_over = 1;
                stop_car_1 = 0;
                flag_stop = 1;  
          }
          if((flag_stop != 1) || (flag_sides == 1 && right)){
            for(i=2; i<=8; i++){
              for(j=-2; j<=2; j++){
                if((ptr[(y+j)*info.xres_virtual + ((x+1)+i)] == color_player_1) ||
                  (ptr[(y+j)*info.xres_virtual + ((x+1)+i)] == color_player_2) || 
                  (ptr[(y+j)*info.xres_virtual + ((x+1)+i)] == color_window)){
                  flag_game_over = 1;
                  stop_car_1 = 0;
                }
              }
            }
        } 
        }
        //прорисовка езды
        if(up && (flag_stop == 0)){
          if(flag_sides == 2){
            turn_left(x,y);
            //y -= 2;

          }else if(flag_sides == 3){
            turn_right(x,y);
            ptr[(y-1)*info.xres_virtual + x] = color_player_1;
            ptr[(y-2)*info.xres_virtual + x] = color_player_1;
            //y -= 2;
          }else if(flag_sides == 1){
            //flag_game_over = 1;
          }
          for(i = -2; i < 2;i++){
            if((ptr[(y-9)*info.xres_virtual + (x+i)] == color_window) ||
            (ptr[(y-9)*info.xres_virtual + (x+i)] == color_player_2) ||
            (ptr[(y-9)*info.xres_virtual + (x+i)] == color_player_1)){
              flag_game_over = 1;
              stop_car_1 = 0;
            }
          }
          for(i=1; i<7; i++){
            for(j=-2; j<=2; j++){
              ptr[(y-i)*info.xres_virtual + (x+j)] = 0x00000000;
            }
          }

          for(i=1; i<=8; i++){
            for(j=-2; j<=2; j++){
              ptr[((y-1)-i)*info.xres_virtual + (x+j)] = color_player_1;
            }
          }
          ptr[y*info.xres_virtual + x] = color_player_1;
          ptr[(y-1)*info.xres_virtual + x] = color_player_1;
          //ptr[(y+1)*info.xres_virtual + x] = color_player_1;
          //ptr[(y+2)*info.xres_virtual + x] = color_player_1;
          y--;
          flag_sides = 0;
        }else if(down && (flag_stop == 0)){
          if(flag_sides == 2){
            turn_left(x,y);
           // y += 2;
          }else if(flag_sides == 3){
            turn_right(x,y);
           // y += 2;
          }else if(flag_sides == 0){
            //flag_game_over = 1;
          }
          for(i = -2; i < 2;i++){
            if((ptr[(y+9)*info.xres_virtual + (x+i)] == color_window) ||
            (ptr[(y+9)*info.xres_virtual + (x+i)] == color_player_2) ||
            (ptr[(y+9)*info.xres_virtual + (x+i)] == color_player_1)){
              flag_game_over = 1;
              stop_car_1 = 0;
            }
          }
          for(i=1; i<7; i++){
            for(j=-2; j<=2; j++){
              ptr[(y+i)*info.xres_virtual + (x+j)] = 0x00000000;
            }
          }
          for(i=1; i<=8; i++){
            for(j=-2; j<=2; j++){
              ptr[((y+1)+i)*info.xres_virtual + (x+j)] = color_player_1;
            }
          }
          ptr[y*info.xres_virtual + x] = color_player_1;
          ptr[(y+1)*info.xres_virtual + x] = color_player_1;
          //ptr[(y-1)*info.xres_virtual + x] = color_player_1;
          //ptr[(y-2)*info.xres_virtual + x] = color_player_1;
          y++;
          flag_sides = 1;
        }else if(left && (flag_stop == 0)){
          if(flag_sides == 0){
            turn_up(x,y);
            //x -= 2;
          }else if(flag_sides == 1){
            turn_down(x,y);
            //x -= 2;
          }else if(flag_sides == 3){
            //flag_game_over = 1;
          }
          for(i = -2; i <= 2;i++){
            if((ptr[(y-i)*info.xres_virtual + (x-9)] == color_window) ||
            (ptr[(y-i)*info.xres_virtual + (x-9)] == color_player_2) ||
            (ptr[(y-i)*info.xres_virtual + (x-9)] == color_player_1)){
              flag_game_over = 1;
              stop_car_1 = 0;
            }
          }
          for(i=1; i<7; i++){
            for(j=-2; j<=2; j++){
              ptr[(y+j)*info.xres_virtual + (x-i)] = 0x00000000;
            }
          }
          for(i=1; i<=8; i++){
            for(j=-2; j<=2; j++){
              ptr[(y+j)*info.xres_virtual + ((x-1)-i)] = color_player_1;
            }
          }
          ptr[y*info.xres_virtual + x] = color_player_1;
          ptr[y*info.xres_virtual + (x-1)] = color_player_1;
          //ptr[y*info.xres_virtual + x+1] = color_player_1;
          //ptr[y*info.xres_virtual + x+2] = color_player_1;
          x--;
          flag_sides = 2;
        }else if(right && (flag_stop == 0)){
          if(flag_sides == 0){
            turn_up(x,y);
           // x += 2;
          }else if(flag_sides == 1){
            turn_down(x,y);
           // x += 2;
          }else if(flag_sides == 2){
            //flag_game_over = 1;
          }
          for(i = -2; i <= 2;i++){
            if((ptr[(y-i)*info.xres_virtual + x+9] == color_window) ||
            (ptr[(y-i)*info.xres_virtual + x+9] == color_player_2) ||
            (ptr[(y-i)*info.xres_virtual + x+9] == color_player_1)){
              flag_game_over = 1;
              stop_car_1 = 0;
            }
          }
          for(i=1; i<7; i++){
            for(j=-2; j<=2; j++){
              ptr[(y+j)*info.xres_virtual + (x+i)] = 0x00000000;
            }
          }
          for(i=1; i<=8; i++){
            for(j=-2; j<=2; j++){
              ptr[(y+j)*info.xres_virtual + ((x+1)+i)] = color_player_1;
            }
          }
          ptr[y*info.xres_virtual + x] = color_player_1;
          ptr[y*info.xres_virtual + (x+1)] = color_player_1;
          //ptr[y*info.xres_virtual + (x-1)] = color_player_1;
          //ptr[y*info.xres_virtual + (x-2)] = color_player_1;
          x++;
          flag_sides = 3;
        }
         if((flag_sides_2 == 3 && down_car_2) || (flag_sides_2 == 2 && down_car_2)){
          if((y_car_2 + 9) > max_y){
              flag_won = 1;
                stop_car_1 = 0;
                flag_stop_2 = 1;    
          }
          if(flag_stop_2 != 1){
            for(i=2; i<=8; i++){
              for(j=-2; j<=2; j++){
                if ((ptr[((y_car_2 + 1)+i)*info.xres_virtual + (x_car_2+j)] == color_player_1)||
                  (ptr[((y_car_2 + 1)+i)*info.xres_virtual + (x_car_2+j)] == color_player_2)||
                  (ptr[((y_car_2 + 1)+i)*info.xres_virtual + (x_car_2+j)] == color_window)){
                  flag_won = 1;
                  stop_car_2 = 0;
                }
              }
            }
        }
        }else if((flag_sides_2 == 3 && up_car_2) || (flag_sides_2 == 2 && up_car_2)){
          if((y_car_2 - 9) <= min_y){
              flag_won = 1;
                stop_car_1 = 0;
                flag_stop_2 = 1;    
          }
          if(flag_stop_2 != 1){
            for(i=2; i<=8; i++){
              for(j=-2; j<=2; j++){
                if ((ptr[((y_car_2 - 1)-i)*info.xres_virtual + (x_car_2+j)] == color_player_1) ||
                  (ptr[((y_car_2 - 1)-i)*info.xres_virtual + (x_car_2+j)] == color_player_2) ||
                  (ptr[((y_car_2 - 1)-i)*info.xres_virtual + (x_car_2+j)] == color_window)){
                  flag_won = 1;
                  stop_car_2 = 0;
                }
              }
            }
        }
        }else if((flag_sides_2 == 0 && left_car_2) || (flag_sides_2 == 1 && left_car_2)){
          if((x_car_2 - 9) <= min_x){
              flag_won = 1;
                stop_car_1 = 0;
                flag_stop_2 = 1;    
          }
        if(flag_sides != 1){
          for(i=2; i<=8; i++){
            for(j=-2; j<=2; j++){
              if((ptr[(y_car_2+j)*info.xres_virtual + ((x_car_2-1)-i)] == color_player_1) ||
                (ptr[(y_car_2+j)*info.xres_virtual + ((x_car_2-1)-i)] == color_player_2) ||
                (ptr[(y_car_2+j)*info.xres_virtual + ((x_car_2-1)-i)] == color_window)){
                flag_won = 1;
                stop_car_2 = 0;
              }
            }
          }
      }
        }else if((flag_sides_2 == 0 && right_car_2) || (flag_sides_2 == 1 && right_car_2)){
          if((x_car_2+9) > max_x){
            flag_won = 1;
                stop_car_1 = 0;
                flag_stop_2 = 1;  
          }
          if(flag_stop_2 != 1){
            for(i=2; i<=8; i++){
              for(j=-2; j<=2; j++){
                if((ptr[(y_car_2+j)*info.xres_virtual + ((x_car_2+1)+i)] == color_player_1) ||
                  (ptr[(y_car_2+j)*info.xres_virtual + ((x_car_2+1)+i)] == color_player_2) || 
                  (ptr[(y_car_2+j)*info.xres_virtual + ((x_car_2+1)+i)] == color_window)){
                  flag_won = 1;
                  stop_car_2 = 0;
                }
              }
            }
        }
        }
          if(up_car_2 && (flag_stop_2 == 0)){
            if(flag_sides_2 == 2){
              turn_left_car_2(x_car_2, y_car_2);
            }else if(flag_sides_2 == 3){
              turn_right_car_2(x_car_2, y_car_2);
            }else if(flag_sides_2 == 1){
              flag_won = 1;
            }
            for(i = -2; i <= 2;i++){
              if((ptr[(y_car_2-9)*info.xres_virtual + (x_car_2+i)] == color_window) ||
              (ptr[(y_car_2-9)*info.xres_virtual + (x_car_2+i)] == color_player_2) ||
              (ptr[(y_car_2-9)*info.xres_virtual + (x_car_2+i)] == color_player_1)){
                flag_won = 1;
                stop_car_2 = 0;
              }
            }
            for(i=1; i<7; i++){
              for(j=-2; j<=2; j++){
                ptr[(y_car_2-i)*info.xres_virtual + (x_car_2+j)] = 0x00000000;
              }
            }

            for(i=1; i<=8; i++){
              for(j=-2; j<=2; j++){
                ptr[((y_car_2-1)-i)*info.xres_virtual + (x_car_2+j)] = color_player_2;
              }
            }
            ptr[y_car_2*info.xres_virtual + x_car_2] = color_player_2;
            ptr[(y_car_2-1)*info.xres_virtual + x_car_2] = color_player_2;
            y_car_2--;
            flag_sides_2 = 0;
          }else if(down_car_2 && (flag_stop_2 == 0)){
          if(flag_sides_2 == 2){
            turn_left_car_2(x_car_2,y_car_2);
          }else if(flag_sides_2 == 3){
            turn_right_car_2(x_car_2,y_car_2);
          }else if(flag_sides_2 == 0){
            flag_won = 1;
          }
          for(i = -2; i <= 2;i++){
            if((ptr[(y_car_2+9)*info.xres_virtual + (x_car_2+i)] == color_window) ||
            (ptr[(y_car_2+9)*info.xres_virtual + (x_car_2+i)] == color_player_2) ||
            (ptr[(y_car_2+9)*info.xres_virtual + (x_car_2+i)] == color_player_1)){
              flag_won = 1;
               stop_car_2 = 0;
            }
          }
          for(i=1; i<7; i++){
            for(j=-2; j<=2; j++){
              ptr[(y_car_2+i)*info.xres_virtual + (x_car_2+j)] = 0x00000000;
            }
          }
          for(i=1; i<=8; i++){
            for(j=-2; j<=2; j++){
              ptr[((y_car_2+1)+i)*info.xres_virtual + (x_car_2+j)] = color_player_2;
            }
          }
          ptr[y_car_2*info.xres_virtual + x_car_2] = color_player_2;
          ptr[(y_car_2+1)*info.xres_virtual + x_car_2] = color_player_2;
          y_car_2++;
          flag_sides_2 = 1;
        }else if(left_car_2 && (flag_stop_2 == 0)){
          if(flag_sides_2 == 0){
            turn_up_car_2(x_car_2,y_car_2);
          }else if(flag_sides_2 == 1){
            turn_down_car_2(x_car_2,y_car_2);
          }else if(flag_sides_2 == 3){
            flag_won = 1;
          }
          for(i = -2; i <= 2;i++){
            if((ptr[(y_car_2-i)*info.xres_virtual + (x_car_2-9)] == color_window) ||
            (ptr[(y_car_2-i)*info.xres_virtual + (x_car_2-9)] == color_player_2) ||
            (ptr[(y_car_2-i)*info.xres_virtual + (x_car_2-9)] == color_player_1)){
              flag_won = 1;
              stop_car_2 = 0;
            }
          }
          for(i=1; i<7; i++){
            for(j=-2; j<=2; j++){
              ptr[(y_car_2+j)*info.xres_virtual + (x_car_2-i)] = 0x00000000;
            }
          }
          for(i=1; i<=8; i++){
            for(j=-2; j<=2; j++){
              ptr[(y_car_2+j)*info.xres_virtual + ((x_car_2-1)-i)] = color_player_2;
            }
          }
          ptr[y_car_2*info.xres_virtual + x_car_2] = color_player_2;
          ptr[y_car_2*info.xres_virtual + (x_car_2-1)] = color_player_2;
          x_car_2--;
          flag_sides_2 = 2;
        }else if(right_car_2 && (flag_stop_2 == 0)){
          if(flag_sides_2 == 0){
            turn_up(x_car_2,y_car_2);
          }else if(flag_sides_2 == 1){
            turn_down(x_car_2,y_car_2);
          }else if(flag_sides_2 == 2){
            flag_won = 1;
          }
          for(i = -2; i <= 2;i++){
            if((ptr[(y_car_2-i)*info.xres_virtual + (x_car_2+9)] == color_window) ||
            (ptr[(y_car_2-i)*info.xres_virtual + (x_car_2+9)] == color_player_2) ||
            (ptr[(y_car_2-i)*info.xres_virtual + (x_car_2+9)] == color_player_1)){
              flag_won = 1;
              stop_car_2 = 0;
            }
          }
          for(i=1; i<7; i++){
            for(j=-2; j<=2; j++){
              ptr[(y_car_2+j)*info.xres_virtual + (x_car_2+i)] = 0x00000000;
            }
          }
          for(i=1; i<=8; i++){
            for(j=-2; j<=2; j++){
              ptr[(y_car_2+j)*info.xres_virtual + ((x_car_2+1)+i)] = color_player_2;
            }
          }
          ptr[y_car_2*info.xres_virtual + x_car_2] = color_player_2;
          ptr[y_car_2*info.xres_virtual + (x_car_2+1)] = color_player_2;
          x_car_2++;
          flag_sides_2 = 3;
        }
        ran_up ++; ran_down ++; ran_left ++; ran_right ++;
        ran_up_2 ++; ran_down_2 ++; ran_left_2 ++; ran_right_2 ++; 
        usleep(62500);
      }
    if((flag_sides == 0) && (flag_sides_2 == 1)){
      //mvprintw(0,0,"SYKA            ");
      //refresh();
      //mvprintw(0,0, "%d", (y_car_2-y));
      //refresh();
      //sleep(1);

      for(i = -2; i <= 2; i++){
        for(j = -2; j <= 2; j++){
          if(((y-9) == (y_car_2+9)) && ((x + i) ==(x_car_2 +j))){
            draw();
          }
        }
      }
    }else if((flag_sides == 1) && (flag_sides_2 == 0)){
      for(i = -2; i <= 2; i++){
        for(j = -2; j <= 2; j++){
          if(((y+9) == (y_car_2-9)) && ((x + i) ==(x_car_2 +j))){
            draw();
          }
        }
      }
    }else if((flag_sides == 3) && (flag_sides_2 == 2)){
      for(i = -2; i <= 2; i++){
        for(j = -2; j <= 2; j++){
          if(((y + i) == (y_car_2 + j)) && ((x+9) ==(x_car_2-9))){
            draw();
          }
        }
      }
    }else if((flag_sides == 2) && (flag_sides_2 == 3)){
      for(i = -2; i <= 2; i++){
        for(j = -2; j <= 2; j++){
          if(((y + i) == (y_car_2 + j)) && ((x-9) ==(x_car_2+9))){
            draw();
          }
        }
      }
    }
    if(flag_won && flag_game_over){
          draw();
          //system("clear");
        }else if(flag_won && (flag_game_over == 0)){
          won();
          //system("clear");
        }else if((flag_won == 0) && flag_game_over){
          game_over();
          //system("clear");
        }
        //pthread_mutex_unlock(&mutex);
  }

}
int main(int argc, char *argv[]){
   if(argc < 4 || argc > 4){
    perror("argument error\n");
    return -1;
  }
  char str1[ 255 ];
  int ip_1 = 0;
  int ip_2 = 0;
  char buf_1[BUFLEN];
  char message[BUFLEN];
  strcpy( str1, argv[3] );
  
  window_x = atoi(argv[1]);
  window_y = atoi(argv[2]);

  signal(SIGINT, handler);

  page_size = sysconf(_SC_PAGESIZE);

  if ( 0 > (fb = open("/dev/fb0", O_RDWR))) {
    perror("open");
    return __LINE__;
  }

  if ( (-1) == ioctl(fb, FBIOGET_VSCREENINFO, &info)) {
    perror("ioctl");
    close(fb);
    return __LINE__;
  }
  while((window_x >= info.xres_virtual) || (window_y >= info.yres_virtual)){
    printf("\nerror\n");
  exit(1);
  }

  fb_size = sizeof(uint32_t) * info.xres_virtual * info.yres_virtual;
  printf("%d", info.xres_virtual);
  map_size = (fb_size + (page_size - 1 )) & (~(page_size-1));
  int i = centre();

  ptr = mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, fb, 0);
  if ( MAP_FAILED == ptr ) {
    perror("mmap");
    close(fb);
    return __LINE__;
  }
  initscr();
  noecho();
  keypad(stdscr,TRUE);
  int status;
  int status_addr;
  const char* google_dns_server = "8.8.8.8";
    int dns_port = 53;

    struct sockaddr_in serv;

    int sock = socket ( AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    //Socket could not be created
    if(sock < 0)
    {
        perror("Socket error");
    }

    memset( &serv, 0, sizeof(serv) );
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr( google_dns_server);
    serv.sin_port = htons( dns_port );

    int err = connect( sock , (const struct sockaddr*) &serv , sizeof(serv) );

    struct sockaddr_in name;
    socklen_t namelen = sizeof(name);
    err = getsockname(sock, (struct sockaddr*) &name, &namelen); //получаем адресс которому привязан сокет

    char buffer[100];
    const char* p = inet_ntop(AF_INET, &name.sin_addr, buffer, 100);

    if(p != NULL)
    {
        printf("Local ip is : %s \n" , buffer);
    }
    else
    {
        //Some error
        printf ("Error number : %d . Error message : %s \n" , errno , strerror(errno));
        exit(1);
    }

    close(sock);

  if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
  {
  die("socket");
  }
  memset((char *) &si_me, 0, sizeof(si_me));
 
  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(PORT);
  si_me.sin_addr.s_addr = inet_addr(buffer); // тут мой id
 
  if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
  {
  die("bind");
  }


  memset((char *) &si_other, 0, sizeof(si_other));
  si_other.sin_family = AF_INET;
  si_other.sin_port = htons(PORT);
 
  if (inet_aton(argv[3], &si_other.sin_addr) == 0) { // тут на какой сервер отправляет данные
    fprintf(stderr, "inet_aton() failed\n");
    exit(1);
  }

  pthread_t play;
  pthread_t server;
  strcpy( str1, argv[3] );
  ip_1 = IP_int(str1);
  //strcpy( str1, argv[2] ); // тут с ip сравнение , ну блин начало, а сравнение ниже
  ip_2 = IP_int(buffer);

  if (ip_1 > ip_2){ // если я напутал с ip и кто где стоит, то просто поменяю if!!!

    color_player_1 = 0x1a75b5;
    color_player_2 = 0x800000;
    color_won = 0x1a75b5;
    color_game_over = 0x800000;

    y = centre_y + (window_y / 2) - 3;
    x = centre_x + (window_x / 2) - 1;
    y_car_2 = centre_y - (window_y / 2) + 3;
    x_car_2 = centre_x - (window_x / 2) + 1;
    mvprintw(0, 0, "you play as a blue car");
    refresh();
    pthread_create(&play, NULL, car_1, 2, 3);

  } else if(ip_1 < ip_2){
    y = centre_y - (window_y / 2) + 3;
    x = centre_x - (window_x / 2) + 1;
    y_car_2 = centre_y + (window_y / 2) - 3;
    x_car_2 = centre_x + (window_x / 2) - 1;

    color_player_1 = 0x800000;
    color_player_2 = 0x1a75b5;
    color_won = 0x800000;
    color_game_over = 0x1a75b5;
    mvprintw(0, 0, "you play as a red car");
    refresh();
    pthread_create(&play, NULL, car_1, 3, 2);
    //pthread_create(&opponent, NULL, car_2, 2);
      
    }else if(ip_1 == ip_2){
      mvprintw(0, 0, "you have specified the same IP");
      refresh();
      endwin();
      sleep(3);
      system("clear");
       exit(1);
    }

  pthread_create(&server, NULL, user_2_server, argv);
  //pthread_mutex_init(&mutex, NULL);

  while (work_flag && stop_car_1 && stop_car_2){
    ch = getch();
   switch(ch){
             case 'a':
             start_game_car_1 = 1;
                if((left != 1) && (right != 1)){
                  if(((up == 1) && (ran_up >= 3)) || ((down == 1) && (ran_down >= 3))){
                    //if(up == 1){ran_up = 0;}else if(down ==1){ran_down =0;}
                    buf_1[0] = 'a';
                    left = 1;
                    up = 0;
                    down = 0;
                    right = 0;
                    ran_left = 0;
                    
                  }
                }

                break;
              case 'd':
              start_game_car_1 = 1;
                if((right != 1) && (left != 1)){
                  if(((up == 1) && (ran_up >= 3)) || ((down == 1) && (ran_down >= 3))){
                   // if(up == 1){ran_up = 0;}else if(down ==1){ran_down =0;}
                    buf_1[0] = 'd';
                    right = 1;
                    left = 0;
                    down = 0;
                    up = 0;
                    ran_right = 0;
                    
                }
                }
                break;
              case 's':
              start_game_car_1 = 1;
                if((down != 1) && (up != 1)){
                  if(((left == 1) && (ran_left >= 3)) || ((right == 1) && (ran_right >= 3))){
                    //if(left == 1){ran_left = 0;}else if(right ==1){ran_right =0;}
                    buf_1[0] = 's';
                    down = 1;
                    right = 0;
                    up = 0;
                    left = 0;
                    ran_down = 0;
                    
                  }
                }
               break;
              case 'w':
              start_game_car_1 = 1;
                if((up != 1) && (down != 1)){
                  if(((left == 1) && (ran_left >= 3)) || ((right == 1) && (ran_right >= 3))){
                    //if(left == 1){ran_left = 0;}else if(right ==1){ran_right =0;}
                    buf_1[0] = 'w';
                    up = 1;
                    left = 0;
                    right = 0;
                    down = 0;
                    ran_up = 0;
                  }
                }
                break;
               default:
                if (ch != 0){
                  start_game_car_1 = 1;
                  buf_1[0] = 'h';
                 break;
                }
    }
    if (sendto(s, buf_1, strlen(buf_1) , 0, (struct sockaddr *) &si_other, slen)==-1){
                       die("sendto()");
                    }
  }
  //pthread_mutex_destroy(&mutex);
  endwin();

  munmap(ptr, map_size);
  close(fb);
  pthread_join(play, (void**)&status_addr);
  pthread_join(server, (void**)&status_addr);
  system("clear");
  return 0;
}
