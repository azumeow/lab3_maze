#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <signal.h>

char *box_1[4] = {"10301","10302","10303","10304"};
int box_2[4] = {10301,10302,10303,10304};
int box_3[4] = {84,1680,102*101,102*101};
int row[4] = {7,21,101,101};
int colume[4] = {12,80,102,102};  //多出來的是為了\n
char path[100];

int findShortestPathLength(char maze[][102], int ROW, int COL, int start_x, int start_y, int dest_x, int dest_y);
void findShortestPath(char maze[][102], int visit[][102], int ROW, int COL, int now_x, int now_y, int dest_x, int dest_y, int *min_dist, int dist);
int isSafe(char maze[][102], int visit[][102], int ROW, int COL, int now_x, int now_y);
int main(int argc, char **argv){
    int count;
    int port;
    int ROW;
    int COL;
    for(int i=0;i<4;i++){
        if(strcmp(argv[3],box_1[i]) == 0){
            port = box_2[i];
            count = box_3[i];
            ROW = row[i];
            COL = colume[i];
            break;
        }
    }
    printf("port: %d\n",port);
    printf("count: %d\n",count);
    printf("ROW: %d\n",ROW);
    printf("COL: %d\n",COL);
    //receive data
    char title[50];
    char resc_data[4096];
    char maze[101][102]; //for \n
    memset(resc_data,0,sizeof(resc_data));
    memset(title,0,sizeof(title));
    //create a socket
    int clientsocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientsocket == -1){
        perror("無法創建clientsocket\n");
        exit(1);
    }

    //server address and port
    struct sockaddr_in serveraddr;
    socklen_t server_len;
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;  //ipv4
    serveraddr.sin_port = htons(port);
    //put server's name into server's ip address
    if (inet_pton(AF_INET, "140.113.213.213", &(serveraddr.sin_addr)) <= 0) {
        printf("Invalid server address.\n");
        return 1;
    }
    
    //tcp需要connect的動作
    if (connect(clientsocket, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) == -1){
        perror("連上server失敗\n");
        exit(1);
    }

    //receive data from server
    recv(clientsocket, title, sizeof(title),0);
    printf("%s",title);
    recv(clientsocket, resc_data, sizeof(resc_data),0);
    printf("%s",resc_data);
    recv(clientsocket, resc_data, sizeof(resc_data),0);
    printf("%s",resc_data);
    
    /*char *result = strstr(resc_data,"#");
    if ( result != NULL)
    {
        int k = result - resc_data;

            for (int i = 0; i < ROW; i++) {
                for (int j = 0; j < COL; j++){
                    if(count <= 0)
                        break;
                    maze[i][j] = resc_data[k];
                    printf("%c", maze[i][j]);
                    k++;
                    count--;
                }
                if(count<=0)
                    break;
            }
    }
    printf("\n");
    printf("count: %d\n",count);
    //find *
    int start_x = 0; //x is row
    int start_y = 0; //y is colume
    for (int i = 0; i < ROW; i++){
        for (int j = 0; j < COL; j++)
            if(maze[i][j] == '*')
            {
                start_x = i;
                start_y = j;
                break;
            }
        if (start_x != 0 && start_y != 0){
            printf("找到起點了\n");
            printf("start_x: %d\n",start_x);
            printf("start_y: %d\n",start_y);
            break;
        }
    }

    //find dest
    int dest_x = 0; //x is row
    int dest_y = 0; //y is colume
    for (int i = 0; i < ROW; i++){
        for (int j = 0; j < COL; j++)
            if(maze[i][j] == 'E')
            {
                dest_x = i;
                dest_y = j;
                break;
            }
        if (dest_x != 0 && dest_y != 0){
            printf("找到終點了\n");
            printf("dest_x: %d\n",dest_x);
            printf("dest_y: %d\n",dest_y);
            break;
        }
    }

    int min_dist = findShortestPathLength(maze,ROW,COL,start_x,start_y,dest_x,dest_y);
    printf("min_dist: %d\n",min_dist);*/
    close(clientsocket);
    return 0;
}
int findShortestPathLength(char maze[][102], int ROW, int COL, int start_x, int start_y, int dest_x, int dest_y){
    int visit[101][102]; //檢查visit, 0 represents no visit
    memset(visit,0,sizeof(visit));
    
    int min_dist = INT_MAX; //最短距離
    findShortestPath(maze,visit,ROW,COL,start_x,start_y,dest_x,dest_y,&min_dist,0);
    if (min_dist != INT_MAX) {
        return min_dist;
    }

    printf("出現錯誤");
    return -1;
}
void findShortestPath(char maze[][102], int visit[][102], int ROW, int COL, int now_x, int now_y, int dest_x, int dest_y, int *min_dist, int dist){
    // 如果找到目的地，更新 `min_dist`
    if (now_x == dest_x && now_y == dest_y)
    {
        if (dist < *min_dist)
            *min_dist = dist;
        return;
    }

    // 將目前位置設置為已訪問
    visit[now_x][now_y] = 1;

    // 往下一格
    if (isSafe(maze,visit,ROW,COL,now_x+1,now_y)) {
        findShortestPath(maze,visit,ROW,COL,now_x+1,now_y,dest_x,dest_y,min_dist,dist+1);
   
    }

    // 往上一格
    if (isSafe(maze,visit,ROW,COL,now_x-1,now_y)) {
        findShortestPath(maze,visit,ROW,COL,now_x-1,now_y,dest_x,dest_y,min_dist,dist+1);

    }

    // 往左一格
    if (isSafe(maze,visit,ROW,COL,now_x,now_y-1)) {
        findShortestPath(maze,visit,ROW,COL,now_x,now_y-1,dest_x,dest_y,min_dist,dist+1);
      
    }

    // 往右一格
    if (isSafe(maze,visit,ROW,COL,now_x,now_y+1)) {
        findShortestPath(maze,visit,ROW,COL,now_x,now_y+1,dest_x,dest_y,min_dist,dist+1);
     
    }

    //back
    visit[now_x][now_y] = 0;
}
int isSafe(char maze[][102], int visit[][102], int ROW, int COL, int now_x, int now_y){
    if (maze[now_x][now_y] == '#' || visit[now_x][now_y] == 1)
        return 0;
    return 1;
}