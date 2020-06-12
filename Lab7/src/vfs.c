#include "include/vfs.h"
#include "include/string.h"
#include "include/mm.h"
#include "include/uart.h"
#include "include/kernel.h"
#include "include/fs/tmpfs.h"

void set_dentry(struct dentry *dentry,struct vnode* vnode,\
                 const char* str){
         
	 dentry->child_count = 0; 
	 dentry->vnode = vnode; 
         strcpy(dentry->dname , str);
}


void rootfs_init(){
  // setting file system for root fs
  	struct filesystem *fs = (struct filesystem*)kmalloc(sizeof(struct filesystem));
  	fs->name = "tmpfs";
  	fs->setup_mount = setup_mount_tmpfs;
  
  	register_filesystem(fs);  
  
  	struct vnode *vnode = (struct vnode*)kmalloc(sizeof(struct vnode));	
  	set_tmpfs_vnode(vnode);

  	struct dentry *dentry=(struct dentry*)kmalloc(sizeof(struct dentry));
  	set_dentry(dentry,vnode,"/");
	dentry->flag = ROOT_DIR;

  	// setup root file sysystem
  	struct mount *mt = (struct mount*)kmalloc(sizeof(struct mount));
  	mt->fs= fs;
  	mt->root = vnode;
  	mt->dentry = dentry;
  	rootfs= mt; 

        //setting current working directory	
	current_dent = dentry;
}


int register_filesystem(struct filesystem* fs) {
  // register the file system to the kernel.
  // you can also initialize memory pool of the file system here.
  if(strcmp(fs->name,"tmpfs")==0){	
	// init vops and fops
	tmpfs_v_ops = (struct vnode_operations*)kmalloc(sizeof(struct vnode_operations));
	tmpfs_f_ops = (struct file_operations*)kmalloc(sizeof(struct file_operations));
	
	tmpfs_v_ops->lookup = lookup_tmpfs;
        tmpfs_v_ops->create = create_tmpfs;
	tmpfs_v_ops->mkdir = mkdir_tmpfs;
	tmpfs_v_ops->ls = ls_tmpfs;

        tmpfs_f_ops->write = write_tmpfs;
        tmpfs_f_ops->read = read_tmpfs;

  	return 0;
  }
  return -1;
}


int parsing(char* component_name, struct dentry** dent,const char* pathname){
  //special case 
  if(strcmp(pathname,"/")==0){
	strcpy(component_name,"/");
	return 1;
  }


  // brute force parsing the pathname
  int pathname_count=0;
  int name_count = 0;

  while(pathname[pathname_count]!='\0'){

	if(pathname[pathname_count]=='/'){
		if(pathname_count!=0){
			component_name[name_count] = '\0';
			name_count = 0;
			int i = 0;
			int child_count = (*dent)->child_count;
			
			if(strcmp(component_name,".")==0){
				//do nothing	
			}
			else if(strcmp(component_name,"..")==0){
				if((*dent)->flag == DIRECTORY)
					*dent = (*dent)->parent_dentry;
				else
					printf("INVALID PATHNAME!!\r\n");
			}
			else{
				for(;i<child_count;i++){
                  			if(strcmp((*dent)->child_dentry[i]->dname, component_name)==0){
                          			*dent = (*dent)->child_dentry[i];
						break;
					}
				}
				// if can't find
				if(i>=child_count){
			 		printf("### DIRECTORY '%s' NOT FOUND!!\r\n",component_name);
					return -1;
				}
			}

                  }
		  else{ // If path start with '/', then look up starts at root directory 
		  	printf("!!! parsing start from root\r\n");
			*dent = rootfs->dentry;
		  }
        }
	else{
		component_name[name_count++] = pathname[pathname_count];
	}
	pathname_count++;
  }
  component_name[name_count] = '\0';
  return 0;
} 

struct file* vfs_open(const char* pathname, int flags) {
  char component_name[DNAME_LEN];
  struct dentry *dent = current_dent; //start from root dentry
  int parse_ret = parsing(component_name,&dent,pathname); 
  if(parse_ret ==-1) //directory not found
	  return (struct file*)NULL;
  else if(parse_ret == 1){ //special case for 'ls'
	 rootfs->root->v_ops->ls(dent);
  	 return (struct file*)NULL;
  }
  
  if(strcmp(component_name,".")==0 || strcmp(component_name,"..")==0){
  	printf("INVALID FILE NAME!\r\n");
	return (struct file*)NULL;
  }


  // Then open the file 
  if(flags == O_CREAT){
	struct vnode* target;
	// Since we don't implement directory in root now
	// Just naive find/create file from root entry
	int ret = rootfs->root->v_ops->lookup(dent,&target,component_name); 
  	if(ret == -1){
		rootfs->root->v_ops->create(dent,&target,component_name);	
	}
	else{
		printf("### TRY TO CREATE FILE '%s' BUT EXIST!!\r\n",pathname);
	}
	struct file* fd = (struct file*)kmalloc(sizeof(struct file));
	fd->vnode = target;
	fd->f_ops = target->f_ops;
	fd->f_pos = 0;
	return fd;
  }
  else{
	struct vnode* target;
	int ret = rootfs->root->v_ops->lookup(dent,&target,component_name); 
        
	if(ret == -1){
		if (strcmp(pathname,"/")!=0)
			printf("### FILE NOT FOUND!\r\n");
  		return (struct file*)NULL;
	}
	printf("### Found File %s\r\n",pathname);
	struct file* fd = (struct file*)kmalloc(sizeof(struct file));
	fd->vnode = target;
	fd->f_ops = target->f_ops;
	fd->f_pos = 0;
	return fd;
  }
  
}

int vfs_close(struct file* file){
	printf("### Close file at %x\r\n",file);
	kfree((unsigned long)file);
	return 0;
}

int vfs_write(struct file* file, const void* buf, size_t len){
	return file->f_ops->write(file,buf,len);
}

int vfs_read(struct file* file, void* buf, size_t len){
	return file->f_ops->read(file,buf,len);
}

int vfs_mkdir(const char* pathname){
  
  char component_name[DNAME_LEN];
  struct dentry *dent = current_dent; //start from root dentry
 
  int parse_ret = parsing(component_name,&dent,pathname); 
  if(parse_ret ==-1) //directory not found
	  return -1;
  else if(parse_ret == 1){ //special case for 'ls'
	 rootfs->root->v_ops->ls(dent);
  	 return -1;
  }
  
  if(strcmp(component_name,".")==0 || strcmp(component_name,"..")==0){
  	printf("INVALID DIRECTORY NAME\r\n");
  	return -1;
  }

  struct vnode* target;
  int ret = rootfs->root->v_ops->lookup(dent,&target,component_name);
  if(ret == -1){
  	ret = rootfs->root->v_ops->mkdir(dent,&target,component_name);
  	return ret;
  }
  else{
  	printf("### ERROR! directory already exist\r\n");
  	return -1;
  }
}

int vfs_chdir(const char* pathname){
	char component_name[DNAME_LEN];
	struct dentry *dent = current_dent; //start from root dentry

	int parse_ret = parsing(component_name,&dent,pathname);
	
	if(parse_ret ==-1) //directory not found
		return -1;
	
  	if(strcmp(component_name,".")==0){
		return 0; //stay
	}  
	else if(strcmp(component_name,"..")==0){
		if(current_dent->flag==DIRECTORY){
			current_dent = dent->parent_dentry;
			return 0;
		}
		else{
			printf("CHANGE DIRECTORY FAILED!!\r\n");
			return -1;
		}
	}
	
		
	int child_count = dent->child_count;
	for(int i=0;i<child_count;i++){
                if(strcmp(dent->child_dentry[i]->dname, component_name)==0 &&\
				dent->child_dentry[i]->flag==DIRECTORY){
			current_dent = dent->child_dentry[i];
			printf("### Now current directory %s\r\n",current_dent->dname);
			return 0;
		}
	}
	
	printf("CHANGE DIRECTORY FAILED!!\r\n");
	return -1;
}
