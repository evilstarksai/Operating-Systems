#include<ppipe.h>
#include<context.h>
#include<memory.h>
#include<lib.h>
#include<entry.h>
#include<file.h>


// Per process information for the ppipe.
struct ppipe_info_per_process {

    // TODO:: Add members as per your need...
    
    // Just store read end for every process because write end is same for all processes.
    int readend;               // read end index for the process with pid in this struct 
    int buffer_length;         // buffer length that process can read

    u32 pid;                   // pid of the process
    int t_write;               // 1 if write end is open else 0
    int t_read;                // 1 if read end is open else 0

};

// Global information for the ppipe.
struct ppipe_info_global {

    char *ppipe_buff;       // Persistent pipe buffer: DO NOT MODIFY THIS.

    // TODO:: Add members as per your need...
    int min_readend;        // Minimum of all the readends
    int writeend;           // Common write end to all the processes
    int max_buffer_length;  // Maximum buffer length
    int n_proc;             // Number of processes with atleast 1 end open

};

// Persistent pipe structure.
// NOTE: DO NOT MODIFY THIS STRUCTURE.
struct ppipe_info {

    struct ppipe_info_per_process ppipe_per_proc [MAX_PPIPE_PROC];
    struct ppipe_info_global ppipe_global;

};


// Function to allocate space for the ppipe and initialize its members.
struct ppipe_info* alloc_ppipe_info() {

    // Allocate space for ppipe structure and ppipe buffer.
    struct ppipe_info *ppipe = (struct ppipe_info*)os_page_alloc(OS_DS_REG);
    char* buffer = (char*) os_page_alloc(OS_DS_REG);

    // Assign ppipe buffer.
    ppipe->ppipe_global.ppipe_buff = buffer;

    /**
     *  TODO:: Initializing pipe fields
     *
     *  Initialize per process fields for this ppipe.
     *  Initialize global fields for this ppipe.
     *
     */ 

    for(int i=1;i<MAX_PPIPE_PROC;i++){
        ppipe->ppipe_per_proc[i].pid = 0;
        ppipe->ppipe_per_proc[i].readend = 0;
        ppipe->ppipe_per_proc[i].buffer_length = 0;
        ppipe->ppipe_per_proc[i].t_read   = 0;
        ppipe->ppipe_per_proc[i].t_write  = 0;
    }

    ppipe->ppipe_global.min_readend   =  0;
    ppipe->ppipe_global.writeend      =  0;
    ppipe->ppipe_global.max_buffer_length =  0;
    ppipe->ppipe_global.n_proc = 1;

    ppipe->ppipe_per_proc[0].t_read   = 1;
    ppipe->ppipe_per_proc[0].t_write  = 1;
    ppipe->ppipe_per_proc[0].pid      = get_current_ctx()->pid;
    ppipe->ppipe_per_proc[0].readend = 0;
    ppipe->ppipe_per_proc[0].buffer_length = 0;

    // Return the ppipe.
    return ppipe;

}

// Function to free ppipe buffer and ppipe info object.
// NOTE: DO NOT MODIFY THIS FUNCTION.
void free_ppipe (struct file *filep) {

    os_page_free(OS_DS_REG, filep->ppipe->ppipe_global.ppipe_buff);
    os_page_free(OS_DS_REG, filep->ppipe);

} 

// Fork handler for ppipe.
int do_ppipe_fork (struct exec_context *child, struct file *filep) {
    
    /**
     *  TODO:: Implementation for fork handler
     *
     *  You may need to update some per process or global info for the ppipe.
     *  This handler will be called twice since ppipe has 2 file objects.
     *  Also consider the limit on no of processes a ppipe can have.
     *  Return 0 on success.
     *  Incase of any error return -EOTHERS.
     *
     */

    // Error handling
    if(filep == NULL || filep->type != PPIPE || filep->mode == O_RDWR || filep->mode == O_EXEC || filep->mode == O_CREAT){
        return -EOTHERS;
    }

    if(filep->ref_count == MAX_PPIPE_PROC){
        return -EOTHERS;
    }

    // Update per process info in the pipe
    for(int i=0;i<MAX_PPIPE_PROC;i++){
        // If the other end was already called with fork
        if(filep->ppipe->ppipe_per_proc[i].pid == child->pid){
            //filep->ppipe->ppipe_per_proc[i].pid = child->pid;
            if(filep->mode == O_WRITE){
                filep->ppipe->ppipe_per_proc[i].t_write = 1;
                filep->ppipe->ppipe_global.n_proc += 1;
            }
            if(filep->mode == O_READ){
                filep->ppipe->ppipe_per_proc[i].t_read = 1;
                filep->ppipe->ppipe_global.n_proc += 1;
            }

            if(filep->ppipe->ppipe_per_proc[i].t_write == 1 && filep->ppipe->ppipe_per_proc[i].t_read == 1){
                filep->ppipe->ppipe_global.n_proc -= 1;
                //filep->ref_count += 1;
            }

            return 0;
        }
    }

    for(int i=0;i<MAX_PPIPE_PROC;i++){
        if(filep->ppipe->ppipe_per_proc[i].pid == 0){
            filep->ppipe->ppipe_per_proc[i].pid = child->pid;
            if(filep->mode == O_WRITE){
                filep->ppipe->ppipe_per_proc[i].t_write = 1;
                filep->ppipe->ppipe_global.n_proc += 1;
            }
            if(filep->mode == O_READ){
                filep->ppipe->ppipe_per_proc[i].t_read = 1;
                filep->ppipe->ppipe_global.n_proc += 1;
            }

            /*if(filep->ppipe->ppipe_per_proc[i].t_write == 1 && filep->ppipe->ppipe_per_proc[i].t_read == 1){
                filep->ppipe->ppipe_global.n_proc -= 1;
                //filep->ref_count += 1;
            }*/

            // finding the readend of the parent process
            for(int j=0;j<MAX_PPIPE_PROC;j++){
                if(child->ppid == filep->ppipe->ppipe_per_proc[j].pid){
                    filep->ppipe->ppipe_per_proc[i].readend = filep->ppipe->ppipe_per_proc[j].readend;
                    filep->ppipe->ppipe_per_proc[i].buffer_length = filep->ppipe->ppipe_per_proc[j].buffer_length;
                    break;
                }
            }
            break;
        }
    }


    // Return successfully.
    return 0;

}


// Function to close the ppipe ends and free the ppipe when necessary.
long ppipe_close (struct file *filep) {

    /**
     *  TODO:: Implementation of Pipe Close
     *
     *  Close the read or write end of the ppipe depending upon the file
     *      object's mode.
     *  You may need to update some per process or global info for the ppipe.
     *  Use free_ppipe() function to free ppipe buffer and ppipe object,
     *      whenever applicable.
     *  After successful close, it return 0.
     *  Incase of any error return -EOTHERS.
     *                                                                          
     */

    int ret_value;

    if(filep == NULL){
        // If we get a NULL argument then raise an error
        return -EOTHERS;
    }

    if(filep->type != PPIPE){
        // If it is not of a ppipe then raise an error
        return -EOTHERS;
    }

    struct exec_context* current_cntxt = get_current_ctx();
    int current_pid = current_cntxt->pid;

    if(filep->mode == O_WRITE){
        
        int nprocesses = -1;               // Number of processes in which read end or write end is open

        // Close the write end of the pipe
        for(int i=0;i<MAX_OPEN_FILES;i++){
            if(current_cntxt->files[i] == filep){
                current_cntxt->files[i] = NULL;
                break;
            }
        }

        // Remove the process pid from pipe_info of the file if bothe ends are closed
        for(int i=0;i<MAX_PPIPE_PROC;i++){
            if(filep->ppipe->ppipe_per_proc[i].pid == current_pid){
                // printk("s\n");
                filep->ppipe->ppipe_per_proc[i].t_write = 0;
                //found_read_end = filep->ppipe->ppipe_per_proc[i].t_read;
                if(filep->ppipe->ppipe_per_proc[i].t_read == 0){
                    filep->ppipe->ppipe_per_proc[i].pid = 0;
                    filep->ppipe->ppipe_per_proc[i].buffer_length = 0;
                    filep->ppipe->ppipe_per_proc[i].readend = 0;
                    filep->ppipe->ppipe_global.n_proc -= 1;
                }
                nprocesses = filep->ppipe->ppipe_global.n_proc;
                //printk("2- %d\n",nprocesses);
                break;
            }
        }

        // If both ends of pipe are closed and the pipe is in a single process then call free_ppipe()
        if(nprocesses == 0){
             //printk("ssiv1-%d",nprocesses);
             free_ppipe(filep);
        }
    }

    else if(filep->mode == O_READ){

        int nprocesses = -1;                 // Number of processes in which read end is open

        // Close the read end of the ppipe
        for(int i=0;i<MAX_OPEN_FILES;i++){
            if(current_cntxt->files[i] == filep){
                current_cntxt->files[i] = NULL;
                break;
            }
        }

        // Remove the process pid from pipe_info of the file and find found_write_end
        for(int i=0;i<MAX_PPIPE_PROC;i++){
            if(filep->ppipe->ppipe_per_proc[i].pid == current_pid){
                // printk("%d\n",filep->ppipe->ppipe_global.n_proc);
                filep->ppipe->ppipe_per_proc[i].t_read = 0;
                //found_write_end = filep->ppipe->ppipe_per_proc[i].t_write;
                if(filep->ppipe->ppipe_per_proc[i].t_write == 0){
                    filep->ppipe->ppipe_per_proc[i].pid = 0;
                    filep->ppipe->ppipe_per_proc[i].buffer_length = 0;
                    filep->ppipe->ppipe_per_proc[i].readend = 0;
                    filep->ppipe->ppipe_global.n_proc -= 1;
                }
                nprocesses = filep->ppipe->ppipe_global.n_proc;
                //printk("1- %d\n",filep->ppipe->ppipe_global.n_proc);
                break;
            }
        }

        // If both ends of pipe are closed and the pipe is in a single process then call free_pipe()
        if(nprocesses == 0){
            
            // printk("ssiv2-%d",nprocesses); // printk("s\n");
            free_ppipe(filep);
        }
    }

    else{
        // a pipe's end can only have one of the above 2 permissions. If not raise an error.
        return -EOTHERS;
    }

    // Close the file.
    ret_value = file_close (filep);         // DO NOT MODIFY THIS LINE.

    // And return.
    return ret_value;

}

// Function to perform flush operation on ppipe.
int do_flush_ppipe (struct file *filep) {

    /**
     *  TODO:: Implementation of Flush system call
     *
     *  Reclaim the region of the persistent pipe which has been read by 
     *      all the processes.
     *  Return no of reclaimed bytes.
     *  In case of any error return -EOTHERS.
     *
     */

    int reclaimed_bytes = 0;

    if(filep == NULL || filep -> ppipe == NULL || filep-> ppipe -> ppipe_per_proc == NULL){
        return -EOTHERS;
    }

    int write_end = filep->ppipe->ppipe_global.writeend;
    int min_read_end = 2*MAX_PPIPE_SIZE;
    for(int i=0;i<MAX_PPIPE_PROC;i++){
        if(filep->ppipe->ppipe_per_proc[i].pid != 0 && filep->ppipe->ppipe_per_proc[i].t_read == 1){
            if(filep->ppipe->ppipe_per_proc[i].readend > write_end){
                if(min_read_end > (filep->ppipe->ppipe_per_proc[i].readend)){
                    min_read_end = filep->ppipe->ppipe_per_proc[i].readend;
                }
            }
            else if(filep->ppipe->ppipe_per_proc[i].readend < write_end){
                if(min_read_end > (MAX_PPIPE_SIZE + filep->ppipe->ppipe_per_proc[i].readend)){
                    min_read_end = MAX_PPIPE_SIZE + filep->ppipe->ppipe_per_proc[i].readend;
                }
            }
            else{
                if(filep->ppipe->ppipe_per_proc[i].buffer_length == MAX_PPIPE_SIZE){
                    min_read_end = write_end;
                }
            }
        }
    }

    if(min_read_end == 2*MAX_PPIPE_SIZE){
        reclaimed_bytes = filep->ppipe->ppipe_global.max_buffer_length;
        filep->ppipe->ppipe_global.max_buffer_length = 0;
        filep->ppipe->ppipe_global.min_readend       = write_end;
    }

    else{
        reclaimed_bytes = (min_read_end - filep->ppipe->ppipe_global.min_readend) % MAX_PPIPE_SIZE;
        filep->ppipe->ppipe_global.min_readend = min_read_end % MAX_PPIPE_SIZE;
        filep->ppipe->ppipe_global.max_buffer_length -= reclaimed_bytes;
    }

    // Return reclaimed bytes.
    return reclaimed_bytes;

}

// Read handler for the ppipe.
int ppipe_read (struct file *filep, char *buff, u32 count) {
    
    /**
     *  TODO:: Implementation of PPipe Read
     *
     *  Read the data from ppipe buffer and write to the provided buffer.
     *  If count is greater than the present data size in the ppipe then just read
     *      that much data.
     *  Validate file object's access right.
     *  On successful read, return no of bytes read.
     *  Incase of Error return valid error code.
     *      -EACCES: In case access is not valid.
     *      -EINVAL: If read end is already closed.
     *      -EOTHERS: For any other errors.
     *
     */

    int bytes_read = 0;

    if(filep == NULL){
        return -EOTHERS;
    }

    if(filep->mode == O_WRITE || filep->mode == O_EXEC || filep->mode == O_CREAT){
        // If there is no read mode
        return -EACCES;
    }

    if(filep->ppipe == NULL || filep->type != PPIPE){
        return -EOTHERS;
    }

    // Check if read end is closed or not
    int read_end_close = 1;      // 1 if read end is closed.
    struct exec_context* current_ctx = get_current_ctx();
    for(int i=0;i<MAX_OPEN_FILES;i++){
        if(current_ctx->files[i] == filep){
            read_end_close = 0;
            break;
        }
    }
    if(read_end_close == 1){
        return -EINVAL;
    }

    // Find the index of pipe_per_proc with this process pid
    int index_pipe_proc = 0;
    for(int i=0;i<MAX_PPIPE_PROC;i++){
        if(filep->ppipe->ppipe_per_proc[i].pid == current_ctx->pid){
            index_pipe_proc = i;
            break;
        }
    }

    // buffer length that can be read at max
    int buff_length = filep->ppipe->ppipe_per_proc[index_pipe_proc].buffer_length;

    if(buff_length==0){
        // Case in which there is no buffer
        bytes_read = 0;
    }

    // Need to read the whole buff_length
    else if(buff_length<=count){
        // Get the present read end of the pipe
        int read_end=filep->ppipe->ppipe_per_proc[index_pipe_proc].readend;

        // Fill the pipe buffer into the user buffer
        int i;
        for(i=0;i<buff_length;i++){
            buff[i]=filep->ppipe->ppipe_global.ppipe_buff[(read_end+i)%MAX_PPIPE_SIZE];
        }

        // Update the read end and buffer length of the pipe
        filep->ppipe->ppipe_per_proc[index_pipe_proc].readend = (read_end+i)%MAX_PPIPE_SIZE;
        filep->ppipe->ppipe_per_proc[index_pipe_proc].buffer_length = 0;

        bytes_read = buff_length;
    }

    else{
        // Get the present read end of the pipe
        int read_end = filep->ppipe->ppipe_per_proc[index_pipe_proc].readend;

        // Fill the pipe buffer into the user buffer
        int i;
        for(i=0;i<count;i++){
            buff[i]=filep->ppipe->ppipe_global.ppipe_buff[(read_end+i)%MAX_PPIPE_SIZE];
        }
 
        // Update the read end and buffer length of the pipe
        filep->ppipe->ppipe_per_proc[index_pipe_proc].readend = (read_end+i)%MAX_PPIPE_SIZE;
        filep->ppipe->ppipe_per_proc[index_pipe_proc].buffer_length = buff_length-count;

        bytes_read = count;
    }

    // Return no of bytes read.
    return bytes_read;
	
}

// Write handler for ppipe.
int ppipe_write (struct file *filep, char *buff, u32 count) {

    /**
     *  TODO:: Implementation of PPipe Write
     *
     *  Write the data from the provided buffer to the ppipe buffer.
     *  If count is greater than available space in the ppipe then just write
     *      data that fits in that space.
     *  Validate file object's access right.
     *  On successful write, return no of written bytes.
     *  Incase of Error return valid error code.
     *      -EACCES: In case access is not valid.
     *      -EINVAL: If write end is already closed.
     *      -EOTHERS: For any other errors.
     *
     */

    int bytes_written = 0;

    if(filep == NULL){
        return -EOTHERS;
    }

    if(filep->mode == O_READ || filep->mode == O_EXEC || filep->mode == O_CREAT){
        // If there is no write mode
        return -EACCES;
    }

    if(filep->ppipe == NULL || filep->type != PPIPE){
        // Error when it is not a ppipe
        return -EOTHERS;
    }

    // Check if write end is closed or not
    int write_end_close = 1;      // 1 if write end is closed.
    struct exec_context* current_cntxt = get_current_ctx();
    for(int i=0;i<MAX_OPEN_FILES;i++){
        if(current_cntxt->files[i] == filep){
            write_end_close = 0;
            break;
        }
    }
    if(write_end_close == 1){
        return -EINVAL;
    }
 
    // Store the length of available space in the pipe buffer.
    int buff_length_avai = MAX_PPIPE_SIZE - filep->ppipe->ppipe_global.max_buffer_length;

    if(buff_length_avai == 0 || count == 0){
        bytes_written = 0;
    }

    else if(buff_length_avai <= count){
        // Get the present write end
        int write_end=filep->ppipe->ppipe_global.writeend;

        // Fill the user buffer in the pipe buffer
        for(int i=0;i<buff_length_avai;i++){
            filep->ppipe->ppipe_global.ppipe_buff[(write_end+i)%MAX_PPIPE_SIZE] = buff[i];
        }
 
        // Update write end and buffer lengths
        filep->ppipe->ppipe_global.writeend = filep->ppipe->ppipe_global.min_readend;
        filep->ppipe->ppipe_global.max_buffer_length = MAX_PPIPE_SIZE;
        for(int i=0;i<MAX_PPIPE_PROC;i++){
            if(filep->ppipe->ppipe_per_proc[i].pid != 0){
                filep->ppipe->ppipe_per_proc[i].buffer_length += buff_length_avai;
            }
        }

        bytes_written = buff_length_avai;
    }

    else{
        // Get the present write end
        int write_end=filep->ppipe->ppipe_global.writeend;
        
        // Fill the user buffer in the pipe buffer
        int i;
        for(i=0;i<count;i++){
            filep->ppipe->ppipe_global.ppipe_buff[(write_end+i)%MAX_PPIPE_SIZE] = buff[i];
        }
        
        // Update write end and buffer lengths
        filep->ppipe->ppipe_global.writeend = (write_end+i)%MAX_PPIPE_SIZE;
        filep->ppipe->ppipe_global.max_buffer_length = filep->ppipe->ppipe_global.max_buffer_length + count;
        for(int i=0;i<MAX_PPIPE_PROC;i++){
            if(filep->ppipe->ppipe_per_proc[i].pid != 0){
                filep->ppipe->ppipe_per_proc[i].buffer_length += count;
            }
        }

        bytes_written = count;
    }

    // Return no of bytes written.
    return bytes_written;

}

// Function to create persistent pipe.
int create_persistent_pipe (struct exec_context *current, int *fd) {

    /**
     *  TODO:: Implementation of PPipe Create
     *
     *  Find two free file descriptors.
     *  Create two file objects for both ends by invoking the alloc_file() function.
     *  Create ppipe_info object by invoking the alloc_ppipe_info() function and
     *      fill per process and global info fields.
     *  Fill the fields for those file objects like type, fops, etc.
     *  Fill the valid file descriptor in *fd param.
     *  On success, return 0.
     *  Incase of Error return valid Error code.
     *      -ENOMEM: If memory is not enough.
     *      -EOTHERS: Some other errors.
     *
     */

    // Finding the file descriptors from current->files.
    int file_descriptors_index=0;
    int index[2];
    for(int i=0;i<MAX_OPEN_FILES && file_descriptors_index<2;i++){
        if(current->files[i]==NULL){
            current->files[i]=alloc_file();

            // Error when alloc_file() returns NULL
            if(current->files[i] == NULL){
                return -ENOMEM;
            }

            index[file_descriptors_index] = i;
            file_descriptors_index++;
        }
    }

    // Error when there is no memory
    if(file_descriptors_index<2){
        return -ENOMEM;
    }

    // Creating pipe_info object
    struct ppipe_info* ppipe=alloc_ppipe_info();
    if(ppipe == NULL){
        return -ENOMEM;
    }
       
    // printk("%d\n",ppipe->ppipe_global.n_proc);

    // Filling the fields of file objects
    current->files[index[0]]->type        = PPIPE;
    current->files[index[0]]->ppipe       = ppipe;
    current->files[index[0]]->pipe        = NULL;
    current->files[index[0]]->fops->read  = ppipe_read;
    current->files[index[0]]->fops->write = NULL;
    current->files[index[0]]->fops->close = ppipe_close;

    current->files[index[1]]->type        = PPIPE;
    current->files[index[1]]->ppipe       = ppipe;
    current->files[index[1]]->pipe        = NULL;
    current->files[index[1]]->fops->read  = NULL;
    current->files[index[1]]->fops->write = ppipe_write;
    current->files[index[1]]->fops->close = ppipe_close;    
    
    current->files[index[0]]->mode       = O_READ;
    current->files[index[1]]->mode       = O_WRITE;

    // Filling valid file descriptor in *fd
    fd[0]=index[0];
    fd[1]=index[1];

    // Simple return.
    return 0;

}
