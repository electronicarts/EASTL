# EASTL Benchmarks

## Introduction

This document provides a number of benchmark results of EASTL. Where possible, these benchmarks are implemented as comparisons with equivalent functionality found in other libraries such as compiler STL libraries or other well-known libraries. These comparison benchmarks concentrate on highlighting the differences between implementations rather than the similarities. In many mundane cases -- such as accessing a vector element via operator [] -- virtually all vector/array implementations you are likely to run into will have identical performance.

It's also important to note that the platform you run on can make a significant difference in the results. On a modern 3+GHz Windows PC many operations are fast due to large memory caches, intelligent branch prediction, and parallel instruction execution. However, on embedded or console systems none of these may be the case.

While EASTL generally outperforms std STL, there are some benchmarks here in which EASTL is slower than std STL. There are three primary explanations of this:

1. EASTL is making some kind of speed, memory, or design tradeoff that results in the given speed difference. In may such cases, EASTL goes slower on one benchmark in order to go faster on another benchmark deemed more important. This explanation constitutes about 60% of the cases.
2. Compiler optimizations and resulting code generation is coincidencally favoring one kind of implementation over another, often when they are visually virtually identical. This explantation constitutes about 30% of the cases.
3. EASTL is simply not yet as optimized as it could be. This explanation constitutes about 10% of the cases (as of this writing there are about three such functions throughout EASTL).

## Benchmarks

Below is a table of links to detailed benchmark results derived from the Benchmark test present in the EASTL package. The detailed results are present below the table. Additional platforms will be added as results become available for those platforms. Debug benchmarks are present because (lack of) debug performance can be significant for highly templated libraries. EASTL has specific optimizations to enhance debug performance relative to other standard libraries; in some cases it is 10x or more faster than alternatives (though there are exceptions where EASTL is slower). Feel free to submit results for additional compilers/platforms.

| Platform | Compiler | STL type | Build | Results |
|------|------|------|------|------|
| Win32 | VC++ 7.1 | Microsoft (Dinkumware) | Debug | [Detail]() |
| Win32 | VC++ 7.1 | Microsoft (Dinkumware) | Release | [Detail]() |
| Win32 | VC++ 7.1 | STLPort | Debug | [Detail]() |
| Win32 | VC++ 7.1 | STLPort | Release | [Detail]() |

### Win32.VC71.MS.Debug

```
EASTL version: 0.96.00
Platform: Windows on X86
Compiler: Microsoft Visual C++ compiler, version 1310
Allocator: PPMalloc::GeneralAllocatorDebug. Thread safety enabled.
Build: Debug. Inlining disabled. STL debug features disabled.

Values are times to complete tests; smaller values are better.
Alarm indicates a greater than 10% difference.

Test                                               VC++        EASTL     Ratio     Alarm
----------------------------------------------------------------------------------------
algorithm/adj_find/vector<TestObject>          33061345      6497757      5.09         *
algorithm/copy/vector<LargePOD>                 5844906      4876076      1.20         *
algorithm/copy/vector<uint32_t>                 1634346       166065      9.84         *
algorithm/copy_backward/vector<LargePOD>        4515974      4638892      0.97
algorithm/copy_backward/vector<uint32_t>        1821168       121746     14.96         *
algorithm/count/vector<uint64_t>               17048884      2720766      6.27         *
algorithm/equal_range/vector<uint64_t>       1111147812    448756888      2.48         *
algorithm/fill/bool[]                           1728722        91936     18.80         *
algorithm/fill/char[]/'d'                       1299200        33745     38.50         *
algorithm/fill/vector<char>/'d'                10205092        33796    100.00         *
algorithm/fill/vector<char>/0                  10200748        33805    100.00         *
algorithm/fill/vector<uint64_t>                10416538      1399687      7.44         *
algorithm/fill/vector<void*>                   10221837      1307700      7.82         *
algorithm/fill_n/bool[]                         1399033        34196     40.91         *
algorithm/fill_n/char[]                         1299225        33754     38.49         *
algorithm/fill_n/vector<uint64_t>               5961637      1371900      4.35         *
algorithm/find_end/string/end                  16569373      2657372      6.24         *
algorithm/find_end/string/middle               16558638     20242410      0.82         *
algorithm/find_end/string/none                 16811207     40480468      0.42         *
algorithm/lex_cmp/schar[]                       1749674       194429      9.00         *
algorithm/lex_cmp/vector<TestObject>           32824195      5253587      6.25         *
algorithm/lex_cmp/vector<uchar>                29852034       202658    100.00         *
algorithm/lower_bound/vector<TestObject>      798624462    350027935      2.28         *
algorithm/min_element/vector<TestObject>       21675298      5314676      4.08         *
algorithm/rand_shuffle/vector<uint64_t>        84236190     43677506      1.93         *
algorithm/reverse/list<TestObject>              3007292      2105799      1.43         *
algorithm/reverse/vector<TestObject>            2974618      2124796      1.40         *
algorithm/search/string<char>                  16228158      3594268      4.52         *
algorithm/search_n/string<char>                16926985      1522096     11.12         *
algorithm/unique/vector<TestObject>            54206243      9988002      5.43         *
algorithm/unique/vector<uint32_t>              26940079      1741991     15.47         *
algorithm/unique/vector<uint64_t>              47621344      5213127      9.13         *
algorithm/upper_bound/vector<uint32_t>        372381295    137901552      2.70         *

bitset<1500>/>>=/1                             90196544     92539832      0.97
bitset<1500>/count                             50753832     53742117      0.94
bitset<1500>/flip                              86935875     85121117      1.02
bitset<1500>/reset                             78153837     79922611      0.98
bitset<1500>/set()                             79214968     79360658      1.00
bitset<1500>/set(i)                            11300589     12199651      0.93
bitset<1500>/test                              11282679     13186450      0.86         *

bitset<15>/>>=/1                               10500577      6000559      1.75         *
bitset<15>/count                                4000356      6399753      0.63         *
bitset<15>/flip                                 7268877      5647944      1.29         *
bitset<15>/reset                                8564235      5800163      1.48         *
bitset<15>/set()                                9935523      5914012      1.68         *
bitset<15>/set(i)                              11199703     12503637      0.90         *
bitset<15>/test                                10600623     12899592      0.82         *

bitset<35>/>>=/1                               13076052      6599834      1.98         *
bitset<35>/count                                4800384     11500330      0.42         *
bitset<35>/flip                                 7915439      5816313      1.36         *
bitset<35>/reset                                9400049      5803180      1.62         *
bitset<35>/set()                               10701152      5840316      1.83         *
bitset<35>/set(i)                              11342936     12271128      0.92
bitset<35>/test                                10670799     13099682      0.81         *

bitset<75>/>>=/1                               14198834     17151088      0.83         *
bitset<75>/count                                5795530      8576373      0.68         *
bitset<75>/flip                                 8516703      8922995      0.95
bitset<75>/reset                                9999970      8526095      1.17         *
bitset<75>/set()                               11124877      9009686      1.23         *
bitset<75>/set(i)                              11300563     12531618      0.90         *
bitset<75>/test                                11031913     13100523      0.84         *

deque<ValuePair>/erase                        743801706    335646802      2.22         *
deque<ValuePair>/insert                       742331809    341912866      2.17         *
deque<ValuePair>/iteration                     29097030     16315827      1.78         *
deque<ValuePair>/operator[]                    49859598     24026313      2.08         *
deque<ValuePair>/push_back                    424807033     34497608     12.31         *
deque<ValuePair>/push_front                   402313373     38006322     10.59         *
deque<ValuePair>/sort                         725101017    581796551      1.25         *

hash_map<string, uint32_t>/clear                 559462       961019      0.58         *
hash_map<string, uint32_t>/count               53377807      8091448      6.60         *
hash_map<string, uint32_t>/erase pos             613573       858084      0.72         *
hash_map<string, uint32_t>/erase range          5488748       461134     11.90         *
hash_map<string, uint32_t>/erase val           35760096     16379858      2.18         *
hash_map<string, uint32_t>/find                43490335     10324823      4.21         *
hash_map<string, uint32_t>/find_as/char*       49343818      8617139      5.73         *
hash_map<string, uint32_t>/insert             107420281    168690439      0.64         *
hash_map<string, uint32_t>/iteration            2456356      1255153      1.96         *
hash_map<string, uint32_t>/operator[]          47209502     12581624      3.75         *

hash_map<uint32_t, TestObject>/clear             533172       546449      0.98
hash_map<uint32_t, TestObject>/count           28667432      2899997      9.89         *
hash_map<uint32_t, TestObject>/erase pos         683239       538289      1.27         *
hash_map<uint32_t, TestObject>/erase range      9632676       253037     38.07         *
hash_map<uint32_t, TestObject>/erase val       25466026      7752188      3.29         *
hash_map<uint32_t, TestObject>/find            20048253      4678502      4.29         *
hash_map<uint32_t, TestObject>/insert          71085798     37686187      1.89         *
hash_map<uint32_t, TestObject>/iteration        1460318      1338317      1.09
hash_map<uint32_t, TestObject>/operator[]      23226692      7888748      2.94         *

heap (uint32_t[])/make_heap                     5399966      6961305      0.78         *
heap (uint32_t[])/pop_heap                    108060534    103511318      1.04
heap (uint32_t[])/push_heap                    22595661     16640688      1.36         *
heap (uint32_t[])/sort_heap                    93559424     83076731      1.13         *

heap (vector<TestObject>)/make_heap            91770743     21724870      4.22         *
heap (vector<TestObject>)/pop_heap           1175599317    284007398      4.14         *
heap (vector<TestObject>)/push_heap           207804541     45918046      4.53         *
heap (vector<TestObject>)/sort_heap           970394145    208321477      4.66         *

list<TestObject>/ctor(it)                     805539509    760938607      1.06
list<TestObject>/ctor(n)                       80959236     75106995      1.08
list<TestObject>/erase                       1052543704   1044976137      1.01
list<TestObject>/find                          97785267     75970884      1.29         *
list<TestObject>/insert                       873895175    807051107      1.08
list<TestObject>/push_back                    812797710    780742425      1.04
list<TestObject>/remove                      1850600714   1436980599      1.29         *
list<TestObject>/reverse                      180270465     80466636      2.24         *
list<TestObject>/size/1                          440148       599642      0.73         *
list<TestObject>/size/10                         439433      1329817      0.33         *   EASTL intentionally implements list::size as O(n).
list<TestObject>/size/100                        439595     11030060      0.04         *   EASTL intentionally implements list::size as O(n).
list<TestObject>/splice                       177106094     69383027      2.55         *

map<TestObject, uint32_t>/clear                  508283       470807      1.08
map<TestObject, uint32_t>/count                43145354     14280357      3.02         *
map<TestObject, uint32_t>/equal_range          38594004     16520447      2.34         *
map<TestObject, uint32_t>/erase/key            33948082     16123175      2.11         *
map<TestObject, uint32_t>/erase/pos              578332       455201      1.27         *   MS uses a code bloating implementation of erase.
map<TestObject, uint32_t>/erase/range            387345       284538      1.36         *
map<TestObject, uint32_t>/find                 22897224     12766100      1.79         *
map<TestObject, uint32_t>/insert               61665800     47286928      1.30         *
map<TestObject, uint32_t>/iteration             1977202       745391      2.65         *
map<TestObject, uint32_t>/lower_bound          19892941     12260928      1.62         *
map<TestObject, uint32_t>/operator[]           24199084     15429634      1.57         *
map<TestObject, uint32_t>/upper_bound          19842409     12064441      1.64         *

set<uint32_t>/clear                             1027625      1000901      1.03
set<uint32_t>/count                            39730182     13329565      2.98         *
set<uint32_t>/equal_range                      34681649     14768827      2.35         *
set<uint32_t>/erase range                        841458       602030      1.40         *
set<uint32_t>/erase/pos                         1380485      1084303      1.27         *   MS uses a code bloating implementation of erase.
set<uint32_t>/erase/val                        31617425     13344023      2.37         *
set<uint32_t>/find                             19582428     10788864      1.82         *
set<uint32_t>/insert                           61434014     48232086      1.27         *
set<uint32_t>/iteration                         1512057       667820      2.26         *
set<uint32_t>/lower_bound                      18394885     10402785      1.77         *
set<uint32_t>/upper_bound                      17189083     10554425      1.63         *

sort/q_sort/TestObject[]                       87088799     15037988      5.79         *
sort/q_sort/TestObject[]/sorted                21502892      3284299      6.55         *
sort/q_sort/vector<TestObject>                 87962047     15004677      5.86         *
sort/q_sort/vector<TestObject>/sorted          21396523      3341163      6.40         *
sort/q_sort/vector<ValuePair>                  80334589     10429161      7.70         *
sort/q_sort/vector<ValuePair>/sorted           22133295      3230553      6.85         *
sort/q_sort/vector<uint32>                     72195388      5940302     12.15         *
sort/q_sort/vector<uint32>/sorted              19635171       995495     19.72         *

string<char16_t>/compare                      523013373    534722089      0.98
string<char16_t>/erase/pos,n                    3446597      3439492      1.00
string<char16_t>/find/p,pos,n                 383873158    441902786      0.87         *
string<char16_t>/find_first_not_of/p,pos,n       174157       134131      1.30         *
string<char16_t>/find_first_of/p,pos,n         11715423      8520944      1.37         *
string<char16_t>/find_last_of/p,pos,n           1871556      1226457      1.53         *
string<char16_t>/insert/pos,p                   3624877      3357058      1.08
string<char16_t>/iteration                   6766787933    581916665     11.63         *
string<char16_t>/operator[]                     4820827      2335579      2.06         *
string<char16_t>/push_back                     59812962      6757466      8.85         *
string<char16_t>/replace/pos,n,p,n              4371279      4459713      0.98
string<char16_t>/reserve                        2307530      1919386      1.20         *
string<char16_t>/rfind/p,pos,n                   734826       372615      1.97         *
string<char16_t>/size                             41608        28866      1.44         *
string<char16_t>/swap                           1033932      1490994      0.69         *

string<char8_t>/compare                        63086797     64194771      0.98
string<char8_t>/erase/pos,n                     2045687      1960270      1.04
string<char8_t>/find/p,pos,n                  123872549    471364764      0.26         *
string<char8_t>/find_first_not_of/p,pos,n        140013       130271      1.07
string<char8_t>/find_first_of/p,pos,n           8051906      8749994      0.92
string<char8_t>/find_last_of/p,pos,n            1318835      1230715      1.07
string<char8_t>/insert/pos,p                    1770610      1724234      1.03
string<char8_t>/iteration                      28112136      2544475     11.05         *
string<char8_t>/operator[]                      4810525      2255841      2.13         *
string<char8_t>/push_back                      54869634      6127447      8.95         *
string<char8_t>/replace/pos,n,p,n               2737578      2847900      0.96
string<char8_t>/reserve                         1123395       394902      2.84         *
string<char8_t>/rfind/p,pos,n                    737299       368518      2.00         *
string<char8_t>/size                              42245        26801      1.58         *
string<char8_t>/swap                            1036142      1491028      0.69         *

vector<uint64>/erase                           56417135     55770251      1.01
vector<uint64>/insert                          56617761     56100468      1.01
vector<uint64>/iteration                       10413895      1291269      8.06         *
vector<uint64>/operator[]                      23507193      3479390      6.76         *
vector<uint64>/push_back                       34687939     13806627      2.51         *
vector<uint64>/sort                           256886550     84669657      3.03         *
```

### Win32.VC71.MS.Release

```
EASTL version: 0.96.00
Platform: Windows on X86
Compiler: Microsoft Visual C++ compiler, version 1310
Allocator: PPMalloc::GeneralAllocator. Thread safety enabled.
Build: Full optimization. Inlining enabled.

Values are times to complete tests; smaller values are better.
Alarm indicates a greater than 10% difference.

Test                                               VC++        EASTL     Ratio     Alarm
----------------------------------------------------------------------------------------
algorithm/adj_find/vector<TestObject>           2783546      2750660      1.01
algorithm/copy/vector<LargePOD>                 6474025      4972738      1.30         *
algorithm/copy/vector<uint32_t>                  157267       173162      0.91
algorithm/copy_backward/vector<LargePOD>        4836406      4374780      1.11         *
algorithm/copy_backward/vector<uint32_t>         104780       120912      0.87         *
algorithm/count/vector<uint64_t>                1368440      1368696      1.00
algorithm/equal_range/vector<uint64_t>        114199387    102783938      1.11         *
algorithm/fill/bool[]                            253215        27353      9.26         *
algorithm/fill/char[]/'d'                        253164        27404      9.24         *
algorithm/fill/vector<char>/'d'                  253105        27362      9.25         *
algorithm/fill/vector<char>/0                    253275        27353      9.26         *
algorithm/fill/vector<uint64_t>                  397001       394323      1.01
algorithm/fill/vector<void*>                     547196       642362      0.85         *
algorithm/fill_n/bool[]                          229177        27361      8.38         *
algorithm/fill_n/char[]                          228845        27404      8.35         *
algorithm/fill_n/vector<uint64_t>                565233      1376822      0.41         *
algorithm/find_end/string/end                   2107116        82356     25.59         *
algorithm/find_end/string/middle                2111672       664283      3.18         *
algorithm/find_end/string/none                  2110423      1519596      1.39         *
algorithm/lex_cmp/schar[]                        741021       176162      4.21         *
algorithm/lex_cmp/vector<TestObject>            2610494      2642183      0.99
algorithm/lex_cmp/vector<uchar>                  697595       167866      4.16         *
algorithm/lower_bound/vector<TestObject>       62462233     58146664      1.07
algorithm/min_element/vector<TestObject>        4350385      2671227      1.63         *
algorithm/rand_shuffle/vector<uint64_t>        10868261     11300818      0.96
algorithm/reverse/list<TestObject>               483718       470024      1.03
algorithm/reverse/vector<TestObject>             476739       484322      0.98
algorithm/search/string<char>                   2560387      1259496      2.03         *
algorithm/search_n/string<char>                 2770991       458524      6.04         *
algorithm/unique/vector<TestObject>             4194520      4658910      0.90         *
algorithm/unique/vector<uint32_t>                538730       787924      0.68         *
algorithm/unique/vector<uint64_t>               3169829      2575636      1.23         *
algorithm/upper_bound/vector<uint32_t>         27495562     25321593      1.09

bitset<1500>/>>=/1                             33464228     33469719      1.00
bitset<1500>/count                             18736116     18814903      1.00
bitset<1500>/flip                              19299309     18605438      1.04
bitset<1500>/reset                             22200487     15262847      1.45         *
bitset<1500>/set()                             14418193     17557319      0.82         *
bitset<1500>/set(i)                             1599250      1599199      1.00
bitset<1500>/test                               1599241      1599233      1.00

bitset<15>/>>=/1                                2199222      2264442      0.97
bitset<15>/count                                1399406      1399193      1.00
bitset<15>/flip                                 1266712      1199197      1.06
bitset<15>/reset                                1399364      1399109      1.00
bitset<15>/set()                                1199197       999201      1.20         *
bitset<15>/set(i)                               1599258      1462952      1.09
bitset<15>/test                                 1599275      1599224      1.00

bitset<35>/>>=/1                                2599266      1933376      1.34         *
bitset<35>/count                                2599240      2592559      1.00
bitset<35>/flip                                 1693124      1199188      1.41         *
bitset<35>/reset                                1399406       999201      1.40         *
bitset<35>/set()                                1599403      1199205      1.33         *
bitset<35>/set(i)                               1599241      1599190      1.00
bitset<35>/test                                 1599250      1599232      1.00

bitset<75>/>>=/1                                4199332      4199213      1.00
bitset<75>/count                                2999497      2199341      1.36         *
bitset<75>/flip                                 2399499      1830178      1.31         *
bitset<75>/reset                                2199468      1199197      1.83         *
bitset<75>/set()                                1999387      1199851      1.67         *
bitset<75>/set(i)                               1599266      1599198      1.00
bitset<75>/test                                 1599241      1662651      0.96

deque<ValuePair>/erase                         90444165     37113253      2.44         *
deque<ValuePair>/insert                        93299349     36175167      2.58         *
deque<ValuePair>/iteration                      2756414      2122076      1.30         *
deque<ValuePair>/operator[]                     5117969      4632075      1.10
deque<ValuePair>/push_back                     30300757      3060357      9.90         *
deque<ValuePair>/push_front                    25498529      2808392      9.08         *
deque<ValuePair>/sort                         142283047    111292464      1.28         *

hash_map<string, uint32_t>/clear                 146769       389699      0.38         *
hash_map<string, uint32_t>/count               13059434      3460324      3.77         *
hash_map<string, uint32_t>/erase pos             184246       331925      0.56         *
hash_map<string, uint32_t>/erase range           382432       167237      2.29         *
hash_map<string, uint32_t>/erase val            6187898      3302114      1.87         *
hash_map<string, uint32_t>/find                11289369      3459024      3.26         *
hash_map<string, uint32_t>/find_as/char*       13559192      3662387      3.70         *
hash_map<string, uint32_t>/insert              17514012     14095176      1.24         *
hash_map<string, uint32_t>/iteration             801014       218450      3.67         *
hash_map<string, uint32_t>/operator[]          11457065      3690385      3.10         *

hash_map<uint32_t, TestObject>/clear             141865       265379      0.53         *
hash_map<uint32_t, TestObject>/count            1766045       703613      2.51         *
hash_map<uint32_t, TestObject>/erase pos         172337       218458      0.79         *
hash_map<uint32_t, TestObject>/erase range       537846       102340      5.26         *
hash_map<uint32_t, TestObject>/erase val        2220132      1441787      1.54         *
hash_map<uint32_t, TestObject>/find             1612994      1043953      1.55         *
hash_map<uint32_t, TestObject>/insert           7141547      4348056      1.64         *
hash_map<uint32_t, TestObject>/iteration         199512       169328      1.18         *
hash_map<uint32_t, TestObject>/operator[]       1831733      1519707      1.21         *

heap (uint32_t[])/make_heap                     3366247      1949093      1.73         *
heap (uint32_t[])/pop_heap                     57280514     53779440      1.07
heap (uint32_t[])/push_heap                     9700217      7582935      1.28         *
heap (uint32_t[])/sort_heap                    47227751     46131948      1.02

heap (vector<TestObject>)/make_heap            11458442     11510819      1.00
heap (vector<TestObject>)/pop_heap            122897267    119061132      1.03
heap (vector<TestObject>)/push_heap            21688481     21176220      1.02
heap (vector<TestObject>)/sort_heap            90867380     88869523      1.02

list<TestObject>/ctor(it)                      74591104     69845817      1.07
list<TestObject>/ctor(n)                        6243998      5838582      1.07
list<TestObject>/erase                        299509298    206013676      1.45         *
list<TestObject>/find                          40927185     14514243      2.82         *
list<TestObject>/insert                        71277251     47234534      1.51         *
list<TestObject>/push_back                     73780527     44116725      1.67         *
list<TestObject>/remove                       786197776    326434612      2.41         *
list<TestObject>/reverse                       49283128     25029678      1.97         *
list<TestObject>/size/1                          159741       139400      1.15         *
list<TestObject>/size/10                         159324       346579      0.46         *   EASTL intentionally implements list::size as O(n).
list<TestObject>/size/100                        159188     97235419      0.00         *   EASTL intentionally implements list::size as O(n).
list<TestObject>/splice                        63548584     19322931      3.29         *

map<TestObject, uint32_t>/clear                  167408       170501      0.98
map<TestObject, uint32_t>/count                10213685      4748346      2.15         *
map<TestObject, uint32_t>/equal_range           9515053      5677558      1.68         *
map<TestObject, uint32_t>/erase/key             6646260      4302300      1.54         *
map<TestObject, uint32_t>/erase/pos              297135       327938      0.91             MS uses a code bloating implementation of erase.
map<TestObject, uint32_t>/erase/range            148614       163702      0.91
map<TestObject, uint32_t>/find                  5637531      4767055      1.18         *
map<TestObject, uint32_t>/insert                9591128      9030349      1.06
map<TestObject, uint32_t>/iteration              323595       325261      0.99
map<TestObject, uint32_t>/lower_bound           5398239      4784089      1.13         *
map<TestObject, uint32_t>/operator[]            5631250      5141166      1.10
map<TestObject, uint32_t>/upper_bound           5436336      4762431      1.14         *

set<uint32_t>/clear                              155983       156026      1.00
set<uint32_t>/count                             9635965      4392146      2.19         *
set<uint32_t>/equal_range                       8504157      5247832      1.62         *
set<uint32_t>/erase range                        140488       119408      1.18         *
set<uint32_t>/erase/pos                          260678       286697      0.91             MS uses a code bloating implementation of erase.
set<uint32_t>/erase/val                         6008225      4012825      1.50         *
set<uint32_t>/find                              5145432      4381945      1.17         *
set<uint32_t>/insert                            8087129      8697251      0.93
set<uint32_t>/iteration                          271507       304538      0.89         *
set<uint32_t>/lower_bound                       4666228      4404250      1.06
set<uint32_t>/upper_bound                       4623600      4402974      1.05

sort/q_sort/TestObject[]                        9596169      5578652      1.72         *
sort/q_sort/TestObject[]/sorted                  602463      1016132      0.59         *
sort/q_sort/vector<TestObject>                  9674828      5430199      1.78         *
sort/q_sort/vector<TestObject>/sorted            606908      1111647      0.55         *
sort/q_sort/vector<ValuePair>                   6284194      3423452      1.84         *
sort/q_sort/vector<ValuePair>/sorted             711629       569364      1.25         *
sort/q_sort/vector<uint32>                      5453379      2916146      1.87         *
sort/q_sort/vector<uint32>/sorted                537047       419144      1.28         *

string<char16_t>/compare                      435083295    251985824      1.73         *
string<char16_t>/erase/pos,n                    3454842      3451858      1.00
string<char16_t>/find/p,pos,n                 401954723    165298157      2.43         *
string<char16_t>/find_first_not_of/p,pos,n       131452        65374      2.01         *
string<char16_t>/find_first_of/p,pos,n         11657444      4144515      2.81         *
string<char16_t>/find_last_of/p,pos,n           1604248       567571      2.83         *
string<char16_t>/insert/pos,p                   3398734      3355460      1.01
string<char16_t>/iteration                    218856504    218771844      1.00
string<char16_t>/operator[]                      714161       240023      2.98         *
string<char16_t>/push_back                     34968235      2444897     14.30         *
string<char16_t>/replace/pos,n,p,n              4226693      4198498      1.01
string<char16_t>/reserve                        1901765       390805      4.87         *
string<char16_t>/rfind/p,pos,n                   195483       150985      1.29         *
string<char16_t>/size                             11169        11245      0.99
string<char16_t>/swap                           1459280       419807      3.48         *

string<char8_t>/compare                        63071275     77209580      0.82         *
string<char8_t>/erase/pos,n                     2008652      1944494      1.03
string<char8_t>/find/p,pos,n                  123201023    167536164      0.74         *
string<char8_t>/find_first_not_of/p,pos,n         93372        67864      1.38         *
string<char8_t>/find_first_of/p,pos,n           7542492      3375758      2.23         *
string<char8_t>/find_last_of/p,pos,n             933972       583576      1.60         *
string<char8_t>/insert/pos,p                    1737213      1750847      0.99
string<char8_t>/iteration                        893834       899130      0.99
string<char8_t>/operator[]                       817879       313437      2.61         *
string<char8_t>/push_back                      20857734      2004410     10.41         *
string<char8_t>/replace/pos,n,p,n               2578696      2607655      0.99
string<char8_t>/reserve                          915127        85289     10.73         *
string<char8_t>/rfind/p,pos,n                    196103       148894      1.32         *
string<char8_t>/size                              11619        11220      1.04
string<char8_t>/swap                            1461056       419874      3.48         *

vector<uint64>/erase                           55235116     55284587      1.00
vector<uint64>/insert                          55166046     55142755      1.00
vector<uint64>/iteration                         553954       509719      1.09
vector<uint64>/operator[]                       1284239       798516      1.61         *
vector<uint64>/push_back                        5399549      3867959      1.40         *
vector<uint64>/sort                            43636314     42619952      1.02
```

### Win32.VC71.STLPort.Debug

```
EASTL version: 0.96.00
Platform: Windows on X86
Compiler: Microsoft Visual C++ compiler, version 1310
Allocator: PPMalloc::GeneralAllocatorDebug. Thread safety enabled.
Build: Debug. Inlining disabled. STL debug features disabled.

Values are times to complete tests; smaller values are better.
Alarm indicates a greater than 10% difference.

Test                                            STLPort        EASTL     Ratio     Alarm
----------------------------------------------------------------------------------------
algorithm/adj_find/vector<TestObject>           5661170      5689517      1.00
algorithm/copy/vector<LargePOD>                 5573815      5124428      1.09
algorithm/copy/vector<uint32_t>                  148273       125782      1.18         *
algorithm/copy_backward/vector<LargePOD>        5429791      4834510      1.12         *
algorithm/copy_backward/vector<uint32_t>         156765       163038      0.96
algorithm/count/vector<uint64_t>                2730922      2730072      1.00
algorithm/equal_range/vector<uint64_t>        639366489    452896251      1.41         *
algorithm/fill/bool[]                           1299326        27361     47.49         *
algorithm/fill/char[]/'d'                         27378        27361      1.00
algorithm/fill/vector<char>/'d'                   34459        27361      1.26         *
algorithm/fill/vector<char>/0                   1299224        27361     47.48         *
algorithm/fill/vector<uint64_t>                 1400647      1400145      1.00
algorithm/fill/vector<void*>                    1308779      1309085      1.00
algorithm/fill_n/bool[]                         1299156        27352     47.50         *
algorithm/fill_n/char[]                         1299258        27369     47.47         *
algorithm/fill_n/vector<uint64_t>               1451162      1313632      1.10
algorithm/find_end/string/end                  13089999      2526412      5.18         *
algorithm/find_end/string/middle               12627412     20190101      0.63         *
algorithm/find_end/string/none                 12704185     40728803      0.31         *
algorithm/lex_cmp/schar[]                       1749844       195806      8.94         *
algorithm/lex_cmp/vector<TestObject>            5060968      4799882      1.05
algorithm/lex_cmp/vector<uchar>                 1668354       189490      8.80         *
algorithm/lower_bound/vector<TestObject>      450240945    353437573      1.27         *
algorithm/min_element/vector<TestObject>        5861744      5326371      1.10
algorithm/rand_shuffle/vector<uint64_t>        40780449     45780090      0.89         *
algorithm/reverse/list<TestObject>              2657678      2130627      1.25         *
algorithm/reverse/vector<TestObject>            2666424      2124889      1.25         *
algorithm/search/string<char>                   3110379      3613460      0.86         *
algorithm/search_n/string<char>                 3061665      1521261      2.01         *
algorithm/unique/vector<TestObject>            12423684      9485439      1.31         *
algorithm/unique/vector<uint32_t>               3718699      1726596      2.15         *
algorithm/unique/vector<uint64_t>               6205110      4591631      1.35         *
algorithm/upper_bound/vector<uint32_t>        185391094    139336317      1.33         *

bitset<1500>/>>=/1                            120666960     92449816      1.31         *   STLPort is broken, neglects wraparound check.
bitset<1500>/count                            201709793     52874726      3.81         *
bitset<1500>/flip                              87360297     81737071      1.07
bitset<1500>/reset                             23950178     77390323      0.31         *
bitset<1500>/set()                             84608107     76912011      1.10
bitset<1500>/set(i)                            18023620     12229604      1.47         *
bitset<1500>/test                              18006553     13276396      1.36         *

bitset<15>/>>=/1                               11935904      6012695      1.99         *   STLPort is broken, neglects wraparound check.
bitset<15>/count                                9368581      6022742      1.56         *
bitset<15>/flip                                11600706      6533635      1.78         *
bitset<15>/reset                                5830957      5874690      0.99
bitset<15>/set()                               11695328      5701621      2.05         *
bitset<15>/set(i)                              16363205     12570216      1.30         *
bitset<15>/test                                16743172     13201452      1.27         *

bitset<35>/>>=/1                               22950918      6774457      3.39         *   STLPort is broken, neglects wraparound check.
bitset<35>/count                               12655309     11736256      1.08
bitset<35>/flip                                13738575      5800042      2.37         *
bitset<35>/reset                               15561434      5800510      2.68         *
bitset<35>/set()                               13564283      5600709      2.42         *
bitset<35>/set(i)                              18519689     12199973      1.52         *
bitset<35>/test                                18000569     13103566      1.37         *

bitset<75>/>>=/1                               25579525     16669664      1.53         *   STLPort is broken, neglects wraparound check.
bitset<75>/count                               18740698      8480492      2.21         *
bitset<75>/flip                                13555630      8300335      1.63         *
bitset<75>/reset                               15200133      8200000      1.85         *
bitset<75>/set()                               14408112      8001959      1.80         *
bitset<75>/set(i)                              18137741     12374257      1.47         *
bitset<75>/test                                18422135     13100038      1.41         *

deque<ValuePair>/erase                        651933790    326443043      2.00         *
deque<ValuePair>/insert                       659786183    333304660      1.98         *
deque<ValuePair>/iteration                     23734592     16173706      1.47         *
deque<ValuePair>/operator[]                    59126816     23911774      2.47         *
deque<ValuePair>/push_back                     58056988     31859266      1.82         *
deque<ValuePair>/push_front                    57780891     31743199      1.82         *
deque<ValuePair>/sort                         818414195    596568113      1.37         *

hash_map<string, uint32_t>/clear                3422133      2204517      1.55         *
hash_map<string, uint32_t>/count                9869545      8624924      1.14         *
hash_map<string, uint32_t>/erase pos            3256350      2069299      1.57         *
hash_map<string, uint32_t>/erase range          3230203      1151392      2.81         *
hash_map<string, uint32_t>/erase val           16860362     15939778      1.06
hash_map<string, uint32_t>/find                10286971      9920910      1.04
hash_map<string, uint32_t>/find_as/char*      118136025      9458468     12.49         *
hash_map<string, uint32_t>/insert             188948336    174490082      1.08
hash_map<string, uint32_t>/iteration            4037049      2021036      2.00         *
hash_map<string, uint32_t>/operator[]          11472127     12887699      0.89         *

hash_map<uint32_t, TestObject>/clear            2522264      1331848      1.89         *
hash_map<uint32_t, TestObject>/count            3210739      2897063      1.11         *
hash_map<uint32_t, TestObject>/erase pos        1862281      1304783      1.43         *
hash_map<uint32_t, TestObject>/erase range       698079       579606      1.20         *
hash_map<uint32_t, TestObject>/erase val        8806722      7041298      1.25         *
hash_map<uint32_t, TestObject>/find             3604875      4709645      0.77         *
hash_map<uint32_t, TestObject>/insert          40785711     40376342      1.01
hash_map<uint32_t, TestObject>/iteration        3064088      1508834      2.03         *
hash_map<uint32_t, TestObject>/operator[]       6053742      8176906      0.74         *

heap (uint32_t[])/make_heap                     5799813      5738596      1.01
heap (uint32_t[])/pop_heap                    113775168    102076134      1.11         *
heap (uint32_t[])/push_heap                    21649151     16854845      1.28         *
heap (uint32_t[])/sort_heap                    97535213     83290735      1.17         *

heap (vector<TestObject>)/make_heap            22215557     22277063      1.00
heap (vector<TestObject>)/pop_heap            275392171    277340039      0.99
heap (vector<TestObject>)/push_heap            51479442     47342577      1.09
heap (vector<TestObject>)/sort_heap           214474736    218497540      0.98

list<TestObject>/ctor(it)                     767753795    753421427      1.02
list<TestObject>/ctor(n)                       74185322     73386245      1.01
list<TestObject>/erase                       1021003824   1033873589      0.99
list<TestObject>/find                          77666072     74917622      1.04
list<TestObject>/insert                       788071150    774188737      1.02
list<TestObject>/push_back                    760490154    737327348      1.03
list<TestObject>/remove                      1682511938   1434771006      1.17         *
list<TestObject>/reverse                       87237327     80394623      1.09
list<TestObject>/size/1                         3828111       599530      6.39         *
list<TestObject>/size/10                        9600605      1329535      7.22         *   EASTL intentionally implements list::size as O(n).
list<TestObject>/size/100                      62952334     15022551      4.19         *   EASTL intentionally implements list::size as O(n).
list<TestObject>/splice                        96536412     60804817      1.59         *

map<TestObject, uint32_t>/clear                 1142127      1099066      1.04
map<TestObject, uint32_t>/count                19659726     14647548      1.34         *
map<TestObject, uint32_t>/equal_range          36680687     18219086      2.01         *
map<TestObject, uint32_t>/erase/key            28892154     16037774      1.80         *
map<TestObject, uint32_t>/erase/pos             1209643      1185495      1.02
map<TestObject, uint32_t>/erase/range            715402       670539      1.07
map<TestObject, uint32_t>/find                 21020992     13429575      1.57         *
map<TestObject, uint32_t>/insert               59530871     51120640      1.16         *
map<TestObject, uint32_t>/iteration              972825      1191946      0.82         *
map<TestObject, uint32_t>/lower_bound          18852651     12495034      1.51         *
map<TestObject, uint32_t>/operator[]           22889573     16676736      1.37         *
map<TestObject, uint32_t>/upper_bound          18603584     12406922      1.50         *

set<uint32_t>/clear                              919555       882988      1.04
set<uint32_t>/count                            17561110     12461084      1.41         *
set<uint32_t>/equal_range                      31522488     15230282      2.07         *
set<uint32_t>/erase range                        687582       564765      1.22         *
set<uint32_t>/erase/pos                         1044352      1045355      1.00
set<uint32_t>/erase/val                        25525304     12940774      1.97         *
set<uint32_t>/find                             17140751     10704866      1.60         *
set<uint32_t>/insert                           56035051     45555664      1.23         *
set<uint32_t>/iteration                          682669       640831      1.07
set<uint32_t>/lower_bound                      16339932     10475740      1.56         *
set<uint32_t>/upper_bound                      17779424     10652599      1.67         *

sort/q_sort/TestObject[]                       17000866     14823515      1.15         *
sort/q_sort/TestObject[]/sorted                 6658559      3263328      2.04         *
sort/q_sort/vector<TestObject>                 17476629     14953285      1.17         *
sort/q_sort/vector<TestObject>/sorted           6667034      3327435      2.00         *
sort/q_sort/vector<ValuePair>                  15391357     10820848      1.42         *
sort/q_sort/vector<ValuePair>/sorted            6617122      3232949      2.05         *
sort/q_sort/vector<uint32>                      8343906      6014846      1.39         *
sort/q_sort/vector<uint32>/sorted               3039430      1003127      3.03         *

string<char16_t>/compare                     1489709846    532664000      2.80         *
string<char16_t>/erase/pos,n                    3528690      3439864      1.03
string<char16_t>/find/p,pos,n                2521448321    443752189      5.68         *
string<char16_t>/find_first_not_of/p,pos,n       661206       137419      4.81         *
string<char16_t>/find_first_of/p,pos,n         54746434      8521335      6.42         *
string<char16_t>/find_last_of/p,pos,n          10607778      1212414      8.75         *
string<char16_t>/insert/pos,p                   3445016      3360126      1.03
string<char16_t>/iteration                    580955636    579452556      1.00
string<char16_t>/operator[]                     2206353      1987809      1.11         *
string<char16_t>/push_back                     22421368      6007808      3.73         *
string<char16_t>/replace/pos,n,p,n              5138454      4464786      1.15         *
string<char16_t>/reserve                     4922413418       335622    100.00         *
string<char16_t>/rfind/p,pos,n                  1440308       380578      3.78         *
string<char16_t>/size                             25355        25398      1.00
string<char16_t>/swap                           2122704      1490823      1.42         *

string<char8_t>/compare                        77222134     77443134      1.00
string<char8_t>/erase/pos,n                     1965344      1956521      1.00
string<char8_t>/find/p,pos,n                 2468091951    474205522      5.20         *
string<char8_t>/find_first_not_of/p,pos,n        660960       130211      5.08         *
string<char8_t>/find_first_of/p,pos,n          55020899      9240171      5.95         *
string<char8_t>/find_last_of/p,pos,n           10576210      1239053      8.54         *
string<char8_t>/insert/pos,p                    1822756      1750880      1.04
string<char8_t>/iteration                       2617889      2540148      1.03
string<char8_t>/operator[]                      2254794      2256443      1.00
string<char8_t>/push_back                      12463022      5210321      2.39         *
string<char8_t>/replace/pos,n,p,n               3744862      2855260      1.31         *
string<char8_t>/reserve                      1372046888       218815    100.00         *
string<char8_t>/rfind/p,pos,n                   1446232       366902      3.94         *
string<char8_t>/size                              26859        25431      1.06
string<char8_t>/swap                            2123350      1490509      1.42         *

vector<uint64>/erase                           55164013     56417449      0.98
vector<uint64>/insert                          55872973     56432664      0.99
vector<uint64>/iteration                        1329102      1324623      1.00
vector<uint64>/operator[]                       5264738      3136746      1.68         *
vector<uint64>/push_back                       14903245     13171175      1.13         *
vector<uint64>/sort                            88429095     88542171      1.00
```

### Win32.VC71.STLPort.Release

```
EASTL version: 0.96.00
Platform: Windows on X86
Compiler: Microsoft Visual C++ compiler, version 1310
Allocator: PPMalloc::GeneralAllocator. Thread safety enabled.
Build: Full optimization. Inlining enabled.

Values are times to complete tests; smaller values are better.
Alarm indicates a greater than 10% difference.

Test                                            STLPort        EASTL     Ratio     Alarm
----------------------------------------------------------------------------------------
algorithm/adj_find/vector<TestObject>           2741046      2731441      1.00
algorithm/copy/vector<LargePOD>                 6065923      5085142      1.19         *
algorithm/copy/vector<uint32_t>                  158304       165555      0.96
algorithm/copy_backward/vector<LargePOD>        4710258      4896476      0.96
algorithm/copy_backward/vector<uint32_t>         146030       142630      1.02
algorithm/count/vector<uint64_t>                1395921      1406334      0.99
algorithm/equal_range/vector<uint64_t>        211692764    118969493      1.78         *
algorithm/fill/bool[]                            366078        33737     10.85         *
algorithm/fill/char[]/'d'                         33736        33771      1.00
algorithm/fill/vector<char>/'d'                   28466        33720      0.84         *
algorithm/fill/vector<char>/0                    366086        33728     10.85         *
algorithm/fill/vector<uint64_t>                  466250       401591      1.16         *
algorithm/fill/vector<void*>                     521603       693481      0.75         *
algorithm/fill_n/bool[]                          599709        33762     17.76         *
algorithm/fill_n/char[]                          599573        33711     17.79         *
algorithm/fill_n/vector<uint64_t>                434971      1374084      0.32         *
algorithm/find_end/string/end                   1494742        85349     17.51         *
algorithm/find_end/string/middle                1480700       687208      2.15         *
algorithm/find_end/string/none                  1540540      1546431      1.00
algorithm/lex_cmp/schar[]                        921638       178797      5.15         *
algorithm/lex_cmp/vector<TestObject>            2623559      2643551      0.99
algorithm/lex_cmp/vector<uchar>                  960899       183608      5.23         *
algorithm/lower_bound/vector<TestObject>       60630534     56531528      1.07
algorithm/min_element/vector<TestObject>        4209022      2768527      1.52         *
algorithm/rand_shuffle/vector<uint64_t>        13762010     15969052      0.86         *
algorithm/reverse/list<TestObject>               673387       731825      0.92
algorithm/reverse/vector<TestObject>             634576       754511      0.84         *
algorithm/search/string<char>                   1262599      1387608      0.91
algorithm/search_n/string<char>                 1166242       458592      2.54         *
algorithm/unique/vector<TestObject>             4912193      5336317      0.92
algorithm/unique/vector<uint32_t>                809387       809081      1.00
algorithm/unique/vector<uint64_t>               4371814      2414255      1.81         *
algorithm/upper_bound/vector<uint32_t>         31899081     29555596      1.08

bitset<1500>/>>=/1                             63308136     40553560      1.56         *   STLPort is broken, neglects wraparound check.
bitset<1500>/count                             62523178     22799473      2.74         *
bitset<1500>/flip                              20302845     19919232      1.02
bitset<1500>/reset                             18892015     15403148      1.23         *
bitset<1500>/set()                             15803302     17322192      0.91
bitset<1500>/set(i)                             2799271      2999310      0.93
bitset<1500>/test                               2999293      2799262      1.07

bitset<15>/>>=/1                                1199239      3199256      0.37         *   STLPort is broken, neglects wraparound check.
bitset<15>/count                                3599461      2199231      1.64         *
bitset<15>/flip                                 1199231      1199188      1.00
bitset<15>/reset                                1199188      1199180      1.00
bitset<15>/set()                                1199214      1199180      1.00
bitset<15>/set(i)                               2599257      1399262      1.86         *
bitset<15>/test                                 2599274      2599283      1.00

bitset<35>/>>=/1                                6643974      4599239      1.44         *   STLPort is broken, neglects wraparound check.
bitset<35>/count                                5151331      5399438      0.95
bitset<35>/flip                                 1999404      1199273      1.67         *
bitset<35>/reset                                9805285      1399313      7.01         *
bitset<35>/set()                                2799279      1199248      2.33         *
bitset<35>/set(i)                               2799246      1599241      1.75         *
bitset<35>/test                                 2999234      2999251      1.00

bitset<75>/>>=/1                                7002045      6999333      1.00             STLPort is broken, neglects wraparound check.
bitset<75>/count                                5999351      3002259      2.00         *
bitset<75>/flip                                 3599334      3599163      1.00
bitset<75>/reset                                9799344      3399218      2.88         *
bitset<75>/set()                                3599232      3599062      1.00
bitset<75>/set(i)                               2799228      1599284      1.75         *
bitset<75>/test                                 2999250      2799339      1.07

deque<ValuePair>/erase                        127108651    115258113      1.10
deque<ValuePair>/insert                       137727889    116552332      1.18         *
deque<ValuePair>/iteration                      7144182      6009899      1.19         *
deque<ValuePair>/operator[]                    34241222     20535039      1.67         *
deque<ValuePair>/push_back                      6585800      3932126      1.67         *
deque<ValuePair>/push_front                     6805865      3993513      1.70         *
deque<ValuePair>/sort                         395352323    348778188      1.13         *

hash_map<string, uint32_t>/clear                 426640       447015      0.95
hash_map<string, uint32_t>/count                4359344      3883089      1.12         *
hash_map<string, uint32_t>/erase pos             584392       458142      1.28         *
hash_map<string, uint32_t>/erase range           221034       196078      1.13         *
hash_map<string, uint32_t>/erase val            3539867      3790813      0.93
hash_map<string, uint32_t>/find                 3966831      3811910      1.04
hash_map<string, uint32_t>/find_as/char*       11591612      4243710      2.73         *
hash_map<string, uint32_t>/insert              16763887     16719194      1.00
hash_map<string, uint32_t>/iteration             909968       478609      1.90         *
hash_map<string, uint32_t>/operator[]           4360041      4108313      1.06

hash_map<uint32_t, TestObject>/clear             302634       283722      1.07
hash_map<uint32_t, TestObject>/count             916487       907426      1.01
hash_map<uint32_t, TestObject>/erase pos         388042       321385      1.21         *
hash_map<uint32_t, TestObject>/erase range       122680       116280      1.06
hash_map<uint32_t, TestObject>/erase val        1710931      1729529      0.99
hash_map<uint32_t, TestObject>/find             1089462      1346527      0.81         *
hash_map<uint32_t, TestObject>/insert           4560310      5072350      0.90         *
hash_map<uint32_t, TestObject>/iteration         960117       495354      1.94         *
hash_map<uint32_t, TestObject>/operator[]       1872830      1890595      0.99

heap (uint32_t[])/make_heap                     3528418      3327257      1.06
heap (uint32_t[])/pop_heap                     63243859     61011853      1.04
heap (uint32_t[])/push_heap                    11602424     10045869      1.15         *
heap (uint32_t[])/sort_heap                    52965362     48744729      1.09

heap (vector<TestObject>)/make_heap            13191456     13089711      1.01
heap (vector<TestObject>)/pop_heap            148555656    144787742      1.03
heap (vector<TestObject>)/push_heap            28696689     26618830      1.08
heap (vector<TestObject>)/sort_heap           112473989    114018643      0.99

list<TestObject>/ctor(it)                      80186731     74006287      1.08
list<TestObject>/ctor(n)                        6232311      6128007      1.02
list<TestObject>/erase                        344556374    212877808      1.62         *
list<TestObject>/find                          39859075     14591347      2.73         *
list<TestObject>/insert                        86935153     56138233      1.55         *
list<TestObject>/push_back                     79569180     46700641      1.70         *
list<TestObject>/remove                       785786758    324201016      2.42         *
list<TestObject>/reverse                       45248186     24852759      1.82         *
list<TestObject>/size/1                          219844       219496      1.00
list<TestObject>/size/10                         519563       519579      1.00             EASTL intentionally implements list::size as O(n).
list<TestObject>/size/100                       4567194    101230266      0.05         *   EASTL intentionally implements list::size as O(n).
list<TestObject>/splice                        68321087     23601687      2.89         *

map<TestObject, uint32_t>/clear                  168011       180540      0.93
map<TestObject, uint32_t>/count                 4830439      5139287      0.94
map<TestObject, uint32_t>/equal_range           8700090      6158531      1.41         *
map<TestObject, uint32_t>/erase/key             6696776      4617038      1.45         *
map<TestObject, uint32_t>/erase/pos              309273       333183      0.93
map<TestObject, uint32_t>/erase/range            137419       136068      1.01
map<TestObject, uint32_t>/find                  4773498      4931352      0.97
map<TestObject, uint32_t>/insert                9651877      9311699      1.04
map<TestObject, uint32_t>/iteration              372946       416364      0.90         *
map<TestObject, uint32_t>/lower_bound           4784234      4915797      0.97
map<TestObject, uint32_t>/operator[]            5040254      5183147      0.97
map<TestObject, uint32_t>/upper_bound           4724292      4915984      0.96

set<uint32_t>/clear                              165300       173289      0.95
set<uint32_t>/count                             4958654      4885086      1.02
set<uint32_t>/equal_range                       8434134      5698681      1.48         *
set<uint32_t>/erase range                        145554       133960      1.09
set<uint32_t>/erase/pos                          299914       324760      0.92
set<uint32_t>/erase/val                         6506155      4335034      1.50         *
set<uint32_t>/find                              4866879      4556043      1.07
set<uint32_t>/insert                            8340523      8957257      0.93
set<uint32_t>/iteration                          294465       343442      0.86         *
set<uint32_t>/lower_bound                       4548095      4756498      0.96
set<uint32_t>/upper_bound                       4559196      4521498      1.01

sort/q_sort/TestObject[]                        7316766      7013894      1.04
sort/q_sort/TestObject[]/sorted                 1668439      1332885      1.25         *
sort/q_sort/vector<TestObject>                  7331530      7017260      1.04
sort/q_sort/vector<TestObject>/sorted           1601629      1247120      1.28         *
sort/q_sort/vector<ValuePair>                   7071643      7067869      1.00
sort/q_sort/vector<ValuePair>/sorted            2136390      1703799      1.25         *
sort/q_sort/vector<uint32>                      3292891      2943627      1.12         *
sort/q_sort/vector<uint32>/sorted                653693       473612      1.38         *

string<char16_t>/compare                      356579259    432760228      0.82         *
string<char16_t>/erase/pos,n                    3430422      3428645      1.00
string<char16_t>/find/p,pos,n                 229263402    225830975      1.02
string<char16_t>/find_first_not_of/p,pos,n       187391        81404      2.30         *
string<char16_t>/find_first_of/p,pos,n          4411831      4413532      1.00
string<char16_t>/find_last_of/p,pos,n            731655       726155      1.01
string<char16_t>/insert/pos,p                   3408628      3319726      1.03
string<char16_t>/iteration                    309993861    310333547      1.00
string<char16_t>/operator[]                      580839       579904      1.00
string<char16_t>/push_back                      3983338      2975553      1.34         *
string<char16_t>/replace/pos,n,p,n              4361095      4211504      1.04
string<char16_t>/reserve                      935141729       247010    100.00         *
string<char16_t>/rfind/p,pos,n                   248956       223397      1.11         *
string<char16_t>/size                             13311        13107      1.02
string<char16_t>/swap                            519129       579445      0.90         *

string<char8_t>/compare                        76695559     76828015      1.00
string<char8_t>/erase/pos,n                     1951566      1947282      1.00
string<char8_t>/find/p,pos,n                  185878944    185605039      1.00
string<char8_t>/find_first_not_of/p,pos,n        196877        81600      2.41         *
string<char8_t>/find_first_of/p,pos,n           4147685      4145356      1.00
string<char8_t>/find_last_of/p,pos,n             605897       598222      1.01
string<char8_t>/insert/pos,p                    1781592      1768264      1.01
string<char8_t>/iteration                        921502       921272      1.00
string<char8_t>/operator[]                       361250       359873      1.00
string<char8_t>/push_back                       3363288      2530493      1.33         *
string<char8_t>/replace/pos,n,p,n               2682600      2633130      1.02
string<char8_t>/reserve                       672517501        78387    100.00         *
string<char8_t>/rfind/p,pos,n                    226202       200013      1.13         *
string<char8_t>/size                              11280        11109      1.02
string<char8_t>/swap                             519393       559759      0.93

vector<uint64>/erase                           55184856     55192217      1.00
vector<uint64>/insert                          56764267     55682726      1.02
vector<uint64>/iteration                         423122       424039      1.00
vector<uint64>/operator[]                       1189397       860991      1.38         *
vector<uint64>/push_back                        5626609      4027317      1.40         *
vector<uint64>/sort                            49227036     49231362      1.00
```

----------------------------------------------
End of document
