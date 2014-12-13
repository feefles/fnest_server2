//mmap.c

#include "mmap.h" //shared idae of what the shared structure is


void exitError(const char *errmesg)
{
    exit(EXIT_FAILURE);
}
mmap_Data *p_mmapData;

void setupFile()
{
    int mmap_fileno = open(mmapFilePath, O_CREAT | O_RDWR, S_IRUSR |
                           S_IWUSR);
    p_mmapData = (mmap_Data *)mmap(NULL, sizeof(mmap_Data),
                                   PROT_READ | PROT_WRITE,
                                   MAP_SHARED,
                                   mmap_fileno, 0);
}

void tearDown()
{
    msync(p_mmapData, sizeof(mmap_Data), MS_INVALIDATE);
}

void change_set_temp(int temp)
{
    setupFile();
    p_mmapData->set_temp = temp;
}



int get_cur_temp() {
    setupFile();
    return p_mmapData->cur_temp;
}

int get_set_temp() {
    setupFile();
    return p_mmapData->set_temp;
}
