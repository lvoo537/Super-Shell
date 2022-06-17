from subprocess import CalledProcessError, STDOUT, check_output, TimeoutExpired, Popen, PIPE 
import os
import shutil
import pty
import datetime
import sys
sys.path.append("..")
from time import sleep 
import subprocess
import multiprocessing 
import random 
random.seed(31)

from tests_helpers import * 

def reset_folder(folder_path):
    os.popen('rm -rf {}'.format(folder_path)).read()   # Remove the directory so we can recreate it
    os.popen('mkdir {}'.format(folder_path)).read()

def setup_folder_structure(folder_root_path: str, files: list):
    """
    Create a file structure.
    folder_root_path represents the root of the file structure. 
    files is a list of files or directories. 
    Directories have the form {name: subfiles}. Files are strings. 
    """
    reset_folder(folder_root_path)
    for file in files:
        if isinstance(file, dict):
          file_name = list(file.keys())[0]
          setup_folder_structure(folder_root_path + "/" + file_name, file[file_name])
        else:
          command = "touch {}/{}".format(folder_root_path, file) 
          os.popen(command) 

def execute_wc_test(comment_file_path, read_file_path, read_file_relative_path, max_output_length=30):
  try:
    expected_chars, expected_words, expected_newline = get_true_counts(read_file_path)
    p = start('./mysh')
    write(p,"wc {}".format(read_file_relative_path))
    word_output = read_stdout(p) 
    correct_word_count = len(word_output) < max_output_length and expected_words in word_output and "word count" in word_output
    character_output = read_stdout(p)  
    correct_character_count = len(character_output) < max_output_length and "character count" in character_output and expected_chars in character_output
    newline_output = read_stdout(p) 
    correct_newline_count = len(newline_output) < max_output_length and expected_newline in newline_output and "newline count" in newline_output
    if correct_word_count and correct_character_count and correct_newline_count:
      finish(comment_file_path, "OK")
    else:
      finish(comment_file_path, "NOT OK")
  except Exception as e:
    finish(comment_file_path, "NOT OK")


def get_true_counts(read_file_path):
    # Run shell wc program to obtain true expected values
    stream = os.popen('wc -l {}'.format(read_file_path))
    output = stream.read()
    output = output.strip()
    expected_newline = output.split(' ')[0]

    stream = os.popen('wc -w {}'.format(read_file_path))
    output = stream.read()
    output = output.strip()
    expected_words = output.split(' ')[0]

    stream = os.popen('wc -m {}'.format(read_file_path))
    output = stream.read()
    output = output.strip()
    expected_chars = output.split(' ')[0]

    return expected_chars, expected_words, expected_newline


def _test_arbritrary_file(comment_file_path, student_dir):
  start_test(comment_file_path, "wc on an arbritrary file")

  file_name = "testfile.txt"
  file_path = student_dir + "/" + file_name
  file_ptr = open(file_path, "w")
  possible_characters = ['a', 'b', 'c', 'word', ' ', '\n']
  for i in range(100):
    file_ptr.write(random.choice(possible_characters))  

  file_ptr.close()
  execute_wc_test(comment_file_path, file_path, file_name)
  remove_file(file_path)

def generate_secret_code():
  possible_characters = "abcdefghijklmnopqrstuvwxyz123456789"
  secret_code = ""
  for i in range(random.randint(5, 15)):
    secret_code += random.choice(possible_characters)
  return secret_code

def _test_many_zeros(comment_file_path, student_dir):
  start_test(comment_file_path, "wc on an a file with many 0s")

  file_name = "testfile.txt"
  file_path = student_dir + "/" + file_name
  file_ptr = open(file_path, "w")
  # Our solution exceeds 10 seconds after 10**8 
  file_ptr.write("0" * 10**5)   
  file_ptr.close()

  execute_wc_test(comment_file_path, file_path, file_name)
  remove_file(file_path)
    



def _test_arbritrary_tree(comment_file_path, student_dir, long_cutoff=30):
  start_test(comment_file_path, "Recursive ls correctly gathers files from an arbritrary deep tree")
  attempts = 10
  # All 10 random trees must pass for tests to pass. 

  for attempt in range(attempts):
    if _arbritrary_attempt(student_dir) == "NOT OK":
      finish(comment_file_path, "NOT OK")
      break  
  else:
    finish(comment_file_path, "OK")
  remove_folder(student_dir + "/testfolder")
  

def _arbritrary_attempt(student_dir, long_cutoff=30):
  chosen_secrets = []

  folder_structure = [{"subdirectory": []}]
  curr_dict = folder_structure[0]
  for i in range(20):
    curr_dict["subdirectory"].append({"subdirectory": []})
    # Add secrets to the tree with a 1/10 probability. 
    if random.randint(1, 10) == 5:
      secret = generate_secret_code()
      chosen_secrets.append(secret)
      curr_dict["subdirectory"].append(secret)
    curr_dict = curr_dict["subdirectory"][0]
    
  setup_folder_structure(student_dir + "/testfolder", folder_structure) 

  try:
    p = start('./mysh')
    write(p,"ls --rec testfolder --d 22")
    output_files = set()
    limit = 500 
    i = 0 
    while i < limit:
      line = read_stdout(p).replace('mysh$ ', '')
      if line in chosen_secrets:
        chosen_secrets.remove(line)
      if len(chosen_secrets) == 0:
        return "OK"
      if len(line) > long_cutoff:
        return "NOT OK"
      i += 1 
  
    return "NOT OK"
  except Exception as e:
    return "NOT OK"


def _test_multiple_directories(comment_file_path, student_dir):
  start_test(comment_file_path, "Recursive ls captures multiple sub-directories")
  setup_folder_structure(student_dir + "/testfolder", [{"subdirectory": ["innerfile"]}, {"subdirectory2": ["foofile"]} ])  
  
  try:
    p = start('./mysh')
    write(p,"ls --rec testfolder --d 2")
    expected_output = [".", "..", "subdirectory",  ".", "..", "innerfile", "subdirectory2", ".", "..","foofile"]
    expected_output_lines = len(expected_output)
    output_files = []
    for _ in range(expected_output_lines):
        line = read_stdout(p).replace('mysh$ ', '')
        output_files.append(line)

    output_files = set(output_files)
    if output_files != set(expected_output):
        finish(comment_file_path, "NOT OK")
    
    if has_memory_leaks(p):
      finish(comment_file_path, "NOT OK")
    else:
      finish(comment_file_path, "OK")

    remove_folder(student_dir + "/testfolder")
    
  except Exception as e:
    finish(comment_file_path, "NOT OK")


def test_milestone3_hidden_suite(comment_file_path, student_dir):
  start_suite(comment_file_path, "Hidden - Advanced wc tests")
  start_with_timeout(_test_arbritrary_file, comment_file_path, student_dir)
  start_with_timeout(_test_many_zeros, comment_file_path, student_dir, timeout=6)
  end_suite(comment_file_path)
  start_suite(comment_file_path, "Hidden - Advanced ls tests")
  start_with_timeout(_test_arbritrary_tree, comment_file_path, student_dir, timeout=10)
  start_with_timeout(_test_multiple_directories, comment_file_path, student_dir)
  end_suite(comment_file_path)
