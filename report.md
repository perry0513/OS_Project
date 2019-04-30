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
        - 我們有構想要使用heap來加快搜尋下一個要執行的process的速度，來把next的複雜度從O(n)降為O(log n)。然而
    - `policy == FIFO`
        - 用一個global variable去紀錄已執行完的process的index，如果下一個已經ready就回傳index+1（在一開始就有將process依照arrival time sort過了），複雜度為 O(1)
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
P1 5088
P2 5089
P3 5090
```

```
[project1] 5088 2685.417490564 2685.439022892
[project1] 5089 2685.439173219 2685.448950350
[project1] 5090 2685.449074087 2685.462643535
```

## 3. 比較實際結果與理論結果，並解釋造成差異的原因
- 可能是因為透過調整process的priority並不會馬上block或wakeup該process

## 4. 各組員的貢獻 
> 大家自己寫ㄛ
- b05901015 陳培威
    - 研究compile kernel
    - my_printk.c
    - schedule.c
    - 研究linux不同版本
    - 幫世丞買晚餐
- b05901061 林承緯
- b05901063 黃世丞
    - 實作compile kernel
    - 開了60G大的ubuntu虛擬機
    - process.c
    - my_gettime.c
- b05901179 詹欣玥
    - 研究medium文章
    - my_printk.c
    - schedule.c
    - 提供蝦味花生豆跟芒果乾
    - 找到大神朋友請教:D
- b03201012 林松逸
    - main.c
