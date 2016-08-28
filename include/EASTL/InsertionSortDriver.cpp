#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <assert.h>
typedef unsigned long ulong;
typedef int32_t i32;

#ifdef EASTL_VALIDATE_COMPARE
    #undef EASTL_VALIDATE_COMPARE
    #define EASTL_VALIDATE_COMPARE(x) ((void)0)
#endif // EASTL_VALIDATE_COMPARE

#include <EASTL/sort.h>

const auto Less=[](i32 a, i32 b){ return a<b; };

enum:size_t{OneMil=10000000u};

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wincrypt.h>
bool FillRandom(void* a, size_t bytes)
{
    HCRYPTPROV   hCryptProv;

    if (CryptAcquireContext(&hCryptProv, nullptr, nullptr, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
    {
        const bool ret=CryptGenRandom(hCryptProv, bytes, static_cast<BYTE*>(a));

        puts(ret ? "Random sequence successfully generated" : "Error during CryptGenRandom");

        return CryptReleaseContext(hCryptProv, 0) ? (puts("Crypt handle released"), ret) : (puts("Crypt handle failed to release"), false);
    }
    else
    {
        puts("A cryptographic service handle could not be acquired");
        return false;
    }
}
#else
#include <fcntl.h>
#include <unistd.h>
bool FillRandom(void* a, size_t bytes)
{
    const int fd=open("/dev/random", O_RDONLY);
    if (fd>=0)
    {
        const ptrdiff_t nRead=read(fd, a, bytes);
        bool ret = (nRead<0 || size_t(nRead)<bytes) ? (perror("read"), false) : true;
        if (close(fd)!=0) perror("close"), ret=false;
        return ret;
    }
    else
    {
        perror("open() failed on /dev/random");
        return false;
    }
}
#endif//WIN32

void TestNewInsertionSort(const int NumTests=30)
{
    enum:size_t{SmallArraySize=20u, N=OneMil};

    static i32 data[N];

    FILE* TempDataFile=tmpfile();
    if (TempDataFile==NULL)
    {
        perror("couldn't create temp file");
        return;
    }

    if (!FillRandom(data, sizeof data)) { return; }
    if (fwrite(data, 4, N, TempDataFile)!=N) { perror("fwrite"); return; }

    int observe=0;
    clock_t start, stop, oldSum=0, newSum=0;
    i32* it=NULL;
    for (int loops=NumTests; loops-- > 0; )
    {
        //testing current version
        rewind(TempDataFile);
        if (fread(data, 4, N, TempDataFile)!=N) { perror("fread"); return; }

        start=clock();
        //test many small arrays instead of a big one, or will take a while
        it=data;
        for (size_t j=SmallArraySize; j<=N; it+=SmallArraySize, j+=SmallArraySize)
        {
            eastl::insertion_sort(it, data+j);
            //assert(eastl::is_sorted(it, data+j));
            observe ^= *it;
        }
        stop=clock();
        oldSum += stop-start;

        //testing new version
        rewind(TempDataFile);
        if (fread(data, 4, N, TempDataFile)!=N) { perror("fread"); return; }

        start=clock();
        //test many small arrays instead of a big one, or will take a while
        it=data;
        for (size_t j=SmallArraySize; j<=N; it+=SmallArraySize, j+=SmallArraySize)
        {
            eastl::insertion_sort_new(it, data+j);
            //assert(eastl::is_sorted(it, data+j));
            observe ^= *it;
        }
        stop=clock();
        newSum += stop-start;
    }

    printf("Old isort took %ld clocks.\n", (long)oldSum);
    printf("New isort took %ld clocks.\n", (long)newSum);
    printf("something observable: %d\n", observe);
    getchar();
}

//lots of repetition here...
void TestNewMergeSortBuffer(const int NumTests=30)
{
    enum:size_t{N=OneMil};

    static i32 data[N];
    static i32 buf_full[N];
    static i32 buf_half[N/2u];//new version needs half the space

    FILE* TempDataFile=tmpfile();
    if (TempDataFile==NULL)
    {
        perror("couldn't create temp file");
        return;
    }

    if (!FillRandom(data, sizeof data)) { return; }
    if (fwrite(data, 4, N, TempDataFile)!=N) { perror("fwrite"); return; }

    int observe=0;
    clock_t start, stop, oldSum=0, newSum=0;
    for (int loops=NumTests; loops-- > 0; )
    {
        //testing current version
        rewind(TempDataFile);
        if (fread(data, 4, N, TempDataFile)!=N) { perror("fread"); return; }

        start=clock();
        eastl::merge_sort_buffer(data, data+N, buf_full, Less);
        stop=clock();
        oldSum += stop-start;
        //assert(eastl::is_sorted(data, data+N));

        //testing new version
        rewind(TempDataFile);
        if (fread(data, 4, N, TempDataFile)!=N) { perror("fread"); return; }

        start=clock();
        eastl::merge_sort_buffer_new(data, N, buf_half, Less);
        stop=clock();
        newSum += stop-start;
        //assert(eastl::is_sorted(data, data+N));
    }

    printf("Old msort_buf took %ld clocks.\n", (long)oldSum);
    printf("New msort_buf took %ld clocks.\n", (long)newSum);
    printf("something observable: %d\n", observe);
    getchar();
}

int main()
{
    //TestNewInsertionSort();
    TestNewMergeSortBuffer(8);

    return 0;
}
