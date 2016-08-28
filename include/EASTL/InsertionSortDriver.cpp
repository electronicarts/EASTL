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

enum:size_t{TenMil=1000u*1000u*10u};

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

bool Read(const char* srcFile, void* destArray, size_t bytes)
{
    if (FILE* const fp=fopen(srcFile, "rb"))
    {
        const size_t cnt=fread(destArray, 1, bytes, fp);
        bool ret;

        if (cnt==bytes)
            printf("All %lu bytes were read from \"%s\"\n", (ulong)bytes, srcFile),
            ret=true;
        else
            fprintf(stderr, "Error: only %lu of requested %lu bytes were read from \"%s\"\n", (ulong)cnt, (ulong)bytes, srcFile),
            perror("info"),
            ret=false;

        if (fclose(fp)!=0)
            perror("fclose"),
            ret=false;

        return ret;
    }
    else
    {
        perror("fopen");
        return false;
    }
}

bool Write(const char* destFile, const void* srcArray, size_t bytes)
{
    if (FILE* const fp=fopen(destFile, "wb"))
    {
        const size_t cnt=fwrite(srcArray, 1, bytes, fp);
        bool ret;

        if (cnt==bytes)
            printf("All %lu bytes were written to \"%s\"\n", (ulong)bytes, destFile),
            ret=true;
        else
            fprintf(stderr, "Error: only %lu of requested %lu bytes were written to \"%s\"\n", (ulong)cnt, (ulong)bytes, destFile),
            perror("info"),
            ret=false;

        if (fclose(fp)!=0)
            perror("fclose"),
            ret=false;

        return ret;
    }
    else
    {
        perror("fopen");
        return false;
    }
}

void TestInsertionSorts(i32* a, size_t n, size_t smallArrayN=20)
{
    clock_t start=clock();
    i32* iter=a;
    i32 observe=0;
    //test many small or may take a while
    for (size_t j=smallArrayN; j<=n; iter+=smallArrayN, j+=smallArrayN)
    {
        eastl::insertion_sort(iter, a+j);
        //eastl::insertion_sort_new(iter, a+j);

        //eastl::insertion_sort(iter, a+j, Less);
        //eastl::insertion_sort_new(iter, a+j, Less);

        //assert(eastl::is_sorted(iter, a+j));

        observe ^= *iter;
    }
    clock_t duration=clock()-start;

    printf("took %ld clocks\nsomething observable: %d\n", long(duration), int(observe));
    getchar();
}

const char* const DataFileName="rand_0.dat";
enum:size_t{N=TenMil};//ten million

#if 1//test insertion sorts
int main()
{
    static i32 a[N];

    #if 0//need to create a file
    if (!FillRandom(a, sizeof a)) return 1;
    if (!Write(DataFileName, a, sizeof a)) return 1;
    #else
    if (!Read(DataFileName, a, sizeof a)) return 1;

    TestInsertionSorts(a, N);//need to go in and comment out all but 1
    #endif

    return 0;
}
#else//test merge sorts
int main()
{
    static i32 a[N];
    if (!Read(DataFileName, a, sizeof a)) return 1;

    #if 0//current implementation
    static i32 buf_full[N];

    clock_t start=clock();
    eastl::merge_sort_buffer(a, a+N, buf_full, Less);
    clock_t duration=clock()-start;

    assert(eastl::is_sorted(a, a+N));

    printf("current msort took %ld clocks\nsomething observable: %d\n", long(duration), int(*a));
    getchar();
    #else//new implementation
    static i32 buf_half[N/2u];

    clock_t start=clock();
    eastl::merge_sort_buffer_new(a, N, buf_half, Less);
    clock_t duration=clock()-start;

    assert(eastl::is_sorted(a, a+N));

    printf("new msort took %ld clocks\nsomething observable: %d\n", long(duration), int(*a));
    getchar();
    #endif

    return 0;
}
#endif
