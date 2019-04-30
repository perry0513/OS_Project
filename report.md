# OS 2019 Project1 
組員： 
- b05901015 陳培威
- b05901061 林承緯
- b05901063 黃世丞
- b05901179 詹欣玥
- b03201012 林松逸

## 1. 設計
### **USER FILE**
### <b>main</b>
- 讀輸入，並確認：
    - policy為FIFO、RR、SJF、PSJF其中之一
    - 各個process name小於32個字元
    - 各個ready time、execution time為合法輸入
- 呼叫`void schedule (struct process* proc, int num_proc, int policy)`

### <b>scheduler</b>
- **`int next_process (struct process* proc, int num_proc, int policy)`**
    - 主要功能：依據policy進到不同的方法，去回傳下一個要執行的process在array中的index
    - `policy == SJF || policy == PSJF`
        - 用for loop遍歷所有proc，找到已經ready且執行時間（如果是PSJF的話則是剩於執行時間）最少的process。
        - 我們有構想要使用heap來加快搜尋下一個要執行的process的速度，來把next的複雜度從O(n)降為O(log n)。
    - `policy == FIFO`
        - 用一個global variable去紀錄已執行完的process的index，如果下一個已經ready就回傳index+1（在一開始有將process依照arrival time sort過了），複雜度為O(1)。
    - `policy == RR`
        - 如果前一個process跑完了，或如果前一個process還沒跑完但是time quantum到期，就要去往下找到下一個可以跑的process。
- **`void schedule (struct process* proc, int num_proc, int policy)`**
    - 主要功能：不斷的跑while loop直到所有process都跑完，每跑一圈就是一個time unit。
    - `while` 中的功能
        - `if` 檢查上一輪的process是不是做完了
        - `for` 檢查在這一輪中有沒有process ready了
        > scheduler.c 13 `#define  rep(start, n)  for(int i = start; i < n; ++i)`
        - `if` 如果還有可以跑的process的話，就去呼叫`next_process`
            - `if` `next_process`所找到的下一個process跟現在的不同的話，則進行context switch。
        - `if` 減少正在跑的process的剩餘execution time。

### **process**
- **`int proc_assign_cpu(int pid, int core)`**
    - 把編號為pid的process assign到編號core的CPU上
- **`int proc_exec(struct process proc)`**
    - 生成一個child process並執行他
    - 一開始先呼叫fork()複製一個process，此時回傳的pid = child process的pid，故會執行下面的`proc_assign_cpu(pid, CHILD_CPU)`把child process assign到另一顆CPU上
    - child process則會執行`if(pid == 0)`裡面的程式碼，也就是跑proc.t_exec次run_unit_time()，執行完畢後printk印出開始和結束時間
    - 最後exit(0)結束process
- **`int proc_block(int pid)`**
    - 把編號pid的process的設置為最低優先權
- **`int proc_wakeup(int pid)`**
    - 把編號pid的process設成SCHED_OTHER(預設值)
---
### **KERNEL FILE**
- **`void my_gettime(struct timespec * ts)`**
    - 利用timespec可以拿到秒及奈秒
```clike=
asmlinkage void sys_my_gettime(struct timespec* ts){
	getnstimeofday(ts);
	return;
}
```
- **`void my_printk(int pid, int start_sec, int start_nsec, int end_sec, int end_nsec)`**
```clike=
asmlinkage void sys_my_printk(int pid, long unsigned start_s, long unsigned start_ns,long unsigned end_s, long unsigned end_ns)
{
	printk("[project1] %d %lu.%09lu %lu.%09lu\n", pid, start_s, start_ns, end_s, end_ns);
	return;
}
```
    - 利用`my_gettime`得到時間，再藉由此函式印出實際得到的結果
## 2. 執行範例測資的結果
- Input：
```
FIFO
3
P1 1 10
P2 2 5
P3 2 7
```
- Result：
```
P1 4164
P2 4165
P3 4166
```
```
[project1] 4164 1556654555.626197521 1556654555.648679332
[project1] 4165 1556654555.648780776 1556654555.662926846
[project1] 4166 1556654555.648965514 1556654555.670555534
```
## 3. 比較實際結果與理論結果，並解釋造成差異的原因
- 範例測資由於process執行的時間太短，context switch和其他操作所花的時間所佔的比例很大，所以和理論值差很多
- 因為透過調整process的priority並不會馬上block或wakeup該process，實際在schedule時也要考慮context switch的及運算scheduling的時間，因此會造成誤差
- 若以average waiting time衡量，我們執行的實際結果與理想結果相聚不遠
```
[FIFO_3] Avg. waiting time (UT) :  15928.57142857143
[FIFO_3] Avg. waiting time (sec):  22.942766087395803
[FIFO_3] Sec to unit time       :  16746.544589340003
[FIFO_3] Actual / Theoretical   :  1.0513525751155157

[RR_5]   Avg. waiting time (UT) :  13014.285714285716
[RR_5]   Avg. waiting time (sec):  20.26079065459115
[RR_5]   Sec to unit time       :  14788.89828802274
[RR_5]   Actual / Theoretical   :  1.1363588146669503

[SJF_3]  Avg. waiting time (UT) :  12128.75
[SJF_3]  Avg. waiting time (sec):  15.89578440785408
[SJF_3]  Sec to unit time       :  11602.762341499329
[SJF_3]  Actual / Theoretical   :  0.9566329870348823

[PSJF_2] Avg. waiting time (UT) :  3400.0
[PSJF_2] Avg. waiting time (sec):  4.785451889038086
[PSJF_2] Sec to unit time       :  3493.030575940209
[PSJF_2] Actual / Theoretical   :  1.0273619341000615
.
.
.
(see more in analysis/results)
```
- 大多SJF的實驗結果比理論結果更快，十分令人訝異，但經過討論後仍然不知原因為何
- scheduler中RR使用的方法，必須用while loop遍歷所有process，才能判斷出下一個process要執行什麼。我們想過RR有兩種實作方式。
    - 用類似linked list，隨時將剩下可以run的process存起來，一定結束就將該node踢除。
        - 優點：當有大量process，且剩下少數個process時，可以省下很多遍歷所有process找可以執行的的時間。
        - 缺點：一開始建立linked list會很久，新增或是移除node也很久
        - 結論：如果是極端測資比如說很多小process一點大process，可能會比較好，但整體來說這樣有點太浪費時間浪費資源了。可能有更接近底層的實作方法可以這樣管理waiting processes
    - 即現在所使用的方法，如設計>USER FILE>sheduler所示
    - 現在RR是數個方法中平均來說`實際/理想`最高的（約1.15~1.2倍時間)

## 4. 各組員的貢獻 
- b05901015 陳培威
    - 研究compile kernel
    - my_printk.c
    - schedule.c
    - 做實驗
- b05901061 林承緯
- b05901063 黃世丞
    - 實作compile kernel
    - process.c
    - my_gettime.c
    - 做實驗
- b05901179 詹欣玥
    - 研究medium文章
    - my_printk.c
    - schedule.c
- b03201012 林松逸
    - main.c
  
