<?php


function unreliable_daemon()
{
	echo " unreliable daemon launched...\n";
	while (1) {
		usleep(300000);
		if (rand(0, 5) == 5) # russian roulette
			echo 'end';//str_repeat('x', 9e9); # crash
	}
}
function watchdog()
{
	$childpid = 0;
	$signals = array(SIGINT, SIGCHLD);
	pcntl_sigprocmask(SIG_BLOCK, $signals);
	do {
		printf("%s watchdog launching daemon...\n", @date('r'));
		if (0 === ($childpid = pcntl_fork())) {
			unreliable_daemon();
			exit(1);
		}
		$signal = pcntl_sigwaitinfo($signals, $info);
	} while ($signal != SIGINT);
	echo "watchdog got SIGINT. killing child...\n";
	posix_kill($childpid, SIGKILL);
	echo "watchdog exiting...\n";
}
watchdog();