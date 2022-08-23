#include<pipe.h>
#include<context.h>
#include<memory.h>
#include<lib.h>
#include<entry.h>
#include<file.h>


// Per process info for the pipe.
struct pipe_info_per_process {

    // TODO:: Add members as per your need...
    u32 pid;
    int t_write;        // t_write is 1 if write end of the pipe is open in the given process else 0
    int t_read;         // t_read is 1 if read end of the pipe is open in the given process else 0
};

// Global information for the pipe.
struct pipe_info_global {

    char *pipe_buff;    // Pipe buffer: DO NOT MODIFY THIS.

    // TODO:: Add members as per your need...
    int readend;        // location of Reading end of the pipe
    int writeend;       // location of writing end of the pipe
    int buffer_length;
    int n_proc;
    //int n_proc;         // number of processes containing this pipe
};

// Pipe information structure.
// NOTE: DO NOT MODIFY THIS STRUCTURE.
struct pipe_info {

    struct pipe_info_per_process pipe_per_proc [MAX_PIPE_PROC];
    struct pipe_info_global pipe_global;

};


// Function to allocate space for the pipe and initialize its members.
struct pipe_info* alloc_pipe_info () {
	
    // Allocate space for pipe structure and pipe buffer.
    struct pipe_info *pipe = (struct pipe_info*)os_page_alloc(OS_DS_REG);
    char* buffer = (char*) os_page_alloc(OS_DS_REG);

    // Assign pipe buffer.
    pipe->pipe_global.pipe_buff = buffer;

    /**
     *  TODO:: Initializing pipe fields
     *  
     *  Initialize per process fields for this pipe.
     *  Initialize global fields for this pipe.
     *
     */
    
    //pipe->pipe_global.n_proc  =  1;

    pipe->pipe_per_proc[0].t_read  = 1;
    pipe->pipe_per_proc[0].t_write = 1;
    pipe->pipe_per_proc[0].pid     = get_current_ctx()->pid;

    for(int i=1;i<MAX_PIPE_PROC;i++){
        pipe->pipe_per_proc[i].pid = 0;
    }

    pipe->pipe_global.readend       =  0;
    pipe->pipe_global.writeend      =  0;
    pipe->pipe_global.buffer_length =  0;
    pipe->pipe_global.n_proc        =  1;

    // Return the pipe.
    return pipe;

}

// Function to free pipe buffer and pipe info object.
// NOTE: DO NOT MODIFY THIS FUNCTION.
void free_pipe (struct file *filep) {

    os_page_free(OS_DS_REG, filep->pipe->pipe_global.pipe_buff);
    os_page_free(OS_DS_REG, filep->pipe);

}

// Fork handler for the pipe.
int do_pipe_fork (struct exec_context *child, struct file *filep) {

    /**
     *  TODO:: Implementation for fork handler
     *
     *  You may need to update some per process or global info for the pipe.
     *  This handler will be called twice since pipe has 2 file objects.
     *  Also consider the limit on no of processes a pipe can have.
     *  Return 0 on success.
     *  Incase of any error return -EOTHERS.
     *
     */

    // Error handling
    if(filep == NULL || filep->type != PIPE || filep->mode == O_RDWR || filep->mode == O_EXEC || filep->mode == O_CREAT){
        return -EOTHERS;
    }

    if(filep->ref_count == MAX_PIPE_PROC){
        return -EOTHERS;
    }

    // Update per process info in the pipe
    for(int i=0;i<MAX_PIPE_PROC;i++){
        if(filep->pipe->pipe_per_proc[i].pid == child->pid){
            filep->pipe->pipe_per_proc[i].pid = child->pid;
            if(filep->mode == O_WRITE){
                filep->pipe->pipe_per_proc[i].t_write = 1;
                filep->pipe->pipe_global.n_proc += 1;
            }
            if(filep->mode == O_READ){
                filep->pipe->pipe_per_proc[i].t_read = 1;
                filep->pipe->pipe_global.n_proc += 1;
            }
            if(filep->pipe->pipe_per_proc[i].t_read == 1 && filep->pipe->pipe_per_proc[i].t_write == 1){
                filep->pipe->pipe_global.n_proc -= 1;
                //filep->ref_count += 1;
            }
            return 0;
        }
    }

    for(int i=0;i<MAX_PIPE_PROC;i++){
        if(filep->pipe->pipe_per_proc[i].pid == 0){
            filep->pipe->pipe_per_proc[i].pid = child->pid;
            if(filep->mode == O_WRITE){
                filep->pipe->pipe_per_proc[i].t_write = 1;
                filep->pipe->pipe_global.n_proc += 1;
            }
            if(filep->mode == O_READ){
                filep->pipe->pipe_per_proc[i].t_read = 1;
                filep->pipe->pipe_global.n_proc += 1;
            }
            return 0;
        }
    }

    // Return successfully.
    return 0;

}

// Function to close the pipe ends and free the pipe when necessary.
long pipe_close (struct file *filep) {

    /**
     *  TODO:: Implementation of Pipe Close
     *
     *  Close the read or write end of the pipe depending upon the file
     *      object's mode.
     *  You may need to update some per process or global info for the pipe.
     *  Use free_pipe() function to free pipe buffer and pipe object,
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

    if(filep->type != PIPE){
        // If it is not of a pipe then raise an error
        return -EOTHERS;
    }

    struct exec_context* current_cntxt = get_current_ctx();
    int current_pid = current_cntxt->pid;

    if(filep->mode == O_WRITE){

        int nprocesses = -1;          // Number of processes in which read end or write end is opened.

        // Close the write end of the pipe
        for(int i=0;i<MAX_OPEN_FILES;i++){
            if(current_cntxt->files[i] == filep){
                current_cntxt->files[i] = NULL;
                break;
            }
            /*if(current_cntxt->files[i] != NULL && filep->pipe == current_cntxt->files[i]->pipe){
                found_read_end=1;
            }*/
        }

        // Remove the process pid from pipe_info of the file and find found_read_end
        for(int i=0;i<MAX_PIPE_PROC;i++){
            if(filep->pipe->pipe_per_proc[i].pid == current_pid){
                filep->pipe->pipe_per_proc[i].t_write = 0;
                if(filep->pipe->pipe_per_proc[i].t_read == 0){
                    filep->pipe->pipe_per_proc[i].pid = 0;
                    filep->pipe->pipe_global.n_proc -= 1;
                }
                nprocesses = filep->pipe->pipe_global.n_proc;
                break;
            }
        }

        // If both ends of pipe are closed and the pipe is in a single process then call free_pipe()
        if(nprocesses == 0){
            free_pipe(filep);
        }
    }

    else if(filep->mode == O_READ){

        int nprocesses = -1;

        // Close the read end of the pipe
        for(int i=0;i<MAX_OPEN_FILES;i++){
            if(current_cntxt->files[i] == filep){
                current_cntxt->files[i] = NULL;
                break;
            }
        }

        // Remove the process pid from pipe_info of the file and find found_write_end
        for(int i=0;i<MAX_PIPE_PROC;i++){
            if(filep->pipe->pipe_per_proc[i].pid == current_pid){
                filep->pipe->pipe_per_proc[i].t_read = 0;
                if(filep->pipe->pipe_per_proc[i].t_write == 0){
                    filep->pipe->pipe_per_proc[i].pid = 0;
                    filep->pipe->pipe_global.n_proc -= 1;
                }
                nprocesses = filep->pipe->pipe_global.n_proc;
                break;
            }
        }

        // If both ends of pipe are closed and the pipe is in a single process then call free_pipe()
        if(nprocesses == 0){
            free_pipe(filep);
        }
    }

    else{
        // a pipe's end can only have one of the above 2 permissions. If not raise an error.
        return -EOTHERS;
    }


    // Close the file and return.
    ret_value = file_close (filep);         // DO NOT MODIFY THIS LINE.

    // And return.
    return ret_value;

}

// Check whether passed buffer is valid memory location for read or write.
int is_valid_mem_range (unsigned long buff, u32 count, int access_bit) {

    /**
     *  TODO:: Implementation for buffer memory range checking
     *
     *  Check whether passed memory range is suitable for read or write.
     *  If access_bit == 1, then it is asking to check read permission.
     *  If access_bit == 2, then it is asking to check write permission.
     *  If range is valid then return 1.
     *  Incase range is not valid or have some permission issue return -EBADMEM.
     *
     */

    int ret_value = -EBADMEM;

    if(access_bit != 1 && access_bit != 2){
        // Invalid bit
        return ret_value;
    }

    struct exec_context* current_ctx = get_current_ctx();

    int bit;
    for(int i=0;i<MAX_MM_SEGS;i++){
        // Check between start and next_free for non-stack regions
        if(buff >= current_ctx->mms[i].start && (buff + count -1) <= current_ctx->mms[i].next_free){
            if((current_ctx->mms[i].access_flags) & access_bit == access_bit){
                ret_value = 1;
            }
        }

        // For stack regions
        else if(i == MAX_MM_SEGS-1 && buff >= current_ctx->mms[i].start && (buff + count -1) <= current_ctx->mms[i].end){
            if((current_ctx->mms[i].access_flags) & access_bit == access_bit){
                ret_value = 1;
            }
        }
    }

    struct vm_area* vm = current_ctx->vm_area;
    while(vm != NULL){
        if(buff >= vm->vm_start && (buff + count -1) <= vm->vm_end){
            if((vm->access_flags) & access_bit == access_bit){
                ret_value = 1;
            }
        }
        vm = vm->vm_next;
    }

    // Return the finding.
    return ret_value;

}

// Function to read given no of bytes from the pipe.
int pipe_read (struct file *filep, char *buff, u32 count) {

    /**
     *  TODO:: Implementation of Pipe Read
     *
     *  Read the data from pipe buffer and write to the provided buffer.
     *  If count is greater than the present data size in the pipe then just read
     *       that much data.
     *  Validate file object's access right.
     *  On successful read, return no of bytes read.
     *  Incase of Error return valid error code.
     *       -EACCES: In case access is not valid.
     *       -EINVAL: If read end is already closed.
     *       -EOTHERS: For any other errors.
     *
     */

    int bytes_read = 0;

    if(filep == NULL){
        return EOTHERS;
    }

    if(filep->mode == O_WRITE || filep->mode == O_EXEC || filep->mode == O_CREAT){
        // If there is no read mode
        return -EACCES;
    }

    if(filep->pipe == NULL || filep->type != PIPE){
        return -EOTHERS;
    }

    // Check if read end is closed or not
    int read_end_close = 1;      // 1 if read end is closed.
    struct exec_context* current_cntxt = get_current_ctx();
    for(int i=0;i<MAX_OPEN_FILES;i++){
        if(current_cntxt->files[i] == filep){
            read_end_close = 0;
            break;
        }
    }
    if(read_end_close == 1){
        return -EINVAL;
    }

    int buff_length = filep->pipe->pipe_global.buffer_length;

    if(buff_length==0){
        // Case in which there is no buffer
        bytes_read = 0;
    }

    else if(buff_length <= count){
        // Invalid access
        if(is_valid_mem_range((unsigned long)buff,buff_length,2) != 1){
            return -EACCES;
        }

        // Get the present read end of the pipe
        int read_end=filep->pipe->pipe_global.readend;

        // Fill the pipe buffer into the user buffer
        for(int i=0;i<buff_length;i++){
            buff[i]=filep->pipe->pipe_global.pipe_buff[(read_end+i)%MAX_PIPE_SIZE];
        }

        // Update the read end, write end and buffer length of the pipe
        filep->pipe->pipe_global.readend = 0;
        filep->pipe->pipe_global.writeend = 0;
        filep->pipe->pipe_global.buffer_length = 0;

        bytes_read = buff_length;
    }

    else{
        // Invalid access
        if(is_valid_mem_range((unsigned long)buff,count,2) != 1){
            return -EACCES;
        }

        // Get the present read end of the pipe
        int read_end = filep->pipe->pipe_global.readend;

        // Fill the pipe buffer into the user buffer
        int i;
        for(i=0;i<count;i++){
            buff[i]=filep->pipe->pipe_global.pipe_buff[(read_end+i)%MAX_PIPE_SIZE];
        }
 
        // Update the read end and buffer length of the pipe
        filep->pipe->pipe_global.readend = (read_end+i)%MAX_PIPE_SIZE;
        filep->pipe->pipe_global.buffer_length = buff_length-count;

        bytes_read = count;
    }
        

    // Return no of bytes read.
    return bytes_read;

}

// Function to write given no of bytes to the pipe.
int pipe_write (struct file *filep, char *buff, u32 count) {

    /**
     *  TODO:: Implementation of Pipe Write
     *
     *  Write the data from the provided buffer to the pipe buffer.
     *  If count is greater than available space in the pipe then just write data
     *       that fits in that space.
     *  Validate file object's access right.
     *  On successful write, return no of written bytes.
     *  Incase of Error return valid error code.
     *       -EACCES: In case access is not valid.
     *       -EINVAL: If write end is already closed.
     *       -EOTHERS: For any other errors.
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

    if(filep->pipe == NULL || filep->type != PIPE){
        // Error when it is not a pipe
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
    int buff_length_avai = MAX_PIPE_SIZE - filep->pipe->pipe_global.buffer_length;

    if(buff_length_avai == 0 || count == 0){
        bytes_written = 0;
    }

    else if(buff_length_avai <= count){
        // Invalid access
        if(is_valid_mem_range((unsigned long)buff,buff_length_avai,1) != 1)return -EACCES;

        // Get the present write end
        int write_end=filep->pipe->pipe_global.writeend;

        // Fill the user buffer in the pipe buffer
        for(int i=0;i<buff_length_avai;i++){
            filep->pipe->pipe_global.pipe_buff[(write_end+i)%MAX_PIPE_SIZE] = buff[i];
        }
 
        // Update write end and buffer lengths
        filep->pipe->pipe_global.writeend = filep->pipe->pipe_global.readend;
        filep->pipe->pipe_global.buffer_length = MAX_PIPE_SIZE;

        bytes_written = buff_length_avai;
    }

    else{
        // Invalid access
        if(is_valid_mem_range((unsigned long)buff,count,1) != 1)return -EACCES;

        // Get the present write end
        int write_end=filep->pipe->pipe_global.writeend;
        
        // Fill the user buffer in the pipe buffer
        int i;
        for(i=0;i<count;i++){
            filep->pipe->pipe_global.pipe_buff[(write_end+i)%MAX_PIPE_SIZE] = buff[i];
        }
        
        // Update write end and buffer lengths
        filep->pipe->pipe_global.writeend = (write_end+i)%MAX_PIPE_SIZE;
        filep->pipe->pipe_global.buffer_length = filep->pipe->pipe_global.buffer_length + count;

        bytes_written = count;
    }

    // Return no of bytes written.
    return bytes_written;

}

// Function to create pipe.
int create_pipe (struct exec_context *current, int *fd) {

    /**
     *  TODO:: Implementation of Pipe Create
     *
     *  Find two free file descriptors.
     *  Create two file objects for both ends by invoking the alloc_file() function. 
     *  Create pipe_info object by invoking the alloc_pipe_info() function and
     *       fill per process and global info fields.
     *  Fill the fields for those file objects like type, fops, etc.
     *  Fill the valid file descriptor in *fd param.
     *  On success, return 0.
     *  Incase of Error return valid Error code.
     *       -ENOMEM: If memory is not enough.
     *       -EOTHERS: Some other errors.
     *
     */
    
    // Finding the file descriptors from current->files.
    int file_descriptors_index=0;
    int index[2];
    for(int i=0;i<MAX_OPEN_FILES && file_descriptors_index<2;i++){
        if(current->files[i]==NULL){
            current->files[i]=alloc_file();

            // Error when alloc_file() returns NULL
            if(current->files[i]==NULL){
                return -EOTHERS;
            }

            index[file_descriptors_index]=i;
            file_descriptors_index++;
        }
    }

    // Error when there is no memory
    if(file_descriptors_index<2){
        return -ENOMEM;
    }

    // Creating pipe_info object
    struct pipe_info* pipe=alloc_pipe_info();

    // Filling the fields of file objects
    for(int i=0;i<2;i++){
        current->files[index[i]]->type        = PIPE;
        current->files[index[i]]->pipe        = pipe;
        current->files[index[i]]->ppipe       = NULL;
        current->files[index[i]]->fops->read  = &pipe_read;
        current->files[index[i]]->fops->write = &pipe_write;
        current->files[index[i]]->fops->close = &pipe_close;

    }
    current->files[index[0]]->mode       = O_READ;
    current->files[index[1]]->mode       = O_WRITE;

    // Filling valid file descriptor in *fd
    fd[0]=index[0];
    fd[1]=index[1];


    // Simple return.
    return 0;

}
