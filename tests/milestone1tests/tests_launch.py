from subprocess import CalledProcessError, STDOUT, check_output, TimeoutExpired, Popen, PIPE
import os
import shutil
import pty
import datetime
import sys
from time import sleep 
import multiprocessing
from multiprocessing import Pool, TimeoutError
from tests_helpers import *



def _test_exit(comment_file_path, student_dir):
  start_test(comment_file_path, "Process exits shell with correct return code")
  p = Popen(['./mysh'], stdout=PIPE, stderr=PIPE, stdin=PIPE)
  p.communicate(input='exit'.encode())
  finish(comment_file_path, "OK")   # Terminated successfully 
  

def _test_shell_message(comment_file_path, student_dir, timeout=0.05):
  start_test(comment_file_path, "Shell message is displayed")
  try:
    p = Popen(['./mysh'], stdout=PIPE, stderr=PIPE, stdin=PIPE)
    stdout = p.communicate(input='exit'.encode(), timeout=timeout)[0]
    if stdout == b"mysh$ ":
      finish(comment_file_path, "OK")  
    else:
      finish(comment_file_path, "NOT OK")
  except Exception:
    finish(comment_file_path, "NOT OK")


def test_launch_suite(comment_file_path, student_dir):
  start_suite(comment_file_path, "Launch Suite")
  start_with_timeout(_test_exit, comment_file_path)
  start_with_timeout(_test_shell_message, comment_file_path)
  end_suite(comment_file_path)