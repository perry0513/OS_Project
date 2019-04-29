# 107-2 Operating System Project 1
組員： 
- b05901015 陳培威
- b05901061 林承緯
- b05901063 黃世丞
- b05901179 詹欣玥
- b03201012 林松逸

## 1. 設計
### <b>scheduler</b>
- `int next_process (struct process* proc, int num_proc, int policy)`
    -  主要功能：依據policy進到不同的方法，去回傳下一個要執行的process在array中的index
    - `policy == SJF || policy == PSJF`: 用for loop遍歷所有proc，找到已經ready且執行時間（如果是PSJF的話則是勝於執行時間）最少的process。
    - `policy == FIFO`: 用一個global variable去紀錄已執行完的process的index，如果下一個已經ready就回傳index+1（在一開始就有將process依照arrival time sort過了）
    - `policy == RR`: 如果前一個process跑完了，或如果前一個process還沒跑完但是time quantum到期，就要去往下找到下一個可以跑的process。
-  `void schedule (struct process* proc, int num_proc, int policy)`
    - 主要功能：不斷的跑while loop直到所有process都跑完，每跑一圈就是一個time unit。
    - 
## 2. 執行範例測資的結果

## 3. 比較實際結果與理論結果，並解釋造成差異的原因

## 4. 各組員的貢獻
- main.c: b03201012 林松逸
- process.h, process.c: b05901061 林承緯 b05901063 黃世丞
- scheduler.h, scheduler.c: b05901015 陳培威 b05901179 詹欣玥
- debug: 