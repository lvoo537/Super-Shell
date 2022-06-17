from subprocess import CalledProcessError, STDOUT, check_output
import os
import shutil
import datetime
import signal 
import sys

TIMEOUT = 5

def current_time(comment_file):
  comment_file.write(datetime.datetime.now().strftime("%d-%B-%Y %H:%M:%S") + "----")


def _test_compiles(student_dir, comment_file):
  name = "Code compiles"
  current_time(comment_file)
  comment_file.write(name)
 
  try:
    output = check_output(["rm", "*.o mysh"], stderr=STDOUT, timeout=TIMEOUT) 
  except CalledProcessError:
    pass    # No .o files, ignore errors
  
  try:
    output = check_output(["make", "clean"], stderr=STDOUT, timeout=TIMEOUT) 
  except CalledProcessError:
    pass   # Nothing to clean, ignore errors

  try:
    # TODO: Add checks to verify -Wall -Werror -fsanitize=address are in the Makefile, or restructure initial rules
    output = check_output(["make"], stderr=STDOUT, timeout=TIMEOUT) 
    comment_file.write("PASS\n")
    return True
  except CalledProcessError:
    comment_file.write("FAIL\n")
    return False  


# TODO: Recompile if errors/warnings are present?
def test_compile_suite(comment_file_path, student_dir):
  comment_file = open(comment_file_path, "a")
  ret = _test_compiles(student_dir, comment_file)
  return ret 
