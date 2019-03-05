#include <stdio.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

static GtkWidget *clocker;
static int n=0,i=1;//累加
static int begin=0;//数字循环从0开始

gint clock_time(){//时钟函数
    time_t now;
    struct tm *ltime;
    gchar buf[50];
    now=time((time_t*)NULL);
    ltime=localtime(&now);
    sprintf(buf,"%02d:%02d:%02d",ltime->tm_hour,ltime->tm_min,ltime->tm_sec);
    gtk_label_set_text(GTK_LABEL(clocker),buf);
    return TRUE;//返回TRUE可以一直显示时间
}
gint sum(gpointer label){//求和函数
    char buf[50];
    n+=i++;
    sprintf(buf,"Till %d, Sum = %d",i-1,n);
    gtk_label_set_text(GTK_LABEL(label),buf);
    if(i>100)return FALSE;//大于100时停止累加
    return TRUE;
}
gint numCicle(gpointer label){//0-9数字循环
    char buf[10];
    sprintf(buf,"%d",begin++);
    begin%=10;
    gtk_label_set_text(GTK_LABEL(label),buf);
    return TRUE;
}
// destroy a window 一个回调函数
void destroy(GtkWidget *widget, gpointer data)
{
	gtk_main_quit();
}

void initProcWind(int argc,char **argv,const char *title)
{
    //GtkWidget 是构件的存储类型
    GtkWidget *window;//申明一个窗口
    GtkWidget *vbox;
    GtkWidget *hbox;
    GtkWidget *label;

	gtk_init(&argc, &argv);//这个函数在所有的GTK程序都要调用，参数由命令行中解析出来并且送到该程序中

    //Creat a new window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);//创建一个窗口
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(destroy),NULL);
    gtk_window_set_title(GTK_WINDOW(window),title);//设置窗口的标题
    gtk_window_set_default_size(GTK_WINDOW(window) , 200 , 200);//设置窗口默认大小为长宽各200像素
    
    //容器
    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    hbox = gtk_vbox_new(FALSE, 0);
    //gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
	//gtk_widget_show(vbox);
	//createProgressBar(vbox, FALSE, FALSE);
    if(strcmp("Time",title)==0){
        //时钟
        gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,5);
        clocker=gtk_label_new(NULL);
        gtk_box_pack_start(GTK_BOX(hbox),clocker,TRUE,TRUE,5);
        g_timeout_add(1000,(GSourceFunc)clock_time,NULL);//设置为1000，精确到秒
        clock_time();
    }else if(strcmp("Add",title)==0){
        gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,5);
        label=gtk_label_new(NULL);
        gtk_box_pack_start(GTK_BOX(hbox),label,TRUE,TRUE,5);
        g_timeout_add(250,(GSourceFunc)sum,gpointer(label));//设置为1000，精确到秒,250
        //sum(gpointer(label));
    }else if(strcmp("Num",title)==0){
        gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,5);
        label=gtk_label_new(NULL);
        gtk_box_pack_start(GTK_BOX(hbox),label,TRUE,TRUE,5);
        g_timeout_add(500,(GSourceFunc)numCicle,gpointer(label));//设置为1000，精确到秒,500
        //numCicle(gpointer(label));
    }

    gtk_widget_show_all(window);//显示新创建的窗口
    gtk_main();//所有的 GTK 程序必须有一个 gtk_main() 函数。程序运行停在这里等待事件 (如键盘事件或鼠标事件) 的发生
}


int main(int argc , char **argv) //主函数
{
    pid_t pid_1,pid_2,pid_3;
    while((pid_1=fork())==-1);
    if(pid_1==0) initProcWind(argc,argv,"Time");
    else{
        while((pid_2=fork())==-1);
        if(pid_2==0)initProcWind(argc,argv,"Num");
        else{
            while((pid_3=fork())==-1);
            if(pid_3==0)initProcWind(argc,argv,"Add");
        }
    }
    wait((void*)pid_1);
    wait((void*)pid_2);
    wait((void*)pid_3);
    return 0;
}