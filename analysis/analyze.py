import sys

if len(sys.argv) < 2: sys.exit()

sec_per_unit_time = 0.00137

testcase = sys.argv[1]
test = open('OS_PJ1_Test/' + testcase + '.txt').readlines()
theo = open('out/' + testcase + '.out').readlines()
actl = open('out/' + testcase + '.msg').readlines()
numproc = int(test[1])

pid2index = dict()
theo_rt, theo_et = [-1] * numproc, [-1] * numproc
actl_rt, actl_et = [-1] * numproc, [-1] * numproc

# assign pid
for i in range(2*numproc, 3*numproc):
	pid = theo[i].split(' ')[-1]
	pid2index[pid[:-1]] = i % numproc
# assign theoretical ready & end time
for i in range(2*numproc):
	s = theo[i][:-2].split(' ')
	index = int(s[0][1:])
	t = int(s[4])
	if s[1] == 'ready':
		theo_rt[index-1] = t
	else: theo_et[index-1] = t

# assign actual ready & end time
for i in range(numproc):
	s = actl[i].split(' ')[3:]
	pid = s[0]
	s_sec, s_nsec = s[1].split('.')
	e_sec, e_nsec = s[2][:-1].split('.')
	ready = int(s_sec) + int(s_nsec) / 10 ** len(s_nsec)
	end = int(e_sec) + int(e_nsec) / 10 ** len(e_nsec)
	actl_rt[pid2index[pid]] = ready
	actl_et[pid2index[pid]] = end

avg_theo_wait_t, avg_actl_wait_t = 0,0
for i in range(numproc):
	avg_theo_wait_t += (theo_et[i] - theo_rt[i]) / numproc
	avg_actl_wait_t += (actl_et[i] - actl_rt[i]) / numproc

print('['+testcase+'] Avg. waiting time (UT) : ', avg_theo_wait_t)
print('['+testcase+'] Avg. waiting time (sec): ', avg_actl_wait_t)
print('['+testcase+'] Sec to unit time       : ', avg_actl_wait_t / sec_per_unit_time)
print('['+testcase+'] Actual / Theoretical   : ', avg_actl_wait_t / sec_per_unit_time / avg_theo_wait_t)
print()

