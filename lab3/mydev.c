    #include <linux/kernel.h> 
    #include <linux/module.h> 
    #include <linux/fs.h> 
    #include <linux/init.h> 
    #include <linux/uaccess.h> 
     
    #if CONFIG_MODVERSIONS == 1 
    #define MODVERSIONS 
    #include <linux/version.h> 
    #endif 
     
    #define DEVICE_NUM 0 //希望获得的设备号 
     
    static int device_num = 0;  //用来保存创建成功后的设备号 
    static char buffer[1024] = "mydev_gyt";   //数据缓冲区,初始设置为     
    static int open_nr = 0;     //打开设备的进程数，用于内核的互斥 
     
    /*在设备驱动程序中有一个非常重要的结构file_operations，该结构的每个域都对应着一个系统调用。
     *用户进程利用系统调用对设备文件进行诸如read/write操作时，系统调用
     *通过设备文件的主设备号找到相应的设备驱动程序，然后读取这个数据结构相应的函数
     *指针，接着把控制权交给该函数。
    
    struct file_operations { 
    int (*seek) (struct inode * ，struct file *， off_t ，int);  
    --int (*read) (struct inode * ，struct file *， char ，int);  
    --int (*write) (struct inode * ，struct file *， off_t ，int);  
    int (*readdir) (struct inode * ，struct file *， struct dirent * ，int);  
    int (*select) (struct inode * ，struct file *， int ，select_table *);  
    int (*ioctl) (struct inode * ，struct file *， unsined int ，unsigned long);  
    int (*mmap) (struct inode * ，struct file *， struct vm_area_struct *);  
    --int (*open) (struct inode * ，struct file *);  
    --int (*release) (struct inode * ，struct file *);  
    int (*fsync) (struct inode * ，struct file *);  
    int (*fasync) (struct inode * ，struct file *，int);  
    int (*check_media_change) (struct inode * ，struct file *);  
    int (*revalidate) (dev_t dev);  
    } 
    
    *编写设备驱动程序的主要工作是编写子函数，并填充file_operations的各个域。
    */
    
    //函数声明 
    static int my_open(struct inode *inode, struct file *filp); 
    static int my_release(struct inode *inode, struct file* filp); 
    static ssize_t my_read(struct file *file, char __user *buf, size_t count, loff_t *f_pos); 
    static ssize_t my_write(struct file *file, const char __user *buf, size_t count, loff_t *f_pos); 
     
    //填充file_operations结构相关入口 
    static struct file_operations mydev_fops = { 
        .read    = my_read, 
        .write   = my_write, 
        .open    = my_open, 
        .release = my_release, 
    };    
     
    //打开函数 
    static int my_open(struct inode *inode, struct file *filp) 
    { 
        printk("\nMain device is %d, and the slave device is %d\n", MAJOR(inode->i_rdev), MINOR(inode->i_rdev)); 
        if(open_nr == 0){ 
            open_nr++; 
            try_module_get(THIS_MODULE); 
            return 0; 
        }  
        else{ 
            printk(KERN_ALERT "Another process open the char device.\n");//进程挂起 
            return -1; 
        }  
    } 
    
    /*
    *由于内核空间与用户空间的内存不能直接互访，因此借助函数来实现
    *函数copy_to_user()完成用户空间到内核空间的复制
    *函数copy_from_user()完成内核空间到用户空间的复制
    */

    //读函数 
    static ssize_t my_read(struct file *file, char __user *buf, size_t count, loff_t *f_pos) 
    { 
        //if (buf == NULL) return 0; 
        if (copy_to_user(buf, buffer, sizeof(buffer))) //读缓冲，buffer->buf 
        { 
            return -1; 
        } 
        return sizeof(buffer); 
    } 
     
    //写函数,将用户的输入字符串写入 
    static ssize_t my_write(struct file *file, const char __user *buf, size_t count, loff_t *f_pos) 
    { 
        //if (buf == NULL) return 0; 
        if (copy_from_user(buffer, buf, sizeof(buffer))) //写缓冲 ,buf->buffer
        { 
            return -1; 
        } 
        return sizeof(buffer); 
    } 
     
    //释放设备函数 
    static int my_release(struct inode *inode, struct file* filp) 
    { 
        open_nr--; //进程数减1 
        printk("The device is released!\n"); 
        module_put(THIS_MODULE); 
        return 0; 
    } 

    //chrdev:字符设备
    //blkdev:块设备

    /*
    *register_chrdev(希望获得的设备号,设备名,登记驱动程序实际执行操作的函数的指针)
    *注册字符设备
    */

    //注册设备函数 
    static int __init my_init(void) 
    { 
        int result; 
     
        printk(KERN_ALERT "Begin to init Char Device!"); //注册设备 
        //向系统的字符登记表登记一个字符设备 
        result = register_chrdev(DEVICE_NUM, "mydev", &mydev_fops); 
     
        if (result < 0) { 
            printk(KERN_WARNING "mydev: register failure\n"); 
            return -1; 
        } 
        else { 
            printk("mydev: register success!\n"); 
            device_num = result; 
            return 0; 
        } 
    } 

    /*
    *unregister_chrdev(主设备号,设备名)
    *卸载字符设备
    */

    //注销设备函数 
    static void __exit my_exit(void) 
    { 
        printk(KERN_ALERT "Unloading...\n"); 
        unregister_chrdev(device_num, "mydev"); //注销设备
        printk("unregister success!\n"); 
    } 
     
    //模块宏定义 
    module_init(my_init); 
    module_exit(my_exit); 

    //模块的许可证声明 
    MODULE_LICENSE("GPL"); 

