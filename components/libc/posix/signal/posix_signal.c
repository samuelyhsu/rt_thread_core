/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017/10/1      Bernard      The first version
 */

#include <rthw.h>
#include <rtthread.h>

#include <sys/errno.h>
#include <sys/time.h>

#include "posix_signal.h"

#define sig_valid(sig_no) (sig_no >= 0 && sig_no < RT_SIG_MAX)

void (*signal(int sig, void (*func)(int)))(int) {
  return rt_signal_install(sig, func);
}

int sigprocmask(int how, const sigset_t *set, sigset_t *oset) {
  rt_base_t level;
  rt_thread_t tid;

  tid = rt_thread_self();

  level = rt_hw_interrupt_disable();
  if (oset)
    *oset = tid->sig_mask;

  if (set) {
    switch (how) {
    case SIG_BLOCK:
      tid->sig_mask |= *set;
      break;
    case SIG_UNBLOCK:
      tid->sig_mask &= ~*set;
      break;
    case SIG_SETMASK:
      tid->sig_mask = *set;
      break;
    default:
      break;
    }
  }
  rt_hw_interrupt_enable(level);

  return 0;
}

int sigpending(sigset_t *set) {
  sigprocmask(SIG_SETMASK, RT_NULL, set);
  return 0;
}

int sigsuspend(const sigset_t *set) {
  int ret = 0;
  sigset_t origin_set;
  sigset_t suspend_set;
  siginfo_t info; /* unless paremeter */

  /* get the origin signal information */
  sigpending(&origin_set);

  /* set the new signal information */
  sigprocmask(SIG_BLOCK, set, RT_NULL);
  sigpending(&suspend_set);

  ret = rt_signal_wait(&suspend_set, &info, RT_WAITING_FOREVER);

  /* restore the original sigprocmask */
  sigprocmask(SIG_UNBLOCK, (sigset_t *)0xffffUL, RT_NULL);
  sigprocmask(SIG_BLOCK, &origin_set, RT_NULL);

  return ret;
}

int sigaction(int signum, const struct sigaction *act,
              struct sigaction *oldact) {
  rt_sighandler_t old = RT_NULL;

  if (!sig_valid(signum))
    return -RT_ERROR;

  if (act)
    old = rt_signal_install(signum, act->sa_handler);
  else {
    old = rt_signal_install(signum, RT_NULL);
    rt_signal_install(signum, old);
  }

  if (oldact)
    oldact->sa_handler = old;

  return 0;
}

int sigtimedwait(const sigset_t *set, siginfo_t *info,
                 const struct timespec *timeout) {
  int ret = 0;
  int tick = RT_WAITING_FOREVER;

  if (timeout) {
    tick = rt_timespec_to_tick(timeout);
  }

  ret = rt_signal_wait(set, info, tick);
  if (ret == 0)
    return 0;

  errno = ret;
  return -1;
}

int sigwait(const sigset_t *set, int *sig) {
  siginfo_t si;
  if (sigtimedwait(set, &si, 0) < 0)
    return -1;

  *sig = si.si_signo;
  return 0;
}

int sigwaitinfo(const sigset_t *set, siginfo_t *info) {
  return sigtimedwait(set, info, NULL);
}

int raise(int sig) {
  rt_thread_kill(rt_thread_self(), sig);
  return 0;
}

#include <sys/types.h>
int sigqueue(pid_t pid, int signo, const union sigval value) {
  /* no support, signal queue */

  return -1;
}
