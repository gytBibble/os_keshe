asmlinkage long sys_mysyscall(char *srcfile, char *tgtfile){
	int source=sys_open(srcfile,O_RDONLY,0);
	int target=sys_open(tgtfile,O_WRONLY|O_CREAT|O_TRUNC,0600);
	char buf[1024];
	mm_segment_t fs=get_fs();
	set_fs(get_ds());
	int i;
	if(source>0&&target>0){
		do{
			i=sys_read(source,buf,1024);
			sys_write(target,buf,i);
		}while(i);
	}
	else printk("Error");
	sys_close(source);
	sys_close(target);
	set_fs(fs);
	return 1;
}
